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
	TSharedPtr<SVerticalBox> ResultInnerBox;
	FMIFinderQueryResult FinderResult;
};