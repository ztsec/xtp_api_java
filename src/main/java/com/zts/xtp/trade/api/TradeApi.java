package com.zts.xtp.trade.api;


import com.zts.xtp.common.enums.JniLogLevel;
import com.zts.xtp.common.enums.TransferProtocol;
import com.zts.xtp.common.enums.XtpLogLevel;
import com.zts.xtp.common.jni.JNILoadLibrary;
import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.common.enums.XtpTeResumeType;

import com.zts.xtp.trade.model.request.*;
import com.zts.xtp.trade.model.response.*;
import com.zts.xtp.trade.spi.TradeSpi;

import java.io.File;


public class TradeApi {

    /**
     * 交易回调接口
     */
    private TradeSpi tradeSpi;
    private long nativeHandle;

    public TradeApi(TradeSpi tradeSpi) {
        this.tradeSpi = tradeSpi;
    }

    //======================Native Functions======================================

    /**
     * 获取API的系统错误
     * @return 返回的错误信息，可以在Login、InsertOrder、CancelOrder返回值为0时调用，获取失败的原因
     */
    public native String getApiLastError();

    /**
     * 初始化交易模块
     * 如果一个账户需要在多个客户端登录，请使用不同的clientId，系统允许一个账户同时登录多个客户端，但是对于同一账户，相同的clientId只能保持一个session连接，后面的登录在前一个session存续期间，无法连接。系统不支持过夜，请确保每天开盘前重新启动
     * @param clientId 客户端id，用于区分同一用户的不同客户端，由用户自定义
     * @param key 用户开发软件Key，用户申请开户时给予
     * @param xtpAPIlogLevel xtp api 日志输出级别
     * @param jniLogLevel  jni c++部分的日志级别
     * @param resumeType 设置公共流（订单响应、成交回报）重传方式
     */
    public void init(short clientId, String key, String logFolder, XtpLogLevel  xtpAPIlogLevel, JniLogLevel jniLogLevel,XtpTeResumeType resumeType){
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
        tradeInit(clientId,key,logFolder,xtpAPIlogLevel,resumeType);
    }

    /**
     *  初始化glog-内部方法调用jni
     * @param logFolder 日志输出的目录，请设定一个真实存在的有可写权限的路径
     * @param logSubFolder 日志输出的子目录，请设定一个真实存在的有可写权限的路径 在init函数中固化为"jni"
     * @param jniLogLevel Jni 日志级别 INFO  WARNING ERROR
     */
    private native void initGlog(String logFolder,String logSubFolder,String jniLogLevel);

    /**
     * 初始化交易模块-内部方法调用jni
     * 如果一个账户需要在多个客户端登录，请使用不同的clientId，系统允许一个账户同时登录多个客户端，但是对于同一账户，相同的clientId只能保持一个session连接，后面的登录在前一个session存续期间，无法连接。系统不支持过夜，请确保每天开盘前重新启动
     * @param clientId 客户端id，用于区分同一用户的不同客户端，由用户自定义
     * @param key 用户开发软件Key，用户申请开户时给予
     * @param logFolder 日志输出的目录，请设定一个真实存在的有可写权限的路径
     * @param logLevel xtp api 日志输出级别
     * @param resumeType XTP_TERT_RESTART:从本交易日开始重传 XTP_TERT_RESUME:(保留字段，此方式暂未支持)从上次收到的续传 XTP_TERT_QUICK:只传送登录后公共流的内容  详细查看subscribePublicTopic方法 通过这里设置和通过subscribePublicTopic是二选一的两种方式。
     */
    private native void tradeInit(short clientId, String key, String logFolder, XtpLogLevel logLevel,XtpTeResumeType resumeType);


    /**
     * 设置公共流（订单响应、成交回报）重传方式
     * 该方法要在Login方法前调用，或通过tradeInit传入resumeType调用。注意在用户断线后，如果不登出就login()，公共流订阅方式不会起作用。用户只会收到断线后的所有消息。如果先logout()再login()，那么公共流订阅方式会起作用，用户收到的数据会根据用户的选择方式而定。jvm中所有用户公用同一个重传方式，以最后一次设置为准。
     * @param resumeType XTP_TERT_RESTART:从本交易日开始重传 XTP_TERT_RESUME:(保留字段，此方式暂未支持)从上次收到的续传 XTP_TERT_QUICK:只传送登录后公共流的内容
     */
    public native void subscribePublicTopic(XtpTeResumeType resumeType);


