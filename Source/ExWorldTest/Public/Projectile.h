// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DecalActor.h"
#include "Projectile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProjectile, Warning, All);

class USphereComponent;
class UProjectileMovementComponent;
UCLASS(Blueprintable, BlueprintType)
class EXWORLDTEST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(EditAnywhere, Category = "Shooting")
	TSubclassOf<ADecalActor> DecalClass = nullptr;

	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* HitComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
		const FHitResult& hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroyProjectile(AActor* OtherActor, const FHitResult& hit);
	void ServerDestroyProjectile_Implementation(AActor* OtherActor, const FHitResult& hit);
	bool ServerDestroyProjectile_Validate(AActor* OtherActor, const FHitResult& hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyProjectile(AActor* OtherActor, const FHitResult& hit);
	void MulticastDestroyProjectile_Implementation(AActor* OtherActor, const FHitResult& hit);

	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	typedef TMap<FName, TFunction<void(AActor*, const FHitResult&)>> FCallbacksMap;
	FCallbacksMap ShotCallbacks;
	FCallbacksMap InitCallbacksMap();
};
