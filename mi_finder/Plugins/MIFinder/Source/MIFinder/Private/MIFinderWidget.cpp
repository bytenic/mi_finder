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
			.IsEnabled(this, &SStaticSwitchParameterWidget::IsEditingEnabled)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioCondition)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &SStaticSwitchParameterWidget::OnIsEqualQueryChanged)
			.IsChecked(this, &SStaticSwitchParameterWidget::IsEqualQuery)
			.IsEnabled(this, &SStaticSwitchParameterWidget::IsEditingEnabled)
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
	if (!WidgetData->TexturePathName.IsEmpty())
	{
		const FSoftObjectPath Path(WidgetData->TexturePathName);
		if (UTexture2D* LoadedTexture = Cast<UTexture2D>(Path.TryLoad()))
		{
			CurrentSelectTexture = LoadedTexture;
		}
		else
		{
			CurrentSelectTexture = nullptr;
		}
	}
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
			.IsEnabled(this, &STextureParameterWidget::IsEditingEnabled)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioTextureName)
		.HAlign(HAlign_Left)
		[
		SNew(SObjectPropertyEntryBox)
				.AllowedClass(UTexture2D::StaticClass())
				.IsEnabled(this, &STextureParameterWidget::IsEditingEnabled)
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
			.IsEnabled(this, &STextureParameterWidget::IsEditingEnabled)
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
                                                       EMaterialParameterAssociation Association, ScalarParameterQueryType QueryType, bool IsActive)
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

	QueryTypeOptions.Add(MakeShared<ScalarParameterQueryType>(MIFinderScalarParameterQueryTypeEqual));
	QueryTypeOptions.Add(MakeShared<ScalarParameterQueryType>(MIFinderScalarParameterQueryTypeLess));
	QueryTypeOptions.Add(MakeShared<ScalarParameterQueryType>(MIFinderScalarParameterQueryTypeGreater));
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
			.IsEnabled(this, &SScalarParameterWidget::IsEditingEnabled)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioFloatValue)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			.Value(this, &SScalarParameterWidget::GetValue)
			.OnValueChanged(this, &SScalarParameterWidget::OnValueChanged)
			.IsEnabled(this, &SScalarParameterWidget::IsEditingEnabled)
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
		.HAlign(HAlign_Left)
		[
			SNew(SComboBox<TSharedPtr<ScalarParameterQueryType>>)
			.OptionsSource(&QueryTypeOptions)
			.OnGenerateWidget(this, &SScalarParameterWidget::MakeComboItemWidget)
			.OnSelectionChanged(this, &SScalarParameterWidget::OnQueryTypeChanged)
			.InitiallySelectedItem(CurrentSelectQueryType)
			[
				SNew(STextBlock)
				.Text(this, &SScalarParameterWidget::GetSelectedQueryTypeText)
			]
			.IsEnabled(this, &SScalarParameterWidget::IsEditingEnabled)
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
	case MIFinderScalarParameterQueryTypeEqual: return FText::FromString(TEXT("Equal"));
	case MIFinderScalarParameterQueryTypeLess: return FText::FromString(TEXT("Less"));
	case MIFinderScalarParameterQueryTypeGreater: return FText::FromString(TEXT("Greater"));
	default: return FText::FromString(TEXT("Error"));
	}
}

void SScalarParameterWidget::OnQueryTypeChanged(TSharedPtr<int32> NewValue,
	ESelectInfo::Type SelectInfo)
{
	if (NewValue.IsValid())
	{
		CurrentSelectQueryType = NewValue;
		WidgetData->QueryType = *NewValue;
	}
}

TSharedRef<SWidget> SScalarParameterWidget::MakeComboItemWidget(TSharedPtr<ScalarParameterQueryType> InItem) const
{
	FText Label;

	switch (*InItem)
	{
	case MIFinderScalarParameterQueryTypeEqual: Label = FText::FromString(TEXT("Equal")); break;
	case MIFinderScalarParameterQueryTypeLess: Label = FText::FromString(TEXT("Less")); break;
	case MIFinderScalarParameterQueryTypeGreater: Label = FText::FromString(TEXT("Greater")); break;
	default: Label = FText::FromString(TEXT("Unknown")); break;
	}
	return SNew(STextBlock).Text(Label);
}


#undef LOCTEXT_NAMESPACE
