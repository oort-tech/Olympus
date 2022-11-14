import { Contracts } from '../../lib/Contracts';
import { AccountOperation } from './AccountOperation';
import { AccountOperationOptions } from '../../types';
import { LimitOrders } from '../LimitOrders';
import { StopLimitOrders } from '../StopLimitOrders';
import { CanonicalOrders } from '../CanonicalOrders';
export declare class Operation {
    private contracts;
    private orderMapper;
    private limitOrders;
    private stopLimitOrders;
    private canonicalOrders;
    private networkId;
    constructor(contracts: Contracts, limitOrders: LimitOrders, stopLimitOrders: StopLimitOrders, canonicalOrders: CanonicalOrders, networkId: number);
    setNetworkId(networkId: number): void;
    initiate(options?: AccountOperationOptions): AccountOperation;
}
