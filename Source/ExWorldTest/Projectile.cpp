// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "ExWorldTestCharacter.h"

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
}

void AProjectile::OnCompHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
	const FHitResult& hit)
{
	if (OtherActor != NULL && OtherActor != this && OtherComponent != NULL)
	{
		if (!hit.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NoActor!!"));
		}
		auto instigator = GetInstigator();
		if (AExWorldTestCharacter* InstigatorCharacter = Cast<AExWorldTestCharacter>(instigator))
		{
			InstigatorCharacter->OnProjectileHit(this, OtherActor, hit);
			Destroy();
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
	FVector StartTrace = GetActorLocation() + ShootDir * 10;
	const FVector DestinationPoint = GetActorLocation() + ShootDir * Speed * GetWorld()->GetDeltaSeconds();

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(FName(TEXT("CollisionTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, DestinationPoint, ECollisionChannel::ECC_Pawn, TraceParams);

	while (Hit.GetActor() == this)
	{
		StartTrace = StartTrace + ShootDir * 10;
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, DestinationPoint, ECollisionChannel::ECC_Pawn, TraceParams);
	}

	if (Hit.GetActor())
	{
		SetActorLocation(Hit.ImpactPoint - ProjectileDirection);
	}
	else
	{
		SetActorLocation(DestinationPoint);
	}
}

