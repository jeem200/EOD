// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOD/Core/EODPreprocessors.h"
#include "EOD/Statics/CharacterLibrary.h"

#include "Components/ActorComponent.h"
#include "SkillBarComponent.generated.h"

class AEODCharacterBase;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EOD_API USkillBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillBarComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Sets up property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	////////////////////////////////////////////////////////////////////////////////
	// EOD
	////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY()
	AEODCharacterBase* CharacterOwner;

	UPROPERTY()
	FName ActiveSkillID;

	FSkillTableRow* ActiveSkill;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "EOD Character Skills")
	float ChainSkillResetDelay;

	/** Data table for character skills */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skills)
	UDataTable* SkillsDataTable;

	FORCEINLINE FSkillTableRow* GetSkill(FName SkillID, const FString& ContextString = FString("AEODCharacterBase::GetSkill(), character skill lookup")) const
	{
		return SkillsDataTable ? SkillsDataTable->FindRow<FSkillTableRow>(SkillID, ContextString) : nullptr;
	}

public:
	void SetCurrentActiveSkill(const FName SkillID);

	FORCEINLINE FName GetCurrentActiveSkillID() const { return ActiveSkillID; }

	FORCEINLINE FSkillTableRow* GetCurrentActiveSkill() const { return ActiveSkill; }


	////////////////////////////////////////////////////////////////////////////////
	// NETWORK
	////////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCurrentActiveSkill(const FName SkillID);


		
};