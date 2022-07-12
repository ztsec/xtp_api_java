package com.zts.xtp.quote.api;


import com.zts.xtp.common.enums.JniLogLevel;
import com.zts.xtp.common.enums.TransferProtocol;
import com.zts.xtp.common.enums.XtpLogLevel;
import com.zts.xtp.common.jni.JNILoadLibrary;
import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.common.model.TickerStringMapCache;
import com.zts.xtp.quote.model.response.*;
import com.zts.xtp.quote.spi.QuoteSpi;
import java.io.File;


public class QuoteApi {
    /**
     * 行情回调接口，为了解耦行情插件与后端逻辑，后端通过构造注入来插入具体的回调实现
     */
    private QuoteSpi quoteSpi;
    private long nativeHandle;
    TickerStringMapCache tickerStringMapCache = TickerStringMapCache.getInstance();

    public QuoteApi(QuoteSpi quoteSpi) {
        this.quoteSpi = quoteSpi;
    }

    //======================native functions======================================

    /**
     * 初始化QuoteApi
     * @param clientId（必须输入）用于区分同一用户的不同客户端，由用户自定义
     * @param logFolder 日志输出的目录，请设定一个真实存在的有可写权限的路径
     * @param xtpAPIlogLevel xtp api 日志输出级别
     * @param jniLogLevel  jni c++部分的日志级别
     * @param threadNum
     * @param ringBufferSize
     * @param fullMarketDataAvailable
     */
    public void init(short clientId, String logFolder, XtpLogLevel xtpAPIlogLevel, JniLogLevel jniLogLevel, int threadNum, int ringBufferSize, int fullMarketDataAvailable){
        if(!JNILoadLibrary.glogHasInited){
            JNILoadLibrary.glogHasInited = true;
            //init glog
            File file = new File(logFolder);
            if (!file.exists()) {
                file.mkdirs();
            }
            String logSubFolder = "jni";
            String smartcpplogFolder = logFolder+File.separator+logSubFolder;
            file = new File(smartcpplogFolder);
            if (!file.exists()) {
                file.mkdirs();
            }
            String strJniLogLevel = "INFO";
            if(jniLogLevel == JniLogLevel.JNI_LOG_LEVEL_WARNING){
                strJniLogLevel = "WARNING";
            }else if(jniLogLevel == JniLogLevel.JNI_LOG_LEVEL_ERROR){
                strJniLogLevel = "ERROR";
            }

            initGlog(logFolder,logSubFolder,strJniLogLevel);
        }
        quoteInit(clientId,logFolder,xtpAPIlogLevel,threadNum,ringBufferSize, fullMarketDataAvailable);
    }

    /**
     * 初始化glog-内部方法调用jni
     * @param logFolder 日志输出的目录，请设定一个真实存在的有可写权限的路径
     * @param logSubFolder 日志输出的子目录，请设定一个真实存在的有可写权限的路径 在init函数中固化为"jni"
     * @param jniLogLevel Jni 日志级别 INFO  WARNING ERROR
     */
    private native void initGlog(String logFolder,String logSubFolder,String jniLogLevel);

    /**
     * 初始化QuoteApi-内部方法调用jni
     * @param clientId（必须输入）用于区分同一用户的不同客户端，由用户自定义
     * @param logFolder 日志输出的目录，请设定一个真实存在的有可写权限的路径
     * @param logLevel xtp api 日志输出级别
     * @param threadNum 处理行情的线程数，取值范围1~16
     * @param ringBufferSize 每个处理行情的线程，行情数据缓冲区大小，取值范围1024~1024*256
     */
    private native void quoteInit(short clientId, String logFolder,XtpLogLevel logLevel,int threadNum,int ringBufferSize, int fullMarketDataAvailable);

    /**
     * 断开于xtp连接
     */
    public native void disconnect();

