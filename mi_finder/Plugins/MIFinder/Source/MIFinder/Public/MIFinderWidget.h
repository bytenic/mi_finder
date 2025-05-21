// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct StaticSwitchParameterDataObject
{
	FString ParameterName{TEXT("None")};
	EMaterialParameterAssociation Association{EMaterialParameterAssociation::GlobalParameter};
	bool IsEqualQuery{false};
	bool IsActive{false};
	
	StaticSwitchParameterDataObject() = default;
	StaticSwitchParameterDataObject(const FString& ParamName, EMaterialParameterAssociation Association, bool IsEqualQuery, bool IsActive);
};

class SStaticSwitchParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStaticSwitchParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<StaticSwitchParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	void OnIsActiveChanged(ECheckBoxState NewState);
	ECheckBoxState IsActive()const;

	void OnIsEqualQueryChanged(ECheckBoxState NewState);
	ECheckBoxState IsEqualQuery()const;
	
	
private:
	TSharedPtr<StaticSwitchParameterDataObject> WidgetData{};
};

struct TextureParameterDataObject
{
	FString ParameterName{TEXT("None")};
	FString TexturePathName{};
	EMaterialParameterAssociation Association{EMaterialParameterAssociation::GlobalParameter};
	bool IsEqualQuery{true};
	bool IsActive{false};

	TextureParameterDataObject() = default;
	TextureParameterDataObject(const FString& ParameterName, const FString& TexturePathName, EMaterialParameterAssociation Association, bool IsEqualQuery, bool IsActive);
};

class STextureParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STextureParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<TextureParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	void OnIsActiveChanged(ECheckBoxState NewState);
	ECheckBoxState OnIsActive()const;
	
	void OnTextureChanged(const FAssetData& InAssetData);

	void OnIsEqualQueryChanged(ECheckBoxState NewState);
	ECheckBoxState IsEqualQuery()const;
	
private:
	TWeakObjectPtr<UTexture2D> CurrentSelectTexture{};
	TSharedPtr<TextureParameterDataObject> WidgetData{};
};

enum class EScalarParameterQueryType :uint8
{
	Less, //未満
	Equal,
	Greater //超過
};

struct FScalarParameterDataObject
{
	FString ParameterName{TEXT("None")};
	float QueryValue{.0f};
	EMaterialParameterAssociation Association{EMaterialParameterAssociation::GlobalParameter};
	EScalarParameterQueryType QueryType{EScalarParameterQueryType::Equal};
	bool IsActive{true};
	
	FScalarParameterDataObject() = default;
	FScalarParameterDataObject(const FString& ParamName, float Value, EMaterialParameterAssociation Association, EScalarParameterQueryType QueryType, bool IsActive);
};

class SScalarParameterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScalarParameterWidget){}
	SLATE_ARGUMENT(TSharedPtr<FScalarParameterDataObject>, InItem)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	void OnIsActiveChanged(ECheckBoxState NewState);
	ECheckBoxState IsActive()const;

	void OnValueChanged(float NewValue);
	TOptional<float> GetValue()const;

	FText GetSelectedQueryTypeText()const;
	void OnQueryTypeChanged(TSharedPtr<EScalarParameterQueryType> NewValue, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> MakeComboItemWidget(TSharedPtr<EScalarParameterQueryType> InItem) const;
private:
	TSharedPtr<FScalarParameterDataObject> WidgetData{};
	TArray<TSharedPtr<EScalarParameterQueryType>> QueryTypeOptions;
	TSharedPtr<EScalarParameterQueryType> CurrentSelectQueryType{};
};
