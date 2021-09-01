// Fill out your copyright notice in the Description page of Project Settings.


#include "eVanParticleModuleDecal.h"

#include "eVan_Engine/particle/eVanParticleDecalParam.h"
#include "Engine/Public/ParticleEmitterInstances.h"
#include "Engine/Classes/Particles/ParticleLODLevel.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColorOverLife.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColor.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColorScaleOverLife.h"
#include "Runtime/Engine/Classes/Particles/Lifetime/ParticleModuleLifetime.h"
#include "Runtime/Engine/Classes/Particles/Rotation/ParticleModuleRotation.h"

ParticleDecalInstance::ParticleDecalInstance(FParticleEmitterInstance* owner,
	UMaterialInterface* decalMaterial,
	int32 sortOrder,
	FVector decalSize,
	FVector decalOffset,
	FRotator decalRotation)
	: Active(true)
	, OwnerEmitterInstance(owner)
	, DecalMaterial(decalMaterial)
	, SortOrder(sortOrder)
	, DecalSize(decalSize)
	, DecalOffset(decalOffset)
	, DecalRotation(decalRotation)
{
}

ParticleDecalInstance::~ParticleDecalInstance()
{
	for (int decalActorIndex = 0; decalActorIndex < SpawnedDecalActors.Num(); decalActorIndex++)
	{
		if (SpawnedDecalActors[decalActorIndex] != nullptr && !SpawnedDecalActors[decalActorIndex]->IsPendingKill())
		{
			SpawnedDecalActors[decalActorIndex]->Destroy();
			SpawnedDecalActors[decalActorIndex] = nullptr;
		}
	}
	SpawnedDecalActors.Empty();
}

void ParticleDecalInstance::SpawnDeacalActor()
{
	if (OwnerEmitterInstance != nullptr && OwnerEmitterInstance->Component != nullptr)
	{
		float LifeTime = 0.0f;

		UParticleModuleLifetime* lifeTimeModule;
		if (OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&lifeTimeModule, nullptr, 0) && lifeTimeModule != nullptr)
		{
			LifeTime = lifeTimeModule->Lifetime.GetValue(0.0f, nullptr);
		}

		if (LifeTime > 0.0f)
		{
			FActorSpawnParameters spawnParam;
			spawnParam.ObjectFlags = RF_Transient;

			FRotator rotation = FRotator::ZeroRotator;
			FVector location = FVector::ZeroVector;

			AeVanParticleDecalActor* DecalActor = Cast<AeVanParticleDecalActor>(OwnerEmitterInstance->GetWorld()->SpawnActor<AeVanParticleDecalActor>(spawnParam));
			if (DecalActor != nullptr)
			{
				SpawnedDecalActors.Add(DecalActor);

				AActor* ownerActor = OwnerEmitterInstance->Component->GetOwner();
				if (ownerActor != nullptr)
				{
					location = ownerActor->GetActorLocation();
					rotation = ownerActor->GetActorRotation();
				}

				if (OwnerEmitterInstance->Component != nullptr)
				{
					UDecalComponent* decalComp = DecalActor->GetDecal();
					if (decalComp != nullptr)
					{
						location += OwnerEmitterInstance->Component->GetRelativeLocation();
						rotation += OwnerEmitterInstance->Component->GetRelativeRotation();
					}
				}

				UParticleModuleRotation* rotationModule = nullptr;
				float zRotation = 0.0f;
				if (OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&rotationModule, nullptr, 0) && rotationModule != nullptr)
				{
					FRandomStream RandomStream(GetTypeHash(DecalActor->GetName()));
					zRotation = rotationModule->StartRotation.GetValue(0.0f, nullptr, &RandomStream);
				}

				static const float YAW_SCALE = 360.0f;
				rotation += DecalRotation;
				location += DecalOffset;
				rotation.Yaw += zRotation * YAW_SCALE;
				DecalActor->SetActorLocation(location);
				DecalActor->SetActorRotation(rotation);
				DecalActor->Initialize(SortOrder, LifeTime, DecalSize, DecalMaterial);

				FLinearColor color = FLinearColor::White;
				UParticleModuleColor* colorModule = nullptr;

				if (OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&colorModule, nullptr, 0) && colorModule != nullptr)
				{
					FRandomStream RandomStream(GetTypeHash(colorModule->GetName()));
					color = colorModule->StartColor.GetValue(0.0f, nullptr, 0, &RandomStream);
					color.A = colorModule->StartAlpha.GetValue(0.0f, nullptr, &RandomStream);
				}

				UParticleModuleColorOverLife* colorOverLifeModule = nullptr;
				if (OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&colorOverLifeModule, nullptr, 0) && colorOverLifeModule != nullptr)
				{
					color = colorOverLifeModule->ColorOverLife.GetValue(0.0f, OwnerEmitterInstance->Component);
					color.A = colorOverLifeModule->AlphaOverLife.GetValue(0.0f, OwnerEmitterInstance->Component);
				}

				UParticleModuleColorScaleOverLife* colorScaleOverLifeModule = nullptr;
				OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&colorScaleOverLifeModule, nullptr, 0);
				if (colorScaleOverLifeModule != nullptr)
				{
					color = colorScaleOverLifeModule->ColorScaleOverLife.GetValue(0.0f, OwnerEmitterInstance->Component);
					color.A = colorScaleOverLifeModule->AlphaScaleOverLife.GetValue(0.0f, OwnerEmitterInstance->Component);
				}
				DecalActor->SetColor(color);

				UeVanParticleDecalParam* decalShaderParam = nullptr;
				OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&decalShaderParam, nullptr, 0);

				if (decalShaderParam != nullptr)
				{
					for (int paramIndex = 0; paramIndex < decalShaderParam->DynamicParams.Num(); paramIndex++)
					{
						float param = decalShaderParam->DynamicParams[paramIndex].ParamValue.GetValue(0.0f, OwnerEmitterInstance->Component);
						DecalActor->SetShaderParam(decalShaderParam->DynamicParams[paramIndex].ParamName, param);
					}
				}
			}
		}
	}
}

