// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "BTTask_RotateToFaceEnemy.h"
#include "Player/EODCharacterBase.h"
#include "Statics/AILibrary.h"

#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_RotateToFaceEnemy::UBTTask_RotateToFaceEnemy(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer), Precision(0.1f)
{
	NodeName = "Rotate to face BB entry";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RotateToFaceEnemy::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	AEODCharacterBase* OwningCharacter = Cast<AEODCharacterBase>(AIController->GetPawn());
	AEODCharacterBase* TargetEnemy = Cast<AEODCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(UAILibrary::BBKey_TargetEnemy));

	FVector OrientationVector = TargetEnemy->GetActorLocation() - OwningCharacter->GetActorLocation();
	FRotator OrientationRotator = OrientationVector.ToOrientationRotator();

	bool bResult = OwningCharacter->DeltaRotateCharacterToDesiredYaw(OrientationRotator.Yaw, 0.f, Precision);
	if (bResult)
	{
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::InProgress;
	}
}

void UBTTask_RotateToFaceEnemy::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	AEODCharacterBase* OwningCharacter = Cast<AEODCharacterBase>(AIController->GetPawn());
	AEODCharacterBase* TargetEnemy = Cast<AEODCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(UAILibrary::BBKey_TargetEnemy));

	FVector OrientationVector = TargetEnemy->GetActorLocation() - OwningCharacter->GetActorLocation();
	FRotator OrientationRotator = OrientationVector.ToOrientationRotator();

	bool bResult = OwningCharacter->DeltaRotateCharacterToDesiredYaw(OrientationRotator.Yaw, DeltaSeconds, Precision);
	if (bResult)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_RotateToFaceEnemy::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	return EBTNodeResult::Type();
}