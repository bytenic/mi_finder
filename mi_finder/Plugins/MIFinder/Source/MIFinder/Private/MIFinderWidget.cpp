// Fill out your copyright notice in the Description page of Project Settings.


#include "MIFinderWidget.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "WidgetLayoutParam.h"

#define LOCTEXT_NAMESPACE "FMIFinderModuleWindow"

StaticSwitchParameterDataObject::StaticSwitchParameterDataObject(const FString& ParamName,EMaterialParameterAssociation Association, bool IsEqualQuery, bool IsActive)
	: ParameterName(ParamName), Association(Association), IsEqualQuery(IsEqualQuery), IsActive(IsActive)
{
}

void SStaticSwitchParameterWidget::Construct(const FArguments& InArgs)
{
	if(!InArgs._InItem)
	{
		return;
	}
	WidgetData = InArgs._InItem;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioIsActive)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("StaticSwitchParameterRegion","StaticSwitchParameterName", "WidgetName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioCondition)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		]
	];
}

TextureParameterDataObject::TextureParameterDataObject(
	const FString& ParameterName,
	const FString& TexturePathName,
	EMaterialParameterAssociation Association, bool IsEqualQuery, bool IsActive)
		:ParameterName(ParameterName), TexturePathName(TexturePathName), Association(Association), IsEqualQuery(IsEqualQuery), IsActive(IsActive)
{
}

void STextureParameterWidget::Construct(const FArguments& InArgs)
{
	if(!InArgs._InItem)
	{
		return;
	}
	WidgetData = InArgs._InItem;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioIsActive)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("TextureParameterRegion","TextureParameterName", "ParameterName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioTextureName)
		.HAlign(HAlign_Left)
		[
		SNew(SObjectPropertyEntryBox)
				.AllowedClass(UTexture2D::StaticClass())
				.ObjectPath(TAttribute<FString>::CreateLambda([this]()
				{
					if(!CurrentSelectTexture.IsValid())
					{
						return FString{};
					}
					return CurrentSelectTexture.Get()->GetPathName();
				}))
				.OnObjectChanged(FOnSetObject::CreateRaw(this, &STextureParameterWidget::OnTextureChanged))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioCondition)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		]
	];
}

void STextureParameterWidget::OnTextureChanged(const FAssetData& InAssetData)
{
}

FScalarParameterDataObject::FScalarParameterDataObject(const FString& ParamName, float Value,
	EMaterialParameterAssociation Association, EScalarParameterQueryType QueryType, bool IsActive)
		: ParameterName(ParamName), QueryValue(Value), Association(Association), QueryType(QueryType), IsActive(IsActive)
{
}

void SScalarParameterWidget::Construct(const FArguments& InArgs)
{
	if(!InArgs._InItem)
	{
		return;
	}
	WidgetData = InArgs._InItem;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioIsActive)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(NSLOCTEXT("ScalarParameterRegion","StaticSwitchParameterName", "WidgetName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioFloatValue)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			//.Value(this, &SPDD_TargetRow::GetScale)
			//.OnValueChanged(this, &SPDD_TargetRow::SetScale)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
		.HAlign(HAlign_Left)
		[
			SNew(SComboBox<TSharedPtr<EScalarParameterQueryType>>)
			//.Value(this, &SPDD_TargetRow::GetScale)
			//.OnValueChanged(this, &SPDD_TargetRow::SetScale)
		]

	];
}

#undef LOCTEXT_NAMESPACE
