// Copyright Epic Games, Inc. All Rights Reserved.

#include "MIFinder.h"
#include "MIFinderStyle.h"
#include "MIFinderCommands.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunctionMaterialLayer.h"
#include "Materials/MaterialFunctionMaterialLayerBlend.h"
#include "WidgetLayoutParam.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionStaticBoolParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter.h"

static const FName MIFinderTabName("MIFinder");

#define LOCTEXT_NAMESPACE "FMIFinderModule"

FMIFinderModule::MaterialParameterWrapper::MaterialParameterWrapper()
{
	constexpr int32 DefaultSize = 64;
	StaticSwitchParameters.Reserve(DefaultSize);
	TextureParameters.Reserve(DefaultSize);
	ScalarParameters.Reserve(DefaultSize);
}

void FMIFinderModule::MaterialParameterWrapper::ClearAll()
{
	
	StaticSwitchParameters.Empty();
	TextureParameters.Empty();
	ScalarParameters.Empty();
}

void FMIFinderModule::MaterialParameterWrapper::BuildParameterFromMaterial(const UMaterial* Material)
{
	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;

	Material->GetAllStaticSwitchParameterInfo(ParameterInfos, ParameterIds);
	for (const FMaterialParameterInfo& Info : ParameterInfos)
	{
		FGuid Guid;
		bool Value;
		if(Material->GetStaticSwitchParameterValue(Info, Value, Guid))
		{
			StaticSwitchParameters.Add(MakeShared<StaticSwitchParameterDataObject>(Info.Name.ToString(), Info.Association, true, false));
		}
		
	}
	// --- Texture Parameters ---
	ParameterInfos.Reset();
	ParameterIds.Reset();

	Material->GetAllTextureParameterInfo(ParameterInfos, ParameterIds);

	for (const FMaterialParameterInfo& Info : ParameterInfos)
	{
		UTexture* Texture = nullptr;
		if (Material->GetTextureParameterValue(Info, Texture))
		{
			TextureParameters.Add(MakeShared<TextureParameterDataObject>(Info.Name.ToString(),Texture->GetPathName(), Info.Association, true,false));
		}
	}

	// --- Scalar Parameters ---
	ParameterInfos.Reset();
	ParameterIds.Reset();

	Material->GetAllScalarParameterInfo(ParameterInfos, ParameterIds);

	for (const FMaterialParameterInfo& Info : ParameterInfos)
	{
		float Value = 0.0f;
		if (Material->GetScalarParameterValue(Info, Value))
		{
			ScalarParameters.Add(MakeShared<FScalarParameterDataObject>(Info.Name.ToString(),Value, Info.Association, MIFinderScalarParameterQueryTypeEqual, false));
		}
	}
}

void FMIFinderModule::MaterialParameterWrapper::BuildParameterFromMaterialLayer(
	const UMaterialFunctionMaterialLayer* Layer)
{
	if (!Layer) { return; }

#if WITH_EDITOR   // FunctionExpressions はエディタ専用プロパティ
	for (UMaterialExpression* Expr : Layer->GetExpressions())
	{
		if (auto* BoolExpr = Cast<UMaterialExpressionStaticBoolParameter>(Expr))
		{
			StaticSwitchParameters.Add(
				MakeShared<StaticSwitchParameterDataObject>(
					BoolExpr->ParameterName.ToString(),
					EMaterialParameterAssociation::LayerParameter,
					static_cast<bool>(BoolExpr->DefaultValue),
					/*IsActive=*/false));
		}
		else if (auto* ScalarExpr = Cast<UMaterialExpressionScalarParameter>(Expr))
		{
			ScalarParameters.Add(
				MakeShared<FScalarParameterDataObject>(
					ScalarExpr->ParameterName.ToString(),
					ScalarExpr->DefaultValue,
					EMaterialParameterAssociation::LayerParameter,
					MIFinderScalarParameterQueryTypeEqual,
					/*IsActive=*/false));
		}
		else if (auto* TexExpr = Cast<UMaterialExpressionTextureSampleParameter>(Expr))
		{
			const FString Path = TexExpr->Texture ? TexExpr->Texture->GetPathName() : FString{};
			TextureParameters.Add(
				MakeShared<TextureParameterDataObject>(
					TexExpr->ParameterName.ToString(),
					Path,
					EMaterialParameterAssociation::LayerParameter,
					/*IsEqualQuery=*/true,
					/*IsActive=*/false));
		}
	}
#endif
}

