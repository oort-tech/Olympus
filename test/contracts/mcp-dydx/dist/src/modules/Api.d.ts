import { address, Integer, ApiOrderQueryV2, ApiOrderV2, ApiOrder, ApiAccount, ApiFillQueryV2, ApiFillV2, ApiTradeQueryV2, ApiTradeV2, ApiMarket, ApiOrderOnOrderbook, ApiMarketName, ApiSide, BigNumberable, SignedCanonicalOrder, ApiMarketMessageV2 } from '../types';
import { CanonicalOrders } from './CanonicalOrders';
export declare class Api {
    private endpoint;
    private canonicalOrders;
    private timeout;
    constructor(canonicalOrders: CanonicalOrders, endpoint?: string, timeout?: number);
    placeCanonicalOrder({ order: { side, market, amount, price, makerAccountOwner, expiration, limitFee, }, fillOrKill, postOnly, clientId, cancelId, cancelAmountOnRevert, }: {
        order: {
            side: ApiSide;
            market: ApiMarketName;
            amount: BigNumberable;
            price: BigNumberable;
            makerAccountOwner: address;
            expiration: BigNumberable;
            limitFee?: BigNumberable;
        };
        fillOrKill?: boolean;
        postOnly?: boolean;
        clientId?: string;
        cancelId?: string;
        cancelAmountOnRevert?: boolean;
    }): Promise<{
        order: ApiOrder;
    }>;
    /**
     * Creates but does not place a signed canonicalOrder
     */
    createCanonicalOrder({ side, market, amount, price, makerAccountOwner, expiration, limitFee, postOnly, }: {
        side: ApiSide;
        market: ApiMarketName;
        amount: BigNumberable;
        price: BigNumberable;
        makerAccountOwner: address;
        expiration: BigNumberable;
        limitFee?: BigNumberable;
        postOnly?: boolean;
    }): Promise<SignedCanonicalOrder>;
    /**
     * Submits an already signed canonicalOrder
     */
    submitCanonicalOrder({ order, fillOrKill, postOnly, cancelId, clientId, cancelAmountOnRevert, }: {
        order: SignedCanonicalOrder;
        fillOrKill: boolean;
        postOnly: boolean;
        cancelId: string;
        clientId?: string;
        cancelAmountOnRevert?: boolean;
    }): Promise<{
        order: ApiOrder;
    }>;
    cancelOrderV2({ orderId, makerAccountOwner, }: {
        orderId: string;
        makerAccountOwner: address;
    }): Promise<{
        order: ApiOrder;
    }>;
    getOrdersV2({ accountOwner, accountNumber, side, status, orderType, market, limit, startingBefore, }: ApiOrderQueryV2): Promise<{
        orders: ApiOrderV2[];
    }>;
    getOrderV2({ id, }: {
        id: string;
    }): Promise<{
        order: ApiOrderV2;
    }>;
    getMarketV2({ market, }: {
        market: string;
    }): Promise<{
        market: ApiMarketMessageV2;
    }>;
    getMarketsV2(): Promise<{
        markets: {
            [market: string]: ApiMarketMessageV2;
        };
    }>;
    getFillsV2({ orderId, side, market, transactionHash, accountOwner, accountNumber, startingBefore, limit, }: ApiFillQueryV2): Promise<{
        fills: ApiFillV2[];
    }>;
    getTradesV2({ orderId, side, market, transactionHash, accountOwner, accountNumber, startingBefore, limit, }: ApiTradeQueryV2): Promise<{
        trades: ApiTradeV2[];
    }>;
    getAccountBalances({ accountOwner, accountNumber, }: {
        accountOwner: address;
        accountNumber: Integer | string;
    }): Promise<ApiAccount>;
    getOrderbookV2({ market, }: {
        market: ApiMarketName;
    }): Promise<{
        bids: ApiOrderOnOrderbook[];
        asks: ApiOrderOnOrderbook[];
    }>;
    getMarkets(): Promise<{
        markets: ApiMarket[];
    }>;
    private axiosRequest;
}
