// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "InstantMeleeSkill.h"
#include "EODCharacterBase.h"
#include "GameplaySkillsComponent.h"
#include "EODCharacterMovementComponent.h"

UInstantMeleeSkill::UInstantMeleeSkill(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UInstantMeleeSkill::CanTriggerSkill() const
{
	return Super::CanTriggerSkill();

	/*
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	EWeaponType EquippedWeaponType = Instigator ? Instigator->GetEquippedWeaponType() : EWeaponType::None;

	bool bHasValidWeapon = EquippedWeaponType != EWeaponType::None && IsWeaponTypeSupported(EquippedWeaponType) && !Instigator->IsWeaponSheathed();
	bool bInCooldown = IsSkillInCooldown();

	bool bInstigatorCanUseSkill = Instigator ? Instigator->IsIdleOrMoving() || Instigator->IsBlocking() || Instigator->IsNormalAttacking() : false;
	if (!bInstigatorCanUseSkill)
	{
		// bInstigatorCanUseSkill = Instigator->()
	}

	return bHasValidWeapon && !bInCooldown && bInstigatorCanUseSkill;
	*/
}

void UInstantMeleeSkill::TriggerSkill()
{
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	if (!Instigator)
	{
		return;
	}

	bool bIsLocalPlayerController = Instigator->Controller && Instigator->Controller->IsLocalPlayerController();
	if (bIsLocalPlayerController)
	{
		float DesiredRotationYaw = Instigator->GetControllerRotationYaw();
		UEODCharacterMovementComponent* MoveComp = Cast<UEODCharacterMovementComponent>(Instigator->GetCharacterMovement());
		if (MoveComp)
		{
			MoveComp->SetDesiredCustomRotationYaw(DesiredRotationYaw);
		}

		Instigator->SetCharacterStateAllowsMovement(false);
		Instigator->SetCharacterStateAllowsRotation(false);

		//~ @todo consume stamina and mana

		StartCooldown();
	}

	bool bHasController = Instigator->Controller != nullptr;
	if (bHasController)
	{
		FCharacterStateInfo StateInfo(ECharacterState::UsingActiveSkill, SkillIndex);
		StateInfo.NewReplicationIndex = Instigator->CharacterStateInfo.NewReplicationIndex + 1;
		Instigator->CharacterStateInfo = StateInfo;
	}

	EWeaponType CurrentWeapon = Instigator->GetEquippedWeaponType();
	UAnimMontage* Montage = SkillAnimations.Contains(CurrentWeapon) ? SkillAnimations[CurrentWeapon] : nullptr;
	if (Montage)
	{
		SkillDuration = Instigator->PlayAnimMontage(Montage, 1.f, AnimationStartSectionName);
		float ActualSkillDuration;

		if (Montage->BlendOutTriggerTime >= 0.f)
		{
			ActualSkillDuration = SkillDuration;
		}
		else
		{
			ActualSkillDuration = SkillDuration - Montage->BlendOut.GetBlendTime();
		}

		UWorld* World = Instigator->GetWorld();
		check(World);
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UInstantMeleeSkill::FinishSkill);
		World->GetTimerManager().SetTimer(SkillTimerHandle, TimerDelegate, ActualSkillDuration, false);
	}
	else
	{
		Instigator->ResetState();
	}
}

bool UInstantMeleeSkill::CanCancelSkill() const
{
	return true;
}

void UInstantMeleeSkill::CancelSkill()
{
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	if (Instigator)
	{
		UWorld* World = Instigator->GetWorld();
		check(World);
		World->GetTimerManager().ClearTimer(SkillTimerHandle);
	}
	
	UGameplaySkillsComponent* SkillsComp = InstigatorSkillComponent.Get();
	if(SkillsComp)
	{
		SkillsComp->OnSkillCancelled(SkillIndex, SkillGroup, this);
	}
}

void UInstantMeleeSkill::FinishSkill()
{
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	if (Instigator)
	{
		Instigator->ResetState();
	}
	
	UGameplaySkillsComponent* SkillsComp = InstigatorSkillComponent.Get();
	if(SkillsComp)
	{
		SkillsComp->OnSkillFinished(SkillIndex, SkillGroup, this);
	}
}
