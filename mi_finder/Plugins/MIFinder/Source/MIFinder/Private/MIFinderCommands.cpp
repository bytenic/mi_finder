// Copyright Epic Games, Inc. All Rights Reserved.

#include "MIFinderCommands.h"

#define LOCTEXT_NAMESPACE "FMIFinderModule"

void FMIFinderCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MIFinder", "Bring up MIFinder window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
