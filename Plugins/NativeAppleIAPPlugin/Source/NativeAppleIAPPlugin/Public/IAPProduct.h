#pragma once

#include "CoreMinimal.h"
#include "IAPProduct.generated.h"

#if PLATFORM_IOS || PLATFORM_MAC
#include <StoreKit/StoreKit.h>
#endif

USTRUCT(BlueprintType)
struct FIAPProduct
{
public:

	GENERATED_USTRUCT_BODY()

	FIAPProduct(const FString& InProductIdentifier, const float InPrice)
	: ProductIdentifier(InProductIdentifier)
	, Price(InPrice)
	{
	}

	FIAPProduct() {}

	UPROPERTY(BlueprintReadOnly)
	FString ProductIdentifier;

	UPROPERTY(BlueprintReadOnly)
	float Price;

};
