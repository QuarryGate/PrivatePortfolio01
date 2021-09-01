// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "eVan_Engine/particle/eVanParticleModuleBase.h"
#include "Runtime/Engine/Classes/Distributions/DistributionFloat.h"

#include "Runtime/Engine/Classes/Particles/Acceleration/ParticleModuleAccelerationDrag.h"
#include "Runtime/Engine/Classes/Particles/Velocity/ParticleModuleVelocityOverLifetime.h"
#include "eVanParticleModuleCircle.generated.h"

#define DEFAULT_SPEED_SCALE 100.0f;

struct FParticleCircleModuleInstancePayload
{
	FVector	Center;
	FVector	UniformVelocityScale;
	float	UniformRotateSpeed;
	float	DragVelocity;
};

class ParticleCircleSpawnInstance
{
public:
	ParticleCircleSpawnInstance(FParticleEmitterInstance* Owner, float startAngle, float endAngle);

	void	Recalculate();
	float	Get();

	FVector GetExternalModuleVelocity();

	void	SetSectorMode(bool sectorMode);
	void	SetStartAngle(float startAngle);
	void	SetEndAngle(float endAngle);
	void	SetUseRotateAnim(bool useRotateAnim);
	void	SetRotateSpeed(struct FRawDistributionFloat* rotateSpeed);
	void	SetUseVelocity(bool useVelocity);
	void	SetVelocityScale(struct FRawDistributionFloat* velocityScale);

	void	Update(uint32 Offset, float deltaTime);

private:

	bool	SectorMode;

	float	StartAngle;
	float	EndAngle;

	int		AngleIndex;
	int		NumAngles;

	bool	UseVelocity;
	bool	UseRotateAnim;

	struct FRawDistributionFloat* VelocityScale;
	struct FRawDistributionFloat* RotateSpeed;

	FRandomStream RandomStream;
	FVector PrevEmitterLocation;

	FParticleEmitterInstance* OwnerEmitterInst;

#if WITH_EDITOR
public:
	void RecalculateIfNeed();
private:
	float	PrevMaxRate;
	float	PrevMaxRateScale;
	float	PrevMaxBurstScale;
	TArray<int> PrevPrevBurstCounts;
#endif
};

/**
 *
 */
UCLASS(editinlinenew, hidecategories = Object, meta = (DisplayName = "Circle"))
class EVAN_ENGINE_API UeVanParticleModuleCircle : public UeVanParticleModuleBase
{
	GENERATED_UCLASS_BODY()
		~UeVanParticleModuleCircle();
	virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
	virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void Render3DPreview(FParticleEmitterInstance* Owner, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
#endif
	virtual uint32 RequiredBytes(UParticleModuleTypeDataBase* TypeData) override;

	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void OnEmitterInstanceDestroy(FParticleEmitterInstance* Owner);
	void DestroyWaitInstances();


	UPROPERTY(EditAnywhere, Category = Circle, meta = (UIMin = "0", UIMax = "1000"))
		float Radius;
	UPROPERTY(EditAnywhere, Category = Circle)
		bool SectorMode;
	UPROPERTY(EditAnywhere, Category = Circle, meta = (UIMin = "-360", UIMax = "360", editcondition = "SectorMode"))
		float StartAngle;
	UPROPERTY(EditAnywhere, Category = Circle, meta = (UIMin = "-360", UIMax = "360", editcondition = "SectorMode"))
		float EndAngle;
	UPROPERTY(EditAnywhere, Category = Circle)
		bool UseVelocity;

	UPROPERTY(EditAnywhere, Category = Circle, meta = (editcondition = "UseVelocity"))
		struct FRawDistributionFloat VelocityScale;

	//UPROPERTY(EditAnywhere, Category = Circle, meta = (editcondition = "UseVelocity", UIMin = "1", UIMax = "1000"))
	//float VelocityScale;

	UPROPERTY(EditAnywhere, Category = Circle)
		bool UseRotateAnim;

	UPROPERTY(EditAnywhere, Category = Circle, meta = (editcondition = "UseRotateAnim"))
		struct FRawDistributionFloat RotateSpeed;
	//UPROPERTY(EditAnywhere, Category = Circle, meta = (editcondition = "UseRotateAnim"))
	//float RotateSpeed;

	FRandomStream RandomStream;

	TQueue<FParticleEmitterInstance*> DestoryWaitQueue;
	TMap<FParticleEmitterInstance*, ParticleCircleSpawnInstance*> CircleSpawnInstances;

	FDelegateHandle OnWorldCleanupDelegateHandle;

};