    /**
     *  此函数必须在Login之前、init之后调用
     *  用于设置检测心跳超时的时间，并非设置发心跳包的频率，默认15秒检测一次，建议设置30秒以上，设置过小会引发断线，该方法一般用于debug加断点时防止断开连接，一般正常业务时无需设置
     * @param interval interval 心跳检测时间间隔，单位为秒
     */
    public native void setHeartBeatInterval(int interval);

    /**
     * 断开连接并清除交易模块
     */
    public native void disconnect();

    /**
     * 用户登录请求
     *
     * 此函数为同步阻塞式，不需要异步等待登录成功，当函数返回即可进行后续操作，此api可支持多个账户连接，但是同一个账户同一个clientId只能有一个session连接，后面的登录在前一个session存续期间，无法连接
     * @param ip 服务器地址，类似“127.0.0.1”
     * @param port 服务器端口号
     * @param user 登录用户名
     * @param password 	登录密码
     * @param transferProtocol 数据传输协议  TransferProtocol.XTP_PROTOCOL_TCP=1   TransferProtocol.XTP_PROTOCOL_UDP=2
     * @return 返回的sessionId表明此资金账号登录是否成功，“0”表示登录失败，可以调用GetApiLastError()来获取错误代码，非“0”表示登录成功，此时需要记录下这个返回值sessionId，与登录的资金账户对应
     */
    public native String login(String ip, int port, String user, String password, TransferProtocol transferProtocol);

    /**
     * 登出请求
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @return 登出是否成功，“0”表示登出成功，“-1”表示登出失败
     */
    public native int logout(String sessionId);

    /**
     * 报单录入请求
     * @param order 报单录入信息，其中order.orderClientId字段是用户自定义字段，用户输入什么值，订单响应OnOrderEvent()返回时就会带回什么值，类似于备注，方便用户自己定位订单。当然，如果你什么都不填，也是可以的。order.orderXtpId字段无需用户填写
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @return 报单在XTP系统中的ID,如果为‘0’表示报单发送失败，此时用户可以调用GetApiLastError()来获取错误代码，非“0”表示报单发送成功，用户需要记录下返回的orderXtpId，它保证一个交易日内唯一，不同的交易日不保证唯一性
     */
    public native String insertOrder(OrderInsertRequest order, String sessionId);

    /**
     * 报单撤销请求
     * 如果撤单成功，会在报单响应函数OnOrderEvent()里返回原单部撤或者全撤的消息，如果不成功，会在OnCancelOrderError()响应函数中返回错误原因
     * @param orderXtpId 需要撤销的委托单在XTP系统中的ID
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @return 撤单在XTP系统中的ID,如果为‘0’表示撤单发送失败，此时用户可以调用GetApiLastError()来获取错误代码，非“0”表示撤单发送成功，用户需要记录下返回的orderCancelXtpId，它保证一个交易日内唯一，不同的交易日不保证唯一性
     */
    public native String cancelOrder(String orderXtpId, String sessionId);

