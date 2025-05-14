// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct StaticSwitchParameterDataObject
{
	FString ParameterName{TEXT("None")};
	bool QueryCondition{false};
	bool IsActive{true};
};

class SStaticSwitchParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStaticSwitchParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<StaticSwitchParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	TSharedPtr<StaticSwitchParameterDataObject> WidgetData{};
};

struct TextureParameterDataObject
{
	FString TexturePath{};
	bool IsEqualQuery{true};
	bool IsActive{false};
};

class STextureParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STextureParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<TextureParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	void OnTextureChanged(const FAssetData& InAssetData);
	
private:
	TWeakObjectPtr<UTexture2D> CurrentSelectTexture{};
	TSharedPtr<TextureParameterDataObject> WidgetData{};
};

enum class EScalarParameterQueryType :uint8
{
	Less,
	Equal,
	Greater
};

struct FScalarParameterDataObject
{
	FString ParameterName{TEXT("None")};
	float QueryValue{.0f};
	EScalarParameterQueryType QueryCondition{false};
	bool IsActive{true};
};

class SScalarParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScalarParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<FScalarParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);
	
private:
	TSharedPtr<FScalarParameterDataObject> WidgetData{};
};
