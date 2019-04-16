// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "HumanCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "PlayerCharacter.generated.h"


class USkillBarWidget;
class UAudioComponent;
class UAnimMontage;
class UPlayerAnimInstance;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInventoryComponent;
class USkillsComponent;
class USphereComponent;
class UDialogueWindowWidget;
class UWeaponDataAsset;
class APlayerCharacter;

/** Delegate for when a player changes it's weapon */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponChangedMCDelegate, FName, WeaponID, UWeaponDataAsset*, WeaponDataAsset);

/**
 * PlayerCharacter is the base class for playable characters
 */
UCLASS()
class EOD_API APlayerCharacter : public AHumanCharacter
{
	GENERATED_BODY()
	
public:

	// --------------------------------------
	//	UE4 Method Overrides
	// --------------------------------------

	/** Create and initialize skeletal armor mesh, camera, and inventory components. */
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Spawn default weapon(s) */
	virtual void PostInitializeComponents() override;

	/** Initializes player animation references. Creates player HUD widget and adds it to the viewport. */
	virtual void BeginPlay() override;

	/** Updates player states */
	virtual void Tick(float DeltaTime) override;

	/** Called once this actor has been deleted */
	virtual void Destroyed() override;

	// --------------------------------------
	//	Load/Save System
	// --------------------------------------

	/** Saves current player state */
	virtual void SaveCharacterState() override;


	// --------------------------------------
	//	Components
	// --------------------------------------

	FORCEINLINE UAudioComponent* GetSystemAudioComponent() const { return SystemAudioComponent; }

	FORCEINLINE USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

	static const FName SystemAudioComponentName;

	static const FName InteractionSphereComponentName;

protected:

	/** Audio component for playing system messages */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* SystemAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Interaction")
	USphereComponent* InteractionSphere;

public:

	// --------------------------------------
	//  Character States
	// --------------------------------------

	/** Put or remove weapon inside sheath */
	virtual void ToggleSheathe() override;

	virtual void StartWeaponSwitch() override;

	virtual void CancelWeaponSwitch() override;

	virtual void FinishWeaponSwitch() override;

	/** Plays BlockAttack animation on blocking an incoming attack */
	virtual void PlayAttackBlockedAnimation() override;

	/** Returns true if character can move */
	virtual bool CanMove() const override;

	/** Returns true if character can jump */
	virtual bool CanJump() const override;

	/** Returns true if character can dodge */
	virtual bool CanDodge() const override;

	/** Returns true if character can guard against incoming attacks */
	virtual bool CanGuardAgainstAttacks() const override;

	/** Returns true if character can use any skill at all */
	virtual bool CanUseAnySkill() const;

	// --------------------------------------
	//  Sounds
	// --------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FPlayerSystemSounds SystemSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FPlayerGameplaySounds GameplaySounds;

	// --------------------------------------
	//  Skill System
	// --------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Skill System")
	void SetCanUseChainSkill(bool bNewValue);

	// --------------------------------------
	//  Rotation
	// --------------------------------------

	//~ DEPRECATED
	/** Determines if this character should be rotated toward DesiredSmoothRotationYaw */
	UPROPERTY(Transient)
	bool bRotateSmoothly;

	//~ DEPRECATED
	/** The yaw to which the character needs to be rotated smoothly */
	UPROPERTY(Transient)
	float DesiredSmoothRotationYaw;

	//~ DEPRECATED
	FORCEINLINE void SetOffSmoothRotation(float DesiredYaw);

	// --------------------------------------
	//  Interaction
	// --------------------------------------

	/** Event called when interaction sphere begins overlap with interactive actors */
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** Event called when interaction sphere ends overlap with interactive actors */
	UFUNCTION()
	void OnInteractionSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual bool CanStartInteraction() const override;

	/** Trigger interaction with an NPC or an in-game interactive object */
	virtual void TriggerInteraction() override;

	virtual void StartInteraction() override;

	virtual void UpdateInteraction() override;

	virtual void CancelInteraction() override;

	virtual void FinishInteraction() override;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Character Interaction")
	TArray<AActor*> OverlappingInteractiveActors;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Character Interaction")
	AActor* ActiveInteractiveActor;

	UFUNCTION(BlueprintCallable, Category = "Character Interaction")
	void DisplayDialogueWidget(FName DialogueWindowID);

	UFUNCTION(BlueprintCallable, Category = "Character Interaction")
	void RemoveDialogueWidget();

	// --------------------------------------
	//  Weapon System
	// --------------------------------------

	UFUNCTION()
	void ActivateStatusEffectFromWeapon(FName WeaponID, UWeaponDataAsset* WeaponDataAsset);

	UFUNCTION()
	void DeactivateStatusEffectFromWeapon(FName WeaponID, UWeaponDataAsset* WeaponDataAsset);

	/** Add or replace primary weapon with a new weapon */
	void AddPrimaryWeapon(FName WeaponID);

