// Copyright Epic Games, Inc. All Rights Reserved.

#include "mi_finderGameMode.h"
#include "mi_finderCharacter.h"
#include "UObject/ConstructorHelpers.h"

Ami_finderGameMode::Ami_finderGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
