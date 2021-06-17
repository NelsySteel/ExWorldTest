// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjReactionComponent.h"
#include "ExWorldTestCharacter.h"
#include "Math/UnrealMathUtility.h"

void UPlayerProjReactionComponent::ReactToProjectileHit(const FHitResult& hit)
{
	if (AExWorldTestCharacter* Character = Cast<AExWorldTestCharacter>(GetOwner()))
	{
		Character->ChangeHealth(-20);
	}
}