// Fill out your copyright notice in the Description page of Project Settings.


#include "eVanParticleModuleCircle.h"
#include "Runtime/Engine/Public/ParticleEmitterInstances.h"
#include "Runtime/Engine/Classes/Particles/ParticleLODLevel.h"
#include "Runtime/Engine/Classes/Particles/Spawn/ParticleModuleSpawn.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Distributions/DistributionFloatUniformCurve.h"
#include "Runtime/Engine/Classes/Distributions/DistributionFloatUniform.h"
#include "Runtime/Engine/Classes/Particles/Velocity/ParticleModuleVelocityOverLifetime.h"
#include "Runtime/Engine/Public/ParticleHelper.h"

ParticleCircleSpawnInstance::ParticleCircleSpawnInstance(FParticleEmitterInstance* owner, float startAngle, float endAngle)
	: SectorMode(false)
	, StartAngle(startAngle)
	, EndAngle(endAngle)
	, AngleIndex(0)
	, NumAngles(-1)
	, UseVelocity(false)
	, UseRotateAnim(false)
	, VelocityScale(nullptr)
	, RotateSpeed(nullptr)
	, RandomStream(GetTypeHash(owner->Component->GetName()))
	, OwnerEmitterInst(owner)
#if WITH_EDITOR
	, PrevMaxRate(0.0f)
	, PrevMaxRateScale(0.0f)
	, PrevMaxBurstScale(0.0f)
#endif
{
#if WITH_EDITOR
	RecalculateIfNeed();
#else
	Recalculate();
#endif

}

void ParticleCircleSpawnInstance::Recalculate()
{
	if (OwnerEmitterInst != nullptr && OwnerEmitterInst->CurrentLODLevel != nullptr && OwnerEmitterInst->CurrentLODLevel->SpawnModule != nullptr)
	{
		if (OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList.Num() > 0)
		{
			float min = 0.0f;
			float maxBurstScale = 0.0f;
			OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstScale.GetOutRange(min, maxBurstScale);

			NumAngles = 0;
			float curBurstCount = 0;
			for (int burstIndex = 0; burstIndex < OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList.Num(); burstIndex++)
			{
				if (OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[burstIndex].CountLow > -1)
				{
					curBurstCount = FMath::Min<int32>(OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[burstIndex].Count,
						OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[burstIndex].CountLow);
				}
				else
				{
					curBurstCount = OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[burstIndex].Count;
				}
				NumAngles += FMath::RoundToInt(curBurstCount * maxBurstScale);
			}
			if (NumAngles == 0)
			{
				NumAngles = -1;
			}
		}
		else
		{
			NumAngles = -1;
		}
	}
}

float ParticleCircleSpawnInstance::Get()
{
#if WITH_EDITOR
	RecalculateIfNeed();
#endif

	if (NumAngles != -1)
	{
		if (AngleIndex >= NumAngles)
		{
			AngleIndex = 0;
		}

		float rotDir = 1.0f;
		float RotateAngle = 0.0f;
		float lessAngle = 0.0f;

		if (SectorMode)
		{
			if (StartAngle > EndAngle)
			{
				RotateAngle = StartAngle - EndAngle;
				lessAngle = EndAngle;
			}
			else
			{
				RotateAngle = EndAngle - StartAngle;
				lessAngle = StartAngle;
			}

			if (NumAngles == 1)
			{
				return (StartAngle + EndAngle) * 0.5f;
			}

			return lessAngle + (static_cast<float>(AngleIndex++)* rotDir* RotateAngle* (1.0f / static_cast<float>(NumAngles - 1)));
		}
		else
		{
			if (NumAngles <= 1)
			{
				return 0.0f;
			}

			float splitedAngle = 360.0f * (1.0f / static_cast<float>(NumAngles));
			return static_cast<float>(AngleIndex++)* splitedAngle;
		}
	}

	return FMath::RandRange(FMath::Min(StartAngle, EndAngle), FMath::Max(StartAngle, EndAngle));
}

