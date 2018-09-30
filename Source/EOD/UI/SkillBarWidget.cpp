// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "SkillBarWidget.h"
#include "UI/EODItemContainer.h"
#include "Player/PlayerCharacter.h"

USkillBarWidget::USkillBarWidget(const FObjectInitializer & ObjectInitializer): Super(ObjectInitializer)
{
}

bool USkillBarWidget::Initialize()
{
	if (Super::Initialize())
	{
		return true;
	}

	return false;
}

void USkillBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USkillBarWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void USkillBarWidget::PressSkillButton(uint32 SkillIndex)
{
	if (!OwningEODPlayer)
	{
		// @todo raise error
		return;
	}

	switch (SkillIndex)
	{
	case 1:
		OwningEODPlayer->StartSkill(Skill_1->EODItemInfo.ItemID);
		break;
	case 2:
		OwningEODPlayer->StartSkill(Skill_2->EODItemInfo.ItemID);
		break;
	case 3:
		OwningEODPlayer->StartSkill(Skill_3->EODItemInfo.ItemID);
		break;
	case 4:
		OwningEODPlayer->StartSkill(Skill_4->EODItemInfo.ItemID);
		break;
	case 5:
		OwningEODPlayer->StartSkill(Skill_5->EODItemInfo.ItemID);
		break;
	case 6:
		OwningEODPlayer->StartSkill(Skill_6->EODItemInfo.ItemID);
		break;
	case 7:
		OwningEODPlayer->StartSkill(Skill_7->EODItemInfo.ItemID);
		break;
	case 8:
		OwningEODPlayer->StartSkill(Skill_8->EODItemInfo.ItemID);
		break;
	case 9:
		OwningEODPlayer->StartSkill(Skill_9->EODItemInfo.ItemID);
		break;
	case 10:
		OwningEODPlayer->StartSkill(Skill_10->EODItemInfo.ItemID);
		break;
	case 11:
		OwningEODPlayer->StartSkill(Skill_11->EODItemInfo.ItemID);
		break;
	case 12:
		OwningEODPlayer->StartSkill(Skill_12->EODItemInfo.ItemID);
		break;
	case 13:
		OwningEODPlayer->StartSkill(Skill_13->EODItemInfo.ItemID);
		break;
	case 14:
		OwningEODPlayer->StartSkill(Skill_14->EODItemInfo.ItemID);
		break;
	case 15:
		OwningEODPlayer->StartSkill(Skill_15->EODItemInfo.ItemID);
		break;
	case 16:
		OwningEODPlayer->StartSkill(Skill_16->EODItemInfo.ItemID);
		break;
	case 17:
		OwningEODPlayer->StartSkill(Skill_17->EODItemInfo.ItemID);
		break;
	case 18:
		OwningEODPlayer->StartSkill(Skill_18->EODItemInfo.ItemID);
		break;
	case 19:
		OwningEODPlayer->StartSkill(Skill_19->EODItemInfo.ItemID);
		break;
	case 20:
		OwningEODPlayer->StartSkill(Skill_20->EODItemInfo.ItemID);
		break;
	default:
		break;
	}
}

void USkillBarWidget::ReleaseSkillButton(uint32 SkillIndex)
{
	if (!OwningEODPlayer)
	{
		// @todo raise error
		return;
	}

	switch (SkillIndex)
	{
	case 1:
		OwningEODPlayer->StopSkill(Skill_1->EODItemInfo.ItemID);
		break;
	case 2:
		OwningEODPlayer->StopSkill(Skill_2->EODItemInfo.ItemID);
		break;
	case 3:
		OwningEODPlayer->StopSkill(Skill_3->EODItemInfo.ItemID);
		break;
	case 4:
		OwningEODPlayer->StopSkill(Skill_4->EODItemInfo.ItemID);
		break;
	case 5:
		OwningEODPlayer->StopSkill(Skill_5->EODItemInfo.ItemID);
		break;
	case 6:
		OwningEODPlayer->StopSkill(Skill_6->EODItemInfo.ItemID);
		break;
	case 7:
		OwningEODPlayer->StopSkill(Skill_7->EODItemInfo.ItemID);
		break;
	case 8:
		OwningEODPlayer->StopSkill(Skill_8->EODItemInfo.ItemID);
		break;
	case 9:
		OwningEODPlayer->StopSkill(Skill_9->EODItemInfo.ItemID);
		break;
	case 10:
		OwningEODPlayer->StopSkill(Skill_10->EODItemInfo.ItemID);
		break;
	case 11:
		OwningEODPlayer->StopSkill(Skill_11->EODItemInfo.ItemID);
		break;
	case 12:
		OwningEODPlayer->StopSkill(Skill_12->EODItemInfo.ItemID);
		break;
	case 13:
		OwningEODPlayer->StopSkill(Skill_13->EODItemInfo.ItemID);
		break;
	case 14:
		OwningEODPlayer->StopSkill(Skill_14->EODItemInfo.ItemID);
		break;
	case 15:
		OwningEODPlayer->StopSkill(Skill_15->EODItemInfo.ItemID);
		break;
	case 16:
		OwningEODPlayer->StopSkill(Skill_16->EODItemInfo.ItemID);
		break;
	case 17:
		OwningEODPlayer->StopSkill(Skill_17->EODItemInfo.ItemID);
		break;
	case 18:
		OwningEODPlayer->StopSkill(Skill_18->EODItemInfo.ItemID);
		break;
	case 19:
		OwningEODPlayer->StopSkill(Skill_19->EODItemInfo.ItemID);
		break;
	case 20:
		OwningEODPlayer->StopSkill(Skill_20->EODItemInfo.ItemID);
		break;
	default:
		break;
	}
}

