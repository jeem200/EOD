// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Effects/GameplayEffectBase.h"
#include "MovementBuff.generated.h"

/**
 * 
 */
UCLASS()
class EOD_API UMovementBuff : public UGameplayEffectBase
{
	GENERATED_BODY()

public:

	UMovementBuff(const FObjectInitializer& ObjectInitializer);

	// --------------------------------------
	//	Gameplay Effect Interface
	// --------------------------------------

	virtual void ActivateEffect() override;

	virtual void DeactivateEffect() override;

	virtual void UpdateEffect(float DeltaTime) override;

	// --------------------------------------
	//  Pseudo Constants
	// --------------------------------------

	FTimerHandle MovementEndTimerHandle;

};