FVector ParticleCircleSpawnInstance::GetExternalModuleVelocity()
{
	FVector ExternalValocity = FVector::OneVector;
	return FVector::OneVector;
	/*if (VelocityOverLifeTime != nullptr)
	{
		if (VelocityOverLifeTime->Absolute)
		{
			FVector OwnerScale(1.0f);
			if ((bApplyOwnerScale == true) && Owner && Owner->Component)
			{
				OwnerScale = Owner->Component->GetComponentTransform().GetScale3D();
			}
			FVector Vel = VelOverLife.GetValue(Particle.RelativeTime, Owner->Component) * OwnerScale;
			Particle.Velocity = Vel;
			Particle.BaseVelocity = Vel;
		}
	}*/

	// = Particle.Velocity * -DragCoefficientRaw.GetValue(Particle.RelativeTime, Owner->Component);
//	Particle.Velocity += Drag * DeltaTime;
//	Particle.BaseVelocity += Drag * DeltaTime;

	//OwnerEmitterInst->CurrentLODLevel->SpawnModules.FindItemByClass(&VelocityOverLifeTime, nullptr, 0);
	//OwnerEmitterInst->CurrentLODLevel->UpdateModules.FindItemByClass(&AccelerationDrag, nullptr, 0);
}

void	ParticleCircleSpawnInstance::SetSectorMode(bool sectorMode)
{
	SectorMode = sectorMode;
}

void	ParticleCircleSpawnInstance::SetStartAngle(float startAngle)
{
	StartAngle = startAngle;
}

void	ParticleCircleSpawnInstance::SetEndAngle(float endAngle)
{
	EndAngle = endAngle;
}

void	ParticleCircleSpawnInstance::SetRotateSpeed(FRawDistributionFloat* rotateSpeed)
{
	RotateSpeed = rotateSpeed;
}

void	ParticleCircleSpawnInstance::SetUseVelocity(bool useVelocity)
{
	UseVelocity = useVelocity;
}

void	ParticleCircleSpawnInstance::SetVelocityScale(FRawDistributionFloat* velocityScale)
{
	VelocityScale = velocityScale;
}

void	ParticleCircleSpawnInstance::SetUseRotateAnim(bool useRotateAnim)
{
	UseRotateAnim = useRotateAnim;
}

void ParticleCircleSpawnInstance::Update(uint32 Offset, float deltaTime)
{
	if (OwnerEmitterInst != nullptr && UseRotateAnim)
	{
		FVector curEmitterLocation = OwnerEmitterInst->Component->GetRelativeTransform().GetLocation();
		FParticleEmitterInstance* Owner = OwnerEmitterInst;

		UDistributionFloatUniformCurve* velocityUniformCurve = Cast<UDistributionFloatUniformCurve>(VelocityScale->Distribution);
		UDistributionFloatUniform* velocityUniform = Cast<UDistributionFloatUniform>(VelocityScale->Distribution);
		UDistributionFloatUniformCurve* rotateUniformCurve = Cast<UDistributionFloatUniformCurve>(RotateSpeed->Distribution);
		UDistributionFloatUniform* rotateUniform = Cast<UDistributionFloatUniform>(RotateSpeed->Distribution);

		UParticleModuleVelocityOverLifetime* velocityOverLifeTime = nullptr;
		UParticleModuleAccelerationDrag* accelerationDrag = nullptr;
		OwnerEmitterInst->CurrentLODLevel->SpawnModules.FindItemByClass(&velocityOverLifeTime, nullptr, 0);
		OwnerEmitterInst->CurrentLODLevel->UpdateModules.FindItemByClass(&accelerationDrag, nullptr, 0);

		BEGIN_UPDATE_LOOP;
		{
			PARTICLE_ELEMENT(FParticleCircleModuleInstancePayload, circlePayload);

			float rotateSpeed = 0.0f;
			if (accelerationDrag != nullptr && accelerationDrag->DragCoefficientRaw.Distribution != nullptr)
			{
				circlePayload.DragVelocity += deltaTime * accelerationDrag->DragCoefficientRaw.GetValue(Particle.RelativeTime, Owner->Component);
			}

			FVector velocityOverLifeTimeModuleVelocityScale = FVector::OneVector;
			if (velocityOverLifeTime != nullptr && velocityOverLifeTime->VelOverLife.Distribution != nullptr)
			{
				velocityOverLifeTimeModuleVelocityScale = velocityOverLifeTime->VelOverLife.GetValue(Particle.RelativeTime, Owner->Component);
			}

			if (rotateUniformCurve != nullptr || rotateUniform != nullptr)
			{
				if (rotateUniformCurve != nullptr)
				{
					rotateSpeed = rotateUniformCurve->GetValue(Particle.RelativeTime, OwnerEmitterInst->Component, &RandomStream);
				}
				else
				{
					rotateSpeed = circlePayload.UniformRotateSpeed;
				}
			}
			else
			{
				rotateSpeed = RotateSpeed->GetValue(Particle.RelativeTime, OwnerEmitterInst->Component);
			}

			if (accelerationDrag != nullptr)
			{
				rotateSpeed = rotateSpeed - circlePayload.DragVelocity;
				rotateSpeed = FMath::Max(rotateSpeed, 0.0f);
			}

			float rotateAmount = deltaTime * rotateSpeed * DEFAULT_SPEED_SCALE;

			FVector centerLocation = FVector::ZeroVector;
			if (!OwnerEmitterInst->UseLocalSpace())
			{
				centerLocation = circlePayload.Center;
			}

			FVector prevLocation = Particle.Location - centerLocation;
			FVector pos = prevLocation;
			FVector dir = pos;
			dir.Normalize();
			if (UseVelocity)
			{
				FVector velocityScale = FVector::OneVector;

				if (velocityUniformCurve != nullptr || velocityUniform != nullptr)
				{
					if (velocityUniformCurve != nullptr)
					{
						velocityScale = VelocityScale->GetValue(Particle.RelativeTime, OwnerEmitterInst->Component, &RandomStream) * FVector::OneVector;
					}
					else
					{
						velocityScale = circlePayload.UniformVelocityScale;
					}
				}
				else
				{
					velocityScale = VelocityScale->GetValue(Particle.RelativeTime, OwnerEmitterInst->Component) * FVector::OneVector;
				}

				if (accelerationDrag != nullptr)
				{
					velocityScale = velocityScale - circlePayload.DragVelocity;
				}
				if (velocityOverLifeTime != nullptr)
				{
					velocityScale *= velocityOverLifeTimeModuleVelocityScale;
				}

				velocityScale.X = FMath::Max(velocityScale.X, 0.0f);
				velocityScale.Y = FMath::Max(velocityScale.Y, 0.0f);
				velocityScale.Z = FMath::Max(velocityScale.Z, 0.0f);

				pos = pos + dir * velocityScale * deltaTime * DEFAULT_SPEED_SCALE;
			}

			FVector rotatedLocation = pos.RotateAngleAxis(rotateAmount, FVector::UpVector);
			Particle.Location = rotatedLocation + centerLocation;
			FVector velocityDir = rotatedLocation - prevLocation;
			velocityDir.Normalize();

			FVector CurrentVelocity = velocityDir;
			Particle.BaseVelocity = CurrentVelocity;
			Particle.Velocity = CurrentVelocity;
		}
		END_UPDATE_LOOP

			if (curEmitterLocation != PrevEmitterLocation)
			{
				PrevEmitterLocation = curEmitterLocation;
			}
	}
}

