// Fill out your copyright notice in the Description page of Project Settings.


#include "MIFinderResultWindow.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "WidgetLayoutParam.h"

void SMIFinderResultWindow::Construct(const FArguments& InArgs)
{
	FinderResult = InArgs._FinderResult;

	ChildSlot
	[
		SNew(SVerticalBox)
		// ─── SearchRoot 表示 ──────────────────────────────
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(FText::Format(
				NSLOCTEXT("MIFinder", "SearchRootLabel", "Search Root : {0}"),
				FText::FromString(FinderResult.SearchRoot)))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SSeparator)
			.SeparatorImage(FAppStyle::GetBrush("Menu.Separator"))
			.Orientation(Orient_Horizontal)
		]
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.AutoHeight()
		//.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::ParameterTextFontSize))
			.Text(FText::Format(NSLOCTEXT("ResultWindow","ResultText", "Search Result : {0}"),
				FText::FromString(FString::FromInt(FinderResult.Results.Num()))))
		]
		// ─── 結果一覧（スクロール）──────────────────────
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(4.f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SAssignNew(ResultInnerBox, SVerticalBox)
			]
		]
	];
	for (const FString& PathStr : FinderResult.Results)
	{
		ResultInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(UMaterialInstance::StaticClass())
			.ObjectPath(PathStr)
			.EnableContentPicker(true)
		];
	}
}