	/** Add or replace secondary weapon with a new weapon */
	void AddSecondaryWeapon(FName WeaponID);

	UPROPERTY()
	int32 ActiveWeaponSlotIndex;

	void AddPrimaryWeaponToCurrentSlot(FName WeaponID);

	void AddPrimaryWeaponToCurrentSlot(FName WeaponID, UWeaponDataAsset* WeaponDataAsset);

	void AddSecondaryWeaponToCurrentSlot(FName WeaponID);

	void AddSecondaryWeaponToCurrentSlot(FName WeaponID, UWeaponDataAsset* WeaponDataAsset);

	void AddPrimaryWeaponToSlot(FName WeaponID, int32 SlotIndex);

	void AddPrimaryWeaponToSlot(FName WeaponID, UWeaponDataAsset* WeaponDataAsset, int32 SlotIndex);

	void AddSecondaryWeaponToSlot(FName WeaponID, int32 SlotIndex);

	void AddSecondaryWeaponToSlot(FName WeaponID, UWeaponDataAsset* WeaponDataAsset, int32 SlotIndex);

	void RemovePrimaryWeaponFromCurrentSlot();

	void RemoveSecondaryWeaponFromCurrentSlot();

	void RemovePrimaryWeaponFromSlot(int32 SlotIndex);

	void RemoveSecondaryWeaponFromSlot(int32 SlotIndex);

	void ToggleWeaponSlot();




	/** [server + local] Change idle-walk-run direction of character */
	// inline void SetIWRCharMovementDir(ECharMovementDirection NewDirection);


	UFUNCTION(BlueprintImplementableEvent)
	void DisplayStatusMessage(const FString& Message);

	/** Called on an animation montage blending out to clean up, reset, or change any state variables */
	virtual void OnMontageBlendingOut(UAnimMontage* AnimMontage, bool bInterrupted) override;

	/** Called on an animation montage ending to clean up, reset, or change any state variables */
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted) override;

	virtual void TurnOnTargetSwitch() override;

	virtual void TurnOffTargetSwitch() override;

	void OnSkillGroupAddedToSkillBar(const FString& SkillGroup);

	void OnSkillGroupRemovedFromSkillBar(const FString& SkillGroup);

	FORCEINLINE bool SkillHasDirectionalAnimations() const;

	UPROPERTY(Transient, BlueprintReadWrite, Category = PlayerInteraction)
	UDialogueWindowWidget* DialogueWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerInteraction)
	TSubclassOf<UDialogueWindowWidget> DialogueWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerInteraction)
	USoundBase* DialogueTriggeredSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerInteraction)
	USoundBase* DialogueEndSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerInteraction)
	USoundBase* InteractiveActorDetectedSound;

	// virtual void UpdateInteraction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = PlayerInteraction)
	void RequestDialogue(AActor* Requestor, FName DialogueWindowID);

	virtual void RequestDialogue_Implementation(AActor* Requestor, FName DialogueWindowID);

	UFUNCTION(BlueprintCallable, Category = PlayerInteraction)
	void FocusCameraOnActor(AActor* TargetActor);

	// UFUNCTION(BlueprintCallable, Category = PlayerInteraction)
	// void UpdateActiveInteraction();

	/** End any interaction currently in progress */
	UFUNCTION(BlueprintCallable, Category = PlayerInteraction)
	void EndInteraction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = PlayerInteraction)
	void FinishDialogue(UDialogueWindowWidget* Widget);

	virtual void FinishDialogue_Implementation(UDialogueWindowWidget* Widget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = PlayerInteraction)
	void ExitDialogue(UDialogueWindowWidget* Widget);

	virtual void ExitDialogue_Implementation(UDialogueWindowWidget* Widget);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combat|Weapons")
	FOnWeaponChangedMCDelegate OnPrimaryWeaponEquipped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combat|Weapons")
	FOnWeaponChangedMCDelegate OnPrimaryWeaponUnequipped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combat|Weapons")
	FOnWeaponChangedMCDelegate OnSecondaryWeaponEquipped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combat|Weapons")
	FOnWeaponChangedMCDelegate OnSecondaryWeaponUnequipped;

	UPROPERTY(EditDefaultsOnly, Category = "Player UI")
	TSubclassOf<USkillBarWidget> SkillBarWidgetClass;

private:

	TArray<UStatusEffectBase*> ManagedStatusEffectsList;

	TArray<UStatusEffectBase*> InflictedStatusEffectsList;

	// TArray<FCombatEvent> EventsOnUsingSkill;
	TArray<FOnGameplayEventMCDelegate> EventsOnUsingSkill;

	TArray<FOnGameplayEventMCDelegate> EventsOnSuccessfulSkillAttack;
	// TArray<FCombatEvent> EventsOnSuccessfulSkillAttack;


	////////////////////////////////////////////////////////////////////////////////
	// WEAPONS
	////////////////////////////////////////////////////////////////////////////////
