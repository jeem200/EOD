// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "CombatLibrary.h"
#include "CharacterLibrary.h"
#include "EODCharacterBase.h"

const float UCombatLibrary::PhysicalCritMultiplier = 1.6f;
const float UCombatLibrary::MagickalCritMultiplier = 1.4f;
const float UCombatLibrary::BlockDetectionAngle = 60.f;

UCombatLibrary::UCombatLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FCollisionQueryParams UCombatLibrary::GenerateCombatCollisionQueryParams(const AActor* ActorToIgnore, EQueryMobilityType MobilityType, bool bReturnPhysicalMaterial, FName TraceTag)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ActorToIgnore);
	Params.bIgnoreTouches = true;
	Params.bIgnoreBlocks = false;
	Params.bReturnFaceIndex = false;
	Params.bReturnPhysicalMaterial = bReturnPhysicalMaterial;
	Params.MobilityType = MobilityType;
	Params.bTraceComplex = false;
	Params.TraceTag = TraceTag;
	// @todo mask filter for teams.

	return Params;
}

float UCombatLibrary::CalculateDamage(float Attack, float Defense)
{
	// @todo definition
	return Attack;
}
