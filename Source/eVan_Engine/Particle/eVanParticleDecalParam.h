// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "eVan_Engine/particle/eVanParticleModuleBase.h"
#include "Distributions/DistributionFloatConstant.h"
#include "eVanParticleDecalParam.generated.h"

USTRUCT()
struct FParticleDecalDynamicShaderParam
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = DynamicShaderParameter)
		FName ParamName;

	UPROPERTY(EditAnywhere, Category = DynamicShaderParameter)
		uint32 bSpawnTimeOnly : 1;

	UPROPERTY(EditAnywhere, Category = DynamicShaderParameter)
		struct FRawDistributionFloat ParamValue;

	FParticleDecalDynamicShaderParam()
		: bSpawnTimeOnly(false)
	{
	}

	FParticleDecalDynamicShaderParam(FName InParamName, uint32 InUseEmitterTime, UDistributionFloatConstant* InDistribution)
		: ParamName(InParamName)
		, bSpawnTimeOnly(false)
	{
		ParamValue.Distribution = InDistribution;
	}
};

/**
 *
 */
UCLASS(editinlinenew, hidecategories = Object, meta = (DisplayName = "DecalParam"))
class EVAN_ENGINE_API UeVanParticleDecalParam : public UeVanParticleModuleBase
{
	GENERATED_UCLASS_BODY()

		UPROPERTY(EditAnywhere, Category = DynamicShaderParameter)
		TArray<struct FParticleDecalDynamicShaderParam> DynamicParams;

};


