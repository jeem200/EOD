// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "HumanCharacter.h"
#include "GameSingleton.h"
#include "EODGlobalNames.h"
#include "PrimaryWeapon.h"
#include "SecondaryWeapon.h"
#include "GameplaySkillsComponent.h"
#include "EODCharacterMovementComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

FName AHumanCharacter::HairComponentName(TEXT("Hair"));
FName AHumanCharacter::HatItemComponentName(TEXT("Hat Item"));
FName AHumanCharacter::FaceItemComponentName(TEXT("Face Item"));
FName AHumanCharacter::ChestComponentName(TEXT("Chest"));
FName AHumanCharacter::HandsComponentName(TEXT("Hands"));
FName AHumanCharacter::LegsComponentName(TEXT("Legs"));
FName AHumanCharacter::FeetComponentName(TEXT("Feet"));

AHumanCharacter::AHumanCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// @note Defaul skeletal mesh component inherited from ACharacter class will contain face mesh
	if (GetMesh())
	{
		GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -90.f));
		GetMesh()->bUseAttachParentBound = true;
	}

	// @note : SetMasterPoseComponent() from constructor doesn't work in packaged game (for some weird reason?!)

	Hair				= CreateNewArmorComponent(AHumanCharacter::HairComponentName, ObjectInitializer);
	HatItem				= CreateNewArmorComponent(AHumanCharacter::HatItemComponentName, ObjectInitializer);
	FaceItem			= CreateNewArmorComponent(AHumanCharacter::FaceItemComponentName, ObjectInitializer);
	Chest				= CreateNewArmorComponent(AHumanCharacter::ChestComponentName, ObjectInitializer);
	Hands				= CreateNewArmorComponent(AHumanCharacter::HandsComponentName, ObjectInitializer);
	Legs				= CreateNewArmorComponent(AHumanCharacter::LegsComponentName, ObjectInitializer);
	Feet				= CreateNewArmorComponent(AHumanCharacter::FeetComponentName, ObjectInitializer);

	bUseControllerRotationYaw = false;

}

void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Load animation references for no-weapon
	LoadAnimationReferencesForWeapon(EWeaponType::None);

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = this;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PrimaryWeapon = World->SpawnActor<APrimaryWeapon>(APrimaryWeapon::StaticClass(), SpawnInfo);
	SecondaryWeapon = World->SpawnActor<ASecondaryWeapon>(ASecondaryWeapon::StaticClass(), SpawnInfo);

	// @note Set secondary weapon first and primary weapon later during initialization
	SetCurrentSecondaryWeapon(SecondaryWeaponID);
	SetCurrentPrimaryWeapon(PrimaryWeaponID);

	SetChestArmor(EquippedArmorIDs.ChestArmorID);
	SetHandsArmor(EquippedArmorIDs.HandsArmorID);
	SetLegsArmor(EquippedArmorIDs.LegsArmorID);
	SetFeetArmor(EquippedArmorIDs.FeetArmorID);
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Since setting master pose component from constructor doesn't work in packaged game
	SetMasterPoseComponentForMeshes();
}

void AHumanCharacter::Destroyed()
{
	Super::Destroyed();

	AnimationReferencesMap.Empty(0);

	//~ Unload animation references for weapons
	TArray<EWeaponType> Weapons;
	AnimationReferencesStreamableHandles.GetKeys(Weapons);
	for (EWeaponType Weapon : Weapons)
	{
		UnloadAnimationReferencesForWeapon(Weapon);
	}

	AnimationReferencesStreamableHandles.Empty(0);
}

void AHumanCharacter::SaveCharacterState()
{
}

USkeletalMeshComponent* AHumanCharacter::CreateNewArmorComponent(const FName Name, const FObjectInitializer& ObjectInitializer)
{
	USkeletalMeshComponent* Sk = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, Name);
	if (Sk)
	{
		Sk->SetupAttachment(GetMesh());
		Sk->SetReceivesDecals(false);
		Sk->bUseAttachParentBound = true;
	}
	return Sk;
}

