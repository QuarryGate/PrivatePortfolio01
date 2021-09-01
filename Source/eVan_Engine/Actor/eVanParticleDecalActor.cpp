// Fill out your copyright notice in the Description page of Project Settings.


#include "eVanParticleDecalActor.h"
#include "Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

AeVanParticleDecalActor::AeVanParticleDecalActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TimeLeft(0.0f)
	, LifeTime(2.0f)
	, DecalMaterialDynamic(nullptr)
{
}

void AeVanParticleDecalActor::Initialize(int32 sortOrder,
	float lifeTime,
	FVector decalSize,
	UMaterialInterface* decalMaterial)
{
	LifeTime = lifeTime;
	DecalComponent = GetDecal();
	if (DecalComponent != nullptr)
	{
		DecalComponent->SortOrder = sortOrder;
		DecalComponent->DecalSize = decalSize;
		DecalComponent->SetDecalMaterial(decalMaterial);
		DecalMaterialDynamic = DecalComponent->CreateDynamicMaterialInstance();
		DecalComponent->MarkRenderStateDirty();
	}
}

void AeVanParticleDecalActor::SetColor(FLinearColor baseColor)
{
	if (DecalMaterialDynamic != nullptr)
	{
		DecalMaterialDynamic->SetVectorParameterValue("ParticleColor", baseColor);
	}
}

void AeVanParticleDecalActor::SetShaderParam(FName paramName, float data)
{
	if (DecalMaterialDynamic != nullptr)
	{
		DecalMaterialDynamic->SetScalarParameterValue(paramName, data);
	}
}

void AeVanParticleDecalActor::UpdateDecal(float DeltaTime)
{
	bool isActorBeingDestroyed = IsActorBeingDestroyed();
	bool isPendingKill = IsPendingKill();

	if (!isActorBeingDestroyed && !isPendingKill)
	{
		if (TimeLeft <= LifeTime)
		{
			TimeLeft += DeltaTime;
		}

		if (TimeLeft >= LifeTime)
		{
			LifeTime = 0.0f;
		}
	}
}

float AeVanParticleDecalActor::GetTimeLeft()
{
	return TimeLeft;
}

float AeVanParticleDecalActor::GetPlayRate()
{
	if (LifeTime != 0.0f)
	{
		return TimeLeft * (1.0f / LifeTime);
	}
	return 0.0f;
}

#if WITH_EDITOR

void AeVanParticleDecalActor::SetSortOrder(int32 sortOrder)
{
	if (DecalComponent != nullptr)
	{
		DecalComponent->SortOrder = sortOrder;
	}
}

void AeVanParticleDecalActor::SetLifeTime(float lifeTime)
{
	LifeTime = lifeTime;
}

void AeVanParticleDecalActor::SetDecalSize(FVector decalSize)
{
	if (DecalComponent != nullptr)
	{
		DecalComponent->DecalSize = decalSize;
	}
}

#endif


