// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DecalActor.h"
#include "Engine/DataTable.h"
#include "Projectile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProjectile, Warning, All);


UENUM(Blueprintable)
enum EActorReactionType
{
	Static					UMETA(DisplayName = "Static"),
	Character				UMETA(DisplayName = "Character"),
	Destructible			UMETA(DisplayName = "Destructible"),
	Undefined				UMETA(DisplayName = "Undefined"),


	Count
};
ENUM_RANGE_BY_COUNT(EActorReactionType, EActorReactionType::Count)
USTRUCT(BlueprintType, Blueprintable)
struct FHitReactionInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FHitReactionInfo(TEnumAsByte<EActorReactionType> tag, const FHitResult& HitResult) : tag(tag), HitResult(HitResult) {}
	FHitReactionInfo() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EActorReactionType>		tag = Static;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult	HitResult = FHitResult();
};

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

	/**
 * Blueprint reaction on projectile hit
 * @param OtherActor Actor that was hit with projectile
 * @param HitResult Hit point info
 * @return True if hit was processed and no default processing is nessessary, False if default processing still needed
 */
	UFUNCTION(BlueprintImplementableEvent)
	bool CustomProcessProjectileHitByType(const FHitReactionInfo& HitReactionInfo);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	typedef TMap<TEnumAsByte<EActorReactionType>, TFunction<void(const FHitReactionInfo&)>> FCallbacksMap;
	FCallbacksMap ShotCallbacks;
	FCallbacksMap InitCallbacksMap();
	TArray<TEnumAsByte<EActorReactionType>> GetActorReactionTypes(AActor* Actor) const;
};
