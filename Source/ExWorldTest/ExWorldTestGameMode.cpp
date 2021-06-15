// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExWorldTestGameMode.h"
#include "ExWorldTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExWorldTestGameMode::AExWorldTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