#if WITH_EDITOR
void ParticleCircleSpawnInstance::RecalculateIfNeed()
{
	if (OwnerEmitterInst != nullptr && OwnerEmitterInst->CurrentLODLevel != nullptr && OwnerEmitterInst->CurrentLODLevel->SpawnModule != nullptr)
	{
		float minDummy = 0.0f;
		float MaxRate = 0.0f;
		float MaxRateScale = 0.0f;
		float MaxBurstScale = 0.0f;

		OwnerEmitterInst->CurrentLODLevel->SpawnModule->Rate.GetOutRange(minDummy, MaxRate);
		OwnerEmitterInst->CurrentLODLevel->SpawnModule->RateScale.GetOutRange(minDummy, MaxRateScale);
		OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstScale.GetOutRange(minDummy, MaxBurstScale);

		bool isChanged = false;
		float curBurstCount = 0.0f;
		if (OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList.Num() == PrevPrevBurstCounts.Num())
		{
			for (int diffIdx = 0; diffIdx < OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList.Num(); diffIdx++)
			{
				curBurstCount = FMath::Min<int32>(OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[diffIdx].Count,
					OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[diffIdx].CountLow);

				if (curBurstCount != PrevPrevBurstCounts[diffIdx])
				{
					isChanged = true;
					break;
				}
			}
		}

		isChanged = isChanged ? isChanged : MaxRate != PrevMaxRate;
		isChanged = isChanged ? isChanged : MaxRateScale != PrevMaxRateScale;
		isChanged = isChanged ? isChanged : MaxBurstScale != PrevMaxBurstScale;

		if (isChanged)
		{
			PrevMaxRate = MaxRate;
			PrevMaxRateScale = MaxRateScale;
			PrevMaxBurstScale = MaxBurstScale;

			PrevPrevBurstCounts.Empty();
			for (int diffIdx = 0; diffIdx < OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList.Num(); diffIdx++)
			{
				curBurstCount = FMath::Min<int32>(OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[diffIdx].Count,
					OwnerEmitterInst->CurrentLODLevel->SpawnModule->BurstList[diffIdx].CountLow);

				PrevPrevBurstCounts.Add(curBurstCount);
			}

			Recalculate();
		}
	}
}
#endif

