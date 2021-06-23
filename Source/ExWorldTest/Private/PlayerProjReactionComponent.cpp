// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjReactionComponent.h"
#include "ExWorldTestCharacter.h"
#include "Math/UnrealMathUtility.h"

void UPlayerProjReactionComponent::ReactToProjectileHit(const FHitReactionInfo& HitInfo)
{
	if (AExWorldTestCharacter* Character = Cast<AExWorldTestCharacter>(GetOwner()))
	{
		Character->ChangeHealth(-20);
		Character->OnBulletHit(HitInfo.HitResult);
	}
}