void FMIFinderModule::MaterialParameterWrapper::BuildParameterFromMaterialBlend(
	const UMaterialFunctionMaterialLayerBlend* Blend)
{
	if (!Blend) { return; }

#if WITH_EDITOR   // FunctionExpressions はエディタ専用プロパティ
	for (UMaterialExpression* Expr : Blend->GetExpressions())
	{
		if (auto* BoolExpr = Cast<UMaterialExpressionStaticBoolParameter>(Expr))
		{
			StaticSwitchParameters.Add(
				MakeShared<StaticSwitchParameterDataObject>(
					BoolExpr->ParameterName.ToString(),
					EMaterialParameterAssociation::BlendParameter,
					static_cast<bool>(BoolExpr->DefaultValue),
					/*IsActive=*/false));
		}
		else if (auto* ScalarExpr = Cast<UMaterialExpressionScalarParameter>(Expr))
		{
			ScalarParameters.Add(
				MakeShared<FScalarParameterDataObject>(
					ScalarExpr->ParameterName.ToString(),
					ScalarExpr->DefaultValue,
					EMaterialParameterAssociation::BlendParameter,
					MIFinderScalarParameterQueryTypeEqual,
					/*IsActive=*/false));
		}
		else if (auto* TexExpr = Cast<UMaterialExpressionTextureSampleParameter>(Expr))
		{
			const FString Path = TexExpr->Texture ? TexExpr->Texture->GetPathName() : FString{};
			TextureParameters.Add(
				MakeShared<TextureParameterDataObject>(
					TexExpr->ParameterName.ToString(),
					Path,
					EMaterialParameterAssociation::BlendParameter,
					/*IsEqualQuery=*/true,
					/*IsActive=*/false));
		}
	}
#endif
}


void FMIFinderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMIFinderStyle::Initialize();
	FMIFinderStyle::ReloadTextures();

	FMIFinderCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMIFinderCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMIFinderModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMIFinderModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MIFinderTabName, FOnSpawnTab::CreateRaw(this, &FMIFinderModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMIFinderTabTitle", "MIFinder"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMIFinderModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FMIFinderStyle::Shutdown();
	FMIFinderCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MIFinderTabName);
}

TSharedRef<SDockTab> FMIFinderModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FMIFinderModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("MIFinder.cpp"))
		);

	auto MainWidget =  SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildRootMaterialBox()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SSeparator)
					//.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					BuildSelectLayerFunctionBox()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SSeparator)
					//.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0)
				[
					BuildParametersBox()
				]
			]
		];
	return MainWidget;
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildRootMaterialBox()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.FillWidth(1.0f)
	.Padding(WidgetLayoutParam::WidgetPadding)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
			.Text(NSLOCTEXT("AssetSearchTab","SearchRootText", "SearchRootPath"))
		]
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SBox)
			.HeightOverride(WidgetLayoutParam::EditableTextBoxHeight)
			[
				SNew(SEditableTextBox)
				.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
				.Text(NSLOCTEXT("AssetSearchTab","SearchRootTextBox", "/Game"))
			]
		]
	]
	+ SHorizontalBox::Slot()
	.FillWidth(1.0f)
	.Padding(WidgetLayoutParam::WidgetPadding)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
			.Text(NSLOCTEXT("AssetSearchTab","RootMaterialText", "Root Material"))
		]
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SObjectPropertyEntryBox)
				.AllowedClass(UMaterial::StaticClass())
				.ObjectPath(TAttribute<FString>::CreateLambda([this]()
				{
					if(!SearchRootMaterial.IsValid())
					{
						return FString{};
					}
					return SearchRootMaterial.Get()->GetPathName();
				}))
				.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialChanged))
		]
	];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildSelectLayerFunctionBox()
{
	return SNew(SHorizontalBox)
	       +SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
					.Text(NSLOCTEXT("FunctionsSelectTab","MateriallayerText", "Material Layer"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMaterialFunctionMaterialLayer::StaticClass())
					.ObjectPath(TAttribute<FString>::CreateLambda([this]()
					{
						if(!MaterialLayerAsset.IsValid())
						{
							return FString{};
						}
						return MaterialLayerAsset.Get()->GetPathName();
					}))
					.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialLayerChanged))
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
		    [
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				. AutoHeight()
				
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
					.Text(NSLOCTEXT("FunctionsSelectTab","MaterialBlendText", "Material Blend"))
				]
				+SVerticalBox::Slot()
				. AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMaterialFunctionMaterialLayerBlend::StaticClass())
					.ObjectPath(TAttribute<FString>::CreateLambda([this]()
									{
										if(!MaterialBlendAsset.IsValid())
										{
											return FString{};
										}
										return MaterialBlendAsset.Get()->GetPathName();
									}))
									.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialBlendChanged))
			]
		];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildParametersBox()
{
	auto ret = SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(WidgetLayoutParam::WidgetPadding)
			.FillWidth(1.0f)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
					.Text(NSLOCTEXT("ParametersTab","StaticSwitchText", "Static Switch"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildStaticSwitchParameterHeader()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SSeparator)
					.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(StaticSwitchScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
					 	SAssignNew(StaticSwitchInnerBox, SVerticalBox)
					 ]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock).Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
					.Text(NSLOCTEXT("ParametersTab","TexturePathText", "Texture"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildTextureParameterHeader()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SSeparator)
					.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(TextureParameterScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
						 SAssignNew(TextureParameterInnerBox, SVerticalBox)
					 ]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
					.Text(NSLOCTEXT("ParametersTab","ScalarText", "Scalar"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildScalarParameterHeader()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SSeparator)
					.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
					.Orientation(Orient_Horizontal)
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(ScalarParameterScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
						 SAssignNew(ScalarParameterInnerBox, SVerticalBox)
					 ]
				]
			];
	return ret;
}

