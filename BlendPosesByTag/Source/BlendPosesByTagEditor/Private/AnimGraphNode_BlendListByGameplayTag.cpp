// Copyright Guilty Saint Games 2026, All Rights Reserved.


#include "AnimGraphNode_BlendListByGameplayTag.h"
#include "BlendPosesByTag.h"

#include "ToolMenus.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

#include "Modules/ModuleManager.h"
#include "GameplayTagsEditorModule.h"
#include "SGameplayTagWidget.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimGraphNode_BlendListByGameplayTag)

DEFINE_LOG_CATEGORY(LogBlendPosesByTag);

#define LOCTEXT_NAMESPACE "A3Nodes"

/////////////////////////////////////////////////////
// UAnimGraphNode_BlendListByGameplayTag

UAnimGraphNode_BlendListByGameplayTag::UAnimGraphNode_BlendListByGameplayTag(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Make sure we start out with a pin
	Node.AddPose();
}

// UEdGraphNode Base interface
FText UAnimGraphNode_BlendListByGameplayTag::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Tooltip", "Blend Poses (by gameplay tag)");
}
FText UAnimGraphNode_BlendListByGameplayTag::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Title", "Blend Poses by Gameplay Tag");
}
void UAnimGraphNode_BlendListByGameplayTag::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	FString PinName = TEXT("Default");
	if (ArrayIndex != Node.GetDefaultPinIndex())
	{
		int32 ExposedTagIndex = Node.ToPoseTagIndex(ArrayIndex);
		
		if (!Node.PoseTags.IsValidIndex(ExposedTagIndex))
		{
			return;
		}
		
		const FGameplayTag& TagAtIndex = Node.PoseTags[ExposedTagIndex];
		PinName = GetGameplayTagLeafName(TagAtIndex);
	}

	FFormatNamedArguments Args;
	Args.Add(TEXT("TagName"), FText::FromString(PinName));

	if (SourcePropertyName == TEXT("BlendPose"))
	{
		Pin->PinFriendlyName = FText::Format(NSLOCTEXT("A3Nodes", "TagPoseFriendlyName", "{TagName} Pose"), Args);
	}
	else if (SourcePropertyName == TEXT("BlendTime"))
	{
		Pin->PinFriendlyName = FText::Format(NSLOCTEXT("A3Nodes", "TagBlendTimeFriendlyName", "{TagName} Blend Time"), Args);
	}
}
// End UEdGraphNode Base interface

// UK2Node interface
void UAnimGraphNode_BlendListByGameplayTag::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		if (const UEdGraphPin* Pin = Context->Pin)
		{
			// CLICKED ON PIN
			FProperty* AssociatedProperty;
			int32 PinIndex;
			GetPinAssociatedProperty(GetFNodeType(), Pin, /*out*/ AssociatedProperty, /*out*/ PinIndex);

			if (PinIndex != Node.GetDefaultPinIndex())
			{
				// Offer remove pin action
				AddContextMenuAction_RemoveBlendPin(Menu, Pin);
			}
		}

		FToolMenuSection* Section = &Menu->AddSection(
			"AnimGraphNodeBlendPosesByTagSection",
			NSLOCTEXT("A3Nodes", "ExposeHeader", "Blend Poses by Tag")
		);

		AddContextMenuAction_SelectGameplayTags(Menu, Section);
	}
}
// End UK2Node interface

