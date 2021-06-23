// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticProjReactionComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

void UStaticProjReactionComponent::ReactToProjectileHit(const FHitReactionInfo& HitInfo)
{
	FHitResult HitResult = HitInfo.HitResult;
	FRotator RotatorToNormal = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
	FVector HitLocation = HitResult.Location;
	FRotator ImpactPointRotation = HitResult.ImpactNormal.Rotation();
	if (!IsValid(DecalMaterial))
	{
		UE_LOG(LogProjectile, Warning, TEXT("Decal creation failed : no decal class!"));
	}
	else
	{
		UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DecalMaterial, DecalSize, HitResult.GetActor()->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
		decalComp->SetFadeScreenSize(0.0001);
	}
}