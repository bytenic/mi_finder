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
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "MIFinder sample test testing"), Category = "MIFinderTesting")
	static float MIFinderSampleFunction(float Param);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenFinderWindow", Keywords = "MIFinder Open"), Category = "MIFinder")
	static float OpenFinderWindow(float Param);

};
