package com.zts.xtp.quote.spi;


import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.*;

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
    void onDisconnected(int reason);

    /**
     * 错误应答 <p> 此函数只有在服务器发生错误时才会调用，一般无需用户处理
     * @param errorMessage 当服务器响应发生错误时的具体的错误代码和错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onError(ErrorMessage errorMessage);

    /**
     * 订阅行情应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 退订行情应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onUnSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 深度行情通知，包含买一卖一队列 <p> 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param depthMarketData 行情数据
     * @param depthQuote 买一卖一报价
     */
    void onDepthMarketData(DepthMarketDataResponse depthMarketData, DepthMarketDataExResponse depthQuote);

    /**
     * 订阅行情订单簿应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 退订行情订单簿应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onUnSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 行情订单簿通知
     * @param orderBook 行情订单簿数据，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onOrderBook(OrderBookResponse orderBook);

    /**
     * 订阅逐笔行情应答 <p> 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约订阅情况
     * @param errorMessage 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 退订逐笔行情应答 <p> 每条取消订阅的合约均对应一条取消订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ticker 详细的合约取消订阅情况
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
    void onUnSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage);

    /**
     * 逐笔行情通知 改为onTickByTickEntrust（逐笔委托）、onTickByTickTrade（逐笔成交）代替
     * @param tickData 逐笔行情数据，包括逐笔委托和逐笔成交，此为共用结构体， 需要根据type来区分是逐笔委托还是逐笔成交，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
//    default void onTickByTick(TickByTickResponse tickData) {
//        // please implement this interface method
//        //为了提高性能，xtp java版 api不再使用 xtp c++版本的api提供的onTickByTick方法作为逐笔行情通知回调，改为onTickByTickEntrust（逐笔委托）、onTickByTickTrade（逐笔成交）两个通知回调，直接传基本类型，避免二次构造java对象
//    }

    /**
     * 逐笔委托行情通知  需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param exchange_id 交易所代码   1:深圳   2:上海   3:未知
     * @param ticker 合约代码（不包含交易所信息）
     * @param seq 预留
     * @param data_time 委托时间 or 成交时间
     * @param type 委托 or 成交   1:委托    2:成交
     * @param channel_no 频道代码
     * @param order_seq 委托序号(在同一个channel_no内唯一，从1开始连续)
     * @param price 委托价格
     * @param qty 委托数量
     * @param side 方向  '1':买; '2':卖; 'G':借入; 'F':出借
     * @param ord_type 订单类别  '1': 市价; '2': 限价; 'U': 本方最优
     */
    void onTickByTickEntrust(int exchange_id,String ticker,long seq,long data_time,int type,int channel_no,long order_seq,double price,long qty, char side,char ord_type);


    /**
     * 逐笔成交行情通知  需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param exchange_id 交易所代码   1:深圳   2:上海   3:未知
     * @param ticker 合约代码（不包含交易所信息）
     * @param seq 预留
     * @param data_time 委托时间 or 成交时间
     * @param type 委托 or 成交   1:委托    2:成交
     * @param channel_no 频道代码
     * @param order_seq 委托序号(在同一个channel_no内唯一，从1开始连续)
     * @param price 委托价格
     * @param qty 委托数量
     * @param money 成交金额(仅适用上交所)
     * @param bid_no 买方订单号
     * @param ask_no 卖方订单号
     * @param trade_flag 上海: 内外盘标识('B':主动买; 'S':主动卖; 'N':未知)   深圳: 成交标识('4':撤; 'F':成交)
     */
     void onTickByTickTrade(int exchange_id,String ticker,long seq,long data_time,int type,int channel_no,long order_seq,double price,long qty, double money,long bid_no,long ask_no,char trade_flag);


    /**
     * 订阅全市场的行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage);

    /**
     * 订阅全市场的行情订单簿应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的行情订单簿应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage);

    /**
     * 订阅全市场的逐笔行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的逐笔行情应答
     * @param exchangeId 交易所代码
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage);

    /**
     * 查询可交易合约的应答
     * @param tickerInfo 可交易合约信息
     * @param errorMessage 查询可交易合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onQueryAllTickers(TickerInfoResponse tickerInfo, ErrorMessage errorMessage);

    /**
     * 查询合约的最新价格信息应答
     * @param tickerInfo 合约的最新价格信息
     * @param errorMessage 查询合约的最新价格信息时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onQueryTickersPriceInfo(TickerPriceInfoResponse tickerInfo, ErrorMessage errorMessage);

    /**
     * 订阅全市场的期权行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的期权行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage);

    /**
     * 订阅全市场的期权行情订单簿应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的期权行情订单簿应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage);

    /**
     * 订阅全市场的期权逐笔行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage);

    /**
     * 退订全市场的期权逐笔行情应答
     * @param exchangeId 表示当前全订阅的市场，如果为XTP_EXCHANGE_UNKNOWN，表示沪深全市场，XTP_EXCHANGE_SH表示为上海全市场，XTP_EXCHANGE_SZ表示为深圳全市场
     * @param errorMessage 取消订阅合约时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
     */
     void onUnSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage);



}
