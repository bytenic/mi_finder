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
		   .OnCheckStateChanged(this, &SStaticSwitchParameterWidget::OnIsActiveChanged)
		   .IsChecked(this, &SStaticSwitchParameterWidget::IsActive)
		   
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(FText::FromString(WidgetData->ParameterName))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioCondition)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &SStaticSwitchParameterWidget::OnIsEqualQueryChanged)
			.IsChecked(this, &SStaticSwitchParameterWidget::IsEqualQuery)
		]
	];
}

void SStaticSwitchParameterWidget::OnIsActiveChanged(ECheckBoxState NewState)
{
	WidgetData->IsActive = NewState == ECheckBoxState::Checked;
}

ECheckBoxState SStaticSwitchParameterWidget::IsActive() const
{
	return WidgetData->IsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SStaticSwitchParameterWidget::OnIsEqualQueryChanged(ECheckBoxState NewState)
{
	WidgetData->IsEqualQuery = NewState == ECheckBoxState::Checked;
}

ECheckBoxState SStaticSwitchParameterWidget::IsEqualQuery() const
{
	return WidgetData->IsEqualQuery ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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
		   .OnCheckStateChanged(this, &STextureParameterWidget::OnIsActiveChanged)
		   .IsChecked(this, &STextureParameterWidget::OnIsActive)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(FText::FromString(WidgetData->ParameterName))
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
			.OnCheckStateChanged(this, &STextureParameterWidget::OnIsEqualQueryChanged)
			.IsChecked(this, &STextureParameterWidget::IsEqualQuery)
		]
	];
}

void STextureParameterWidget::OnIsActiveChanged(ECheckBoxState NewState)
{
	WidgetData->IsActive = NewState == ECheckBoxState::Checked;
}

ECheckBoxState STextureParameterWidget::OnIsActive() const
{
	return WidgetData->IsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void STextureParameterWidget::OnTextureChanged(const FAssetData& InAssetData)
{
	if(const auto Texture = Cast<UTexture2D>(InAssetData.GetAsset()))
	{
		CurrentSelectTexture = Texture;
		WidgetData->TexturePathName = Texture->GetPathName();
	}
	else
	{
		CurrentSelectTexture = nullptr;
		WidgetData->TexturePathName.Empty();
	}
}

void STextureParameterWidget::OnIsEqualQueryChanged(ECheckBoxState NewState)
{
	WidgetData->IsEqualQuery = NewState == ECheckBoxState::Checked;	
}

ECheckBoxState STextureParameterWidget::IsEqualQuery() const
{
	return WidgetData->IsEqualQuery ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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

	QueryTypeOptions.Add(MakeShared<EScalarParameterQueryType>(EScalarParameterQueryType::Equal));
	QueryTypeOptions.Add(MakeShared<EScalarParameterQueryType>(EScalarParameterQueryType::Greater));
	QueryTypeOptions.Add(MakeShared<EScalarParameterQueryType>(EScalarParameterQueryType::Less));
	// デフォルト選択
	CurrentSelectQueryType = QueryTypeOptions[0];

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioIsActive)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		   .OnCheckStateChanged(this, &SScalarParameterWidget::OnIsActiveChanged)
		   .IsChecked(this, &SScalarParameterWidget::IsActive)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioParamName)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(FText::FromString(WidgetData->ParameterName))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioFloatValue)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			.Value(this, &SScalarParameterWidget::GetValue)
			.OnValueChanged(this, &SScalarParameterWidget::OnValueChanged)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
		.HAlign(HAlign_Left)
		[
			SNew(SComboBox<TSharedPtr<EScalarParameterQueryType>>)
			.OptionsSource(&QueryTypeOptions)
			.OnGenerateWidget(this, &SScalarParameterWidget::MakeComboItemWidget)
			.OnSelectionChanged(this, &SScalarParameterWidget::OnQueryTypeChanged)
			.InitiallySelectedItem(CurrentSelectQueryType)
			[
				SNew(STextBlock)
				.Text(this, &SScalarParameterWidget::GetSelectedQueryTypeText)
			]
		]

	];
}

void SScalarParameterWidget::OnIsActiveChanged(ECheckBoxState NewState)
{
	WidgetData->IsActive = NewState == ECheckBoxState::Checked;
}

ECheckBoxState SScalarParameterWidget::IsActive() const
{
	return WidgetData->IsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SScalarParameterWidget::OnValueChanged(float NewValue)
{
	WidgetData->QueryValue = NewValue;
}

TOptional<float> SScalarParameterWidget::GetValue() const
{
	return WidgetData->QueryValue;
}

FText SScalarParameterWidget::GetSelectedQueryTypeText() const
{
	if (!CurrentSelectQueryType.IsValid())
	{
		return FText::FromString(TEXT("Error"));
	}

	switch (*CurrentSelectQueryType)
	{
	case EScalarParameterQueryType::Less: return FText::FromString(TEXT("Less"));
	case EScalarParameterQueryType::Equal: return FText::FromString(TEXT("Equal"));
	case EScalarParameterQueryType::Greater: return FText::FromString(TEXT("Greater"));
	default: return FText::FromString(TEXT("Error"));
	}
}

void SScalarParameterWidget::OnQueryTypeChanged(TSharedPtr<EScalarParameterQueryType> NewValue,
	ESelectInfo::Type SelectInfo)
{
	if (NewValue.IsValid())
	{
		CurrentSelectQueryType = NewValue;
		
	}
}

TSharedRef<SWidget> SScalarParameterWidget::MakeComboItemWidget(TSharedPtr<EScalarParameterQueryType> InItem) const
{
	FText Label;

	switch (*InItem)
	{
	case EScalarParameterQueryType::Less: Label = FText::FromString(TEXT("Less")); break;
	case EScalarParameterQueryType::Equal: Label = FText::FromString(TEXT("Equal")); break;
	case EScalarParameterQueryType::Greater: Label = FText::FromString(TEXT("Greater")); break;
	default: Label = FText::FromString(TEXT("Unknown")); break;
	}
	return SNew(STextBlock).Text(Label);
}


#undef LOCTEXT_NAMESPACE
