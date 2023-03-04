// Alex Mason


#include "AsyncPurchaseProduct.h"

#if PLATFORM_IOS || PLATFORM_MAC
#include "NativeAppleIAPObject.h"
#endif

UAsyncPurchaseProduct* UAsyncPurchaseProduct::PurchaseProduct(const FString& InIdentifier)
{
	UAsyncPurchaseProduct* AsyncNode = NewObject<UAsyncPurchaseProduct>();
	AsyncNode->Identifier = InIdentifier;
	check(AsyncNode);
	return AsyncNode;
}

void UAsyncPurchaseProduct::Activate()
{
	Super::Activate();
	
#if PLATFORM_IOS || PLATFORM_MAC
    dispatch_async(dispatch_get_main_queue(), ^{
        NativeAppleIAPObject* AppleIAPObj = [NativeAppleIAPObject shared];
        check(AppleIAPObj != NULL);
        if (AppleIAPObj != NULL)
        {
            NSString* productID = [NSString stringWithUTF8String : TCHAR_TO_UTF8(*Identifier)];
            
            [AppleIAPObj purchaseProductWithIdentifier:productID : ^ (NSArray<SKPaymentTransaction*> *transactions) {
                check(productID != NULL);
                if (!transactions)
                {
                    NSLog(@"No transactions were returned, failing out");
                    HandleFailure();
                    return;
                }

                for (SKPaymentTransaction* transaction in transactions) {
                    if ([[[transaction payment] productIdentifier] isEqualToString:productID]) {
                        NSLog(@"Yes this is our transaction, let us process it");
                        switch ([transaction transactionState]) {
                            case SKPaymentTransactionStatePurchasing:
                                NSLog(@"Item is purchasing");
                                break;
                                
                            case SKPaymentTransactionStatePurchased:
                                // Do we need to handle the state in here?
                                NSLog(@"Item has been purchased");
                                HandleSuccess();
                                return;
                                
                            case SKPaymentTransactionStateFailed:
                                NSLog(@"Item has failed purchase");
                                HandleFailure();
                                return;
                                
                            case SKPaymentTransactionStateRestored:
                                NSLog(@"Transaction state restored");
                                break;
                                
                            case SKPaymentTransactionStateDeferred:
                                NSLog(@"State has been deferred");
                                break;
                                
                        }
                    }
                }
                
                NSLog(@"Unfortunately, not a good spot, failure!");
            }];
        }
    });
#endif
}

void UAsyncPurchaseProduct::HandleSuccess()
{
#if PLATFORM_IOS || PLATFORM_MAC
    // Submit the payment to finish
    NativeAppleIAPObject* AppleIAPObj = [NativeAppleIAPObject shared];
    check(AppleIAPObj != NULL);
    if (AppleIAPObj != NULL)
    {
        NSString* productID = [NSString stringWithUTF8String : TCHAR_TO_UTF8(*Identifier)];
        if (![AppleIAPObj finishTransactionWithIdentifier : productID]) {
            checkNoEntry();
        }
    }
#endif

    AsyncTask(ENamedThreads::GameThread, [this]()
    {
        OnSuccess.Broadcast();
        SetReadyToDestroy();
    });
}

void UAsyncPurchaseProduct::HandleFailure()
{
    AsyncTask(ENamedThreads::GameThread, [this]() 
    {
        OnFailure.Broadcast();
        SetReadyToDestroy();
    });
}