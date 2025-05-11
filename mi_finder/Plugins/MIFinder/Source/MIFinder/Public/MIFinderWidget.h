// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct StaticSwitchParameterDataObject
{
	FString ParameterName{TEXT("None")};
	bool QueryCondition{false};
	bool IsEqualQuery{true};
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
