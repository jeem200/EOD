// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "DynamicSkillTreeWidget.h"
#include "SkillPointsInfoWidget.h"

#include "Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanelSlot.h"

UDynamicSkillTreeWidget::UDynamicSkillTreeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UDynamicSkillTreeWidget::Initialize()
{
	if (Super::Initialize() &&
		AssassinCanvas &&
		BerserkerCanvas &&
		ClericCanvas &&
		DefenderCanvas &&
		SorcererCanvas &&
		AssassinInfoCanvas &&
		BerserkerInfoCanvas &&
		ClericInfoCanvas &&
		DefenderInfoCanvas &&
		SorcererInfoCanvas &&
		SkillTreeSwitcher &&
		AssassinTab &&
		BerserkerTab &&
		ClericTab &&
		DefenderTab &&
		SorcererTab &&
		SkillTreeSwitcher)
	{
		DefaultButtonStyle = AssassinTab->WidgetStyle;

		AssassinTab->OnClicked.AddDynamic(this, &UDynamicSkillTreeWidget::ActivateAssassinTab);
		BerserkerTab->OnClicked.AddDynamic(this, &UDynamicSkillTreeWidget::ActivateBerserkerTab);
		ClericTab->OnClicked.AddDynamic(this, &UDynamicSkillTreeWidget::ActivateClericTab);
		DefenderTab->OnClicked.AddDynamic(this, &UDynamicSkillTreeWidget::ActivateDefenderTab);
		SorcererTab->OnClicked.AddDynamic(this, &UDynamicSkillTreeWidget::ActivateSorcererTab);

		// Berserker tab will open by default
		ActivateBerserkerTab();

		return true;
	}

	return false;
}

void UDynamicSkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDynamicSkillTreeWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDynamicSkillTreeWidget::ActivateAssassinTab()
{
	if (CurrentActiveTabIndex != 0 && SkillTreeSwitcher)
	{
		ResetTabStyleByIndex(CurrentActiveTabIndex);
		SetButtonStyleToSelected(AssassinTab);
		SkillTreeSwitcher->SetActiveWidgetIndex(0);
		CurrentActiveTabIndex = 0;

		AddSkillPointsInfoToCanvas(AssassinInfoCanvas);
	}
}

void UDynamicSkillTreeWidget::ActivateBerserkerTab()
{
	if (CurrentActiveTabIndex != 1 && SkillTreeSwitcher)
	{
		ResetTabStyleByIndex(CurrentActiveTabIndex);
		SetButtonStyleToSelected(BerserkerTab);
		SkillTreeSwitcher->SetActiveWidgetIndex(1);
		CurrentActiveTabIndex = 1;

		AddSkillPointsInfoToCanvas(BerserkerInfoCanvas);
	}
}

void UDynamicSkillTreeWidget::ActivateClericTab()
{
	if (CurrentActiveTabIndex != 2 && SkillTreeSwitcher)
	{
		ResetTabStyleByIndex(CurrentActiveTabIndex);
		SetButtonStyleToSelected(ClericTab);
		SkillTreeSwitcher->SetActiveWidgetIndex(2);
		CurrentActiveTabIndex = 2;

		AddSkillPointsInfoToCanvas(ClericInfoCanvas);
	}
}

void UDynamicSkillTreeWidget::ActivateDefenderTab()
{
	if (CurrentActiveTabIndex != 3 && SkillTreeSwitcher)
	{
		ResetTabStyleByIndex(CurrentActiveTabIndex);
		SetButtonStyleToSelected(DefenderTab);
		SkillTreeSwitcher->SetActiveWidgetIndex(3);
		CurrentActiveTabIndex = 3;

		AddSkillPointsInfoToCanvas(DefenderInfoCanvas);
	}
}

void UDynamicSkillTreeWidget::ActivateSorcererTab()
{
	if (CurrentActiveTabIndex != 4 && SkillTreeSwitcher)
	{
		ResetTabStyleByIndex(CurrentActiveTabIndex);
		SetButtonStyleToSelected(SorcererTab);
		SkillTreeSwitcher->SetActiveWidgetIndex(4);
		CurrentActiveTabIndex = 4;

		AddSkillPointsInfoToCanvas(SorcererInfoCanvas);
	}
}

void UDynamicSkillTreeWidget::AddSkillPointsInfoToCanvas(UCanvasPanel* CPanel)
{
	if (CPanel && SkillPointsInfo)
	{
		UCanvasPanelSlot* CPSlot = CPanel->AddChildToCanvas(SkillPointsInfo);
		CPSlot->SetSize(FVector2D(400.f, 320.f));
		CPSlot->SetPosition(FVector2D(0.f, -25.f));
		CPSlot->SetAnchors(FAnchors(0.5f, 1.f, 0.5f, 1.f));
		CPSlot->SetAlignment(FVector2D(0.5f, 1.f));
	}
}