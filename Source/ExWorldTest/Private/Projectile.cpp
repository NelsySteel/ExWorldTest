// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "ExWorldTestCharacter.h"
#include "ProjectileReactionComponent.h"
#include "StaticProjReactionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

DEFINE_LOG_CATEGORY(LogProjectile);

AProjectile::FCallbacksMap AProjectile::InitCallbacksMap()
{
	FCallbacksMap CallbacksMap;
	CallbacksMap.Add("Static", [this](AActor* Actor, const FHitResult& HitData)
	{
		if (IsValid(Actor))
		{
			if (IsValid(DecalClass))
			{
				FRotator RotatorToNormal = UKismetMathLibrary::MakeRotFromX(HitData.Normal);
				FVector HitLocation = HitData.Location;
				FRotator ImpactPointRotation = HitData.ImpactNormal.Rotation();
				ADecalActor* DecalImpostor = Actor->GetWorld()->SpawnActor<ADecalActor>(DecalClass);
				DecalImpostor->SetHidden(true);

				UMaterialInterface* DefaultDecalMaterial = DecalImpostor->GetDecalMaterial();
				FVector DefaultDecalSize = DecalImpostor->GetDecal()->DecalSize;

				UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DefaultDecalMaterial, DefaultDecalSize, Actor->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
				if (decalComp)
				{
					decalComp->SetFadeScreenSize(0.0001);
				}
				else
				{
					UE_LOG(LogProjectile, Warning, TEXT("Decal Component couldn't be created"));
				}
				DecalImpostor->Destroy();
			}
			else
			{
				UE_LOG(LogProjectile, Warning, TEXT("Decal creation failed : no decal class!"));
			}
		}
	});
	CallbacksMap.Add("Character", [](AActor* Actor, const FHitResult& HitData)
	{
		if (AExWorldTestCharacter* Character = Cast<AExWorldTestCharacter>(Actor))
		{
			Character->ChangeHealth(-20);
			Character->OnBulletHit(HitData);
		}
	});
	CallbacksMap.Add("Destructible", [](AActor* Actor, const FHitResult& HitData)
	{
	});
	return CallbacksMap;
};

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComp->InitialSpeed = 100;
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(false);
	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	MeshComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	MeshComp->OnComponentHit.AddDynamic(this, &AProjectile::OnCompHit);
	RootComponent = MeshComp;
}

void AProjectile::OnConstruction(const FTransform& Transform)
{
	AActor::OnConstruction(Transform);
}

void AProjectile::OnCompHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
	const FHitResult& HitData)
{
	if (OtherActor != NULL && OtherActor != this && OtherComponent != NULL)
	{
		if (!HitData.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NoActor!!"));
		}
		if (HasAuthority())
		{
			ServerDestroyProjectile(OtherActor, HitData);
		}
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovementComp->Velocity = GetInstigator()->GetActorForwardVector() * ProjectileMovementComp->InitialSpeed;
	ShotCallbacks = InitCallbacksMap();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AProjectile::ServerDestroyProjectile_Implementation(AActor* OtherActor, const FHitResult& HitData)
{
	MulticastDestroyProjectile(OtherActor, HitData);
	Destroy();
}

bool AProjectile::ServerDestroyProjectile_Validate(AActor* OtherActor, const FHitResult& HitData)
{
	return true;
}

void AProjectile::MulticastDestroyProjectile_Implementation(AActor* OtherActor, const FHitResult& HitData)
{
	if (OtherActor != nullptr)
	{
		if (UProjectileReactionComponent* ReactionComponent = Cast<UProjectileReactionComponent>(OtherActor->FindComponentByClass(UProjectileReactionComponent::StaticClass())))
		{
			ReactionComponent->ReactToProjectileHit(HitData);
		}
		else
		{
			for (const FName& tag : OtherActor->Tags)
			{
				if (ShotCallbacks.Contains(tag))
				{
					ShotCallbacks[tag](OtherActor, HitData);
				}
			}
		}
	}
}
