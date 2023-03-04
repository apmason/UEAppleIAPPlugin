#pragma once

#include "CoreMinimal.h"

#if PLATFORM_IOS || PLATFORM_MAC

#include <StoreKit/StoreKit.h>

@interface NativeAppleIAPObject : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>

+ (id)shared;

-(void)getProducts:(NSArray*)productIdentifiers : (void(^)(NSArray<SKProduct*> *products))completion;

-(void)validateProductIdentifiers : (NSArray*)productIdentifiers;

-(void)purchaseProductWithIdentifier:(NSString*)productIdentifier : (void(^)(NSArray<SKPaymentTransaction*> *transactions))completion;

-(void)addTransactionObserverIfNeeded;

-(void)productsRequest:(SKProductsRequest*)request didReceiveResponse : (SKProductsResponse*)response;

-(void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions : (NSArray<SKPaymentTransaction*> *)transactions;

-(BOOL)finishTransactionWithIdentifier:(NSString*)productIdentifier;

@property (nonatomic, strong) SKProductsRequest* request;

@property (nonatomic, strong) NSMutableArray *cachedProducts;

/** Callback for getProducts call */
@property (nonatomic, copy) void(^ getProductsCompletion)(NSArray<SKProduct*> *products);

/** Callback for purchase transactions updated */
@property (nonatomic, copy) void(^ transactionsCompletion)(NSArray<SKPaymentTransaction*> *transactions);

@end

#endif
