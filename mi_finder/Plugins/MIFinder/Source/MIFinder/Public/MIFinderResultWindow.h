// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MIFinder.h"                // FMIFinderQueryResult
#include "Widgets/SCompoundWidget.h"

class SMIFinderResultWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMIFinderResultWindow) {}
	SLATE_ARGUMENT(FMIFinderQueryResult, FinderResult)  
SLATE_END_ARGS()
	
void Construct(const FArguments& InArgs);

private:
	FReply OnCopyPathsClicked();
	/** 検索結果を JSON ファイルへ保存 */
	FReply OnSaveJsonClicked();
	/** JSON 文字列を書き出して保存出来たら true */
	bool ExportJson(const FString& FilePath) const;
	
private:
	TSharedPtr<SVerticalBox> ResultInnerBox;
	FMIFinderQueryResult FinderResult;
};