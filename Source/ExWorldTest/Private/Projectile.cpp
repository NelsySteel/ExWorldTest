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
	CallbacksMap.Add(EActorReactionType::Static, [this](const FHitReactionInfo& HitReactionInfo)
	{
		AActor* Actor = HitReactionInfo.HitResult.GetActor();
		if (IsValid(Actor))
		{
			if (IsValid(DecalClass))
			{
				FRotator RotatorToNormal = UKismetMathLibrary::MakeRotFromX(HitReactionInfo.HitResult.Normal);
				FVector HitLocation = HitReactionInfo.HitResult.Location;
				FRotator ImpactPointRotation = HitReactionInfo.HitResult.ImpactNormal.Rotation();
				ADecalActor* DecalImpostor = Actor->GetWorld()->SpawnActor<ADecalActor>(DecalClass);
				DecalImpostor->SetHidden(true);

				UMaterialInterface* DecalMaterial = DecalImpostor->GetDecalMaterial();
				FVector DecalSize = DecalImpostor->GetDecal()->DecalSize;

				UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DecalMaterial, DecalSize, Actor->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
				if (decalComp)
				{
					decalComp->SetFadeScreenSize(0.0001);
				}
				else
				{
					UE_LOG(LogProjectile, Warning, TEXT("Decal Component couldn't be created"));
					if (!IsValid(DecalMaterial))
					{
						UE_LOG(LogProjectile, Warning, TEXT("No Decal Material"));
					}
				}
				DecalImpostor->Destroy();
			}
			else
			{
				UE_LOG(LogProjectile, Warning, TEXT("Decal creation failed : no decal class!"));
			}
		}
	});
	CallbacksMap.Add(EActorReactionType::Character, [](const FHitReactionInfo& HitReactionInfo)
	{
		if (AExWorldTestCharacter* Character = Cast<AExWorldTestCharacter>(HitReactionInfo.HitResult.GetActor()))
		{
			Character->ChangeHealth(-20);
			Character->OnBulletHit(HitReactionInfo.HitResult);
		}
	});
	CallbacksMap.Add(EActorReactionType::Destructible, [](const FHitReactionInfo& HitReactionInfo)
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

TArray<TEnumAsByte<EActorReactionType>> AProjectile::GetActorReactionTypes(AActor* Actor) const
{
	TArray<TEnumAsByte<EActorReactionType>> result;
	for (const FName& Tag : Actor->Tags)
	{
		for (EActorReactionType Type : TEnumRange<EActorReactionType>())
		{
			if (FName(UEnum::GetValueAsString<EActorReactionType>(Type)) == Tag)
			{
				result.Add(Type);
				break;
			}
		}
	}
	return result;
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
			//Type doesn't matter here, components don't care
			ReactionComponent->ReactToProjectileHit(FHitReactionInfo(EActorReactionType::Undefined, HitData));
		}
		else
		{
			for (TEnumAsByte<EActorReactionType> Type : GetActorReactionTypes(OtherActor))
			{
				if (!CustomProcessProjectileHitByType(FHitReactionInfo(Type, HitData)))
				{
					if (ShotCallbacks.Contains(Type))
					{
						ShotCallbacks[Type](FHitReactionInfo(Type, HitData));
					}
				}
			}
		}
	}
}
