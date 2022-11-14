import { OrderMapper } from '@dydxprotocol/exchange-wrappers';
import { LimitOrders } from '../LimitOrders';
import { StopLimitOrders } from '../StopLimitOrders';
import { CanonicalOrders } from '../CanonicalOrders';
import { Contracts } from '../../lib/Contracts';
import { Deposit, Withdraw, SendOptions, TxResult, Buy, Sell, Transfer, Trade, Liquidate, Vaporize, SetExpiry, SetExpiryV2, SetApprovalForExpiryV2, Refund, DaiMigrate, AccountActionWithOrder, Call, Decimal, Integer, AccountOperationOptions, address, LimitOrder, SignedLimitOrder, StopLimitOrder, SignedStopLimitOrder, CanonicalOrder, SignedCanonicalOrder, Operation, SignedOperation } from '../../types';
export declare class AccountOperation {
    private contracts;
    private actions;
    private committed;
    private orderMapper;
    private limitOrders;
    private stopLimitOrders;
    private canonicalOrders;
    private accounts;
    private proxy;
    private sendEthTo;
    private auths;
    private networkId;
    constructor(contracts: Contracts, orderMapper: OrderMapper, limitOrders: LimitOrders, stopLimitOrders: StopLimitOrders, canonicalOrders: CanonicalOrders, networkId: number, options: AccountOperationOptions);
    deposit(deposit: Deposit): AccountOperation;
    withdraw(withdraw: Withdraw): AccountOperation;
    transfer(transfer: Transfer): AccountOperation;
    buy(buy: Buy): AccountOperation;
    sell(sell: Sell): AccountOperation;
    liquidate(liquidate: Liquidate): AccountOperation;
    vaporize(vaporize: Vaporize): AccountOperation;
    setExpiry(args: SetExpiry): AccountOperation;
    setApprovalForExpiryV2(args: SetApprovalForExpiryV2): AccountOperation;
    setExpiryV2(args: SetExpiryV2): AccountOperation;
    approveLimitOrder(args: AccountActionWithOrder): AccountOperation;
    cancelLimitOrder(args: AccountActionWithOrder): AccountOperation;
    approveStopLimitOrder(args: AccountActionWithOrder): AccountOperation;
    cancelStopLimitOrder(args: AccountActionWithOrder): AccountOperation;
    approveCanonicalOrder(args: AccountActionWithOrder): AccountOperation;
    cancelCanonicalOrder(args: AccountActionWithOrder): AccountOperation;
    setCanonicalOrderFillArgs(primaryAccountOwner: address, primaryAccountId: Integer, price: Decimal, fee: Decimal): AccountOperation;
    call(args: Call): AccountOperation;
    trade(trade: Trade): AccountOperation;
    fillSignedLimitOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: SignedLimitOrder, weiAmount: Integer, denotedInMakerAmount?: boolean): AccountOperation;
    fillPreApprovedLimitOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: LimitOrder, weiAmount: Integer, denotedInMakerAmount?: boolean): AccountOperation;
    fillSignedDecreaseOnlyStopLimitOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: SignedStopLimitOrder, denotedInMakerAmount?: boolean): AccountOperation;
    fillSignedStopLimitOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: SignedStopLimitOrder, weiAmount: Integer, denotedInMakerAmount?: boolean): AccountOperation;
    fillPreApprovedStopLimitOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: StopLimitOrder, weiAmount: Integer, denotedInMakerAmount?: boolean): AccountOperation;
    fillCanonicalOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: CanonicalOrder | SignedCanonicalOrder, amount: Integer, price: Decimal, fee: Decimal): AccountOperation;
    fillDecreaseOnlyCanonicalOrder(primaryAccountOwner: address, primaryAccountNumber: Integer, order: CanonicalOrder | SignedCanonicalOrder, price: Decimal, fee: Decimal): AccountOperation;
    refund(refundArgs: Refund): AccountOperation;
    daiMigrate(migrateArgs: DaiMigrate): AccountOperation;
    liquidateExpiredAccount(liquidate: Liquidate, maxExpiry?: Integer): AccountOperation;
    liquidateExpiredAccountV2(liquidate: Liquidate, maxExpiry?: Integer): AccountOperation;
    private liquidateExpiredAccountInternal;
    fullyLiquidateExpiredAccount(primaryAccountOwner: address, primaryAccountNumber: Integer, expiredAccountOwner: address, expiredAccountNumber: Integer, expiredMarket: Integer, expiryTimestamp: Integer, blockTimestamp: Integer, weis: Integer[], prices: Integer[], spreadPremiums: Integer[], collateralPreferences: Integer[]): AccountOperation;
    fullyLiquidateExpiredAccountV2(primaryAccountOwner: address, primaryAccountNumber: Integer, expiredAccountOwner: address, expiredAccountNumber: Integer, expiredMarket: Integer, expiryTimestamp: Integer, blockTimestamp: Integer, weis: Integer[], prices: Integer[], spreadPremiums: Integer[], collateralPreferences: Integer[]): AccountOperation;
    private fullyLiquidateExpiredAccountInternal;
    finalSettlement(settlement: Liquidate): AccountOperation;
    /**
     * Adds all actions from a SignedOperation and also adds the authorization object that allows the
     * proxy to process the actions.
     */
    addSignedOperation(signedOperation: SignedOperation): AccountOperation;
    /**
     * Takes all current actions/accounts and creates an Operation struct that can then be signed and
     * later used with the SignedOperationProxy.
     */
    createSignableOperation(options?: {
        expiration?: Integer;
        salt?: Integer;
        sender?: address;
        signer?: address;
    }): Operation;
    /**
     * Commits the operation to the chain by sending a transaction.
     */
    commit(options?: SendOptions): Promise<TxResult>;
    /**
     * Internal logic for filling limit orders (either signed or pre-approved orders)
     */
    private fillLimitOrderInternal;
    /**
     * Internal logic for filling stop-limit orders (either signed or pre-approved orders)
     */
    private fillStopLimitOrderInternal;
    private exchange;
    private addActionArgs;
    private getPrimaryAccountId;
    private getAccountId;
    private generateAuthData;
}
