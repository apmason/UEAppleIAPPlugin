

#include "NativeAppleIAPObject.h"
#include "Engine/Engine.h"

#if PLATFORM_IOS || PLATFORM_MAC

#import <Foundation/Foundation.h>

@interface NativeAppleIAPObject()

@end

@implementation NativeAppleIAPObject

+ (id)shared
{
    static NativeAppleIAPObject *sharedManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedManager = [[self alloc] init];
    });
    
    return sharedManager;
}

/** Fetching product identifiers  */
- (void)getProducts:(NSArray*)productIdentifiers : (void(^)(NSArray<SKProduct*> *products))completion;
{
    self.getProductsCompletion = completion;
    [self validateProductIdentifiers:productIdentifiers];
}

- (void)validateProductIdentifiers:(NSArray*)productIdentifiers {
    if (!productIdentifiers || productIdentifiers.count == 0) {
        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("No product identifiers submitted!"));
        self.getProductsCompletion(NULL);
        self.getProductsCompletion = NULL;
        return;
    }
    
    SKProductsRequest* productsRequest = [[SKProductsRequest alloc]initWithProductIdentifiers:[NSSet setWithArray : productIdentifiers]];
    self.request = productsRequest;
    productsRequest.delegate = self;

    [productsRequest start];
}

- (void)productsRequest:(SKProductsRequest*)request didReceiveResponse : (SKProductsResponse*)response
{
    if (!_cachedProducts) {
        _cachedProducts = [NSMutableArray new];
      }
    
    [_cachedProducts removeAllObjects];
    // When we receive product responses, we cache these for our own use for later.
    // When a user wants to purchase a product, they only pass in the product identifier.

    // A user requests cached products. 
    [_cachedProducts addObjectsFromArray:response.products];

    checkf(self.getProductsCompletion != NULL, TEXT("Get products completion is NULL"));

    if (self.getProductsCompletion != NULL) {
        self.getProductsCompletion(response.products);
        self.getProductsCompletion = NULL;
    }
}

/** Purchasing product  */

-(void)purchaseProductWithIdentifier:(NSString*)productIdentifier : (void(^)(NSArray<SKPaymentTransaction*> *transactions))completion
{
    self.transactionsCompletion = completion;
    
    checkf(_cachedProducts != NULL, TEXT("Cached product is null"));
    checkf(_cachedProducts.count > 0, TEXT("No items in cached product"));
    
    [self addTransactionObserverIfNeeded];
    
    for (SKProduct* product in _cachedProducts) {
        if ([product.productIdentifier isEqualToString:productIdentifier]) {
            SKPayment* payment = [SKPayment paymentWithProduct:product];
            check(payment != NULL);
            [[SKPaymentQueue defaultQueue]addPayment:payment];
            GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("JUST ADDED TO QUEUE!")));
        }
    }
}

-(void)addTransactionObserverIfNeeded
{
    for (id<SKPaymentTransactionObserver> observer in [[SKPaymentQueue defaultQueue] transactionObservers]) {
        if (observer == self) {
            return;
        }
    }
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

- (void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions : (NSArray<SKPaymentTransaction*> *)transactions
{
    // If a completion block exists, let it handle the transaction.
    if (self.transactionsCompletion != NULL) {
        self.transactionsCompletion(transactions);
    }
}

- (BOOL)finishTransactionWithIdentifier:(NSString*)productIdentifier
{
    NSArray<SKPaymentTransaction*>* transactions = [[SKPaymentQueue defaultQueue] transactions];
    for (SKPaymentTransaction* transaction in transactions) {
        if ([[[transaction payment] productIdentifier] isEqualToString: productIdentifier]) {
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
            return YES;
        }
    }

    return NO;
}

@end

#endif