    /**
     * 此函数为同步阻塞式，不需要异步等待登录成功，当函数返回即可进行后续操作，此api只能有一个连接
     * @return 登录是否成功，“0”表示登录成功，“-1”表示连接服务器出错，此时用户可以调用GetApiLastError()来获取错误代码，“-2”表示已存在连接，不允许重复登录，如果需要重连，请先logout，“-3”表示输入有错误
     * @param ip 服务器ip地址，类似“127.0.0.1”
     * @param port 服务器端口号
     * @param user 登陆用户名
     * @param password 登陆密码
     * @param transferProtocol 数据传输协议  TransferProtocol.XTP_PROTOCOL_TCP=1   TransferProtocol.XTP_PROTOCOL_UDP=2
     * @param localIp 本地网卡地址，类似“127.0.0.1”
     */
    public native int login(String ip, int port, String user, String password, TransferProtocol transferProtocol, String localIp);

    /**
     * 登出
     * @return 登出是否成功，“0”表示登出成功，“-1”表示登出失败
     */
    public native int logout();

    /**
     * 此函数必须在Login之前、init之后调用
     * 用于设置检测心跳超时的时间，并非设置发心跳包的频率，默认15秒检测一次，建议设置30秒以上，设置过小会引发断线，该方法一般用于debug加断点时防止断开连接，一般正常业务时无需设置
     * @param interval interval 心跳检测时间间隔，单位为秒
     */
    public native void setHeartBeatInterval(int interval);
    
    /**
     * 此函数必须在Login之前、init之后调用调用
     * @param buff_size buff_size 接口接受行情的缓存，单位为M
     */
    public native void setUDPBufferSize(int buff_size);

    /**
     * 订阅行情，包括股票、指数和期权。可以一次性订阅同一证券交易所的多个合约，无论用户因为何种问题需要重新登录行情服务器，都需要重新订阅行情
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeMarketData(String[] ticker, int count, int exchangeId);

    /**
     * 退订行情，包括股票、指数和期权。可以一次性取消订阅同一证券交易所的多个合约，需要与订阅行情接口配套使用
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeMarketData(String[] ticker, int count, int exchangeId);

    /**
     * 订阅行情订单簿，包括股票、指数和期权。
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeOrderBook(String[] ticker, int count, int exchangeId);

    /**
     * 退订行情订单簿，包括股票、指数和期权。
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeOrderBook(String[] ticker, int count, int exchangeId);

    /**
     * 订阅逐笔行情，包括股票、指数和期权。
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeTickByTick(String[] ticker, int count, int exchangeId);

    /**
     * 退订逐笔行情，包括股票、指数和期权。
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeTickByTick(String[] ticker, int count, int exchangeId);

    /**
     * 订阅全市场的股票行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllMarketData(int exchangeId);

    /**
     * 退订全市场的股票行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllMarketData(int exchangeId);

    /**
     * 订阅全市场的股票行情订单簿
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllOrderBook(int exchangeId);

    /**
     * 退订全市场的股票行情订单簿
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllOrderBook(int exchangeId);

    /**
     * 订阅全市场的股票逐笔行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllTickByTick(int exchangeId);

    /**
     * 退订全市场的股票逐笔行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllTickByTick(int exchangeId);

    /**
     * 获取当前交易日可交易合约
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int queryAllTickers(int exchangeId);

    /**
     * 获取合约的最新价格信息
     * @param ticker 合约ID数组
     * @param count 要订阅/退订行情的合约个数
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int queryTickersPriceInfo(String[] ticker, int count, int exchangeId);

    /**
     * 订阅全市场的期权行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllOptionMarketData(int exchangeId);

    /**
     * 退订全市场的期权行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllOptionMarketData(int exchangeId);

    /**
     * 订阅全市场的期权行情订单簿
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllOptionOrderBook(int exchangeId);

    /**
     * 退订全市场的期权行情订单簿
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllOptionOrderBook(int exchangeId);

    /**
     * 订阅全市场的期权逐笔行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int subscribeAllOptionTickByTick(int exchangeId);

    /**
     * 退订全市场的期权逐笔行情
     * @param exchangeId 交易所代码
     * @return 订阅接口调用是否成功，“0”表示接口调用成功，非“0”表示接口调用出错
     */
    public native int unSubscribeAllOptionTickByTick(int exchangeId);

