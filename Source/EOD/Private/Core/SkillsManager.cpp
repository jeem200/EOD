// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "SkillsManager.h"

ASkillsManager::ASkillsManager(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASkillsManager::BeginPlay()
{
	Super::BeginPlay();

}

void ASkillsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
