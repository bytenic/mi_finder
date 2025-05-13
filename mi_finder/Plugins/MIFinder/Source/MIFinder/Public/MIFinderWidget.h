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

	static constexpr float LayoutPadding = 2.5f;
	static constexpr float FontSize = 12.0f;
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
	static constexpr float LayoutPadding = 2.5f;
	static constexpr float FontSize = 12.0f;
};