private:
	/** An actor for primary weapon equipped by the player */
	// UPROPERTY(Transient)
	// APrimaryWeapon* PrimaryWeapon;


	FORCEINLINE void SetPrimaryWeaponID(FName NewWeaponID);

protected:

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void EquipPrimaryWeapon(FName WeaponID);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Slot")
	int32 MaxWeaponSlots;

	void SetActiveWeaponSlotIndex(int32 NewSlotIndex);


private:


	UPROPERTY()
	UWeaponDataAsset* PrimaryWeaponDataAsset;

	UPROPERTY()
	UWeaponDataAsset* SecondaryWeaponDataAsset;

	UPROPERTY(Transient)
	bool bSkillHasDirectionalAnimations;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skills, meta = (AllowPrivateAccess = "true"))
	uint8 MaxNumberOfSkills;


private:





	//~ @note Pressing and releasing skill keys are separate events to support charge events (e.g. charge rage)

	



	////////////////////////////////////////////////////////////////////////////////
	// ACTIONS
	////////////////////////////////////////////////////////////////////////////////
private:

	/** Display or hide character stats UI */
	void OnToggleCharacterStatsUI();


public:

	// --------------------------------------
	//  Effects
	// --------------------------------------

	virtual void CreateGhostTrail_Implementation() override;

	// --------------------------------------
	//  Crowd Control Effect
	// --------------------------------------

	/** Flinch this character (visual feedback) */
	virtual bool CCEFlinch_Implementation(const float BCAngle) override;

	/** Interrupt this character's current action */
	virtual bool CCEInterrupt_Implementation(const float BCAngle) override;

	/** Applies stun to this character */
	virtual bool CCEStun_Implementation(const float Duration) override;

	/** Removes 'stun' crowd control effect from this character */
	virtual void CCERemoveStun_Implementation() override;

	/** Freeze this character */
	virtual bool CCEFreeze_Implementation(const float Duration) override;

	/** Removes 'freeze' crowd control effect from this character */
	virtual void CCEUnfreeze_Implementation() override;

	/** Knockdown this character */
	virtual bool CCEKnockdown_Implementation(const float Duration) override;

	/** Removes 'knock-down' crowd control effect from this character */
	virtual void CCEEndKnockdown_Implementation() override;

	/** Knockback this character */
	virtual bool CCEKnockback_Implementation(const float Duration, const FVector& ImpulseDirection) override;

	// --------------------------------------
	//  Input Handling
	// --------------------------------------

	/** Display or hide mouse cursor */
	void OnToggleMouseCursor();

	/** Handles player pressing a skill key */
	void OnPressingSkillKey(const uint32 SkillButtonIndex);

	/** Handles player releasing a skill key */
	void OnReleasingSkillKey(const uint32 SkillButtonIndex);

	/** Called when player presses a skill key */
	template<uint32 SkillButtonIndex>
	void PressedSkillKey();

	/** Called when player releases a skill key */
	template<uint32 SkillButtonIndex>
	void ReleasedSkillKey();

protected:

	// --------------------------------------
	//  Network
	// --------------------------------------

	virtual void OnRep_PrimaryWeaponID() override;
	virtual	void OnRep_SecondaryWeaponID() override;

	//~ Begin AEODCharacterBase RPC overrides
	virtual void OnRep_CharacterStateInfo(const FCharacterStateInfo& OldStateInfo) override;
	virtual void Server_Dodge_Implementation(uint8 DodgeIndex, float RotationYaw);
	virtual void Multicast_Dodge_Implementation(uint8 DodgeIndex, float RotationYaw);
	virtual void Server_NormalAttack_Implementation(uint8 AttackIndex);
	//~ End AEODCharacterBase RPC overrides

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPrimaryWeaponID(FName NewWeaponID);
	virtual void Server_SetPrimaryWeaponID_Implementation(FName NewWeaponID);
	virtual bool Server_SetPrimaryWeaponID_Validate(FName NewWeaponID);

};

FORCEINLINE void APlayerCharacter::SetPrimaryWeaponID(FName NewWeaponID)
{
	if (Role < ROLE_Authority)
	{
		Server_SetPrimaryWeaponID(NewWeaponID);
	}
	else
	{
		PrimaryWeaponID = NewWeaponID;
		SetCurrentPrimaryWeapon(NewWeaponID);
	}
}

template<uint32 SkillButtonIndex>
inline void APlayerCharacter::PressedSkillKey()
{
	OnPressingSkillKey(SkillButtonIndex);
}

template<uint32 SkillButtonIndex>
inline void APlayerCharacter::ReleasedSkillKey()
{
	OnReleasingSkillKey(SkillButtonIndex);
}

FORCEINLINE void APlayerCharacter::SetOffSmoothRotation(float DesiredYaw)
{
	bRotateSmoothly = true;
	DesiredSmoothRotationYaw = DesiredYaw;
}

FORCEINLINE bool APlayerCharacter::SkillHasDirectionalAnimations() const
{
	return bSkillHasDirectionalAnimations;
}