void ParticleDecalInstance::Update(float deltaTime)
{
	if (Active && OwnerEmitterInstance != nullptr && OwnerEmitterInstance->CurrentLODLevel != nullptr)
	{
		UParticleModuleColorOverLife* colorOverLifeModule = nullptr;
		OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&colorOverLifeModule, nullptr, 0);

		UParticleModuleColorScaleOverLife* colorScaleOverLifeModule = nullptr;
		OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&colorScaleOverLifeModule, nullptr, 0);

		UeVanParticleDecalParam* decalShaderParam = nullptr;
		OwnerEmitterInstance->CurrentLODLevel->SpawnModules.FindItemByClass(&decalShaderParam, nullptr, 0);

		for (int decalActorIndex = SpawnedDecalActors.Num() - 1; decalActorIndex >= 0; decalActorIndex--)
		{
			if (SpawnedDecalActors[decalActorIndex] != nullptr && !SpawnedDecalActors[decalActorIndex]->IsPendingKill())
			{
				SpawnedDecalActors[decalActorIndex]->UpdateDecal(deltaTime);
				SpawnedDecalActors[decalActorIndex]->SetActorLocation(OwnerEmitterInstance->Component->GetRelativeLocation() + DecalOffset);

				//일단 업데이트에서 차후 visiblity 이벤트 콜백 심는게 더 효율적일듯
				bool ownerCompVisible = OwnerEmitterInstance->Component->IsVisible();
				UDecalComponent* decalComp = SpawnedDecalActors[decalActorIndex]->GetDecal();
				if (decalComp != nullptr)
				{
					if (decalComp->IsVisible() != ownerCompVisible)
					{
						decalComp->SetVisibility(ownerCompVisible);
					}
				}

				if (ownerCompVisible)
				{
					if (!SpawnedDecalActors[decalActorIndex]->IsPlaying())
					{
						SpawnedDecalActors[decalActorIndex]->Destroy();
						SpawnedDecalActors.RemoveAt(decalActorIndex);
						continue;
					}
					else
					{
						float currentDecalActorTimeLeft = SpawnedDecalActors[decalActorIndex]->GetPlayRate();

						FLinearColor color = FLinearColor::White;
						if (colorScaleOverLifeModule != nullptr)
						{
							color = colorScaleOverLifeModule->ColorScaleOverLife.GetValue(currentDecalActorTimeLeft, OwnerEmitterInstance->Component);
							color.A = colorScaleOverLifeModule->AlphaScaleOverLife.GetValue(currentDecalActorTimeLeft, OwnerEmitterInstance->Component);

							SpawnedDecalActors[decalActorIndex]->SetColor(color);
						}
						else
						{
							if (colorOverLifeModule != nullptr)
							{
								color = colorOverLifeModule->ColorOverLife.GetValue(currentDecalActorTimeLeft, OwnerEmitterInstance->Component);
								color.A = colorOverLifeModule->AlphaOverLife.GetValue(currentDecalActorTimeLeft, OwnerEmitterInstance->Component);

								SpawnedDecalActors[decalActorIndex]->SetColor(color);
							}
						}

						if (decalShaderParam != nullptr)
						{
							for (int paramIndex = 0; paramIndex < decalShaderParam->DynamicParams.Num(); paramIndex++)
							{
								if (!decalShaderParam->DynamicParams[paramIndex].bSpawnTimeOnly)
								{
									float param = decalShaderParam->DynamicParams[paramIndex].ParamValue.GetValue(currentDecalActorTimeLeft, OwnerEmitterInstance->Component);
									SpawnedDecalActors[decalActorIndex]->SetShaderParam(decalShaderParam->DynamicParams[paramIndex].ParamName, param);
								}
							}
						}
					}
				}
			}
			else
			{
				SpawnedDecalActors.RemoveAt(decalActorIndex);
			}
		}
	}
}

