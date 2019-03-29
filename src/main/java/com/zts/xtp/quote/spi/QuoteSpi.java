package com.zts.xtp.quote.spi;


import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.DepthMarketDataResponse;
import com.zts.xtp.quote.model.response.DepthMarketDataExResponse;
import com.zts.xtp.quote.model.response.OrderBookResponse;
import com.zts.xtp.quote.model.response.SpecificTickerResponse;
import com.zts.xtp.quote.model.response.TickByTickResponse;
import com.zts.xtp.quote.model.response.TickerInfoResponse;
import com.zts.xtp.quote.model.response.TickerPriceInfoResponse;

/**
 * XTP 行情回调响应
 *
 * @author roger on 2018/5/10
 */
public interface QuoteSpi {

    /**
     * 当客户端与行情后台通信连接断开时，该方法被调用。 <p> api不会自动重连，当断线发生时，请用户自行选择后续操作。可以在此函数中调用Login重新登录。注意用户重新登录后，需要重新订阅行情
     * @param reason 错误原因，请与错误代码表对应
     */
    default void onDisconnected(int reason) {
        // do nothing
    }

    /**
     * 错误应答 <p> 此函数只有在服务器发生错误时才会调用，一般无需用户处理
     * @param errorMessage 当服务器响应发生错误时的具体的错误代码和错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onError(ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅行情应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订行情应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 深度行情通知，包含买一卖一队列 <p> 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param depthMarketData 行情数据
     * @param depthQuote 买一卖一报价
     */
    default void onDepthMarketData(DepthMarketDataResponse depthMarketData, DepthMarketDataExResponse depthQuote) {
        // do nothing
    }

    /**
     * 订阅行情订单簿应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订行情订单簿应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 行情订单簿通知
     * @param orderBook 行情订单簿数据，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    default void onOrderBook(OrderBookResponse orderBook) {
        // do nothing
    }

    /**
     * 订阅逐笔行情应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订逐笔行情应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 逐笔行情通知
     * @param tickData 逐笔行情数据，包括逐笔委托和逐笔成交，此为共用结构体， 需要根据type来区分是逐笔委托还是逐笔成交，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    default void onTickByTick(TickByTickResponse tickData) {
        // do nothing
    }

    /**
     * 订阅全市场的行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订全市场的行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅全市场的行情订单簿应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订全市场的行情订单簿应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅全市场的逐笔行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订全市场的逐笔行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 查询可交易合约的应答
     * @param tickerInfo 可交易合约信息
     * @param errorMessage 查询可交易合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onQueryAllTickers(TickerInfoResponse tickerInfo, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 查询合约的最新价格信息应答
     * @param tickerInfo 合约的最新价格信息
     * @param errorMessage 查询合约的最新价格信息时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onQueryTickersPriceInfo(TickerPriceInfoResponse tickerInfo, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅全市场的期权行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订全市场的期权行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅全市场的期权行情订单簿应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 退订全市场的期权行情订单簿应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }

    /**
     * 订阅全市场的期权逐笔行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }


    /**
     * 退订全市场的期权逐笔行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    default void onUnSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        // do nothing
    }



}
