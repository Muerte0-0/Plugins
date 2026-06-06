// Copyright 2024 Creative Elicya. All Rights Reserved.

#include "CanvasItem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CustomBlueprintThumbnailRenderer.h"

void UCustomBlueprintThumbnailRenderer::GetThumbnailSize(UObject *Object, float Zoom, uint32 &OutWidth, uint32 &OutHeight) const
{
	// Get Asset Object
	UObject *AssetObject = GetAssetObject(Object);
	if (AssetObject)
	{
		// Check if it implements AssetThumbnailInterface
		if (AssetObject->Implements<UAssetThumbnailInterface>())
		{
			// Get the thumbnail icon
			UTexture2D *Texture2D = IAssetThumbnailInterface::Execute_GetThumbnailIcon(AssetObject);
			if (Texture2D)
			{
				// Override Outwidth and OutHeight with given texture.
				OutWidth = FMath::TruncToInt(Zoom * (float)Texture2D->GetSurfaceWidth());
				OutHeight = FMath::TruncToInt(Zoom * (float)Texture2D->GetSurfaceHeight());
			}
		}
	}

	// Go back to the parent method.
	Super::GetThumbnailSize(Object, Zoom, OutWidth, OutHeight);
}

void UCustomBlueprintThumbnailRenderer::Draw(
	UObject *Object,
	int32 X, int32 Y,
	uint32 Width, uint32 Height,
	FRenderTarget *RenderTarget,
	FCanvas *Canvas,
	bool bAdditionalViewFamily)
{
	UObject *AssetObject = GetAssetObject(Object);
	UObject *ThumbnailObject = nullptr;

	if (AssetObject && AssetObject->Implements<UAssetThumbnailInterface>())
	{
		ThumbnailObject = AssetObject;
	}
	else if (Object && Object->Implements<UAssetThumbnailInterface>())
	{
		ThumbnailObject = Object;
	}

	bool bHasCustomIcon = false;
	UTexture2D *CustomTexture = nullptr;
	if (ThumbnailObject)
	{
		CustomTexture = IAssetThumbnailInterface::Execute_GetThumbnailIcon(ThumbnailObject);
		bHasCustomIcon = (CustomTexture != nullptr);
	}

	// Border
	float BorderPct = 0.f;
	FLinearColor BorderColor = FLinearColor::Transparent;
	if (ThumbnailObject)
	{
		BorderPct = IAssetThumbnailInterface::Execute_GetThumbnailBorderWidth(ThumbnailObject);
		BorderColor = IAssetThumbnailInterface::Execute_GetThumbnailBorderColor(ThumbnailObject);
	}

	float B = 0.f;
	if (BorderPct > 0.f && BorderColor.A > 0.f)
	{
		const float Base = FMath::Min((float)Width, (float)Height);
		B = 0.5f * Base * FMath::Clamp(BorderPct, 0.f, 1.f);
	}

	// Thumbnail area without border
	const float InnerX = X + B;
	const float InnerY = Y + B;
	const float InnerW = Width - (2.f * B);
	const float InnerH = Height - (2.f * B);

	// Background behind the icon (inner area), #1A1A1A
	if (InnerW > 0.f && InnerH > 0.f)
	{
		const FLinearColor BackgroundColor =
			FLinearColor::FromSRGBColor(FColor(0x1A, 0x1A, 0x1A, 0xFF));

		Canvas->DrawTile(
			InnerX,
			InnerY,
			InnerW,
			InnerH,
			0.f, 0.f, 1.f, 1.f,
			BackgroundColor);
	}

	// Custom icon OR default engine thumbnail
	if (bHasCustomIcon && CustomTexture && CustomTexture->GetResource())
	{
		const float IconSize = FMath::Min(InnerW, InnerH);
		const float OffsetX = InnerX + (InnerW - IconSize) * 0.5f;
		const float OffsetY = InnerY + (InnerH - IconSize) * 0.5f;

		FCanvasTileItem Tile(
			FVector2D(OffsetX, OffsetY),
			CustomTexture->GetResource(),
			FVector2D(IconSize, IconSize),
			FLinearColor::White);

		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}
	else
	{
		if (InnerW > 0.f && InnerH > 0.f)
		{
			Super::Draw(
				Object,
				(int32)InnerX,
				(int32)InnerY,
				(uint32)InnerW,
				(uint32)InnerH,
				RenderTarget,
				Canvas,
				bAdditionalViewFamily);
		}
		else
		{
			Super::Draw(Object, X, Y, Width, Height, RenderTarget, Canvas, bAdditionalViewFamily);
		}
	}

	// Border
	if (B > 0.f && BorderColor.A > 0.f)
	{
		Canvas->DrawTile(X, Y, Width, B, 0, 0, 1, 1, BorderColor);
		Canvas->DrawTile(X, Y + Height - B, Width, B, 0, 0, 1, 1, BorderColor);
		Canvas->DrawTile(X, Y, B, Height, 0, 0, 1, 1, BorderColor);
		Canvas->DrawTile(X + Width - B, Y, B, Height, 0, 0, 1, 1, BorderColor);
	}

	// Text
	if (ThumbnailObject)
	{
		FString Title = IAssetThumbnailInterface::Execute_GetThumbnailTitle(AssetObject);
		if (!Title.IsEmpty())
		{
			const float RawScale = InnerW / 86.0f;
			const float Scale = FMath::Clamp(RawScale, 0.5f, 3.0f);
			const int32 MaxCharsPerLine = 12;
			UFont *Font = GEngine->GetLargeFont();
			TArray<FString> Words;
			Title.ParseIntoArray(Words, TEXT(" "), true);
			FString CurrentLine;
			int32 CurrentLen = 0;
			FString FinalText;
			int32 LineCount = 0;
			float MaxLineW = 0.f;
			float LineH = 0.f;

			auto AddLineToFinalText = [&](const FString &Line)
			{
				if (!FinalText.IsEmpty())
				{
					FinalText += TEXT("\n");
				}
				FinalText += Line;
				int32 LineHeight = 0;
				int32 WidthInt = 0;
				Font->GetStringHeightAndWidth(Line, LineHeight, WidthInt);
				const float W = (float)WidthInt;
				const float H = (float)LineHeight;
				MaxLineW = FMath::Max(MaxLineW, W);
				LineH = FMath::Max(LineH, H);
				++LineCount;
			};

			for (int32 i = 0; i < Words.Num(); ++i)
			{
				const FString &Word = Words[i];
				const int32 WordLen = Word.Len();
				int32 NewLen = CurrentLen;
				if (CurrentLine.IsEmpty())
				{
					NewLen = WordLen;
				}
				else
				{
					NewLen = CurrentLen + 1 + WordLen;
				}

				if (NewLen <= MaxCharsPerLine || CurrentLine.IsEmpty())
				{
					if (CurrentLine.IsEmpty())
					{
						CurrentLine = Word;
					}
					else
					{
						CurrentLine += TEXT(" ");
						CurrentLine += Word;
					}
					CurrentLen = NewLen;
				}
				else
				{
					AddLineToFinalText(CurrentLine);
					CurrentLine = Word;
					CurrentLen = WordLen;
				}
			}

			if (!CurrentLine.IsEmpty())
			{
				AddLineToFinalText(CurrentLine);
			}

			const float TextBlockW = MaxLineW * Scale;
			const float TextBlockH = LineH * LineCount * Scale;

			EAssetThumbnailTextPosition Pos = IAssetThumbnailInterface::Execute_GetThumbnailTitlePosition(AssetObject);
			const float Margin = 4.0f;
			FVector2D TextPos(InnerX + Margin, InnerY + Margin);

			switch (Pos)
			{
			case EAssetThumbnailTextPosition::TopLeft:
				TextPos.X = InnerX + Margin;
				TextPos.Y = InnerY + Margin;
				break;

			case EAssetThumbnailTextPosition::TopCenter:
				TextPos.X = InnerX + (InnerW - TextBlockW) * 0.5f;
				TextPos.Y = InnerY + Margin;
				break;

			case EAssetThumbnailTextPosition::TopRight:
				TextPos.X = InnerX + InnerW - Margin - TextBlockW;
				TextPos.Y = InnerY + Margin;
				break;

			case EAssetThumbnailTextPosition::MiddleLeft:
				TextPos.X = InnerX + Margin;
				TextPos.Y = InnerY + (InnerH - TextBlockH) * 0.5f;
				break;

			case EAssetThumbnailTextPosition::Center:
				TextPos.X = InnerX + (InnerW - TextBlockW) * 0.5f;
				TextPos.Y = InnerY + (InnerH - TextBlockH) * 0.5f;
				break;

			case EAssetThumbnailTextPosition::MiddleRight:
				TextPos.X = InnerX + InnerW - Margin - TextBlockW;
				TextPos.Y = InnerY + (InnerH - TextBlockH) * 0.5f;
				break;

			case EAssetThumbnailTextPosition::BottomLeft:
				TextPos.X = InnerX + Margin;
				TextPos.Y = InnerY + InnerH - Margin - TextBlockH;
				break;

			case EAssetThumbnailTextPosition::BottomCenter:
				TextPos.X = InnerX + (InnerW - TextBlockW) * 0.5f;
				TextPos.Y = InnerY + InnerH - Margin - TextBlockH;
				break;

			case EAssetThumbnailTextPosition::BottomRight:
				TextPos.X = InnerX + InnerW - Margin - TextBlockW;
				TextPos.Y = InnerY + InnerH - Margin - TextBlockH;
				break;

			default:
				TextPos.X = InnerX + Margin;
				TextPos.Y = InnerY + Margin;
				break;
			}

			FCanvasTextItem TextItem(
				TextPos,
				FText::FromString(FinalText),
				Font,
				FLinearColor::White);

			TextItem.bOutlined = true;
			TextItem.OutlineColor = FLinearColor::Black;
			TextItem.EnableShadow(FLinearColor::Black);
			TextItem.Scale = FVector2D(Scale, Scale);
			TextItem.bCentreX = false;
			TextItem.bCentreY = false;
			Canvas->DrawItem(TextItem);
		}
	}
}

bool UCustomBlueprintThumbnailRenderer::CanVisualizeAsset(UObject *Object)
{
	if (Super::CanVisualizeAsset(Object))
	{
		return true;
	}

	UObject *AssetObject = GetAssetObject(Object);
	if (AssetObject && AssetObject->Implements<UAssetThumbnailInterface>())
	{
		return true;
	}

	if (Object && Object->Implements<UAssetThumbnailInterface>())
	{
		return true;
	}

	return false;
}

UObject *UCustomBlueprintThumbnailRenderer::GetAssetObject(UObject *Object) const
{
	// By default return null pointer
	UObject *AssetObject = nullptr;

	// Blueprint case
	if (Object->GetClass()->IsChildOf(UBlueprint::StaticClass()))
	{
		UBlueprint *Blueprint = Cast<UBlueprint>(Object);
		if (Blueprint)
		{
			AssetObject = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject() : nullptr;
		}
	}
	// DataAsset case
	else if (Object->GetClass()->IsChildOf(UDataAsset::StaticClass()))
	{
		AssetObject = Cast<UDataAsset>(Object);
	}

	return AssetObject;
};