void AHumanCharacter::LoadAnimationReferencesForWeapon(EWeaponType WeaponType)
{
	// If there is no data table containing animation references then we simply can't load animations
	if (!PlayerAnimationReferencesDataTable)
	{
		return;
	}

	// If the animation references are already loaded
	if (AnimationReferencesMap.Contains(WeaponType) && AnimationReferencesStreamableHandles.Contains(WeaponType))
	{
		return;
	}

	FName RowID = GetAnimationReferencesRowID(WeaponType, Gender);
	FPlayerAnimationReferencesTableRow* PlayerAnimationReferences = PlayerAnimationReferencesDataTable->FindRow<FPlayerAnimationReferencesTableRow>(RowID,
		FString("AHumanCharacter::LoadAnimationReferencesForWeapon()"));

	if (!PlayerAnimationReferences)
	{
		return;
	}

	TSharedPtr<FStreamableHandle> AnimationHandle = LoadAnimationReferences(PlayerAnimationReferences);;

	if (AnimationReferencesMap.Contains(WeaponType))
	{
		AnimationReferencesMap[WeaponType] = PlayerAnimationReferences;
	}
	else
	{
		AnimationReferencesMap.Add(WeaponType, PlayerAnimationReferences);
	}
	
	if (AnimationReferencesStreamableHandles.Contains(WeaponType))
	{
		AnimationReferencesStreamableHandles[WeaponType] = AnimationHandle;
	}
	else
	{
		AnimationReferencesStreamableHandles.Add(WeaponType, AnimationHandle);
	}
}

void AHumanCharacter::UnloadAnimationReferencesForWeapon(EWeaponType WeaponType)
{
	if (AnimationReferencesStreamableHandles.Contains(WeaponType))
	{
		TSharedPtr<FStreamableHandle> AnimationHandle = AnimationReferencesStreamableHandles[WeaponType];
		if (AnimationHandle.IsValid())
		{
			AnimationHandle.Get()->ReleaseHandle();
			AnimationHandle.Reset();
		}
	}
	AnimationReferencesStreamableHandles.Remove(WeaponType);
	AnimationReferencesMap.Remove(WeaponType);
}

FName AHumanCharacter::GetAnimationReferencesRowID(EWeaponType WeaponType, ECharacterGender CharGender)
{
	FString Prefix;
	if (CharGender == ECharacterGender::Female)
	{
		Prefix = FString("Female_");
	}
	else
	{
		Prefix = FString("Male_");
	}

	FString Postfix;
	switch (WeaponType)
	{
	case EWeaponType::GreatSword:
		Postfix = FString("GreatSword");
		break;
	case EWeaponType::WarHammer:
		Postfix = FString("WarHammer");
		break;
	case EWeaponType::LongSword:
		Postfix = FString("LongSword");
		break;
	case EWeaponType::Mace:
		Postfix = FString("Mace");
		break;
	case EWeaponType::Dagger:
		Postfix = FString("Daggers");
		break;
	case EWeaponType::Staff:
		Postfix = FString("Staff");
		break;
	case EWeaponType::Shield:
	case EWeaponType::None:
	default:
		Postfix = FString("NoWeapon");
		break;
	}

	FString RowIDString = Prefix + Postfix;
	FName RowID = FName(*RowIDString);

	return RowID;
}

TSharedPtr<FStreamableHandle> AHumanCharacter::LoadAnimationReferences(FPlayerAnimationReferencesTableRow* AnimationReferences)
{
	TSharedPtr<FStreamableHandle> StreamableHandle;

	UGameSingleton* GameSingleton = GEngine ? Cast<UGameSingleton>(GEngine->GameSingleton) : nullptr;

	if (!GameSingleton)
	{
		return StreamableHandle;
	}

	TArray<FSoftObjectPath> AssetsToLoad;
	if (AnimationReferences)
	{
		AddAnimationSoftObjectPathToArray(AnimationReferences->Flinch, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Interrupt, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Knockdown, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Stun, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->AttackDeflect, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->BlockAttack, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->NormalAttacks, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Jump, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Dodge, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->LootStart, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->LootEnd, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->WeaponSwitchFullBody, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->WeaponSwitchUpperBody, AssetsToLoad);
		AddAnimationSoftObjectPathToArray(AnimationReferences->Die, AssetsToLoad);
	}

	// StreamableHandle = GameSingleton->StreamableManager.RequestAsyncLoad(AssetsToLoad);
	StreamableHandle = GameSingleton->StreamableManager.RequestSyncLoad(AssetsToLoad, true);
	return StreamableHandle;
}

EWeaponType AHumanCharacter::GetEquippedWeaponType() const
{
	return PrimaryWeapon ? PrimaryWeapon->GetWeaponType() : EWeaponType::None;
}

