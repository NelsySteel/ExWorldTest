// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileReactionComponent.h"
#include "StaticProjReactionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EXWORLDTEST_API UStaticProjReactionComponent : public UProjectileReactionComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UMaterial* DecalMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	FVector    DecalSize = FVector(5.0f, 10.0f, 10.0f);

	virtual void ReactToProjectileHit(const FHitResult& hit) override;

private:
	FVector LocationOffset;
};
