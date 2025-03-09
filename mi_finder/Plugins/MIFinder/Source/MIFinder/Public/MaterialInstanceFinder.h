// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"


struct FMIFinderStaticSwitchQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("ParameterName", ParameterName);

	END_JSON_SERIALIZER

	FString ParameterName{};
};

struct FMIFinderQuery : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("SearchRoot", SearchRoot);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("StaticSwitch", StaticSwitchQueries, FMIFinderStaticSwitchQuery);
	END_JSON_SERIALIZER

	FString SearchRoot{};
	TArray<FMIFinderStaticSwitchQuery> StaticSwitchQueries{};
	
};

class MIFINDER_API FMaterialInstanceFinder
{
public:
	FMaterialInstanceFinder();
	~FMaterialInstanceFinder();
};
