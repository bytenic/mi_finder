// Fill out your copyright notice in the Description page of Project Settings.


#include "MIFinderWidget.h"

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
		.FillWidth(.5f)
		.HAlign(HAlign_Left)
		[
		   SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(3.0f)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),FontSize))
			.Text(NSLOCTEXT("StaticSwitchParameterRegion","StaticSwitchParameterName", "WidgetName"))
		]
		+ SHorizontalBox::Slot()
		.Padding(LayoutPadding)
		.FillWidth(.5f)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		]
	];
}

#undef LOCTEXT_NAMESPACE