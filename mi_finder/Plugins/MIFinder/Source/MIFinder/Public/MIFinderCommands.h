// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "MIFinderStyle.h"

class FMIFinderCommands : public TCommands<FMIFinderCommands>
{
public:

	FMIFinderCommands()
		: TCommands<FMIFinderCommands>(TEXT("MIFinder"), NSLOCTEXT("Contexts", "MIFinder", "MIFinder Plugin"), NAME_None, FMIFinderStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};