void AHumanCharacter::SetCurrentPrimaryWeapon(const FName WeaponID)
{
	//  You would call SetCurrentPrimaryWeapon(NAME_None) when you want to remove equipped primary weapon
	if (WeaponID == NAME_None)
	{
		RemovePrimaryWeapon();
		return;
	}

	FWeaponTableRow* WeaponData = UWeaponLibrary::GetWeaponData(WeaponID);
	// If it's an invalid weapon
	if (!WeaponData || WeaponData->WeaponMesh.IsNull())
	{
		return;
	}

	RemovePrimaryWeapon();
	if (UWeaponLibrary::IsWeaponDualHanded(WeaponData->WeaponType))
	{
		RemoveSecondaryWeapon();
	}
	PrimaryWeapon->OnEquip(WeaponID, WeaponData);
	PrimaryWeaponID = WeaponID;

	LoadAnimationReferencesForWeapon(WeaponData->WeaponType);
	// UpdateCurrentWeaponAnimationType();

	// @todo add weapon stats
}

void AHumanCharacter::SetCurrentSecondaryWeapon(const FName WeaponID)
{
	//  You would call SetCurrentSecondaryWeapon(NAME_None) when you want to remove equipped secondary weapon
	if (WeaponID == NAME_None)
	{
		RemoveSecondaryWeapon();
		return;
	}

	FWeaponTableRow* WeaponData = UWeaponLibrary::GetWeaponData(WeaponID);
	// If it's an invalid weapon
	if (!WeaponData || WeaponData->WeaponMesh.IsNull())
	{
		return;
	}

	// Since secondary weapon is guaranteed to be single handed
	RemoveSecondaryWeapon();
	if (UWeaponLibrary::IsWeaponDualHanded(PrimaryWeapon->GetWeaponType()))
	{
		RemovePrimaryWeapon();
	}
	SecondaryWeaponID = WeaponID;
	SecondaryWeapon->OnEquip(WeaponID, WeaponData);


	// @todo add weapon stats
}

void AHumanCharacter::RemovePrimaryWeapon()
{
	// OnPrimaryWeaponUnequipped.Broadcast(PrimaryWeaponID, PrimaryWeaponDataAsset);
	PrimaryWeaponID = NAME_None;
	// PrimaryWeaponDataAsset = nullptr;

	/*
	PrimaryWeapon->OnUnEquip();
	PrimaryWeaponID = NAME_None;
	UnloadEquippedWeaponAnimationReferences();
	*/

	// @todo remove weapon stats
}

void AHumanCharacter::RemoveSecondaryWeapon()
{
	// OnSecondaryWeaponUnequipped.Broadcast(SecondaryWeaponID, SecondaryWeaponDataAsset);
	SecondaryWeaponID = NAME_None;
	// SecondaryWeaponDataAsset = nullptr;

	/*
	SecondaryWeaponID = NAME_None;
	SecondaryWeapon->OnUnEquip();
	*/
	// @todo remove weapon stats
}

void AHumanCharacter::SetChestArmor(const FName& ArmorID)
{
}

void AHumanCharacter::SetHandsArmor(const FName& ArmorID)
{
}

void AHumanCharacter::SetLegsArmor(const FName& ArmorID)
{
}

void AHumanCharacter::SetFeetArmor(const FName& ArmorID)
{
}

void AHumanCharacter::RemoveChestArmor()
{
}

void AHumanCharacter::RemoveHandsArmor()
{
}

void AHumanCharacter::RemoveLegsArmor()
{
}

void AHumanCharacter::RemoveFeetArmor()
{
}

bool AHumanCharacter::CanDodge() const
{
	FPlayerAnimationReferencesTableRow* AnimationRef = GetActiveAnimationReferences();
	UAnimMontage* Animation = AnimationRef ? AnimationRef->Dodge.Get() : nullptr;

	// @todo add UsingSkill, Looting, Interacting, etc. to this too
	bool bStateAllowsDodge = IsIdleOrMoving() || IsBlocking() || IsCastingSpell() || IsNormalAttacking();

	// If we have a valid animation for dodge and the character state allows dodging
	return Animation && bStateAllowsDodge;
}

