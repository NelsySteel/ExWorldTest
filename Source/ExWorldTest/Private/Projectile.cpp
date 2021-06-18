// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "ExWorldTestCharacter.h"
#include "ProjectileReactionComponent.h"
#include "StaticProjReactionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

AProjectile::FCallbacksMap AProjectile::InitCallbacksMap()
{
	FCallbacksMap CallbacksMap;
	CallbacksMap.Add("Static", [](AActor* Actor, const FHitResult& HitData, FAdditionalClasses AdditionalClasses)
	{
		if (AdditionalClasses.Contains("Decal"))
		{
			FRotator RotatorToNormal = UKismetMathLibrary::MakeRotFromX(HitData.Normal);
			FVector HitLocation = HitData.Location;
			FRotator ImpactPointRotation = HitData.ImpactNormal.Rotation();
			ADecalActor* DecalImpostor = Actor->GetWorld()->SpawnActor<ADecalActor>(AdditionalClasses["Decal"]);
			DecalImpostor->SetHidden(true);

			UMaterialInterface* DefaultDecalMaterial = DecalImpostor->GetDecalMaterial();
			FVector DefaultDecalSize = DecalImpostor->GetDecal()->DecalSize;

			UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAttached(DefaultDecalMaterial, DefaultDecalSize, Actor->GetRootComponent(), "", HitLocation, ImpactPointRotation, EAttachLocation::KeepWorldPosition, 2.0f);
			decalComp->SetFadeScreenSize(0.0001);
			DecalImpostor->Destroy();
		}
		
	});
	CallbacksMap.Add("Character", [](AActor* Actor, const FHitResult& HitData, FAdditionalClasses AdditionalClasses)
	{
		if (AExWorldTestCharacter* Character = Cast<AExWorldTestCharacter>(Actor))
		{
			Character->ChangeHealth(-20);
			Character->OnBulletHit(HitData);
		}
	});
	CallbacksMap.Add("Destructible", [](AActor* Actor, const FHitResult& HitData, FAdditionalClasses AdditionalClasses)
	{
	});
	return CallbacksMap;
};

AProjectile::FCallbacksMap AProjectile::ShotCallbacks = InitCallbacksMap();

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	if (!IsValid(Mesh))
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/Sphere.Sphere'"));
		
		if (SphereMeshAsset.Succeeded())
		{
			Mesh = SphereMeshAsset.Object;
		}
	}
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComp->SetStaticMesh(Mesh);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(false);
	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	MeshComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	MeshComp->GetBodyInstance()->bOverrideMass = true;
	MeshComp->SetMassOverrideInKg(NAME_None, 100.f);

	MeshComp->OnComponentHit.AddDynamic(this, &AProjectile::OnCompHit);
	RootComponent = MeshComp;

	static ConstructorHelpers::FObjectFinder<UClass>SphereMeshAsset(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_DefaultDecal.BP_DefaultDecal_C'"));
	DefaultDecalClass = SphereMeshAsset.Object;
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
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector ProjectileDirection = GetActorForwardVector();;
	
	const FVector ShootDir = ProjectileDirection;
	FVector StartTrace = GetActorLocation();
	const FVector DestinationPoint = StartTrace + ShootDir * Speed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(DestinationPoint);
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
	if (UProjectileReactionComponent* ReactionComponent = Cast<UProjectileReactionComponent>(OtherActor->FindComponentByClass(UProjectileReactionComponent::StaticClass())))
	{
		ReactionComponent->ReactToProjectileHit(HitData);
	}
	else
	{
		FAdditionalClasses AdditionalClasses;
		AdditionalClasses.Add("Decal", DefaultDecalClass);
		for (const FName& tag : OtherActor->Tags)
		{
			if (ShotCallbacks.Contains(tag))
			{
				ShotCallbacks[tag](OtherActor, HitData, AdditionalClasses);
			}
		}
	}
}
