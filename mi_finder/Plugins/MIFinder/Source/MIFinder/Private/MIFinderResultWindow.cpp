// Fill out your copyright notice in the Description page of Project Settings.


#include "MIFinderResultWindow.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSeparator.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"

#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "WidgetLayoutParam.h"

void SMIFinderResultWindow::Construct(const FArguments& InArgs)
{
	FinderResult = InArgs._FinderResult;
	ChildSlot
	[
		
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SHorizontalBox)
			// クリップボードコピー
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(NSLOCTEXT("MIFinder", "CopyPathsButton", "Copy Paths"))
				.OnClicked(this, &SMIFinderResultWindow::OnCopyPathsClicked)
			]
			// JSON 保存
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(4.f, 0.f))
			[
				SNew(SButton)
				.Text(NSLOCTEXT("MIFinder", "SaveJsonButton", "Save JSON"))
				.OnClicked(this, &SMIFinderResultWindow::OnSaveJsonClicked)
			]
		]
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

FReply SMIFinderResultWindow::OnCopyPathsClicked()
{
	FString ClipboardText;
	for (const FString& Path : FinderResult.Results)
	{
		ClipboardText += Path;
		ClipboardText += LINE_TERMINATOR;   // 改行区切り
	}
	FPlatformApplicationMisc::ClipboardCopy(*ClipboardText);

	return FReply::Handled();
}

FReply SMIFinderResultWindow::OnSaveJsonClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return FReply::Unhandled();
	}

	// 保存ダイアログ初期値
	const FString DefaultPath      = FPaths::ProjectSavedDir();           // Saved フォルダ
	const FString DefaultFileName  = FString::Printf(
		TEXT("MIFinderResult_%s.json"),
		*FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const FString FileTypes        = TEXT("JSON file (*.json)|*.json");

	// ファイル選択
	TArray<FString> OutFiles;
	const bool bFileChosen = DesktopPlatform->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		NSLOCTEXT("MIFinder", "SaveDialogTitle", "Save Finder Result").ToString(),
		DefaultPath,
		DefaultFileName,
		FileTypes,
		EFileDialogFlags::None,
		OutFiles);

	if (bFileChosen && OutFiles.Num() > 0 && ExportJson(OutFiles[0]))
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

bool SMIFinderResultWindow::ExportJson(const FString& FilePath) const
{
	return FFileHelper::SaveStringToFile(FinderResult.ToJson(), *FilePath);
}