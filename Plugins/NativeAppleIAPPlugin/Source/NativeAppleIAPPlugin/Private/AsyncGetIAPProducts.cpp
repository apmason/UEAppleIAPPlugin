// Sigil Technologies Inc.


#include "AsyncGetIAPProducts.h"
#include "Engine/Engine.h"

UAsyncGetIAPProducts* UAsyncGetIAPProducts::GetIAPProducts(const TArray<FString> InIdentifiers)
{
    UAsyncGetIAPProducts* AsyncNode = NewObject<UAsyncGetIAPProducts>();
    check(AsyncNode);
    if (IsValid(AsyncNode))
    {
        AsyncNode->Identifiers = InIdentifiers;
    }

    return AsyncNode;
}

void UAsyncGetIAPProducts::Activate() {
    Super::Activate();
    
#if PLATFORM_IOS || PLATFORM_MAC
    dispatch_async(dispatch_get_main_queue(), ^{
        NativeAppleIAPObject* AppleIAPObj = [NativeAppleIAPObject shared];
        check(AppleIAPObj != NULL);
        if (AppleIAPObj != NULL)
        {
            NSMutableArray<NSString*> *NativeArray = [NSMutableArray arrayWithCapacity:Identifiers.Num()];
            for (const auto& InString : Identifiers)
            {
                NSString* OutString = [NSString stringWithUTF8String:TCHAR_TO_UTF8(*InString)];
                [NativeArray addObject:OutString];
            }

            [AppleIAPObj getProducts:NativeArray : ^ (NSArray<SKProduct*> *products) {
                if (!products || ([products count] == 0))
                {
                    HandleFailure();
                    return;
                }

                TArray<FIAPProduct> OutProducts;
                for (SKProduct* product in products) {
                    if (product) {
                        FIAPProduct Product;
                        FString ProductID = FString(UTF8_TO_TCHAR([product.productIdentifier UTF8String]));
                        const float ProductPrice = float(product.price.doubleValue);
                        
                        Product = FIAPProduct(ProductID, ProductPrice);
                        OutProducts.Add(Product);
                    }
                }
                
                HandleSuccess(OutProducts);
            }];
        }
        else
        {
            HandleFailure();
        }
    });
#elif WITH_EDITOR // In the editor we dummy a succesful transaction.
    TArray<FIAPProduct> OutProducts;
    OutProducts.Add(FIAPProduct(FString(TEXT("com.editor.example")), 7.99));
    HandleSuccess(OutProducts);
#endif
}

void UAsyncGetIAPProducts::HandleSuccess(const TArray<FIAPProduct>& OutProducts)
{
    AsyncTask(ENamedThreads::GameThread, [this, OutProducts]()
    {
        OnSuccess.Broadcast(OutProducts);
        SetReadyToDestroy();
    });
}

void UAsyncGetIAPProducts::HandleFailure()
{
    AsyncTask(ENamedThreads::GameThread, [this]()
    {
        TArray<FIAPProduct> EmptyArray;
        OnFailure.Broadcast(EmptyArray);
        SetReadyToDestroy();
    });
}
