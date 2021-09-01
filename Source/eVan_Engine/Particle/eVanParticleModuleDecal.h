// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "eVan_Engine/particle/eVanParticleModuleBase.h"
#include "eVanParticleModuleBase.h"
#include "eVan_Engine/particle/eVanParticleDecalParam.h"
#include "Runtime/Engine/Classes/Engine/DecalActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "eVan_Engine/Actor/eVanParticleDecalActor.h"

#include "eVanParticleModuleDecal.generated.h"

class ParticleDecalInstance
{
public:

	ParticleDecalInstance(FParticleEmitterInstance* owner,
		UMaterialInterface* decalMaterial,
		int32 SortOrder,
		FVector DecalSize,
		FVector DecalOffset,
		FRotator DecalRotatio);
	~ParticleDecalInstance();

	void SpawnDeacalActor();
	void Update(float deltaTime);

	void SetActive(bool active);
	bool IsActive() { return Active; }

	FParticleEmitterInstance* GetOwnerEmitterInstance() { return OwnerEmitterInstance; }

private:

	bool Active;

	FParticleEmitterInstance* OwnerEmitterInstance;
	class UMaterialInterface* DecalMaterial;

	int32		SortOrder;
	FVector		DecalSize;
	FVector		DecalOffset;
	FRotator	DecalRotation;

	TArray<AeVanParticleDecalActor*>	SpawnedDecalActors;

	FDelegateHandle	OnEmitterInstanceDestroyDelegateHandle;
};

/**
 *
 */
UCLASS(editinlinenew, hidecategories = Object, meta = (DisplayName = "Decal"))
class EVAN_ENGINE_API UeVanParticleModuleDecal : public UeVanParticleModuleBase
{
	GENERATED_UCLASS_BODY()
		virtual ~UeVanParticleModuleDecal();
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		class UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal)
		int32 SortOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal, meta = (AllowPreserveRatio = "true"))
		FVector DecalSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal, meta = (AllowPreserveRatio = "true"))
		FVector DecalOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal, meta = (AllowPreserveRatio = "true"))
		FRotator DecalRotation;

	AActor* OwnerActor;

	TArray<AeVanParticleDecalActor*> SpawnedDecalActors;
	TSharedPtr<TMap<FParticleEmitterInstance*, TSharedPtr<ParticleDecalInstance, ESPMode::ThreadSafe>>, ESPMode::ThreadSafe> DecalInstancesPtr;

	FDelegateHandle OnWorldCleanupDelegateHandle;

public:

	virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
	virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
	void ClearInstances();

	virtual bool CanTickInAnyThread() override { return false; }
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void OnEmitterInstanceDesteoyed(FParticleEmitterInstance* Owner);
};


