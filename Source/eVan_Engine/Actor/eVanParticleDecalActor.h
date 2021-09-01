// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "eVanParticleDecalActor.generated.h"

DECLARE_DELEGATE_OneParam(ParticleRemoveDecalActorCallback, uint32);

/**
 *
 */
UCLASS()
class EVAN_ENGINE_API AeVanParticleDecalActor : public ADecalActor
{
	GENERATED_UCLASS_BODY()

public:

	void	Initialize(int32 sortOrder,
		float lifeTime,
		FVector decalSize,
		UMaterialInterface* decalMaterial);

	void	UpdateDecal(float DeltaTime);

	void	SetColor(FLinearColor baseColor);
	void	SetShaderParam(FName paramName, float data);
	float	GetTimeLeft();
	float	GetPlayRate();
	bool	IsPlaying() { return LifeTime > 0; }

#if WITH_EDITOR

	void	SetSortOrder(int32 sortOrder);
	void	SetLifeTime(float lifeTime);
	void	SetDecalSize(FVector decalSize);

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

#endif

private:

	float						TimeLeft;
	float						LifeTime;

	UDecalComponent* DecalComponent;
	UMaterialInstanceDynamic* DecalMaterialDynamic;

};

