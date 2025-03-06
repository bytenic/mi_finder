// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MIFinderEntryPoints.generated.h"

/**
 * 
 */
UCLASS()
class MIFINDER_API UMIFinderEntryPoints : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="MIFinder")
	static bool FilterMaterials();
	
	
};
