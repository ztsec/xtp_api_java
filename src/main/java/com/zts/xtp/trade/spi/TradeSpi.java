package com.zts.xtp.trade.spi;


import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.trade.model.response.*;


/**
 * XTP 交易回调接口
 *
 */
public interface TradeSpi {

    /**
     * 当客户端的某个连接与交易后台通信连接断开时，该方法被调用。
     * 用户主动调用logout导致的断线，不会触发此函数。api不会自动重连，当断线发生时，请用户自行选择后续操作，可以在此函数中调用Login重新登录，并更新session_id，此时用户收到的数据跟断线之前是连续的
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @param reason 	错误原因，请与错误代码表对应
     */
    default void onDisconnect(String sessionId, int reason) {

    }

    /**
     * 错误应答
     * @param errorMessage 当服务器响应发生错误时的具体的错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     */
    default void onError(ErrorMessage errorMessage) {

    }

    /**
     * 资金划拨通知
     * 当资金划拨订单有状态变化的时候，会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的资金划拨通知。
     * @param fundTransferInfo 资金划拨通知的具体信息
     * @param errorMessage 资金划拨订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 报单通知
     * 每次订单状态更新时，都会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线，在订单未成交、全部成交、全部撤单、部分撤单、已拒绝这些状态时会有响应，对于部分成交的情况，请由订单的成交回报来自行确认。所有登录了此用户的客户端都将收到此用户的订单响应
     * @param orderInfo 订单响应具体信息
     * @param errorMessage 订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onOrderEvent(OrderResponse orderInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询资金账户响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param assetInfo 查询到的资金账户情况
     * @param errorMessage 查询资金账户发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryAsset(AssetResponse assetInfo, ErrorMessage errorMessage, String sessionId) {

    }

    /**
     * 请求查询ETF清单文件的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param etfBaseInfo 查询到的ETF清单文件情况
     * @param errorMessage 	查询ETF清单文件发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryETF(ETFBaseResponse etfBaseInfo, ErrorMessage errorMessage, String sessionId) {

    }

    /**
     * 请求查询ETF股票篮的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param etfComponentInfo 查询到的ETF合约的相关成分股信息
     * @param errorMessage 查询ETF股票篮发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryETFBasket(ETFComponentResponse etfComponentInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询资金划拨订单响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundTransferInfo 查询到的资金账户情况
     * @param errorMessage 查询资金账户发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询今日新股申购信息列表的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ipoTickerInfo 查询到的今日新股申购的一只股票信息
     * @param errorMessage 查询今日新股申购信息列表发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryIPOInfoList(IPOTickerResponse ipoTickerInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询用户新股申购额度信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ipoQuotaInfo 查询到的用户某个市场的今日新股申购额度信息
     * @param errorMessage 查询用户新股申购额度信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryIPOQuotaInfo(IPOQuotaResponse ipoQuotaInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询报单响应
     * 由于支持分时段查询，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param orderInfo 查询到的一个报单
     * @param errorMessage 查询报单时发生错误时，返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryOrder(OrderResponse orderInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询投资者持仓响应
     * 由于用户可能持有多个股票，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockPositionInfo 查询到的一只股票的持仓情况
     * @param errorMessage 	查询账户持仓发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryPosition(StockPositionResponse stockPositionInfo, ErrorMessage errorMessage,
        String sessionId) {

    }

    /**
     * 请求查询分级基金信息响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param structuredFundInfo 查询到的分级基金情况
     * @param errorMessage 	查询分级基金发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryStructuredFund(StructuredFundResponse structuredFundInfo,
        ErrorMessage errorMessage, String sessionId) {

    }

    /**
     * 请求查询成交响应
     * 由于支持分时段查询，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param tradeInfo 查询到的一个成交回报
     * @param errorMessage 	查询成交回报发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onQueryTrade(TradeResponse tradeInfo, ErrorMessage errorMessage, String sessionId) {

    }

    /**
     * 成交通知
     * 订单有成交发生的时候，会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的成交回报。相关订单为部成状态，需要用户通过成交回报的成交数量来确定，OnOrderEvent()不会推送部成状态。
     * @param tradeInfo 成交回报的具体信息，用户可以通过trade_info.order_xtp_id来管理订单，通过GetClientIDByXTPID() == client_id来过滤自己的订单。对于上交所，exec_id可以唯一标识一笔成交。当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交了。对于深交所，exec_id是唯一的，暂时无此判断机制。report_index+market字段可以组成唯一标识表示成交回报。
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onTradeEvent(TradeResponse tradeInfo, String sessionId) {

    }

    /**
     * 撤单出错响应
     * @param orderCancelInfo 撤单具体信息
     * @param errorMessage 撤单被拒绝或者发生错误时错误代码和错误信息
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    default void onCancelOrderError(OrderCancelResponse orderCancelInfo, ErrorMessage errorMessage, String sessionId) {

    }

    default void OnQueryOptionAuctionInfo(OptionAuctionInfoResponse optionAuctionInfoResponse, ErrorMessage errorMessage, String sessionId){

    }






}