// Single Pin Actions
void UAnimGraphNode_BlendListByGameplayTag::AddBlendPinForGameplayTag(const FGameplayTag& TagToExpose)
{
	if (!Node.PoseTags.Contains(TagToExpose))
	{
		FScopedTransaction Transaction(NSLOCTEXT("A3Nodes", "AddBlendPinForGameplayTag", "Add Blend Pin for Gameplay Tag"));
		Modify();

		// Add the Tag to our runtime profile
		Node.PoseTags.Add(TagToExpose);

		// BlendListBase Interface - Add a matching Pose Pin
		Node.AddPose();

		// Update Graph
		ReconstructNode();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}
void UAnimGraphNode_BlendListByGameplayTag::RemoveBlendPinForGameplayTag(const FGameplayTag& TagToRemove)
{
	if (Node.PoseTags.IsEmpty())
	{
		return;
	}

	// Mark for undo
	FScopedTransaction Transaction(NSLOCTEXT("A3Nodes", "RemoveTag", "Remove Tag Pin"));
	Modify();

	int32 PoseTagIndex = Node.PoseTags.Find(TagToRemove);
	if (!Node.PoseTags.IsValidIndex(PoseTagIndex))
	{
		UE_LOG(LogBlendPosesByTag, Warning, TEXT("Tag %s not found in node pose tags"), *TagToRemove.ToString());
		return;
	}

	// Remove the tag from our runtime profile
	Node.PoseTags.RemoveAt(PoseTagIndex);

	// Remove the matching Pose Pin
	RemovedPinArrayIndex = Node.ToBlendPinIndex(PoseTagIndex); // Used by ReconstructNode to clean up links
	Node.RemovePose(RemovedPinArrayIndex);

	// Reconstruct the node & update the graph (remove Blend Pose Time pins as we go)
	ReconstructNode();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}
void UAnimGraphNode_BlendListByGameplayTag::RemoveBlendPin(const UEdGraphPin* Pin)
{
	// Called when the user right-clicks to remove a pose pin
	FScopedTransaction Transaction(NSLOCTEXT("A3Nodes", "RemoveTag", "Remove Tag Pin"));
	Modify();

	FProperty* AssociatedProperty;
	int32 PinIndex;
	GetPinAssociatedProperty(GetFNodeType(), Pin, /*out*/ AssociatedProperty, /*out*/ PinIndex); // Returns Real Pin Index

	// Remove the matching Pose Pin
	RemovedPinArrayIndex = PinIndex; // Used by ReconstructNode to clean up links
	Node.RemovePose(PinIndex);

	// Remove the tag from our runtime profile
	int32 PoseTagIndex = Node.ToPoseTagIndex(PinIndex); // Convert to Exposed PoseTagIndex
	if (Node.PoseTags.IsValidIndex(PoseTagIndex))
	{
		Node.PoseTags.RemoveAt(PoseTagIndex);
	}

	// Update the graph
	ReconstructNode();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}
// End Single Pin Actions

// Bulk Node Actions
void UAnimGraphNode_BlendListByGameplayTag::RemoveAllBlendPins()
{
	if (Node.PoseTags.IsEmpty()) return;

	// 1. Mark for undo
	FScopedTransaction Transaction(NSLOCTEXT("A3Nodes", "RemoveTag", "Remove Tag Pin"));
	Modify();

	// 2. Clear all tags and pose pins
	for (int32 ExposedTagIndex = Node.PoseTags.Num() - 1; ExposedTagIndex >= 0; --ExposedTagIndex)
	{
		// 1. Remove the tag from our runtime profile
		Node.PoseTags.RemoveAt(ExposedTagIndex);

		// 2. Remove the matching Pose Pin
		int32 PinIndex = Node.ToBlendPinIndex(ExposedTagIndex);
		RemovedPinArrayIndex = PinIndex; // Used by ReconstructNode to clean up links
		Node.RemovePose(PinIndex);

		// 3. Reconstruct the node (remove Blend Pose Time pins as we go)
		ReconstructNode();
	}

	// 3. Update the graph
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}
// End Bulk Node Actions

void UAnimGraphNode_BlendListByGameplayTag::OpenGameplayTagSelectionWindow()
{
	IGameplayTagsEditorModule& TagsModule = IGameplayTagsEditorModule::Get();

	// Pre-fill with existing tags
	TSharedPtr<FGameplayTagContainer> PendingTagContainer = MakeShared<FGameplayTagContainer>();
	for (const FGameplayTag& PoseTag : Node.PoseTags)
	{
		PendingTagContainer->AddTag(PoseTag);
	}
	
	if (GameplayTagSelectionWindow.IsValid())
	{
		// A Gameplay Tag Picker Window is already open, close it before opening a new one
		GameplayTagSelectionWindow->RequestDestroyWindow();
		GameplayTagSelectionWindow.Reset();
	}

	TSharedRef<SWidget> GameplayTagSelectionWidget =
		TagsModule.MakeGameplayTagContainerWidget(
			FOnSetGameplayTagContainer::CreateUObject(
				this,
				&UAnimGraphNode_BlendListByGameplayTag::OnGameplayTagSelectionChanged
			),
			PendingTagContainer,
			Node.GetGameplayTagFilterString()
		);

	GameplayTagSelectionWindow = SNew(SWindow)
		.Title(FText::FromString("Select Gameplay Tags"))
		.ClientSize(FVector2D(400, 600))
		.FocusWhenFirstShown(true)
		.IsTopmostWindow(true)
		[
			GameplayTagSelectionWidget
		];

	FSlateApplication::Get().AddWindow(GameplayTagSelectionWindow.ToSharedRef());
}
void UAnimGraphNode_BlendListByGameplayTag::OnGameplayTagSelectionChanged(const FGameplayTagContainer& SelectedTags)
{
	TSet<FGameplayTag> SelectedTagsSet(SelectedTags.GetGameplayTagArray());
	TSet<FGameplayTag> ExistingTagsSet(Node.PoseTags);
	TSet<FGameplayTag> AddedTags = SelectedTagsSet.Difference(ExistingTagsSet); // Get the tags that are selected but don't exist in the node yet
	TSet<FGameplayTag> DeselectedTags = ExistingTagsSet.Difference(SelectedTagsSet); // Get the tags that still exist in the node but no longer selected

	// If there is no difference between selection and exisitng poses, return
	if (AddedTags.IsEmpty() && DeselectedTags.IsEmpty())
	{
		return;
	}

	// Remove all deselected tags
	for (const FGameplayTag& DeselectedTag : DeselectedTags)
	{
		RemoveBlendPinForGameplayTag(DeselectedTag);
	}

	// Make container to ensure we don't add unnecessary parent tags as pins
	FGameplayTagContainer ExisitingTagsContainer;
	for (auto& ExistingTag : Node.PoseTags)
	{
		ExisitingTagsContainer.AddTag(ExistingTag);
	}

	// Add relevant selected tags
	for (const FGameplayTag& AddedTag : AddedTags)
	{
		// Ensure we don't add parent tags in the case of Existing = {A.1} and we are trying to Added = {A}
		// This can occur because we used TSet<FGameplatyTag>::Difference logic, instead of FGameplayTagContainer::HasTag() comparisons
		if (!ExisitingTagsContainer.HasTag(AddedTag))
		{
			AddBlendPinForGameplayTag(AddedTag);
		}
	}
}

// Context Menu Actions
void UAnimGraphNode_BlendListByGameplayTag::AddContextMenuAction_SelectGameplayTags(UToolMenu* Menu, FToolMenuSection* Section) const
{
	FUIAction Action = FUIAction(FExecuteAction::CreateUObject(
		const_cast<UAnimGraphNode_BlendListByGameplayTag*>(this),
		&UAnimGraphNode_BlendListByGameplayTag::OpenGameplayTagSelectionWindow
	));

	FText PrettyActionName = LOCTEXT("OpenGameplayTagSelectionWindow", "Select Gameplay Tags");
	FText ToolTip = LOCTEXT("OpenGameplayTagSelectionWindowToolTip", "Select the Gameplay Tags to use as Pose Pins");
	Section->AddMenuEntry("OpenGameplayTagSelectionWindow", PrettyActionName, ToolTip, FSlateIcon(), Action);
}
void UAnimGraphNode_BlendListByGameplayTag::AddContextMenuAction_RemoveBlendPin(UToolMenu* Menu, const UEdGraphPin* Pin) const
{
	// REMOVE PIN ACTION
	FUIAction Action = FUIAction(FExecuteAction::CreateUObject(
		const_cast<UAnimGraphNode_BlendListByGameplayTag*>(this), 
		&UAnimGraphNode_BlendListByGameplayTag::RemoveBlendPin, 
		Pin
	));

	FToolMenuSection& Section = Menu->AddSection("RemovePose");
	Section.AddMenuEntry("RemovePose", LOCTEXT("RemovePose", "Remove Pose"), FText::GetEmpty(), FSlateIcon(), Action);
}
void UAnimGraphNode_BlendListByGameplayTag::AddContextMenuAction_RemoveAllBlendPins(UToolMenu* Menu, FToolMenuSection* Section) const
{
	FUIAction Action = FUIAction(FExecuteAction::CreateUObject(
		const_cast<UAnimGraphNode_BlendListByGameplayTag*>(this),
		&UAnimGraphNode_BlendListByGameplayTag::RemoveAllBlendPins
	));

	FText PrettyActionName = LOCTEXT("RemoveAllBlendPins", "Remove All Blend Pins");
	Section->AddMenuEntry("RemoveAllBlendListPins", PrettyActionName, PrettyActionName, FSlateIcon(), Action);
}
//End Context Menu Actions

FString UAnimGraphNode_BlendListByGameplayTag::GetGameplayTagLeafName(const FGameplayTag& Tag)
{
	FString TagString = Tag.ToString();
	FString TagLeafName;
	if (int32 LastDotIndex; TagString.FindLastChar('.', LastDotIndex))
	{
		TagLeafName = TagString.Mid(LastDotIndex + 1);
	}

	return TagLeafName.IsEmpty() ? TagString : TagLeafName;
}

#undef LOCTEXT_NAMESPACE
	