void ParticleDecalInstance::SetActive(bool active)
{
	for (AeVanParticleDecalActor* curDecalActor : SpawnedDecalActors)
	{
		UDecalComponent* decalComp = curDecalActor->GetDecal();
		if (decalComp != nullptr)
		{
			decalComp->SetVisibility(active);
		}
	}
	Active = false;
}

UeVanParticleModuleDecal::UeVanParticleModuleDecal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DecalSize(100.0f, 100.0f, 100.0f)
	, DecalOffset(FVector::ZeroVector)
	, DecalRotation(-90.0f, 0.0f, 0.0f)
	, OwnerActor(nullptr)
	, DecalInstancesPtr(new TMap<FParticleEmitterInstance*, TSharedPtr<ParticleDecalInstance, ESPMode::ThreadSafe>>())
{
	bSpawnModule = true;
	bUpdateModule = true;

	OnWorldCleanupDelegateHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &UeVanParticleModuleDecal::OnWorldCleanup);
}
UeVanParticleModuleDecal::~UeVanParticleModuleDecal()
{
	if (OnWorldCleanupDelegateHandle.IsValid())
	{
		FWorldDelegates::OnWorldCleanup.Remove(OnWorldCleanupDelegateHandle);
	}

	ClearInstances();
}

void UeVanParticleModuleDecal::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
	Super::Spawn(Owner, Offset, SpawnTime, ParticleBase);

	ParticleBase->Size = FVector::ZeroVector;
	ParticleBase->BaseSize = FVector::ZeroVector;

	TSharedPtr<ParticleDecalInstance, ESPMode::ThreadSafe> decalInstPtr;
	if (DecalInstancesPtr.IsValid())
	{
		if (!DecalInstancesPtr->Contains(Owner))
		{
			decalInstPtr = TSharedPtr<ParticleDecalInstance, ESPMode::ThreadSafe>(new ParticleDecalInstance(Owner, DecalMaterial, SortOrder, DecalSize, DecalOffset, DecalRotation));
			DecalInstancesPtr->Add(Owner, decalInstPtr);

			Owner->OnParticleEmitterInstanceDestroy.AddUObject(this, &UeVanParticleModuleDecal::OnEmitterInstanceDesteoyed);
			if (Owner->Component != nullptr)
			{
				Owner->Component->OnSystemPreActivationChange.IsBound();
			}
		}
		else
		{
			decalInstPtr = (*DecalInstancesPtr)[Owner];
		}

		if (decalInstPtr.IsValid())
		{
			decalInstPtr->SpawnDeacalActor();
		}
	}
}

void UeVanParticleModuleDecal::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
	Super::Update(Owner, Offset, DeltaTime);

	if (DecalInstancesPtr.IsValid())
	{
		if (DecalInstancesPtr->Contains(Owner) && (*DecalInstancesPtr)[Owner].IsValid())
		{
			(*DecalInstancesPtr)[Owner]->Update(DeltaTime);
		}
	}

}

void UeVanParticleModuleDecal::ClearInstances()
{
	if (DecalInstancesPtr.IsValid())
	{
		DecalInstancesPtr->Empty();
	}
}

void UeVanParticleModuleDecal::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	ClearInstances();
}

void UeVanParticleModuleDecal::OnEmitterInstanceDesteoyed(FParticleEmitterInstance* Owner)
{
	if (DecalInstancesPtr.IsValid() && DecalInstancesPtr->Contains(Owner))
	{
		DecalInstancesPtr->Remove(Owner);
	}
}