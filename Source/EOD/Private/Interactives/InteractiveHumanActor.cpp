// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "InteractiveHumanActor.h"
#include "EODGlobalNames.h"

#include "Engine/CollisionProfile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

const FName AInteractiveHumanActor::CapsuleComponentName(TEXT("CollisionCylinder"));
const FName AInteractiveHumanActor::HairComponentName(TEXT("Hair"));
const FName AInteractiveHumanActor::HatItemComponentName(TEXT("Hat Item"));
const FName AInteractiveHumanActor::FaceComponentName(TEXT("Face"));
const FName AInteractiveHumanActor::FaceItemComponentName(TEXT("Face Item"));
const FName AInteractiveHumanActor::ChestComponentName(TEXT("Chest"));
const FName AInteractiveHumanActor::HandsComponentName(TEXT("Hands"));
const FName AInteractiveHumanActor::LegsComponentName(TEXT("Legs"));
const FName AInteractiveHumanActor::FeetComponentName(TEXT("Feet"));
const FName AInteractiveHumanActor::InteractionCameraComponentName(TEXT("Camera"));

AInteractiveHumanActor::AInteractiveHumanActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(AInteractiveHumanActor::CapsuleComponentName);
	if (CapsuleComponent)
	{
		CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
		CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

		CapsuleComponent->CanCharacterStepUpOn = ECB_No;
		CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
		CapsuleComponent->SetCanEverAffectNavigation(false);
		CapsuleComponent->bDynamicObstacle = true;
		RootComponent = CapsuleComponent;
	}

	Face = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, AInteractiveHumanActor::FaceComponentName);
	if (Face)
	{
		Face->SetupAttachment(CapsuleComponent);
		Face->SetReceivesDecals(false);
		Face->bUseAttachParentBound = true;
	}

	InteractionCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, AInteractiveHumanActor::InteractionCameraComponentName);
	if (InteractionCamera)
	{
		InteractionCamera->SetupAttachment(RootComponent);
		InteractionCamera->AddLocalRotation(FRotator(0.f, 180.f, 0.f));
		InteractionCamera->SetWorldLocation(FVector(400.f, 0.f, 0.f));
	}

	Hair				= CreateNewArmorComponent(AInteractiveHumanActor::HairComponentName, ObjectInitializer);
	HatItem				= CreateNewArmorComponent(AInteractiveHumanActor::HatItemComponentName, ObjectInitializer);
	FaceItem			= CreateNewArmorComponent(AInteractiveHumanActor::FaceItemComponentName, ObjectInitializer);
	Chest				= CreateNewArmorComponent(AInteractiveHumanActor::ChestComponentName, ObjectInitializer);
	Hands				= CreateNewArmorComponent(AInteractiveHumanActor::HandsComponentName, ObjectInitializer);
	Legs				= CreateNewArmorComponent(AInteractiveHumanActor::LegsComponentName, ObjectInitializer);
	Feet				= CreateNewArmorComponent(AInteractiveHumanActor::FeetComponentName, ObjectInitializer);

	SetMasterPoseComponentForMeshes();

	ClothOverrideColor = FLinearColor::Black;
	HairOverrideColor = FLinearColor::Black;
	HatItemOverrideColor = FLinearColor::Black;
}

void AInteractiveHumanActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMeshColors();
}

void AInteractiveHumanActor::BeginPlay()
{
	Super::BeginPlay();

	// Since setting master pose component from constructor doesn't work in packaged game
	SetMasterPoseComponentForMeshes();

	DeleteUnusedComponents();
}

void AInteractiveHumanActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractiveHumanActor::EnableCustomDepth_Implementation()
{
	if (Hair) { Hair->SetRenderCustomDepth(true); }
	if (HatItem) { HatItem->SetRenderCustomDepth(true); }
	if (Face) { Face->SetRenderCustomDepth(true); }
	if (FaceItem) { FaceItem->SetRenderCustomDepth(true); }
	if (Chest) { Chest->SetRenderCustomDepth(true); }
	if (Hands) { Hands->SetRenderCustomDepth(true); }
	if (Legs) { Legs->SetRenderCustomDepth(true); }
	if (Feet) { Feet->SetRenderCustomDepth(true); }
}

void AInteractiveHumanActor::DisableCustomDepth_Implementation()
{
	if (Hair) { Hair->SetRenderCustomDepth(false); }
	if (HatItem) { HatItem->SetRenderCustomDepth(false); }
	if (Face) { Face->SetRenderCustomDepth(false); }
	if (FaceItem) { FaceItem->SetRenderCustomDepth(false); }
	if (Chest) { Chest->SetRenderCustomDepth(false); }
	if (Hands) { Hands->SetRenderCustomDepth(false); }
	if (Legs) { Legs->SetRenderCustomDepth(false); }
	if (Feet) { Feet->SetRenderCustomDepth(false); }
}

void AInteractiveHumanActor::OnInteract_Implementation(AEODCharacterBase* Character)
{
}

USkeletalMeshComponent* AInteractiveHumanActor::CreateNewArmorComponent(const FName Name, const FObjectInitializer& ObjectInitializer)
{
	USkeletalMeshComponent* Sk = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, Name);
	if (Sk)
	{
		Sk->SetupAttachment(Face);
		Sk->SetReceivesDecals(false);
		Sk->bUseAttachParentBound = true;
	}
	return Sk;
}

void AInteractiveHumanActor::DeleteUnusedComponents()
{
	TArray<UActorComponent*> SkeletalMeshComponentArray = this->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	for (UActorComponent* ActorComp : SkeletalMeshComponentArray)
	{
		USkeletalMeshComponent* SMComp = Cast<USkeletalMeshComponent>(ActorComp);
		if (SMComp && !IsValid(SMComp->SkeletalMesh))
		{
			SMComp->Deactivate();
			SMComp->DestroyComponent();
			SMComp = nullptr;
		}
	}
}

void AInteractiveHumanActor::UpdateMeshColors()
{
	TArray<UActorComponent*> SkeletalMeshComponentArray = this->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	for (UActorComponent* ActorComp : SkeletalMeshComponentArray)
	{
		USkeletalMeshComponent* SMComp = Cast<USkeletalMeshComponent>(ActorComp);
		if (SMComp)
		{
			if (SMComp->GetFName() == AInteractiveHumanActor::HairComponentName)
			{
				SMComp->SetVectorParameterValueOnMaterials(MaterialParameterNames::BaseColor, FVector(HairOverrideColor));
			}
			else if (SMComp->GetFName() == AInteractiveHumanActor::HatItemComponentName)
			{
				SMComp->SetVectorParameterValueOnMaterials(MaterialParameterNames::BaseColor, FVector(HatItemOverrideColor));
			}
			else
			{
				SMComp->SetVectorParameterValueOnMaterials(MaterialParameterNames::BaseColor, FVector(ClothOverrideColor));
			}
		}
	}
}
