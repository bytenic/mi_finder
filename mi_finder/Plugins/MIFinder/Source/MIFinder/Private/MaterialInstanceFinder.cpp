// MaterialInstanceFinder.cpp — 完全実装版
// ********************************************************************************************
// 本ファイルはマテリアルインスタンス検索ユーティリティの実装です。
// 検索ルート以下に存在する UMaterialInstance(U) アセットを列挙し、
// 与えられたクエリ（テクスチャ・スタティックスイッチ・スカラー）に一致する
// ものを抽出します。
// ********************************************************************************************

#include "MaterialInstanceFinder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"

#include "Misc/ScopedSlowTask.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/Package.h"

#if WITH_EDITOR
#include "MaterialUtilities.h"   // UnrealEd モジュール
#include "MaterialEditingLibrary.h"
#include "MaterialStatsCommon.h"
#endif

namespace MIFilterInternal
{
	/**
	 * @brief JSON では int で表現される Association を列挙型へ変換します。
	 */
	FORCEINLINE EMaterialParameterAssociation ToAssociation(int32 InAssociation)
	{
		return static_cast<EMaterialParameterAssociation>(InAssociation);
	}

	/**
	 * @brief 浮動小数点の誤差を許容した比較を行います。
	 */
	FORCEINLINE bool NearlyEqual(float A, float B, float Tolerance = KINDA_SMALL_NUMBER)
	{
		return FMath::Abs(A - B) <= Tolerance;
	}

	void GetTextureFetchCount(
	UMaterialInterface*      InMaterialInstance,
	int32&                   OutNumVSTextures,
	int32&                   OutNumPSTextures)
	{
#if WITH_EDITOR
		if (!InMaterialInstance)
		{
			return;
		}
		//note: UMaterialEditingLibrary::GetStatisticsがGCを呼ぶ可能性がある？DynamicMaterialModelEditorOnlyData.cppに記載あり
		TStrongObjectPtr<UObject> ScopeReference(InMaterialInstance);
		const FMaterialStatistics Stats = UMaterialEditingLibrary::GetStatistics(InMaterialInstance);
		OutNumVSTextures = Stats.NumVertexTextureSamples;	
		OutNumPSTextures = Stats.NumPixelTextureSamples;
#endif
	}
	
}

/***************************************************************************************************
 *                               FMaterialInstanceFinder                                           *
 ***************************************************************************************************/

/**
 * @brief デフォルトコンストラクタ。
 */
FMaterialInstanceFinder::FMaterialInstanceFinder() = default;

/**
 * @brief クエリ付きコンストラクタ。
 * @param InQuery 検索条件を格納した FMIFinderQuery 構造体。
 */
FMaterialInstanceFinder::FMaterialInstanceFinder(FMIFinderQuery&& InQuery)
	: Query(MoveTemp(InQuery))
{
	Result.Query      = Query;
	Result.SearchRoot = Query.SearchRoot;
}

/**
 * @brief デストラクタ。
 */
FMaterialInstanceFinder::~FMaterialInstanceFinder() = default;

/***************************************************************************************************
 *                                         Public API                                              *
 ***************************************************************************************************/

/**
 * @brief 検索処理を開始します。
 *
 * 現在は同期処理のみ実装しています。非同期処理が必要な場合は
 * StreamableManager でラップして呼び出してください。
 *
 * @param bAsync true を指定すると非同期で検索を行う予定（未実装）。
 * @return 検索結果を格納した FMIFinderQueryResult。
 */
FMIFinderQueryResult FMaterialInstanceFinder::Execute(bool /*bAsync*/)
{
	return ExecuteSequential();
}

/***************************************************************************************************
 *                                     Private Helpers                                             *
 ***************************************************************************************************/

/**
 * @brief 検索ルート以下に存在するマテリアルインスタンスを列挙します。
 * @return ソフト参照の配列。
 */
