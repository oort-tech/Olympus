import { ApiOrderOnOrderbook, ApiMarketName, ApiOrderbookUpdate } from '../types';
export declare enum Channel {
    ORDERBOOK = "orderbook"
}
declare enum OutgoingMessageType {
    SUBSCRIBE = "subscribe"
}
export interface OutgoingMessage {
    type: OutgoingMessageType;
}
export interface SubscribeMessage extends OutgoingMessage {
    channel: Channel;
    id: string;
}
export declare class Websocket {
    private wsOrigin;
    private endpoint;
    private timeout;
    private ws;
    private subscribedCallbacks;
    private listeners;
    constructor(timeout?: number, endpoint?: string, wsOrigin?: string);
    connect({ onClose, onError, }?: {
        onClose?: () => void;
        onError?: (error: Error) => void;
    }): Promise<void>;
    reconnect({ onClose, onError, }?: {
        onClose?: () => void;
        onError?: (error: Error) => void;
    }): Promise<void>;
    watchOrderbook({ market, onUpdates, }: {
        market: ApiMarketName;
        onUpdates: (updates: ApiOrderbookUpdate[]) => void;
    }): Promise<{
        bids: ApiOrderOnOrderbook[];
        asks: ApiOrderOnOrderbook[];
    }>;
}
export {};
