// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileReactionComponent.h"
#include "PlayerProjReactionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXWORLDTEST_API UPlayerProjReactionComponent : public UProjectileReactionComponent
{
	GENERATED_BODY()
public:
	virtual void ReactToProjectileHit(const FHitResult& hit) override;
};
