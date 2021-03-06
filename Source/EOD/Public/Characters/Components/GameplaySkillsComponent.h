// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterLibrary.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameplaySkillsComponent.generated.h"

class AEODCharacterBase;
class UGameplaySkillBase;
class UGameplayEffectBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EOD_API UGameplaySkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// --------------------------------------
	//  UE4 Method Overrides
	// --------------------------------------

	UGameplaySkillsComponent(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --------------------------------------
	//  Skill System
	// --------------------------------------

	/**
	 * Trigger a skill
	 * @note This may result in either instantly activating a skill (e.g. Nocturne) or this may start charging the skill (for skills that can be charged by holding down the attack key)
	 */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void TriggerSkill(uint8 SkillIndex, UGameplaySkillBase* Skill = nullptr);

	/** If the character is currently charging a chargeable skill then this activates the skill. Otherwise it does nothing */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void ReleaseSkill(uint8 SkillIndex, UGameplaySkillBase* Skill = nullptr, float ReleaseDelay = 0.f);

	/** If the character is currently using skill corresponding to given SkillIndex then cancel it, otherwise do nothing. */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void CancelSkill(uint8 SkillIndex, UGameplaySkillBase* Skill = nullptr);

	/** Cancels all active skills that the character is currently using */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void CancelAllActiveSkills();

	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void CancelSkillsWithTag(FGameplayTag Tag);

	/** Returns true if the character can use any skill right now */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual bool CanUseAnySkill() const;

	/** Returns true if the character can use skill corresponding to given SkillIndex */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual bool CanUseSkill(uint8 SkillIndex, UGameplaySkillBase* Skill = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void ActivateChainSkill(UGameplaySkillBase* CurrentSkill);

	/** Returns the skill index of the skill with corresponding SkillGroup */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	uint8 GetSkillIndexForSkillGroup(FName SkillGroup) const;

	/** Returns the skill corresponding to given SkillGroup */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	UGameplaySkillBase* GetSkillForSkillGroup(FName SkillGroup) const;

	/** Event called when a skill gets cancelled */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void OnSkillCancelled(uint8 SkillIndex, FName SkillGroup, UGameplaySkillBase* Skill);

	/** Event called when a skill gets finished */
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	virtual void OnSkillFinished(uint8 SkillIndex, FName SkillGroup, UGameplaySkillBase* Skill);

	virtual void OnSkillTriggered(uint8 SkillIndex, FName SkillGroup, UGameplaySkillBase* Skill);
	virtual void OnSkillReleased(uint8 SkillIndex, FName SkillGroup, UGameplaySkillBase* Skill);

	void BroadcastGameplayEvents(FName EventType, UGameplaySkillBase* SourceSkill);

	inline FGameplaySkillTableRow* GetGameplaySkillTableRow(FName SkillID, const FString& ContextString = FString("AEODCharacterBase::GetSkill(), character skill lookup")) const;

	inline const TMap<uint8, UGameplaySkillBase*>& GetSkillsMap() const { return SkillIndexToSkillMap; }

	FORCEINLINE FName GetActivePrecedingChainSkillGroup() const { return ActivePrecedingChainSkillGroup; }

	FORCEINLINE bool CanUseChainSkill() const { return bCanUseChainSkill; }

	FORCEINLINE void SetCanUseChainSkill(bool bValue) { bCanUseChainSkill = bValue; }

	FORCEINLINE FName GetLastUsedSkillGroup() const { return LastUsedSkillGroup; }

	FORCEINLINE uint8 GetLastUsedSkillIndex() const { return LastUsedSkillIndex; }

	FORCEINLINE TPair<uint8, uint8> GetSupersedingChainSkillGroup() const { return SupersedingChainSkillGroup; };

	FORCEINLINE TArray<UGameplaySkillBase*> GetActiveSkills() const { return ActiveSkills; }
	
protected:

	UPROPERTY(Transient)
	TArray<UGameplaySkillBase*> ActiveSkills;

	/** Skill index to skil map. Skill index will be used during replication */
	UPROPERTY(Transient)
	TMap<uint8, UGameplaySkillBase*> SkillIndexToSkillMap;

	UPROPERTY(Transient)
	TMap<FName, UGameplaySkillBase*> SkillGroupToSkillMap;

	UPROPERTY(Transient)
	TMap<FName, uint8> SkillGroupToSkillIndexMap;

	UPROPERTY(Transient)
	TArray<UGameplayEffectBase*> ActiveGameplayEffects;

	UPROPERTY(Transient)
	bool bCanUseChainSkill;

	/** The skill that the character used last time */
	UPROPERTY(Transient)
	uint8 LastUsedSkillIndex;

	/** The skill that the character used last time */
	UPROPERTY(Transient)
	FName LastUsedSkillGroup;

	/**
	 * The skill that the player last used.
	 * This is used to determine whether a skill that requires a 'ActivePrecedingChainSkillGroup' type skill to be used previously, can be used
	 * It will auto deactivate after 'ChainSkillResetDelay' seconds.
	 */
	UPROPERTY(Transient)
	FName ActivePrecedingChainSkillGroup;

	/**
	 * The skill group that will get triggered on pressing the skill key at index 'ActiveSupersedingChainSkillGroup.Key'
	 * This will auto deactivate after 'ChainSkillResetDelay' seconds.
	 */
	TPair<uint8, uint8> SupersedingChainSkillGroup;

	UPROPERTY(Transient)
	bool bSkillCharging;

	UPROPERTY(Transient)
	float SkillChargeDuration;

	FTimerHandle ChainSkillTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill System")
	float ChainSkillResetDelay;

	/** Data table for character skills */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill System")
	UDataTable* SkillsDataTable;

	virtual void ResetChainSkill();

public:

	// --------------------------------------
	//  Gameplay
	// --------------------------------------

	inline void StartChargingSkill();

	inline void StopChargingSkill();

	virtual void InitializeSkills(AEODCharacterBase* CompOwner = nullptr);

	virtual void UpdateSkillCooldown(FName SkillGroup, float RemainingCooldown);

	virtual void UpdateSkillCooldown(uint8 SkillIndex, float RemainingCooldown);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Effects")
	virtual void AddGameplayEffect(UGameplayEffectBase* GameplayEffect);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Effects")
	virtual void RemoveGameplayEffect(UGameplayEffectBase* GameplayEffect);

	AEODCharacterBase* GetCharacterOwner();

	TMap<FName, TMap<UGameplaySkillBase*, FGameplayEventInfo>> GameplayEvents;

	void ActivateGameplayEffect(
		UClass* GameplayEffectClass,
		int32 Level,
		AActor* Instigator,
		TArray<AActor*> Targets,
		bool bDetermineTargetDynamically = false);

	virtual bool IsGameplayEffectTypeActive(TSubclassOf<UGameplayEffectBase> GameplayEffectClass, UGameplayEffectBase* GameplayEffectToIgnore = nullptr);

private:

	/** Cached pointer to EOD character owner */
	UPROPERTY(Transient)
	AEODCharacterBase* EODCharacterOwner;

protected:

	// --------------------------------------
	//  Network
	// --------------------------------------

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TriggerSkill(uint8 SkillIndex);
	virtual void Server_TriggerSkill_Implementation(uint8 SkillIndex);
	virtual bool Server_TriggerSkill_Validate(uint8 SkillIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReleaseSkill(uint8 SkillIndex, float ChargeDuration);
	virtual void Server_ReleaseSkill_Implementation(uint8 SkillIndex, float ChargeDuration);
	virtual bool Server_ReleaseSkill_Validate(uint8 SkillIndex, float ChargeDuration);

};

inline FGameplaySkillTableRow* UGameplaySkillsComponent::GetGameplaySkillTableRow(FName SkillID, const FString& ContextString) const
{
	return SkillsDataTable ? SkillsDataTable->FindRow<FGameplaySkillTableRow>(SkillID, ContextString) : nullptr;
}

inline void UGameplaySkillsComponent::StartChargingSkill()
{
	bSkillCharging = true;
	SkillChargeDuration = 0.f;
}

inline void UGameplaySkillsComponent::StopChargingSkill()
{
	bSkillCharging = false;
	SkillChargeDuration = 0.f;
}
