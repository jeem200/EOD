// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "CombatInterface.h"

UCombatInterface::UCombatInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

TSharedPtr<FAttackInfo> ICombatInterface::GetAttackInfoPtr(const FName& SkillGroup, const int32 CollisionIndex)
{
	return TSharedPtr<FAttackInfo>();
}

AActor* ICombatInterface::GetInterfaceOwner()
{
	return nullptr;
}

bool ICombatInterface::IsEnemyOf(ICombatInterface* TargetCI) const
{
	return true;
}

void ICombatInterface::PostAttack(const TArray<FAttackResponse>& AttackResponses, const TArray<AActor*> HitActors)
{
}

TSharedPtr<FAttackResponse> ICombatInterface::ReceiveAttack(
	AActor* HitInstigator,
	ICombatInterface* InstigatorCI,
	const TSharedPtr<FAttackInfo>& AttackInfoPtr,
	const FHitResult& DirectHitResult,
	const bool bLineHitResultFound,
	const FHitResult& LineHitResult)
{
	return TSharedPtr<FAttackResponse>(nullptr);
}

float ICombatInterface::GetActualDamage(
	AActor* HitInstigator,
	ICombatInterface* InstigatorCI,
	const TSharedPtr<FAttackInfo>& AttackInfoPtr,
	const bool bCritHit,
	const bool bAttackBlocked)
{
	return 0.0f;
}

USoundBase* ICombatInterface::GetMeleeHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	return nullptr;
}

USoundBase* ICombatInterface::GetMeleeHitMissSound() const
{
	return nullptr;
}