    /**
     * 获取当前交易日合约的详细静态信息
     * @param exchangeId 交易所代码，必须提供 1-上海 2-深圳
     * @return 发送查询请求是否成功，“0”表示发送查询请求成功，非“0”表示发送查询请求不成功
     */
    public native int queryAllTickersFullInfo(int exchangeId);

    /**
     * 使用UDP接收行情时，设置接收行情线程绑定的cpu
     * @param cpuNo 设置绑定的cpu，例如绑定cpu 0，可以设置0，绑定cpu 2，可以设置2，建议绑定后面的cpu
     * @remark 此函数可不调用，如果调用则必须在Login之前调用，否则不会生效
     */
    public native void setUDPRecvThreadAffinity(int cpuNo);

    /**
     * 使用UDP接收行情时，设置解析行情线程绑定的cpu
     * @param cpuNo 设置绑定的cpu，例如绑定cpu 0，可以设置0，绑定cpu 2，可以设置2，建议绑定后面的cpu
     * @remark 此函数可不调用，如果调用则必须在Login之前调用，否则不会生效
     */
    public native void setUDPParseThreadAffinity(int cpuNo);

    /**
     * 设定UDP收行情时是否输出异步日志
     * @param flag 是否输出标识，默认为true，如果不想输出“udpseq”开头的异步日志，请设置此参数为false
     * @remark 此函数可不调用，如果调用则必须在Login之前调用，否则不会生效
     */
    public native void setUDPSeqLogOutPutFlag(boolean flag);


    //======================callback functions=====================================

    private void onDisconnected(int reason) {
        quoteSpi.onDisconnected(reason);
    }

    private void onError(ErrorMessage errorMessage) {
        quoteSpi.onError(errorMessage);
    }

    private void onSubMarketData(SpecificTickerResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onSubMarketData(rsp, errorMessage);
    }

    private void onUnSubMarketData(SpecificTickerResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onUnSubMarketData(rsp, errorMessage);
    }

