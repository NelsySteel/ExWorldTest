// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticProjReactionComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

UStaticProjReactionComponent::UStaticProjReactionComponent() : UProjectileReactionComponent()
{
	static ConstructorHelpers::FObjectFinder<UClass>SphereMeshAsset(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_DefaultDecal.BP_DefaultDecal_C'"));
	DefaultDecalClass = SphereMeshAsset.Object;
}

void UStaticProjReactionComponent::ReactToProjectileHit(const FHitReactionInfo& HitInfo)
{
	FHitResult HitResult = HitInfo.HitResult;
	FRotator RotatorToNormal = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
	FVector HitLocation = HitResult.Location;
	FRotator ImpactPointRotation = HitResult.ImpactNormal.Rotation();
	if (!IsValid(DecalMaterial))
	{
		// Happens when projectile hits into the instigator
		if (!HitResult.GetActor())
		{
			return;
		}
		ADecalActor* DecalImpostor = GetWorld()->SpawnActor<ADecalActor>(DefaultDecalClass);
		DecalImpostor->SetHidden(true);

		UMaterialInterface* DefaultDecalMaterial = DecalImpostor->GetDecalMaterial();
		FVector DefaultDecalSize = DecalImpostor->GetDecal()->DecalSize;

		UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DefaultDecalMaterial, DefaultDecalSize, HitResult.GetActor()->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
		decalComp->SetFadeScreenSize(0.0001);
		DecalImpostor->Destroy();
	}
	else
	{
		UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DecalMaterial, DecalSize, HitResult.GetActor()->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
		decalComp->SetFadeScreenSize(0.0001);
	}
}