UeVanParticleModuleCircle::UeVanParticleModuleCircle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Radius(30.0f)
	, SectorMode(false)
	, StartAngle(0.0f)
	, EndAngle(360.0f)
	, UseVelocity(false)
	, UseRotateAnim(false)
	, RandomStream(GetTypeHash(GetName()))
{

	bSpawnModule = true;
	bFinalUpdateModule = true;
#if WITH_EDITOR
	bSupported3DDrawMode = true;
#endif

	OnWorldCleanupDelegateHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &UeVanParticleModuleCircle::OnWorldCleanup);
}

UeVanParticleModuleCircle::~UeVanParticleModuleCircle()
{
	FWorldDelegates::OnWorldCleanup.Remove(OnWorldCleanupDelegateHandle);
}

void UeVanParticleModuleCircle::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
	DestroyWaitInstances();

	ParticleCircleSpawnInstance* particleSpawnInst = nullptr;
	UParticleModuleAccelerationDrag* accelerationDrag = nullptr;
	if (Owner != nullptr && Owner->CurrentLODLevel != nullptr)
	{
		Owner->CurrentLODLevel->UpdateModules.FindItemByClass(&accelerationDrag, nullptr, 0);

		if (CircleSpawnInstances.Contains(Owner))
		{
			particleSpawnInst = CircleSpawnInstances[Owner];
		}
		else
		{
			particleSpawnInst = new ParticleCircleSpawnInstance(Owner, StartAngle, EndAngle);
			particleSpawnInst->SetUseRotateAnim(UseRotateAnim);
			particleSpawnInst->SetRotateSpeed(&RotateSpeed);
			particleSpawnInst->SetUseVelocity(UseVelocity);
			particleSpawnInst->SetVelocityScale(&VelocityScale);
			particleSpawnInst->SetSectorMode(SectorMode);

			CircleSpawnInstances.Add(Owner, particleSpawnInst);
			Owner->OnParticleEmitterInstanceDestroy.AddUObject(this, &UeVanParticleModuleCircle::OnEmitterInstanceDestroy);
		}
		SPAWN_INIT;
		{
			float currentAngle = particleSpawnInst->Get();
			FVector spawnLocation = FVector::ForwardVector * Radius;
			FVector rotatedLocation = spawnLocation.RotateAngleAxis(currentAngle, FVector::UpVector);
			if (Owner->UseLocalSpace())
			{
				Particle.Location = rotatedLocation;
			}
			else
			{
				Particle.Location = rotatedLocation + Owner->Location;
			}

			PARTICLE_ELEMENT(FParticleCircleModuleInstancePayload, circlePayload);
			circlePayload.Center = Owner->Location;
			if (UseVelocity && !UseRotateAnim)
			{
				float velocityScale = 0.0f;
				if (VelocityScale.IsUniform())
				{
					velocityScale = VelocityScale.GetValue(0.0f, Owner->Component, &RandomStream);
				}
				else
				{
					velocityScale = VelocityScale.GetValue(0.0f, Owner->Component);
				}

				FVector dir = rotatedLocation;
				dir.Normalize();
				Particle.BaseVelocity = dir * velocityScale * DEFAULT_SPEED_SCALE;
				Particle.Velocity = dir * velocityScale * DEFAULT_SPEED_SCALE;
			}
			else
			{
				UDistributionFloatUniform* VelocityUniform = Cast<UDistributionFloatUniform>(VelocityScale.Distribution);
				UDistributionFloatUniformCurve* VelocityUniformCurve = Cast<UDistributionFloatUniformCurve>(VelocityScale.Distribution);

				if (VelocityUniform != nullptr && VelocityUniformCurve == nullptr)
				{
					UParticleModuleVelocityOverLifetime* velocityOverLifeTime = nullptr;
					Owner->CurrentLODLevel->SpawnModules.FindItemByClass(&velocityOverLifeTime, nullptr, 0);
					FVector externalModuleVelocityAmount = FVector::OneVector;
					if (velocityOverLifeTime != nullptr)
					{
						if (velocityOverLifeTime->Absolute)
						{

							FVector OwnerScale(1.0f);
							if ((velocityOverLifeTime->bApplyOwnerScale == true) && Owner && Owner->Component)
							{
								OwnerScale = Owner->Component->GetComponentTransform().GetScale3D();
							}
							externalModuleVelocityAmount = velocityOverLifeTime->VelOverLife.GetValue(Particle.RelativeTime, Owner->Component) * OwnerScale;
						}
					}

					circlePayload.UniformVelocityScale = VelocityScale.GetValue(Particle.RelativeTime, Owner->Component, &RandomStream) * externalModuleVelocityAmount;
				}

				UDistributionFloatUniformCurve* RotateUniformCurve = Cast<UDistributionFloatUniformCurve>(RotateSpeed.Distribution);
				UDistributionFloatUniform* RotateUniform = Cast<UDistributionFloatUniform>(RotateSpeed.Distribution);
				if (RotateUniform != nullptr && RotateUniformCurve == nullptr)
				{
					circlePayload.UniformRotateSpeed = RotateSpeed.GetValue(Particle.RelativeTime, Owner->Component, &RandomStream);
				}
			}
		}
	}
}