    private void onDepthMarketData(int exchangeType,int nTicker,int nTickerLength,double lastPrice,double preClosePrice,double openPrice,
                                   double highPrice,double lowPrice,double closePrice,double upperLimitPrice,
                                   double lowerLimitPrice,long dataTime,long qty,double turnover,double avgPrice,
                                   double bid1,double bid2, double bid3,double bid4,double bid5,double bid6,double bid7,double bid8,double bid9,double bid10,
                                   double ask1,double ask2, double ask3,double ask4,double ask5,double ask6,double ask7,double ask8,double ask9,double ask10,
                                   long bidQty1,long bidQty2,long bidQty3,long bidQty4,long bidQty5,long bidQty6,long bidQty7,long bidQty8,long bidQty9,long bidQty10,
                                   long askQty1,long askQty2,long askQty3,long askQty4,long askQty5,long askQty6,long askQty7,long askQty8,long askQty9,long askQty10,
                                   long tradesCount,String tickerStatus,double stkIopv, int dataType, MarketDataStockExDataResponse stkEx,
                                   MarketDataOptionExDataResponse optionEx, MarketDataBondExDataResponse bondEx) {

        String ticker = "";
        if (exchangeType == 1) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, 2);
        } else if (exchangeType == 2) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, 1);
        } else {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, exchangeType);
        }

        double bid[] = new double[]{bid1,bid2,bid3,bid4,bid5,bid6,bid7,bid8,bid9,bid10};
        double ask[] = new double[]{ask1,ask2,ask3,ask4,ask5,ask6,ask7,ask8,ask9,ask10};
        long bidQty[] = new long[]{bidQty1,bidQty2,bidQty3,bidQty4,bidQty5,bidQty6,bidQty7,bidQty8,bidQty9,bidQty10};
        long askQty[] = new long[]{askQty1,askQty2,askQty3,askQty4,askQty5,askQty6,askQty7,askQty8,askQty9,askQty10};

        quoteSpi.onDepthMarketData(exchangeType, ticker, lastPrice, preClosePrice, openPrice,
                highPrice, lowPrice, closePrice, upperLimitPrice, lowerLimitPrice, dataTime, qty,
                turnover, avgPrice, bid, ask, bidQty, askQty, tradesCount, tickerStatus, stkIopv, dataType, stkEx, optionEx, bondEx);
    }

    private void onDepthFullMarketData(int exchangeType,int nTicker,int nTickerLength,double lastPrice,double preClosePrice,double openPrice,
                                   double highPrice,double lowPrice,double closePrice,double upperLimitPrice,
                                   double lowerLimitPrice,long dataTime,long qty,double turnover,double avgPrice,
                                   double bid1,double bid2, double bid3,double bid4,double bid5,double bid6,double bid7,double bid8,double bid9,double bid10,
                                   double ask1,double ask2, double ask3,double ask4,double ask5,double ask6,double ask7,double ask8,double ask9,double ask10,
                                   long bidQty1,long bidQty2,long bidQty3,long bidQty4,long bidQty5,long bidQty6,long bidQty7,long bidQty8,long bidQty9,long bidQty10,
                                   long askQty1,long askQty2,long askQty3,long askQty4,long askQty5,long askQty6,long askQty7,long askQty8,long askQty9,long askQty10,
                                   long tradesCount,String tickerStatus,double stkIopv, int dataType, MarketDataStockExDataResponse stkEx,
                                   MarketDataOptionExDataResponse optionEx, MarketDataBondExDataResponse bondEx, long[] bid1Qty, int bid1Count,
                                   int maxBid1Count, long[] ask1Qty, int ask1Count, int maxAsk1Count) {

        String ticker = "";
        if (exchangeType == 1) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, 2);
        } else if (exchangeType == 2) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, 1);
        } else {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLength, exchangeType);
        }

        double bid[] = new double[]{bid1,bid2,bid3,bid4,bid5,bid6,bid7,bid8,bid9,bid10};
        double ask[] = new double[]{ask1,ask2,ask3,ask4,ask5,ask6,ask7,ask8,ask9,ask10};
        long bidQty[] = new long[]{bidQty1,bidQty2,bidQty3,bidQty4,bidQty5,bidQty6,bidQty7,bidQty8,bidQty9,bidQty10};
        long askQty[] = new long[]{askQty1,askQty2,askQty3,askQty4,askQty5,askQty6,askQty7,askQty8,askQty9,askQty10};

        quoteSpi.onDepthFullMarketData(exchangeType, ticker, lastPrice, preClosePrice, openPrice,
                highPrice, lowPrice, closePrice, upperLimitPrice, lowerLimitPrice, dataTime, qty,
                turnover, avgPrice, bid, ask, bidQty, askQty, tradesCount, tickerStatus, stkIopv,
                dataType, stkEx, optionEx, bondEx, bid1Qty, bid1Count, maxBid1Count, ask1Qty, ask1Count, maxAsk1Count);
    }

    private void onSubOrderBook(SpecificTickerResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onSubOrderBook(rsp, errorMessage);
    }

    private void onUnSubOrderBook(SpecificTickerResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onUnSubOrderBook(rsp, errorMessage);
    }

    private  void onOrderBook(int exchangeId, int nTicker, int nTickerLen, double lastPrice, long qty, double turnover, long tradeCount,
                              double bid0, double bid1, double bid2, double bid3, double bid4, double bid5, double bid6, double bid7, double bid8, double bid9,
                              double ask0, double ask1, double ask2, double ask3, double ask4, double ask5, double ask6, double ask7, double ask8, double ask9,
                              long bidQty0, long bidQty1, long bidQty2, long bidQty3, long bidQty4, long bidQty5, long bidQty6, long bidQty7, long bidQty8, long bidQty9,
                              long askQty0, long askQty1, long askQty2, long askQty3, long askQty4, long askQty5, long askQty6, long askQty7, long askQty8, long askQty9,
                              long dataTime) {

        String ticker = "";
        if (exchangeId == 1) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLen, 2);
        } else if (exchangeId == 2) {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLen, 1);
        } else {
            ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLen, exchangeId);
        }
