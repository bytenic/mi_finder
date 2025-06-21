// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

static constexpr int32 MIFinderScalarParameterQueryTypeEqual = 0;
static constexpr int32 MIFinderScalarParameterQueryTypeLess = 1;
static constexpr int32 MIFinderScalarParameterQueryTypeGreater = 2;
using ScalarParameterQueryType = int32;

static constexpr int32 MIFinderParameterAssociationLayerParameter = 0;
static constexpr int32 MIFinderParameterAssociationBlendParameter = 1;
static constexpr int32 MIFinderParameterAssociationGlobalParameter = 2;
using MIFinderParameterAssociation = int32;


static_assert(MIFinderParameterAssociationLayerParameter == LayerParameter,"[MIFinder]Error MIFinderParameterAssociationLayerParameter");
static_assert(MIFinderParameterAssociationBlendParameter == BlendParameter,"[MIFinder]Error MIFinderParameterAssociationBlendParameter");
static_assert(MIFinderParameterAssociationGlobalParameter == GlobalParameter,"[MIFinder]Error MIFinderParameterAssociationGlobalParameter");

struct FMIFinderScalarQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("ParameterName", ParameterName);
	JSON_SERIALIZE("Value", Value);
	JSON_SERIALIZE("QueryType", QueryType);
	JSON_SERIALIZE("Association", Association);
	END_JSON_SERIALIZER

	FMIFinderScalarQuery() = default;
	explicit FMIFinderScalarQuery(FString InParameterName,
								  float   InValue,
								  int32   InQueryType   = TypeEqual,
								  int32   InAssociation = MIFinderParameterAssociationGlobalParameter)
		: ParameterName(MoveTemp(InParameterName))
		, Value         (InValue)
		, QueryType     (InQueryType)
		, Association   (InAssociation)
	{}
	FMIFinderScalarQuery(const FMIFinderScalarQuery&)            = default;
	FMIFinderScalarQuery(FMIFinderScalarQuery&&)      noexcept   = default;
	FMIFinderScalarQuery& operator=(const FMIFinderScalarQuery&) = default;
	FMIFinderScalarQuery& operator=(FMIFinderScalarQuery&&)      noexcept = default;
	virtual ~FMIFinderScalarQuery() override = default;
	
	FString ParameterName{};
	float Value{.0f};
	int32 QueryType{TypeLess};
	int32 Association{MIFinderParameterAssociationGlobalParameter};

	static constexpr int32 TypeLess = 0;
	static constexpr int32 TypeEqual = 1;
	static constexpr int32 TypeGreater = 2;
};

struct FMIFinderTexturePathQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("ParameterName", ParameterName);
	JSON_SERIALIZE("TexturePath", TexturePath);
	JSON_SERIALIZE("IsEqualQuery", IsEqualQuery);
	JSON_SERIALIZE("Association", Association);
	END_JSON_SERIALIZER
	
	FMIFinderTexturePathQuery() = default;
	explicit FMIFinderTexturePathQuery(FString InParameterName,
									   FString InTexturePath,
									   bool    bEqual        = true,
									   int32   InAssociation = MIFinderParameterAssociationGlobalParameter)
		: ParameterName(MoveTemp(InParameterName))
		, TexturePath  (MoveTemp(InTexturePath))
		, IsEqualQuery (bEqual)
		, Association  (InAssociation)
	{}
	FMIFinderTexturePathQuery(const FMIFinderTexturePathQuery&)            = default;
	FMIFinderTexturePathQuery(FMIFinderTexturePathQuery&&)      noexcept   = default;
	FMIFinderTexturePathQuery& operator=(const FMIFinderTexturePathQuery&) = default;
	FMIFinderTexturePathQuery& operator=(FMIFinderTexturePathQuery&&)      noexcept = default;
	virtual ~FMIFinderTexturePathQuery() override = default;
	
	FString ParameterName{};
	FString TexturePath{};
	bool IsEqualQuery{true};
	int32 Association{MIFinderParameterAssociationGlobalParameter};
};

struct FMIFinderStaticSwitchQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("ParameterName", ParameterName);
	JSON_SERIALIZE("Condition", Condition);
	JSON_SERIALIZE("Association", Association);
	END_JSON_SERIALIZER

	FMIFinderStaticSwitchQuery() = default;
	explicit FMIFinderStaticSwitchQuery(FString InParameterName,
										bool    bCondition    = true,
										int32   InAssociation = MIFinderParameterAssociationGlobalParameter)
		: ParameterName(MoveTemp(InParameterName))
		, Condition   (bCondition)
		, Association (InAssociation)
	{}

	FMIFinderStaticSwitchQuery(const FMIFinderStaticSwitchQuery&)            = default;
	FMIFinderStaticSwitchQuery(FMIFinderStaticSwitchQuery&&)      noexcept   = default;
	FMIFinderStaticSwitchQuery& operator=(const FMIFinderStaticSwitchQuery&) = default;
	FMIFinderStaticSwitchQuery& operator=(FMIFinderStaticSwitchQuery&&)      noexcept = default;
	virtual ~FMIFinderStaticSwitchQuery() override = default;
	
	FString ParameterName{};
	bool Condition{true};
	int32 Association{MIFinderParameterAssociationGlobalParameter};
	
};

struct FMIFinderQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("SearchRoot", SearchRoot);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("StaticSwitch", StaticSwitchQueries, FMIFinderStaticSwitchQuery);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("TexturePath", TexturePathQueries, FMIFinderTexturePathQuery);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("Scalar", ScalarQueries, FMIFinderScalarQuery);
	END_JSON_SERIALIZER

	FMIFinderQuery() = default;
	explicit FMIFinderQuery(FString                                 InSearchRoot,
							TArray<FMIFinderStaticSwitchQuery>&&    InStaticSwitch,
							TArray<FMIFinderTexturePathQuery> &&    InTexturePath,
							TArray<FMIFinderScalarQuery>&&          InScalar)
		: SearchRoot          (MoveTemp(InSearchRoot))
		, StaticSwitchQueries (MoveTemp(InStaticSwitch))
		, TexturePathQueries  (MoveTemp(InTexturePath))
		, ScalarQueries       (MoveTemp(InScalar))
	{}

	FMIFinderQuery(const FMIFinderQuery&)            = default;
	FMIFinderQuery(FMIFinderQuery&&)      noexcept   = default;
	FMIFinderQuery& operator=(const FMIFinderQuery&) = default;
	FMIFinderQuery& operator=(FMIFinderQuery&&)      noexcept = default;
	virtual ~FMIFinderQuery() override = default;
	
	FString SearchRoot{};
	TArray<FMIFinderStaticSwitchQuery> StaticSwitchQueries{};
	TArray<FMIFinderTexturePathQuery> TexturePathQueries{};
	TArray<FMIFinderScalarQuery> ScalarQueries{};
};

class MIFINDER_API FMaterialInstanceFinder
{
public:
	FMaterialInstanceFinder();
	~FMaterialInstanceFinder();

private:
	FMIFinderQuery Query{};
};