void AHumanCharacter::StartDodge()
{
	if (Controller && Controller->IsLocalController())
	{
		// DodgeIndex
		// 0 = Forward Dodge
		// 1 = Backward Dodge
		// 2 = Left Dodge
		// 3 = Right Dodge

		uint8 DodgeIndex = 0;
		if (ForwardAxisValue == 0)
		{
			if (RightAxisValue > 0)
			{
				DodgeIndex = 3;
			}
			else if (RightAxisValue < 0)
			{
				DodgeIndex = 2;
			}
			else
			{
				DodgeIndex = 1;
			}
		}
		else
		{
			if (ForwardAxisValue > 0)
			{
				DodgeIndex = 0;
			}
			else if (ForwardAxisValue < 0)
			{
				DodgeIndex = 1;
			}
		}

		float DesiredYaw = GetControllerRotationYaw();
		if (ForwardAxisValue != 0)
		{
			DesiredYaw = GetRotationYawFromAxisInput();
		}

		// Initiate dodge over network
		if (Role < ROLE_Authority)
		{
			Server_Dodge(DodgeIndex, DesiredYaw);
		}
		else
		{
			TriggeriFrames(DodgeImmunityDuration, DodgeImmunityTriggerDelay);
		}

		FCharacterStateInfo NewStateInfo(ECharacterState::Dodging, DodgeIndex);
		NewStateInfo.NewReplicationIndex = CharacterStateInfo.NewReplicationIndex + 1;
		CharacterStateInfo = NewStateInfo;

		SetActorRotation(FRotator(0.f, DesiredYaw, 0.f));
		UEODCharacterMovementComponent* MoveComp = Cast<UEODCharacterMovementComponent>(GetCharacterMovement());
		if (MoveComp)
		{
			MoveComp->bUseControllerDesiredRotation = false;
			MoveComp->SetDesiredCustomRotationYaw_LocalOnly(DesiredYaw);
		}
	}

	// Following variables are only relevant to owner
	bCharacterStateAllowsMovement = false;
	bCharacterStateAllowsRotation = false;

	FName SectionToPlay = NAME_None;
	if (CharacterStateInfo.SubStateIndex == 0)
	{
		SectionToPlay = UCharacterLibrary::SectionName_ForwardDodge;
	}
	else if (CharacterStateInfo.SubStateIndex == 1)
	{
		SectionToPlay = UCharacterLibrary::SectionName_BackwardDodge;
	}
	else if (CharacterStateInfo.SubStateIndex == 2)
	{
		SectionToPlay = UCharacterLibrary::SectionName_LeftDodge;
	}
	else if (CharacterStateInfo.SubStateIndex == 3)
	{
		SectionToPlay = UCharacterLibrary::SectionName_RightDodge;
	}

	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimMontage* DodgeMontage = AnimRef ? AnimRef->Dodge.Get() : nullptr;
	check(DodgeMontage);

	float MontageDuration = PlayAnimMontage(DodgeMontage, 1.f, SectionToPlay);

	// The total duration of dodge montage is 4 times the dodge animation duration since it includes dodge animations for all 4 directions (left, right, forward, backward)
	float ActualDuration = (MontageDuration / 4) - DodgeMontage->GetDefaultBlendOutTime();

	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().SetTimer(FinishDodgeTimerHandle, this, &AHumanCharacter::FinishDodge, ActualDuration, false);
}

void AHumanCharacter::CancelDodge()
{
	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().ClearTimer(FinishDodgeTimerHandle);

	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimMontage* DodgeMontage = AnimRef ? AnimRef->Dodge.Get() : nullptr;
	check(DodgeMontage);

	StopAnimMontage(DodgeMontage);
}

void AHumanCharacter::FinishDodge()
{
	ResetState();
}

bool AHumanCharacter::CanNormalAttack() const
{
	return IsIdleOrMoving() &&
		// The player must have a weapon equipped to normal attack and it shouldn't be sheathed
		(GetEquippedWeaponType() != EWeaponType::None && !IsWeaponSheathed());
}