void USkillBarWidget::SetOwningEODPlayer(APlayerCharacter * NewOwner)
{
	OwningEODPlayer = NewOwner;
}

FORCEINLINE FName USkillBarWidget::GetSkillAtIndex(int32 SkillIndex)
{
	switch (SkillIndex)
	{
	case 1:
		return Skill_1->EODItemInfo.ItemID;
	case 2:
		return Skill_2->EODItemInfo.ItemID;
	case 3:
		return Skill_3->EODItemInfo.ItemID;
	case 4:
		return Skill_4->EODItemInfo.ItemID;
	case 5:
		return Skill_5->EODItemInfo.ItemID;
	case 6:
		return Skill_6->EODItemInfo.ItemID;
	case 7:
		return Skill_7->EODItemInfo.ItemID;
	case 8:
		return Skill_8->EODItemInfo.ItemID;
	case 9:
		return Skill_9->EODItemInfo.ItemID;
	case 10:
		return Skill_10->EODItemInfo.ItemID;
	case 11:
		return Skill_11->EODItemInfo.ItemID;
	case 12:
		return Skill_12->EODItemInfo.ItemID;
	case 13:
		return Skill_13->EODItemInfo.ItemID;
	case 14:
		return Skill_14->EODItemInfo.ItemID;
	case 15:
		return Skill_15->EODItemInfo.ItemID;
	case 16:
		return Skill_16->EODItemInfo.ItemID;
	case 17:
		return Skill_17->EODItemInfo.ItemID;
	case 18:
		return Skill_18->EODItemInfo.ItemID;
	case 19:
		return Skill_19->EODItemInfo.ItemID;
	case 20:
		return Skill_20->EODItemInfo.ItemID;
	default:
		break;
	}

	return NAME_None;
}

FORCEINLINE void USkillBarWidget::PutSkillOnCooldownTimer(int32 SkillIndex, float Duration, float Interval)
{
	switch (SkillIndex)
	{
	case 1:
		Skill_1->StartCooldown(Duration, Interval);
		break;
	case 2:
		Skill_2->StartCooldown(Duration, Interval);
		break;
	case 3:
		Skill_3->StartCooldown(Duration, Interval);
		break;
	case 4:
		Skill_4->StartCooldown(Duration, Interval);
		break;
	case 5:
		Skill_5->StartCooldown(Duration, Interval);
		break;
	case 6:
		Skill_6->StartCooldown(Duration, Interval);
		break;
	case 7:
		Skill_7->StartCooldown(Duration, Interval);
		break;
	case 8:
		Skill_8->StartCooldown(Duration, Interval);
		break;
	case 9:
		Skill_9->StartCooldown(Duration, Interval);
		break;
	case 10:
		Skill_10->StartCooldown(Duration, Interval);
		break;
	case 11:
		Skill_11->StartCooldown(Duration, Interval);
		break;
	case 12:
		Skill_12->StartCooldown(Duration, Interval);
		break;
	case 13:
		Skill_13->StartCooldown(Duration, Interval);
		break;
	case 14:
		Skill_14->StartCooldown(Duration, Interval);
		break;
	case 15:
		Skill_15->StartCooldown(Duration, Interval);
		break;
	case 16:
		Skill_16->StartCooldown(Duration, Interval);
		break;
	case 17:
		Skill_17->StartCooldown(Duration, Interval);
		break;
	case 18:
		Skill_18->StartCooldown(Duration, Interval);
		break;
	case 19:
		Skill_19->StartCooldown(Duration, Interval);
		break;
	case 20:
		Skill_20->StartCooldown(Duration, Interval);
		break;
	default:
		break;
	}
}

bool USkillBarWidget::IsSkillInCooldown(int32 SkillIndex)
{
	switch (SkillIndex)
	{
	case 1:
		return Skill_1->bInCooldown;
	case 2:
		return Skill_2->bInCooldown;
	case 3:
		return Skill_3->bInCooldown;
	case 4:
		return Skill_4->bInCooldown;
	case 5:
		return Skill_5->bInCooldown;
	case 6:
		return Skill_6->bInCooldown;
	case 7:
		return Skill_7->bInCooldown;
	case 8:
		return Skill_8->bInCooldown;
	case 9:
		return Skill_9->bInCooldown;
	case 10:
		return Skill_10->bInCooldown;
	case 11:
		return Skill_11->bInCooldown;
	case 12:
		return Skill_12->bInCooldown;
	case 13:
		return Skill_13->bInCooldown;
	case 14:
		return Skill_14->bInCooldown;
	case 15:
		return Skill_15->bInCooldown;
	case 16:
		return Skill_16->bInCooldown;
	case 17:
		return Skill_17->bInCooldown;
	case 18:
		return Skill_18->bInCooldown;
	case 19:
		return Skill_19->bInCooldown;
	case 20:
		return Skill_20->bInCooldown;
	default:
		break;
	}

	return false;
}

APlayerCharacter * USkillBarWidget::GetOwningEODPlayer() const
{
	return OwningEODPlayer;
}