void UeVanParticleModuleCircle::FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
	DestroyWaitInstances();

	if (UseRotateAnim)
	{
		if (CircleSpawnInstances.Contains(Owner))
		{
			CircleSpawnInstances[Owner]->Update(Offset, DeltaTime);
		}
	}
}

#if WITH_EDITOR
void UeVanParticleModuleCircle::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_StartAngle = FName(TEXT("StartAngle"));
	static const FName NAME_EndAngle = FName(TEXT("EndAngle"));
	static const FName NAME_RotateAnim = FName(TEXT("UseRotateAnim"));
	static const FName NAME_RotateSpeed = FName(TEXT("RotateSpeed"));
	static const FName NAME_UseVelocity = FName(TEXT("UseVelocity"));
	static const FName NAME_VelocityScale = FName(TEXT("VelocityScale"));
	static const FName NAME_SectorMode = FName(TEXT("SectorMode"));


	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == NAME_StartAngle)
	{
		for (auto cur : CircleSpawnInstances)
		{
			cur.Value->SetStartAngle(StartAngle);
		}
	}
	else if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == NAME_EndAngle)
	{
		for (auto cur : CircleSpawnInstances)
		{
			cur.Value->SetEndAngle(StartAngle);
		}
	}
	else if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == NAME_UseVelocity)
	{
		for (auto cur : CircleSpawnInstances)
		{
			cur.Value->SetUseVelocity(UseVelocity);
		}
	}
	else if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == NAME_SectorMode)
	{
		for (auto cur : CircleSpawnInstances)
		{
			cur.Value->SetSectorMode(SectorMode);
		}
	}
}

void UeVanParticleModuleCircle::Render3DPreview(FParticleEmitterInstance* Owner, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (Owner != nullptr)
	{
		DrawWireStar(PDI, Owner->Location, 3.0f, FColor::Red, SDPG_World);
		DrawCircle(PDI, Owner->Location, FVector::ForwardVector, FVector::LeftVector, FColor::Red, Radius, 360, SDPG_World);

		if (SectorMode)
		{
			FVector RefPos = FVector::ForwardVector * Radius;

			FVector startAngleEndPos = RefPos.RotateAngleAxis(StartAngle, FVector::UpVector);
			FVector endAngleEndPos = RefPos.RotateAngleAxis(EndAngle, FVector::UpVector);

			PDI->DrawLine(Owner->Location, startAngleEndPos, FColor::Blue, SDPG_World);
			PDI->DrawLine(Owner->Location, endAngleEndPos, FColor::Green, SDPG_World);
		}
	}
}
#endif

uint32 UeVanParticleModuleCircle::RequiredBytes(UParticleModuleTypeDataBase* TypeData)
{
	return sizeof(FParticleCircleModuleInstancePayload);
}

void UeVanParticleModuleCircle::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	DestroyWaitInstances();
}

void UeVanParticleModuleCircle::OnEmitterInstanceDestroy(FParticleEmitterInstance* Owner)
{
	DestoryWaitQueue.Enqueue(Owner);

	if (Owner->Component != nullptr && Owner->Component->IsBeingDestroyed())
	{
		DestroyWaitInstances();
	}
}

void UeVanParticleModuleCircle::DestroyWaitInstances()
{
	FParticleEmitterInstance* curDeleteInstance = nullptr;
	while (DestoryWaitQueue.Dequeue(curDeleteInstance))
	{
		if (CircleSpawnInstances.Contains(curDeleteInstance))
		{
			ParticleCircleSpawnInstance* deleteTargetInstance = CircleSpawnInstances[curDeleteInstance];
			CircleSpawnInstances.Remove(curDeleteInstance);
			if (deleteTargetInstance != nullptr)
			{
				delete deleteTargetInstance;
			}
		}
	}
}