TArray<TSoftObjectPtr<UMaterialInstance>> FMaterialInstanceFinder::GetMaterialPaths() const
{
	TArray<TSoftObjectPtr<UMaterialInstance>> MaterialSoftPtrs;

	if (Query.SearchRoot.IsEmpty())
	{
		return MaterialSoftPtrs; // 検索対象なし
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry              = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UMaterialInstance::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(*Query.SearchRoot);
	Filter.bRecursivePaths = true;

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssets(Filter, AssetDatas);

	MaterialSoftPtrs.Reserve(AssetDatas.Num());
	for (const FAssetData& Data : AssetDatas)
	{
		MaterialSoftPtrs.Emplace(Data.ToSoftObjectPath());
	}

	return MaterialSoftPtrs;
}

/**
 * @brief 同期検索の本体処理。
 * @return 検索結果。
 */
FMIFinderQueryResult FMaterialInstanceFinder::ExecuteSequential()
{
	const TArray<TSoftObjectPtr<UMaterialInstance>> MaterialPaths = GetMaterialPaths();

	Result.Results.Reset();
	Result.Results.Reserve(MaterialPaths.Num());

	FScopedSlowTask SlowTask(MaterialPaths.Num(), NSLOCTEXT("MaterialInstanceFinder", "ScanningMaterials", "マテリアルインスタンスをスキャン中"));

	for (const TSoftObjectPtr<UMaterialInstance>& SoftPtr : MaterialPaths)
	{
		SlowTask.EnterProgressFrame(1.0f);

		UMaterialInstance* MI = SoftPtr.LoadSynchronous();
		if (!MI)
		{
			continue; // 読み込み失敗はスキップ
		}

		if (QueryMaterial(MI))
		{
			Result.Results.Emplace(MI->GetPathName());
		}
	}

	return Result;
}

/**
 * @brief 個々のマテリアルがクエリを満たすか判定します。
 * @param InMaterialInstance 判定対象のマテリアルインスタンス。
 * @return すべての条件を満たす場合 true。
 */
bool FMaterialInstanceFinder::QueryMaterial(UMaterialInstance* InMaterialInstance) const
{
	if (!InMaterialInstance)
	{
		return false;
	}

	// すべての個別クエリを AND 条件で評価
	return TextureNameQuery(InMaterialInstance) &&
	       StaticSwitchQuery(InMaterialInstance) &&
	       ScalarQuery(InMaterialInstance) &&
	       TextureFetchQuery(InMaterialInstance);
}

/*-------------------------------- テクスチャパス ----------------------------------------------*/

/**
 * @brief テクスチャパス条件を評価します。
 */
bool FMaterialInstanceFinder::TextureNameQuery(UMaterialInstance* InMaterialInstance) const
{
	if (Query.TexturePathQueries.IsEmpty())
		return false;
	
	for (const FMIFinderTexturePathQuery& TextureQuery : Query.TexturePathQueries)
	{
		FMaterialParameterInfo Info(*TextureQuery.ParameterName, MIFilterInternal::ToAssociation(TextureQuery.Association));

		UTexture* TextureValue = nullptr;
		if (!InMaterialInstance->GetTextureParameterValue(Info, TextureValue))
		{
			return false; // パラメータが存在しない
		}

		const FString FoundPath = TextureValue ? TextureValue->GetPathName() : FString();
		const bool    bEqual    = FoundPath.Equals(TextureQuery.TexturePath, ESearchCase::IgnoreCase);

		// IsEqualQuery が true なら一致必須、false なら不一致必須
		if (TextureQuery.IsEqualQuery ? !bEqual : bEqual)
		{
			return false;
		}
	}

	return true;
}

/*-------------------------------- スタティックスイッチ ----------------------------------------*/

/**
 * @brief スタティックスイッチ条件を評価します。
 */
bool FMaterialInstanceFinder::StaticSwitchQuery(UMaterialInstance* InMaterialInstance) const
{
	if (Query.StaticSwitchQueries.IsEmpty())
		return false;
	
	FGuid Guid;
	for (const FMIFinderStaticSwitchQuery& SwitchQuery : Query.StaticSwitchQueries)
	{
		FHashedMaterialParameterInfo Info(*SwitchQuery.ParameterName, MIFilterInternal::ToAssociation(SwitchQuery.Association));
		bool Value   = false;
		bool bFound = InMaterialInstance->GetStaticSwitchParameterValue(Info, Value, Guid);
		if (!bFound || Value != SwitchQuery.Condition)
		{
			return false;
		}
	}

	return true;
}

/*-------------------------------- スカラー -----------------------------------------------------*/

/**
 * @brief スカラーパラメータ条件を評価します。
 */
bool FMaterialInstanceFinder::ScalarQuery(UMaterialInstance* InMaterialInstance) const
{
	if (Query.ScalarQueries.IsEmpty())
		return false;
	
	for (const FMIFinderScalarQuery& ScalarQueryElem : Query.ScalarQueries)
	{
		FMaterialParameterInfo Info(*ScalarQueryElem.ParameterName, MIFilterInternal::ToAssociation(ScalarQueryElem.Association));

		float ScalarValue = 0.0f;
		if (!InMaterialInstance->GetScalarParameterValue(Info, ScalarValue))
		{
			return false;
		}

		// クエリタイプ別に判定
		switch (ScalarQueryElem.QueryType)
		{
		case FMIFinderScalarQuery::TypeLess:
			if (!(ScalarValue < ScalarQueryElem.Value)) { return false; }
			break;

		case FMIFinderScalarQuery::TypeEqual:
			if (!MIFilterInternal::NearlyEqual(ScalarValue, ScalarQueryElem.Value)) { return false; }
			break;

		case FMIFinderScalarQuery::TypeGreater:
			if (!(ScalarValue > ScalarQueryElem.Value)) { return false; }
			break;

		default:
			return false; // 未対応のクエリタイプ
		}
	}

	return true;
}

bool FMaterialInstanceFinder::TextureFetchQuery(UMaterialInstance* InMaterialInstance) const
{
	if (Query.NumVertexTextureFetch == FMIFinderQuery::InvalidTextureFetchCount &&
		Query.NumPixelTextureFetch == FMIFinderQuery::InvalidTextureFetchCount)
	{
		return true;
	}
	
	int32 NumVertexTextureFetch = 0;
	int32 NumPixelTextureFetch = 0;
	MIFilterInternal::GetTextureFetchCount(InMaterialInstance, NumVertexTextureFetch, NumPixelTextureFetch);

	bool VertexResult = false;
	if (Query.NumVertexTextureFetch > FMIFinderQuery::InvalidTextureFetchCount)
	{
		VertexResult = NumVertexTextureFetch > Query.NumVertexTextureFetch;
	}

	bool PixelResult = false;
	if (Query.NumVertexTextureFetch > FMIFinderQuery::InvalidTextureFetchCount)
	{
		PixelResult = NumPixelTextureFetch > Query.NumPixelTextureFetch;
	}
	return VertexResult && PixelResult;
}
