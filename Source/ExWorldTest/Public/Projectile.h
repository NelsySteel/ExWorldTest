// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;

UCLASS(Blueprintable, BlueprintType)
class EXWORLDTEST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, Category = "Moving")
	float Speed = 1000.f;

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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	typedef TMap<FString, UClass*> FAdditionalClasses;
	typedef TMap<FName, TFunction<void(AActor*, const FHitResult&, FAdditionalClasses)>> FCallbacksMap;
	static FCallbacksMap ShotCallbacks;
	static FCallbacksMap InitCallbacksMap();

	UClass* DefaultDecalClass = nullptr;
};
