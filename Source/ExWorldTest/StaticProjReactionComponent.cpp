// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticProjReactionComponent.h"
#include "Engine/DecalActor.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

void UStaticProjReactionComponent::ReactToProjectileHit(const FHitResult& hit)
{
	if (IsValid(DecalMaterial))
	{
		ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(hit.Location, FRotator());
		if (decal)
		{
			decal->SetDecalMaterial(DecalMaterial);
			decal->SetLifeSpan(2.0f);
			decal->GetDecal()->DecalSize = DecalSize;
		}
		//decal->AttachToActor(hit.GetActor(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
