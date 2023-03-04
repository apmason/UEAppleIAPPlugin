// Alex Mason

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IAPProduct.h"
#include "AsyncPurchaseProduct.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPurchaseResult);

/**
 * 
 */
UCLASS()
class UAsyncPurchaseProduct : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Flow Control")
	static UAsyncPurchaseProduct* PurchaseProduct(const FString& InIdentifier);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FPurchaseResult OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FPurchaseResult OnFailure;

	UPROPERTY()
	FString Identifier;

private:

	void HandleSuccess();

	void HandleFailure();

};
