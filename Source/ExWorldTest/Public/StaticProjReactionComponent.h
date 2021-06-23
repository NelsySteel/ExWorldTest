// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileReactionComponent.h"
#include "Projectile.h"
#include "StaticProjReactionComponent.generated.h"


class ADecalActor;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXWORLDTEST_API UStaticProjReactionComponent : public UProjectileReactionComponent
{
	GENERATED_BODY()
public:
	UStaticProjReactionComponent() {}

	UPROPERTY(EditAnywhere, Category = "Decal")
	UMaterial* DecalMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Decal")
	FVector    DecalSize = FVector(15.0f, 10.0f, 10.0f);

	virtual void ReactToProjectileHit(const FHitReactionInfo& HitInfo) override;
};