    /**
     * 根据报单ID请求查询报单
     * @param orderXtpId 需要查询的报单在xtp系统中的ID，即InsertOrder()orderXtpId
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryOrderByXtpId(String orderXtpId, String sessionId, int requestId);

    /**
     * 请求查询报单
     * @param queryOrderReq 需要查询的订单相关筛选条件，其中合约代码可以为空，则默认所有存在的合约代码，其中起始时间格式为YYYYMMDDHHMMSSsss，为0则默认当前交易日0点，结束时间格式为YYYYMMDDHHMMSSsss，为0则默认当前时间
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryOrders(OrderQueryRequest queryOrderReq, String sessionId, int requestId);

    /**
     * 分页请求查询报单
     * 该方法支持分页查询，注意用户需要记录下最后一笔查询结果的reference以便用户下次查询使用
     * @param queryOrderByPageReq 需要分页查询订单的条件，如果第一次查询，那么queryOrderByPageReq.reference填0
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native void queryOrdersByPage(OrderQueryByPageReq queryOrderByPageReq, String sessionId, int requestId);

    /**
     * 根据委托编号请求查询相关成交
     * 此函数查询出的结果可能对应多个查询结果响应
     * @param orderXtpId 需要查询的报单在xtp系统中的ID，即InsertOrder()orderXtpId
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryTradesByXtpId(String orderXtpId, String sessionId, int requestId);

    /**
     * 请求查询成交回报
     * 该方法支持分时段查询，如果股票代码为空，则默认查询时间段内的所有成交回报，否则查询时间段内所有跟股票代码相关的成交回报，此函数查询出的结果可能对应多个查询结果响应
     * @param queryTraderReq 需要查询的成交回报筛选条件，其中合约代码可以为空，则默认所有存在的合约代码，其中起始时间格式为YYYYMMDDHHMMSSsss，为0则默认当前交易日0点，结束时间格式为YYYYMMDDHHMMSSsss，为0则默认当前时间
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryTrades(TraderQueryRequest queryTraderReq, String sessionId, int requestId);


    /**
     * 分页请求查询成交回报
     * 该方法支持分页查询，注意用户需要记录下最后一笔查询结果的reference以便用户下次查询使用
     * @param queryTradeByPageReq 需要分页查询成交回报的条件，如果第一次查询，那么queryTradeByPageReq.reference填0
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native void queryTradesByPage(TradeQueryByPageReq queryTradeByPageReq, String sessionId, int requestId);

    /**
     * 请求查询投资者持仓
     * 该方法如果用户提供了合约代码，则会查询此合约的持仓信息，如果合约代码为空，则默认查询所有持仓信息
     * @param ticker 需要查询的持仓合约代码，可以为空
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryPosition(String ticker, String sessionId, int requestId);

    /**
     * 请求查询资产
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryAsset(String sessionId, int requestId);

    /**
     * 请求查询分级基金
     * 此函数查询出的结果可能对应多个查询结果响应
     * @param queryStructuredFundInfoReq 需要查询的分级基金筛选条件，其中母基金代码可以为空，则默认所有存在的母基金，其中交易市场不能为空
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryStructuredFund(StructuredFundInfoQueryRequest queryStructuredFundInfoReq, String sessionId, int requestId);

    /**
     * 资金划拨请求
     * @param fundTransferReq 用户资金请求信息
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @return 资金划拨订单在XTP系统中的ID,如果为‘0’表示消息发送失败，此时用户可以调用GetApiLastError()来获取错误代码，非“0”表示消息发送成功，用户需要记录下返回的serialId，它保证一个交易日内唯一，不同的交易日不保证唯一性
     */
    public native String fundTransfer(FundTransferRequest fundTransferReq, String sessionId);

    /**
     * 请求查询资金划拨
     * @param queryFundTransferLogReq 需要查询的资金划拨订单筛选条件，其中serialId可以为0，则默认所有资金划拨订单，如果不为0，则请求特定的资金划拨订单
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryFundTransfer(FundTransferLogQueryRequest queryFundTransferLogReq, String sessionId, int requestId);

    /**
     * 请求查询ETF清单文件
     * @param queryETFBaseReq 需要查询的ETF清单文件的筛选条件，其中合约代码可以为空，则默认所有存在的ETF合约代码，market字段也可以为初始值，则默认所有市场的ETF合约
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryETF(ETFBaseQueryRequest queryETFBaseReq, String sessionId, int requestId);

    /**
     * 请求查询ETF股票篮
     * @param queryETFComponentReq 需要查询股票篮的的ETF合约，其中合约代码不可以为空，market字段也必须指定
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryETFTickerBasket(ETFComponentQueryRequest queryETFComponentReq, String sessionId, int requestId);

    /**
     * 请求查询今日新股申购信息列表
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryIPOInfoList(String sessionId, int requestId);

    /**
     * 请求查询用户新股申购额度信息
     * @param sessionId 资金账户对应的sessionId,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     */
    public native int queryIPOQuotaInfo(String sessionId, int requestId);

    /**
     * 请求查询期权合约
     * @return 查询是否成功，“0”表示成功，非“0”表示出错，此时用户可以调用GetApiLastError()来获取错误代码
     * @param optionAuctionInfoRequest 需要查询的期权合约的筛选条件，可以为NULL（为NULL表示查询所有的期权合约）
     * @param sessionId 资金账户对应的session_id,登录时得到
     * @param requestId 用于用户定位查询响应的ID，由用户自定义
     */
    public native int queryOptionAuctionInfo(OptionAuctionInfoRequest optionAuctionInfoRequest, String sessionId, int requestId);

