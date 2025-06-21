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
