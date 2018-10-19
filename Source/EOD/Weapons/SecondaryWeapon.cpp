// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "SecondaryWeapon.h"
#include "Player/PlayerCharacter.h"
#include "Weapons/PrimaryWeapon.h"

#include "Components/SkeletalMeshComponent.h"

ASecondaryWeapon::ASecondaryWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// This actor doesn't tick
	PrimaryActorTick.bCanEverTick = false;

	LeftHandWeaponMeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Left Hand Weapon"));
	SheathedWeaponMeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Sheathed Weapon"));
	FallenWeaponMeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Fallen Weapon"));

	// Weapon's skeletal components do not have any collision. Also, we do not setup attachment for these components at construction.
	LeftHandWeaponMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	LeftHandWeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandWeaponMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	SheathedWeaponMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	SheathedWeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SheathedWeaponMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	FallenWeaponMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	FallenWeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FallenWeaponMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	RootComponent = LeftHandWeaponMeshComp;

	// @todo Render settings
	// RightHandWeaponMeshComp->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
}

void ASecondaryWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecondaryWeapon::OnEquip(FName NewWeaponID, FWeaponTableRow * NewWeaponData)
{
	APlayerCharacter* OwningPlayer = Cast<APlayerCharacter>(GetOwner());
	check(NewWeaponData && OwningPlayer);

	USkeletalMesh* NewSkeletalMesh = nullptr;
	if (NewWeaponData->WeaponMesh.IsPending())
	{
		NewSkeletalMesh = NewWeaponData->WeaponMesh.LoadSynchronous();
	}
	else if (NewWeaponData->WeaponMesh.IsValid())
	{
		NewSkeletalMesh = NewWeaponData->WeaponMesh.Get();
	}

	LeftHandWeaponMeshComp->Activate();
	SheathedWeaponMeshComp->Activate();
	FallenWeaponMeshComp->Activate();

	LeftHandWeaponMeshComp->SetSkeletalMesh(NewSkeletalMesh);
	SheathedWeaponMeshComp->SetSkeletalMesh(NewSkeletalMesh);
	FallenWeaponMeshComp->SetSkeletalMesh(NewSkeletalMesh);

	if (NewWeaponData->WeaponType == EWeaponType::Dagger)
	{
		LeftHandWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("DGL"));
		SheathedWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("DGL_b"));
		FallenWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("DGL_w"));
	}
	else if (NewWeaponData->WeaponType == EWeaponType::Shield)
	{
		LeftHandWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SLD"));
		SheathedWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SLD_b"));
		FallenWeaponMeshComp->AttachToComponent(OwningPlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SLD_w"));
	}

	/*
	// Change player animation references if the newer weapon type is different from previous weapon type
	if (NewWeaponData->WeaponType != WeaponType && NewWeaponData->WeaponType == EWeaponType::Dagger)
	{
		OwningPlayer->UpdateEquippedWeaponAnimationReferences(NewWeaponData->WeaponType);
		// OwningPlayer->UpdateNormalAttackSectionToSkillMap();
	}
	*/

	bEquipped = true;
	WeaponID = NewWeaponID;
	WeaponType = NewWeaponData->WeaponType;

	// @todo intialize weapon stats
}

void ASecondaryWeapon::OnUnEquip()
{
	LeftHandWeaponMeshComp->SetSkeletalMesh(nullptr);
	SheathedWeaponMeshComp->SetSkeletalMesh(nullptr);
	FallenWeaponMeshComp->SetSkeletalMesh(nullptr);
	
	FDetachmentTransformRules DetachmentRules(FAttachmentTransformRules::KeepRelativeTransform, true);
	LeftHandWeaponMeshComp->DetachFromComponent(DetachmentRules);
	SheathedWeaponMeshComp->DetachFromComponent(DetachmentRules);
	FallenWeaponMeshComp->DetachFromComponent(DetachmentRules);

	LeftHandWeaponMeshComp->Deactivate();
	SheathedWeaponMeshComp->Deactivate();
	FallenWeaponMeshComp->Deactivate();

	bEquipped = false;
	WeaponID = NAME_None;
	WeaponType = EWeaponType::None;
	// @todo reset weapon stats

	/*
	// If no primary weapon is equipped by owning player then update player animation references to 'no weapon equipped'
	if (OwningPlayer && OwningPlayer->GetPrimaryWeapon() && !OwningPlayer->GetPrimaryWeapon()->bEquipped)
	{
		OwningPlayer->UpdateEquippedWeaponAnimationReferences(EWeaponType::None);
	}
	*/
}