void FMIFinderModule::OnRootMaterialChanged(const FAssetData& InAssetData)
{
	if(const auto Material = Cast<UMaterial>(InAssetData.GetAsset()))
	{
		SearchRootMaterial = Material;
		ClearAllParameterWidget();
		MaterialParameters.ClearAll();
		MaterialParameters.BuildParameterFromMaterial(Material);
	}
	else
	{
		SearchRootMaterial = nullptr;
		ClearAllParameterWidget();
		MaterialParameters.ClearAll();
	}
	BuildParameterWidget();
}

void FMIFinderModule::OnRootMaterialLayerChanged(const FAssetData& InAssetData)
{
    if (auto* Layer = Cast<UMaterialFunctionMaterialLayer>(InAssetData.GetAsset()))
    {
        MaterialLayerAsset = Layer;
        ClearAllParameterWidget();
        MaterialParameters.ClearAll();
        MaterialParameters.BuildParameterFromMaterialLayer(Layer);   // ★ 追加
    }
    else
    {
        MaterialLayerAsset = nullptr;
        ClearAllParameterWidget();
        MaterialParameters.ClearAll();
    }
    BuildParameterWidget();
}

void FMIFinderModule::OnRootMaterialBlendChanged(const FAssetData& InAssetData)
{
	if (auto* Blend = Cast<UMaterialFunctionMaterialLayerBlend>(InAssetData.GetAsset()))
	{
		MaterialBlendAsset = Blend;
		ClearAllParameterWidget();
		MaterialParameters.ClearAll();
		MaterialParameters.BuildParameterFromMaterialBlend(Blend);   // ★ 追加
	}
	else
	{
		MaterialBlendAsset = nullptr;
		ClearAllParameterWidget();
		MaterialParameters.ClearAll();
	}
	BuildParameterWidget();
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildStaticSwitchParameterHeader()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioIsActive)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","IsEnable", "Valid"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioParamName)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","Name", "Name"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioCondition)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","Condition", "Cond"))
	];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildTextureParameterHeader()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioIsActive)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","IsEnable", "Valid"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioParamName)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Name", "Name"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioTextureName)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Texture", "Name"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioCondition)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Condition", "Cond"))
		];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildScalarParameterHeader()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioIsActive)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","IsEnable", "Valid"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioParamName)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","Name", "Name"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioFloatValue)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","ScalarValue", "Value"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","Condition", "Type"))
	];
}

void FMIFinderModule::ClearAllParameterWidget()
{
	if( !StaticSwitchInnerBox.IsValid() ||!TextureParameterInnerBox.IsValid() || !ScalarParameterInnerBox.IsValid())
	{
		return;
	}
	StaticSwitchInnerBox->ClearChildren();
	TextureParameterInnerBox->ClearChildren();
	ScalarParameterInnerBox->ClearChildren();
}

void FMIFinderModule::BuildParameterWidget()
{
	if(!StaticSwitchInnerBox.IsValid() || !TextureParameterInnerBox.IsValid() ||!ScalarParameterInnerBox.IsValid())
		return;
	StaticSwitchInnerBox->ClearChildren();
	TextureParameterInnerBox->ClearChildren();
	ScalarParameterInnerBox->ClearChildren();
	for(const auto& StaticSwitch : MaterialParameters.StaticSwitchParameters)
	{
		StaticSwitchInnerBox->AddSlot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SStaticSwitchParameterWidget)
					.InItem(StaticSwitch)
				];
	}

	for(const auto& Texture : MaterialParameters.TextureParameters)
	{
		TextureParameterInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextureParameterWidget)
			.InItem(Texture)
		];
	}

	for(const auto& Scalar : MaterialParameters.ScalarParameters)
	{
		ScalarParameterInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SScalarParameterWidget)
			.InItem(Scalar)
		];
	}
}


void FMIFinderModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MIFinderTabName);
}

void FMIFinderModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMIFinderCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMIFinderCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMIFinderModule, MIFinder)