//        String ticker = tickerStringMapCache.getTickerFromTickerStringMapCache(nTicker, nTickerLen);
        double bid[] = new double[]{bid0,bid1,bid2,bid3,bid4,bid5,bid6,bid7,bid8,bid9};
        double ask[] = new double[]{ask0,ask1,ask2,ask3,ask4,ask5,ask6,ask7,ask8,ask9};
        long bidQty[] = new long[]{bidQty0,bidQty1,bidQty2,bidQty3,bidQty4,bidQty5,bidQty6,bidQty7,bidQty8,bidQty9};
        long askQty[] = new long[]{askQty0,askQty1,askQty2,askQty3,askQty4,askQty5,askQty6,askQty7,askQty8,askQty9};

        quoteSpi.onOrderBook(exchangeId, ticker, lastPrice, qty, turnover, tradeCount, bid, ask, bidQty, askQty, dataTime);
    }

    void onTickByTickEntrust(int exchange_id,String ticker,long seq,long data_time,int type,int channel_no,long order_seq,double price,long qty, char side,char ord_type){
        quoteSpi.onTickByTickEntrust(exchange_id, ticker, seq, data_time, type, channel_no, order_seq, price, qty,  side, ord_type);
    }

    void onTickByTickTrade(int exchange_id,String ticker,long seq,long data_time,int type,int channel_no,long order_seq,double price,long qty, double money,long bid_no,long ask_no,char trade_flag){
        quoteSpi.onTickByTickTrade(exchange_id,ticker, seq, data_time, type, channel_no, order_seq, price, qty,  money, bid_no, ask_no, trade_flag);
    }

    private void onSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllMarketData(exchangeId, errorMessage);
    }

    private void onUnSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllMarketData(exchangeId, errorMessage);
    }

    private void onSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllOrderBook(exchangeId, errorMessage);
    }

    private void onUnSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllOrderBook(exchangeId, errorMessage);
    }

    private void onSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllTickByTick(exchangeId, errorMessage);
    }

    private void onUnSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllTickByTick(exchangeId, errorMessage);
    }

    private void onQueryAllTickers(TickerInfoResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onQueryAllTickers(rsp, errorMessage);
    }

    private void onQueryAllTickersFullInfo(TickerFullInfoResponse rsp, ErrorMessage errorMessage) {
        quoteSpi.onQueryAllTickersFullInfo(rsp, errorMessage);
    }

    private void onSubTickByTick (SpecificTickerResponse ticker, ErrorMessage errorMessage){
        quoteSpi.onSubTickByTick(ticker, errorMessage);
    }

    private void onUnSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        quoteSpi.onUnSubTickByTick(ticker, errorMessage);
    }

    private void onQueryTickersPriceInfo(TickerPriceInfoResponse tickerInfo, ErrorMessage errorMessage) {
        quoteSpi.onQueryTickersPriceInfo(tickerInfo, errorMessage);
    }

    private void onSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllOptionMarketData(exchangeId,errorMessage);
    }

    private void onUnSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllOptionMarketData(exchangeId,errorMessage);
    }

    private void onSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllOptionOrderBook(exchangeId,errorMessage);
    }

    private void onUnSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllOptionOrderBook(exchangeId,errorMessage);
    }

    private void onSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onSubscribeAllOptionTickByTick(exchangeId,errorMessage);
    }

    private void onUnSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        quoteSpi.onUnSubscribeAllOptionTickByTick(exchangeId,errorMessage);
    }
}
