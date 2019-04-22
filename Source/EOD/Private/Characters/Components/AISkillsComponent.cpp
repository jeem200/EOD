// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "AISkillsComponent.h"
#include "AISkillBase.h"
#include "AICharacterBase.h"
#include "EODAIControllerBase.h"

UAISkillsComponent::UAISkillsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAISkillsComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeSkills();
}

void UAISkillsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAISkillsComponent::InitializeSkills(AEODCharacterBase* CompOwner)
{
	// If skills have already been initialized
	if (SkillIndexToSkillMap.Num() > 0)
	{
		return;
	}

	if (!CompOwner)
	{
		// If CompOwner is invalid, try to initalize it from locally cached component owner
		CompOwner = GetCharacterOwner();
	}

	check(CompOwner);
	if (!SkillsDataTable)
	{
		return;
	}

	FString ContextString = FString("UAISkillsComponent::InitializeSkills()");
	TArray<FName> Keys = SkillsDataTable->GetRowNames();
	uint8 SkillIndex = 1;
	for (FName Key : Keys)
	{
		FGameplaySkillTableRow* Row = SkillsDataTable->FindRow<FGameplaySkillTableRow>(Key, ContextString);
		check(Row);
		UGameplaySkillBase* GameplaySkill = NewObject<UGameplaySkillBase>(this, Row->SkillClass, Key, RF_Transient);
		check(GameplaySkill);
		GameplaySkill->InitSkill(CompOwner, CompOwner->Controller);
		GameplaySkill->SetSkillIndex(SkillIndex);

		SkillIndexToSkillMap.Add(GameplaySkill->GetSkillIndex(), GameplaySkill);
		SkillGroupToSkillMap.Add(GameplaySkill->GetSkillGroup(), GameplaySkill);
		SkillGroupToSkillIndexMap.Add(GameplaySkill->GetSkillGroup(), GameplaySkill->GetSkillIndex());

		SkillIndex++;
	}
}

void UAISkillsComponent::TriggerSkill(uint8 SkillIndex, UGameplaySkillBase* Skill)
{
	AEODCharacterBase* CharOwner = GetCharacterOwner();

	check(SkillIndexToSkillMap.Contains(SkillIndex));
	UAISkillBase* AISkill = Skill ? Cast<UAISkillBase>(Skill) : Cast<UAISkillBase>(SkillIndexToSkillMap[SkillIndex]);

	if (!AISkill|| !CharOwner)
	{
		return;
	}

	bool bIsLocalCharacter = CharOwner->Controller && CharOwner->Controller->IsLocalController();
	if (bIsLocalCharacter)
	{
		if (AISkill->CanTriggerSkill())
		{
			// For AI characters, the controller only exists on server (i.e., where the characters are local)
			// So, we don't really need to check for CharOwner's role and call server RPC to trigger skill
			AISkill->TriggerSkill();

			if (AISkill->bSkillCanBeCharged)
			{
				StartChargingSkill();
			}

			LastUsedSkillGroup = AISkill->GetSkillGroup();
			LastUsedSkillIndex = SkillIndex;
			ActiveSkills.Add(AISkill);
		}
		else
		{

		}
	}
	else
	{
		AISkill->TriggerSkill();
		ActiveSkills.Add(AISkill);
	}


	//~ @todo Attack Info
}

void UAISkillsComponent::ReleaseSkill(uint8 SkillIndex, UGameplaySkillBase* Skill, float ReleaseDelay)
{
}

void UAISkillsComponent::CancelSkill(uint8 SkillIndex, UGameplaySkillBase* Skill)
{
}

void UAISkillsComponent::CancelAllActiveSkills()
{
}

bool UAISkillsComponent::CanUseAnySkill() const
{
	return false;
}

bool UAISkillsComponent::CanUseSkill(uint8 SkillIndex, UGameplaySkillBase* Skill)
{
	if (!SkillIndexToSkillMap.Contains(SkillIndex))
	{
		return false;
	}

	UAISkillBase* AISkill = Skill ? Cast<UAISkillBase>(Skill) : Cast<UAISkillBase>(SkillIndexToSkillMap[SkillIndex]);
	if (!AISkill)
	{
		return false;
	}

	bool bCanTriggerSkill = AISkill->CanTriggerSkill();
	return bCanTriggerSkill;
}

FName UAISkillsComponent::GetMostWeightedMeleeSkillID(const AEODCharacterBase* TargetCharacter) const
{
	FName MostWeightedSkillID = NAME_None;
	TArray<FName> EligibleSkills;
	// TArray<FName> MostWeightedSkills;

	if (TargetCharacter->HasBeenHit())
	{
		for (FName SkillID : MeleeSkills)
		{
			if (FlinchSkills.Contains(SkillID))
			{
				EligibleSkills.Add(SkillID);
			}
		}

		for (FName SkillID : EligibleSkills)
		{
			if (MostWeightedSkillID == NAME_None)
			{
				MostWeightedSkillID = SkillID;
				continue;
			}

			/* @fix
			if (SkillIDToWeightMap[SkillID] > SkillIDToWeightMap[MostWeightedSkillID])
			{
				MostWeightedSkillID = SkillID;
			}
			*/
		}
	}
	else
	{
		for (FName SkillID : MeleeSkills)
		{
			if (!FlinchSkills.Contains(SkillID))
			{
				EligibleSkills.Add(SkillID);
			}
		}

		for (FName SkillID : EligibleSkills)
		{
			if (MostWeightedSkillID == NAME_None)
			{
				MostWeightedSkillID = SkillID;
				continue;
			}

			/* @fix
			if (SkillIDToWeightMap[SkillID] > SkillIDToWeightMap[MostWeightedSkillID])
			{
				MostWeightedSkillID = SkillID;
			}
			*/
		}
	}

	return MostWeightedSkillID;
}

void UAISkillsComponent::GenerateSkillTypesList()
{
	TArray<FName> Keys;
	SkillGroupToSkillMap.GetKeys(Keys);

	for (FName Key : Keys)
	{
		UAISkillBase* AISkill = Cast<UAISkillBase>(SkillGroupToSkillMap[Key]);
		check(AISkill);

		if (AISkill->GetSkillEffect() == ESkillEffect::BuffParty)
		{
			PartyBuffSkills.Add(Key);
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::BuffSelf)
		{
			SelfBuffSkills.Add(Key);
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::DamageMelee)
		{
			MeleeSkills.Add(Key);
			if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Crystalized)
			{
				CrystalizeSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Flinch)
			{
				FlinchSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Interrupt)
			{
				InterruptSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::KnockedBack)
			{
				KnockBackSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::KnockedDown)
			{
				KnockDownSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Stunned)
			{
				StunSkills.Add(Key);
			}
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::DamageRanged)
		{
			RangedSkills.Add(Key);
			if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Crystalized)
			{
				CrystalizeSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Flinch)
			{
				FlinchSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Interrupt)
			{
				InterruptSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::KnockedBack)
			{
				KnockBackSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::KnockedDown)
			{
				KnockDownSkills.Add(Key);
			}
			else if (AISkill->SkillInfo.CrowdControlEffect == ECrowdControlEffect::Stunned)
			{
				StunSkills.Add(Key);
			}
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::DebuffEnemy)
		{
			DebuffSkills.Add(Key);
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::HealParty)
		{
			PartyHealSkills.Add(Key);
		}
		else if (AISkill->GetSkillEffect() == ESkillEffect::HealSelf)
		{
			SelfHealSkills.Add(Key);
		}
	}
}
