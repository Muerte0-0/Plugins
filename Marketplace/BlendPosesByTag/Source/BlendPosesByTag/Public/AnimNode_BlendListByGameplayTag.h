// Copyright Guilty Saint Games 2026, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "AnimNode_BlendListByGameplayTag.generated.h"

// Deprecated: This is no longer needed to use gameplay tags with the plugin
namespace BlendPosesByTag { BLENDPOSESBYTAG_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimBlendTags); }

USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_BlendListByGameplayTag : public FAnimNode_BlendListBase
{
	GENERATED_BODY()
	
private:
#if WITH_EDITORONLY_DATA
	/**
	 * Gameplay Tag Filters
	 *
	 * Select filters for the available tags to choose as poses
	 * 
	 * ** No Runtime Effect **
	 * This is meant to help find tags when a project has a large number of tags
	 */
	UPROPERTY(EditAnywhere, Category = BlendPosesByGameplayTag, meta = (FoldProperty))
	mutable FGameplayTagContainer GameplayTagFilters;

	/**
	 * Active Tag
	 *
	 * This gameplay tag determines the output pose
	 */
	UPROPERTY(EditAnywhere, Category = Runtime, meta = (PinShownByDefault, FoldProperty))
	mutable FGameplayTag ActiveTag;
#endif

public:
	FAnimNode_BlendListByGameplayTag() = default;

	UPROPERTY()
	TArray<FGameplayTag> PoseTags;

	BLENDPOSESBYTAG_API FGameplayTag GetActiveGameplayTag() const;
	BLENDPOSESBYTAG_API FGameplayTagContainer GetGameplayTagFilters() const;
	BLENDPOSESBYTAG_API FString GetGameplayTagFilterString() const;

	BLENDPOSESBYTAG_API int32 GetDefaultPinIndex() const { return 0; }
	BLENDPOSESBYTAG_API int32 ToPoseTagIndex(int32 Index) const { return --Index; }
	BLENDPOSESBYTAG_API int32 ToBlendPinIndex(int32 Index) const { return ++Index; }

	//~ Begin FAnimNode_BlendListBase Interface
	BLENDPOSESBYTAG_API virtual int32 GetActiveChildIndex() override;
	//~ End FAnimNode_BlendListBase Interface
	
protected:
	virtual FString GetNodeName(FNodeDebugData& DebugData) override { return DebugData.GetNodeName(this); }
};
