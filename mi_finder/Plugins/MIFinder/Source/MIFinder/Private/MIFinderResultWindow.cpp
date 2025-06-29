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
		.Padding(4.f, 2.f)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::MaterialParameterTextFontSize))
			.Text(FText::Format(
				NSLOCTEXT("MIFinder", "SearchRootLabel", "Search Root : {0}"),
				FText::FromString(FinderResult.SearchRoot)))
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
