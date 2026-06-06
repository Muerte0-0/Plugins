// Copyright Guilty Saint Games 2026, All Rights Reserved.

#include "AnimNode_BlendListByGameplayTag.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNode_BlendListByGameplayTag)

// Deprecated: This is no longer needed to use gameplay tags with the plugin
namespace BlendPosesByTag { UE_DEFINE_GAMEPLAY_TAG_COMMENT(AnimBlendTags, "AnimBlendTags", "Root tag for anim graph pose blending by gameplay tag."); }

/////////////////////////////////////////////////////
// FAnimNode_BlendListByGameplayTag

int32 FAnimNode_BlendListByGameplayTag::GetActiveChildIndex()
{
	const FGameplayTag& CurrentActiveTag = GetActiveGameplayTag();
	
	// Find which pin index matches the current ActiveTag
	if (!CurrentActiveTag.IsValid()
		|| PoseTags.IsEmpty()
		|| !PoseTags.Contains(CurrentActiveTag))
		return 0;
	
	return ToBlendPinIndex(PoseTags.Find(CurrentActiveTag));
}

FGameplayTag FAnimNode_BlendListByGameplayTag::GetActiveGameplayTag() const
{
	return GET_ANIM_NODE_DATA(FGameplayTag, ActiveTag);
}

FGameplayTagContainer FAnimNode_BlendListByGameplayTag::GetGameplayTagFilters() const
{
	return GET_ANIM_NODE_DATA(FGameplayTagContainer, GameplayTagFilters);
}

FString FAnimNode_BlendListByGameplayTag::GetGameplayTagFilterString() const
{
	if (GetGameplayTagFilters().IsEmpty())
	{
		return FString();
	}

	TArray<FString> TagStrings;

	for (const FGameplayTag& Tag : GetGameplayTagFilters())
	{
		TagStrings.Add(Tag.ToString());
	}

	return FString::Join(TagStrings, TEXT(","));
}