    /**
     * 获取当前交易日
     * 只有登录成功后,才能得到正确的交易日
     * @return 获取到的交易日
     */
    public native String getTradingDay();

    /**
     * 获取API的发行版本号
     * @return 返回api发行版本号
     */
    public native String getApiVersion();

    /**
     * 通过报单在xtp系统中的ID获取下单的客户端id
     * @remark 由于系统允许同一用户在不同客户端上登录操作，每个客户端通过不同的client_id进行区分
     * @param orderXtpId 报单在xtp系统中的ID
     * @return 返回客户端id，可以用此方法过滤自己下的订单
     */
    public native short getClientIDByXTPID(String orderXtpId);

    /**
     * 通过报单在xtp系统中的ID获取相关资金账户名
     * @remark 只有资金账户登录成功后,才能得到正确的信息
     * @param orderXtpId 报单在xtp系统中的ID
     * @return 返回资金账户名
     */
    public native String getAccountByXTPID(String orderXtpId);


    /**
     * 服务器是否重启过
     * 此函数必须在Login之后调用
     * @param session_id 资金账户对应的session_id,登录时得到
     * @return true表示重启过，false表示没有重启过
     */
    public native  boolean isServerRestart(String session_id);

    //====================================Callback Functions==========================================
    private void onDisconnect(String sessionId, int reason) {
        tradeSpi.onDisconnect(sessionId, reason);
    }

    private void onError(ErrorMessage errorMessage) {
        tradeSpi.onError(errorMessage);
    }

    private void onOrderEvent(OrderResponse orderInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onOrderEvent(orderInfo, errorMessage, sessionId);
    }

    private void onTradeEvent(TradeResponse tradeInfo, String sessionId) {
        tradeSpi.onTradeEvent(tradeInfo, sessionId);
    }

    private void onCancelOrderError(OrderCancelResponse cancelInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onCancelOrderError(cancelInfo, errorMessage, sessionId);
    }

    private void onQueryOrder(OrderResponse orderInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryOrder(orderInfo, errorMessage, sessionId);
    }

    private void onQueryOrderByPage(OrderResponse orderInfo, long reqCount, long orderSequence, long queryReference, String sessionId) {
        tradeSpi.onQueryOrderByPage(orderInfo, reqCount, orderSequence, queryReference, sessionId);
    }

    private void onQueryTrade(TradeResponse tradeInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryTrade(tradeInfo, errorMessage, sessionId);
    }

    private void onQueryTradeByPage(TradeResponse tradeInfo, long reqCount, long tradeSequence, long queryReference, String sessionId) {
        tradeSpi.onQueryTradeByPage(tradeInfo, reqCount, tradeSequence, queryReference, sessionId);
    }

    private void onQueryPosition(StockPositionResponse stockPositionInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryPosition(stockPositionInfo, errorMessage, sessionId);
    }

    private void onQueryAsset(AssetResponse assetInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryAsset(assetInfo, errorMessage, sessionId);
    }

    private void onQueryStructuredFund(StructuredFundResponse structuredFundInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryStructuredFund(structuredFundInfo, errorMessage, sessionId);
    }

    private void onQueryFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryFundTransfer(fundTransferInfo, errorMessage, sessionId);
    }

    public void onFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onFundTransfer(fundTransferInfo, errorMessage, sessionId);
    }

    private void onQueryETF(ETFBaseResponse etfBaseInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryETF(etfBaseInfo, errorMessage, sessionId);
    }

    private void onQueryETFBasket(ETFComponentResponse etfComponentInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryETFBasket(etfComponentInfo, errorMessage, sessionId);
    }

    private void onQueryIPOInfoList(IPOTickerResponse ipoTickerInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryIPOInfoList(ipoTickerInfo, errorMessage, sessionId);
    }

    private void onQueryIPOQuotaInfo(IPOQuotaResponse ipoQuotaInfo, ErrorMessage errorMessage, String sessionId) {
        tradeSpi.onQueryIPOQuotaInfo(ipoQuotaInfo, errorMessage, sessionId);
    }

    private void OnQueryOptionAuctionInfo(OptionAuctionInfoResponse optionAuctionInfoResponse, ErrorMessage errorMessage, String sessionId){
        tradeSpi.OnQueryOptionAuctionInfo(optionAuctionInfoResponse, errorMessage, sessionId);
    }
}
