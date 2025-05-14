// Fill out your copyright notice in the Description page of Project Settings.


#include "MIFinderWidget.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SNumericEntryBox.h"

#define LOCTEXT_NAMESPACE "FMIFinderModuleWindow"

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
		.Padding(LayoutPadding)
		.FillWidth(.25f)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(2.0f)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),FontSize))
			.Text(NSLOCTEXT("StaticSwitchParameterRegion","StaticSwitchParameterName", "WidgetName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(.25f)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		]
	];
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
		.Padding(LayoutPadding)
		.FillWidth(.2f)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(1.0f)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),FontSize))
			.Text(NSLOCTEXT("TextureParameterRegion","TextureParameterName", "ParameterName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(1.f)
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
		.Padding(LayoutPadding)
		.FillWidth(.1f)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		]
	];
}

void STextureParameterWidget::OnTextureChanged(const FAssetData& InAssetData)
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
		.Padding(LayoutPadding)
		.FillWidth(.25f)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(2.0f)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),FontSize))
			.Text(NSLOCTEXT("ScalarParameterRegion","StaticSwitchParameterName", "WidgetName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(2.0f)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			//.Value(this, &SPDD_TargetRow::GetScale)
			//.OnValueChanged(this, &SPDD_TargetRow::SetScale)
		]
		//+ SHorizontalBox::Slot()
		//.Padding(LayoutPadding)
		//.FillWidth(.25f)
		//.HAlign(HAlign_Left)
		//[
		//	SNew(SCheckBox)
		//]
	];
}

#undef LOCTEXT_NAMESPACE
