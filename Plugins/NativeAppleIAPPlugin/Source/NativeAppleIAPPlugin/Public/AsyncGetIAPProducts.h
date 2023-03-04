// Sigil Technologies Inc.

#pragma once

#if PLATFORM_IOS || PLATFORM_MAC
#include "NativeAppleIAPObject.h"
#endif

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IAPProduct.h"
#include "AsyncGetIAPProducts.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetIAPProductResult, const TArray<FIAPProduct>&, Products);

/**
 * 
 */
UCLASS()
class UAsyncGetIAPProducts : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
    /** 
     * Gets the IAP products associated with this BundleID. You'll get back an array of FIAPProducts, which
     * is a simple wrapper representing native Apple IAP objects. When you purchase an object you will need to pass in
     * the identifier, so store it somehow.
     * 
     * Identifiers are the identifiers of your registered IAP products on Apple's backend (com.game.productId)
     * Only identifiers that you pass in will be validated and returned.
     * 
     */
    UFUNCTION(BlueprintCallable, Category = "Flow Control")
	static UAsyncGetIAPProducts* GetIAPProducts(const TArray<FString> InIdentifiers);

	virtual void Activate() override;	
	
    UPROPERTY(BlueprintAssignable)
    FGetIAPProductResult OnSuccess;
    
    UPROPERTY(BlueprintAssignable)
    FGetIAPProductResult OnFailure;

private:

    UPROPERTY()
    TArray<FString> Identifiers;
    
    void HandleSuccess(const TArray<FIAPProduct>& OutProducts);

    void HandleFailure();
    
};