void AHumanCharacter::StartNormalAttack()
{
	if (Controller && Controller->IsLocalController())
	{
		uint8 AttackIndex;
		if (bForwardPressed)
		{
			AttackIndex = 11;
		}
		else if (bBackwardPressed)
		{
			AttackIndex = 12;
		}
		else
		{
			AttackIndex = 1;
		}

		FCharacterStateInfo NewStateInfo(ECharacterState::Attacking, AttackIndex);
		NewStateInfo.NewReplicationIndex = CharacterStateInfo.NewReplicationIndex + 1;
		CharacterStateInfo = NewStateInfo;

		if (Role < ROLE_Authority)
		{
			Server_NormalAttack(AttackIndex);
		}

		UEODCharacterMovementComponent* MoveComp = Cast<UEODCharacterMovementComponent>(GetCharacterMovement());
		if (MoveComp)
		{
			MoveComp->bUseControllerDesiredRotation = false;
		}
	}

	bCharacterStateAllowsMovement = false;
	bCharacterStateAllowsRotation = false;

	// Determine what normal attack section should we start with
	FName SectionToPlay = NAME_None;
	if (CharacterStateInfo.SubStateIndex == 11)
	{
		SectionToPlay = UCharacterLibrary::SectionName_ForwardSPSwing;
	}
	else if (CharacterStateInfo.SubStateIndex == 12)
	{
		SectionToPlay = UCharacterLibrary::SectionName_BackwardSPSwing;
	}
	else if (CharacterStateInfo.SubStateIndex == 1)
	{
		SectionToPlay = UCharacterLibrary::SectionName_FirstSwing;
	}

	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimMontage* AttackMontage = AnimRef ? AnimRef->NormalAttacks.Get() : nullptr;

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage, 1.0, SectionToPlay);
	}
	// @note The rotation is handled through the AnimNotify_NormalAttack that gets called when a normal attack starts.
}

void AHumanCharacter::CancelNormalAttack()
{
	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimMontage* NormalAttackMontage = AnimRef ? AnimRef->NormalAttacks.Get() : nullptr;
	StopAnimMontage(NormalAttackMontage);
}

void AHumanCharacter::FinishNormalAttack()
{
	ResetState();
}

void AHumanCharacter::UpdateNormalAttackState(float DeltaTime)
{
	if (!bNormalAttackSectionChangeAllowed)
	{
		return;
	}

	// This update function must be called only on owner client
	if (!Controller || !Controller->IsLocalPlayerController())
	{
		return;
	}

	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimMontage* NormalAttackMontage = AnimRef ? AnimRef->NormalAttacks.Get() : nullptr;
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	if (!NormalAttackMontage || !AnimInstance)
	{
		return;
	}

	FName CurrentSection = AnimInstance->Montage_GetCurrentSection(NormalAttackMontage);
	FName ExpectedNextSection = GetNextNormalAttackSectionName(CurrentSection);
	if (ExpectedNextSection != NAME_None)
	{
		uint8 AttackIndex = GetNormalAttackIndex(ExpectedNextSection);
		FCharacterStateInfo StateInfo(ECharacterState::Attacking, AttackIndex);
		StateInfo.NewReplicationIndex = CharacterStateInfo.NewReplicationIndex + 1;
		CharacterStateInfo = StateInfo;

		if (Role < ROLE_Authority)
		{
			Server_NormalAttack(AttackIndex);
		}

		ChangeNormalAttackSection(CurrentSection, ExpectedNextSection);
	}

	bNormalAttackSectionChangeAllowed = false;
}

FName AHumanCharacter::GetNextNormalAttackSectionName(const FName& CurrentSection) const
{
	if (CurrentSection == UCharacterLibrary::SectionName_FirstSwing ||
		CurrentSection == UCharacterLibrary::SectionName_FirstSwingEnd)
	{
		return UCharacterLibrary::SectionName_SecondSwing;
	}
	else if (CurrentSection == UCharacterLibrary::SectionName_SecondSwing ||
		CurrentSection == UCharacterLibrary::SectionName_SecondSwingEnd)
	{
		return UCharacterLibrary::SectionName_ThirdSwing;
	}
	else if (CurrentSection == UCharacterLibrary::SectionName_ThirdSwing ||
		CurrentSection == UCharacterLibrary::SectionName_ThirdSwingEnd)
	{
		if (GetEquippedWeaponType() == EWeaponType::GreatSword ||
			GetEquippedWeaponType() == EWeaponType::WarHammer)
		{
			return NAME_None;
		}
		else
		{
			return UCharacterLibrary::SectionName_FourthSwing;
		}
	}
	else if (CurrentSection == UCharacterLibrary::SectionName_FourthSwing ||
		CurrentSection == UCharacterLibrary::SectionName_FourthSwingEnd)
	{
		if (GetEquippedWeaponType() == EWeaponType::Staff)
		{
			return NAME_None;
		}
		else
		{
			return UCharacterLibrary::SectionName_FifthSwing;
		}
	}

	return NAME_None;
}

