// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EODLibrary.h"

#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UImage;
class UScrollBox;
class UGridPanel;

/**
 * InventoryWidget is the visual representation of player inventory
 */
UCLASS()
class EOD_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void AddItem(const FInventoryItem& Item);

	UFUNCTION()
	void RemoveItem(const FInventoryItem& Item);

	// void RemoveItem()

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UGridPanel* GridPanel;
	*/
	

	
};