void AHumanCharacter::ChangeNormalAttackSection(FName OldSection, FName NewSection)
{
	FPlayerAnimationReferencesTableRow* AnimRef = GetActiveAnimationReferences();
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	UAnimMontage* NormalAttackMontage = AnimRef ? AnimRef->NormalAttacks.Get() : nullptr;

	if (!NormalAttackMontage || !AnimInstance)
	{
		return;
	}

	if (OldSection == NAME_None)
	{
		AnimInstance->Montage_Play(NormalAttackMontage);
		AnimInstance->Montage_JumpToSection(NewSection, NormalAttackMontage);
	}
	else
	{
		FString OldSectionString = OldSection.ToString();
		if (OldSectionString.EndsWith("End"))
		{
			AnimInstance->Montage_Play(NormalAttackMontage);
			AnimInstance->Montage_JumpToSection(NewSection, NormalAttackMontage);
		}
		else
		{
			AnimInstance->Montage_SetNextSection(OldSection, NewSection, NormalAttackMontage);
		}
	}
}

void AHumanCharacter::OnNormalAttackSectionStart(FName SectionName)
{
	FString SkillIDString = FString("");
	SkillIDString += GetGenderPrefix();
	SkillIDString += GetEquippedWeaponPrefix();
	SkillIDString += GetNormalAttackSuffix(SectionName);

	if (IsValid(GetGameplaySkillsComponent()) && IsValid(GetController()) && GetController()->IsLocalPlayerController())
	{
		// GetGameplaySkillsComponent()->SetCurrentActiveSkill(FName(*SkillIDString));
	}

	// @todo set current active skill

	if (CurrentAttackInfoPtr.IsValid())
	{
		CurrentAttackInfoPtr.Reset();
	}

	CurrentAttackInfoPtr = MakeShareable(new FAttackInfo);
	if (CurrentAttackInfoPtr.IsValid())
	{
		CurrentAttackInfoPtr->bUnblockable = false;
		CurrentAttackInfoPtr->bUndodgable = false;
		CurrentAttackInfoPtr->CrowdControlEffect = ECrowdControlEffect::Flinch;
		CurrentAttackInfoPtr->DamageType = EDamageType::Physical;
	}
}

void AHumanCharacter::SetNormalAttackSectionChangeAllowed(bool bNewValue)
{
	bNormalAttackSectionChangeAllowed = bNewValue;
}

void AHumanCharacter::OnPressedForward()
{
	UWorld* World = GetWorld();
	if (World)
	{
		bBackwardPressed = false;
		bForwardPressed = true;
		World->GetTimerManager().SetTimer(SPAttackTimerHandle, this, &AHumanCharacter::DisableForwardPressed, 0.1f, false);
	}
}

void AHumanCharacter::OnPressedBackward()
{
	UWorld* World = GetWorld();
	if (World)
	{
		bForwardPressed = false;
		bBackwardPressed = true;
		World->GetTimerManager().SetTimer(SPAttackTimerHandle, this, &AHumanCharacter::DisableBackwardPressed, 0.1f, false);
	}
}

void AHumanCharacter::OnReleasedForward()
{
}

void AHumanCharacter::OnReleasedBackward()
{
}

void AHumanCharacter::DisableForwardPressed()
{
	bForwardPressed = false;
}

void AHumanCharacter::DisableBackwardPressed()
{
	bBackwardPressed = false;
}

void AHumanCharacter::TurnOnTargetSwitch()
{
	// Change material parameter only if character is NOT controlled by local player
	if (GetController() && GetController()->IsLocalPlayerController())
	{
		Super::TurnOnTargetSwitch();

		Hair->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		HatItem->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		FaceItem->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		Chest->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		Hands->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		Legs->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
		Feet->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 1.f);
	}
}

void AHumanCharacter::TurnOffTargetSwitch()
{
	// Change material parameter only if character is NOT controlled by local player
	if (GetController() && GetController()->IsLocalPlayerController())
	{
		Super::TurnOnTargetSwitch();

		Hair->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		HatItem->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		FaceItem->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		Chest->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		Hands->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		Legs->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
		Feet->SetScalarParameterValueOnMaterials(MaterialParameterNames::TargetSwitchOn, 0.f);
	}


	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
	}
}

void AHumanCharacter::OnRep_PrimaryWeaponID()
{
}

void AHumanCharacter::OnRep_SecondaryWeaponID()
{
}

void AHumanCharacter::OnRep_ArmorIDs()
{
}
