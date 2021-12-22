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
    void onDisconnect(String sessionId, int reason) ;

    /**
     * 错误应答
     * @param errorMessage 当服务器响应发生错误时的具体的错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     */
    void onError(ErrorMessage errorMessage) ;

    /**
     * 资金划拨通知
     * 当资金划拨订单有状态变化的时候，会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的资金划拨通知。
     * @param fundTransferInfo 资金划拨通知的具体信息
     * @param errorMessage 资金划拨订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 委托回报
     * @param orderXtpId XTP系统订单ID，在XTP系统中唯一
     * @param orderClientId 报单引用，用户自定义
     * @param orderCancelClientId 报单操作引用，用户自定义（暂未使用）
     * @param orderCancelXtpId 撤单在XTP系统中的id，在XTP系统中唯一
     * @param ticker 合约代码
     * @param marketType 交易市场
     * @param price 价格
     * @param quantity 数量，此订单的报单数量
     * @param priceType 报单价格条件
     * @param sideType 买卖方向
     * @param positionEffectType 开平标志
     * @param businessType 业务类型
     * @param qtyTraded 今成交数量，为此订单累计成交数量
     * @param qtyLeft 剩余数量，当撤单成功时，表示撤单数量
     * @param insertTime 委托时间，格式为YYYYMMDDHHMMSSsss
     * @param updateTime 最后修改时间，格式为YYYYMMDDHHMMSSsss
     * @param cancelTime 撤销时间，格式为YYYYMMDDHHMMSSsss
     * @param tradeAmount 成交金额，为此订单的成交总金额
     * @param orderLocalId 本地报单编号 OMS生成的单号，不等同于order_xtp_id，为服务器传到报盘的单号
     * @param orderStatusType 报单状态，订单响应中没有部分成交状态的推送，在查询订单结果中，会有部分成交状态
     * @param orderSubmitStatusType 报单提交状态，OMS内部使用，用户无需关心
     * @param txtPOrderTypeType 报单类型
     * @param requestId 此消息响应函数对应的请求ID
     * @param lastResp 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
     * @param errorMessage 订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onOrderEvent(String orderXtpId, int orderClientId, int orderCancelClientId, String orderCancelXtpId,
                      String ticker, int marketType, double price, long quantity, int priceType, int sideType,
                      int positionEffectType, int businessType, long qtyTraded, long qtyLeft, long insertTime,
                      long updateTime, long cancelTime, double tradeAmount, String orderLocalId,
                      int orderStatusType, int orderSubmitStatusType, char txtPOrderTypeType, int requestId,
                      boolean lastResp,  ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询资金账户响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param assetInfo 查询到的资金账户情况
     * @param errorMessage 查询资金账户发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 该接口保留用于接收查询资产出错的情况 20210428
     */
    void onQueryAsset(AssetResponse assetInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询ETF清单文件的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param etfBaseInfo 查询到的ETF清单文件情况
     * @param errorMessage 	查询ETF清单文件发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryETF(ETFBaseResponse etfBaseInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询ETF股票篮的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param etfComponentInfo 查询到的ETF合约的相关成分股信息
     * @param errorMessage 查询ETF股票篮发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryETFBasket(ETFComponentResponse etfComponentInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询资金划拨订单响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundTransferInfo 查询到的资金账户情况
     * @param errorMessage 查询资金账户发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询其他节点可用资金的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param rsp 查询到的其他节点可用资金情况
     * @param errorMessage 查询其他节点可用资金发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryOtherServerFund(XTPFundQueryRsp rsp, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询今日新股申购信息列表的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ipoTickerInfo 查询到的今日新股申购的一只股票信息
     * @param errorMessage 查询今日新股申购信息列表发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryIPOInfoList(IPOTickerResponse ipoTickerInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询用户新股申购额度信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param ipoQuotaInfo 查询到的用户某个市场的今日新股申购额度信息
     * @param errorMessage 查询用户新股申购额度信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryIPOQuotaInfo(IPOQuotaResponse ipoQuotaInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询报单响应
     * 由于支持分时段查询，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param orderInfo 查询到的一个报单
     * @param errorMessage 查询报单时发生错误时，返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryOrder(OrderResponse orderInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 分页请求查询报单响应
     * 当orderSequence为0，表明当次查询没有查到任何记录，如果orderSequence等于reqCount，那么表示还有报单，可以进行下一次分页查询，如果不等，表示所有报单已经查询完毕。需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。
     * @param orderInfo 查询到的一个报单
     * @param reqCount 分页请求的最大数量
     * @param orderSequence 当前报单信息所对应的查询索引，需要记录下来，在进行下一次分页查询的时候需要用到
     * @param queryReference 查询到的一个报单
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryOrderByPage(OrderResponse orderInfo, long reqCount, long orderSequence, long queryReference, String sessionId);

    /**
     * 请求查询投资者持仓响应
     * 由于用户可能持有多个股票，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockPositionInfo 查询到的一只股票的持仓情况
     * @param errorMessage 	查询账户持仓发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 该接口保留用于接收查询持仓出错的情况20210428
     */
    void onQueryPosition(StockPositionResponse stockPositionInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询分级基金信息响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param structuredFundInfo 查询到的分级基金情况
     * @param errorMessage 	查询分级基金发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryStructuredFund(StructuredFundResponse structuredFundInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 请求查询成交响应
     * 由于支持分时段查询，一个查询请求可能对应多个响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param tradeInfo 查询到的一个成交回报
     * @param errorMessage 	查询成交回报发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryTrade(TradeResponse tradeInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 分页请求查询成交响应
     * 当tradeSequence为0，表明当次查询没有查到任何记录，如果tradeSequence等于reqCount，那么表示还有回报，可以进行下一次分页查询，如果不等，表示所有回报已经查询完毕。需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。
     * @param tradeInfo 查询到的一个成交信息
     * @param reqCount 分页请求的最大数量
     * @param tradeSequence 当前回报信息所对应的查询索引，需要记录下来，在进行下一次分页查询的时候需要用到
     * @param queryReference 查询到的一个
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onQueryTradeByPage(TradeResponse tradeInfo, long reqCount, long tradeSequence, long queryReference, String sessionId);

    /**
     * 成交回报
     * @param orderXtpId XTP系统订单ID，此成交回报相关的订单ID，在XTP系统中唯一
     * @param orderClientId 报单引用
     * @param ticker 合约代码
     * @param marketType 交易市场
     * @param localOrderId 订单号，引入XTPID后，该字段实际和order_xtp_id重复。接口中暂时保留
     * @param execId 成交编号，深交所唯一，上交所每笔交易唯一，当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交
     * @param price 价格，此次成交的价格
     * @param quantity 数量，此次成交的数量，不是累计数量
     * @param tradeTime 成交时间，格式为YYYYMMDDHHMMSSsss
     * @param tradeAmount 成交金额，此次成交的总金额 = price*quantity
     * @param reportIndex 成交序号 –回报记录号，每个交易所唯一,report_index+market字段可以组成唯一标识表示成交回报
     * @param orderExchId 报单编号 –交易所单号，上交所为空，深交所有此字段
     * @param tradeType 成交类型 –成交回报中的执行类型
     * @param sideType 买卖方向
     * @param positionEffectType 开平标志
     * @param businessType 业务类型
     * @param branchPbu 交易所交易员代码
     * @param requestId 此消息响应函数对应的请求ID
     * @param lastResp 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onTradeEvent(String orderXtpId, int orderClientId, String ticker, int marketType, String localOrderId, String execId,
                      double price, long quantity, long tradeTime, double tradeAmount, String reportIndex, String orderExchId,
                      char tradeType, int sideType, int positionEffectType, int businessType, String branchPbu, int requestId,
                      boolean lastResp, String sessionId);

    /**
     * 撤单出错响应
     * @param orderCancelInfo 撤单具体信息
     * @param errorMessage 撤单被拒绝或者发生错误时错误代码和错误信息
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void onCancelOrderError(OrderCancelResponse orderCancelInfo, ErrorMessage errorMessage, String sessionId) ;

    /**
     * 查询期权合约响应
     * 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param optionAuctionInfoResponse 查询到的期权合约情况
     * @param errorMessage 撤单被拒绝或者发生错误时错误代码和错误信息
     * @param sessionId 资金账户对应的sessionId，登录时得到
     */
    void OnQueryOptionAuctionInfo(OptionAuctionInfoResponse optionAuctionInfoResponse, ErrorMessage errorMessage, String sessionId);

    /**
     * xalgo业务中报送策略单的响应
     * @param strategyInfo 用户报送的策略单的具体信息
     * @param errorMessage 创建策略发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onInsertAlgoOrder(XTPStrategyInfoStruct strategyInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * xalgo业务中撤销策略单的响应
     * @param strategyInfo 用户创建的策略的具体信息
     * @param errorMessage 创建策略发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onCancelAlgoOrder(XTPStrategyInfoStruct strategyInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * algo业务中查询策略列表的响应
     * @param strategyInfo 策略具体信息
     * @param strategyParam 此策略中包含的参数，如果errorMessage.errorId为0时，有意义
     * @param errorMessage 查询查询策略列表发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryStrategy(XTPStrategyInfoStruct strategyInfo, String strategyParam, ErrorMessage errorMessage, String sessionId);

    /**
     * algo业务中用户建立算法通道的消息响应
     * @param user 用户名
     * @param errorMessage 二次验证发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误，即算法通道成功
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 算法通道建立成功后，才能对用户创建策略等操作，一个用户只能拥有一个算法通道，如果之前已经建立，则无需重复建立
     */
    void onAlgoUserEstablishChannel(String user, ErrorMessage errorMessage, String sessionId);

    /**
     * algo业务中策略运行时策略状态通知
     * @param strategyInfo 用户策略运行情况的状态通知
     * @param errorMessage 错误代码和错误信息
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onStrategyStateReport(XTPStrategyStateReportStruct strategyInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 当客户端与AlgoBus通信连接断开时，该方法被调用
     * @param reason 错误原因，请与错误代码表对应
     * @remark 请不要堵塞此线程，否则会影响algo的登录
     */
    void onAlgoDisconnected(int reason);

    /**当客户端与AlgoBus断线连接时，该方法被调用，仅在断线重连成功后会被调用*/
    void onAlgoConnected();

    /**
     * 融资融券业务中现金直接还款的响应
     * @param cashRepayRsp 现金直接还款通知的具体信息，用户可以通过cashRepayRsp.orderXtpId来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 现金还款发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
   void onCreditCashRepay(XTPCrdCashRepayRsp cashRepayRsp, ErrorMessage errorMessage, String sessionId);

    /**
     * 融资融券业务中现金还息的响应
     * @param cashRepayDebtInterestFeeRsp 现金还息通知的具体信息，用户可以通过cashRepayDebtInterestFeeRsp.orderXtpId来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 现金还息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onCreditCashRepayDebtInterestFee(XTPCrdCashRepayDebtInterestFeeRsp cashRepayDebtInterestFeeRsp, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询融资融券业务中的现金直接还款报单的响应
     * @param cashRepayInfo 查询到的某一笔现金直接还款通知的具体信息
     * @param errorMessage 查询现金直接报单发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditCashRepayInfo(XTPCrdCashRepayInfo cashRepayInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询信用账户额外信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundInfo 查询到的信用账户额外信息情况
     * @param errorMessage 查询信用账户额外信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditFundInfo(XTPCrdFundInfo fundInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询信用账户负债信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtInfo 查询到的信用账户合约负债情况
     * @param errorMessage 查询信用账户负债信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditDebtInfo(XTPCrdDebtInfo debtInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询信用账户指定证券负债未还信息响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtStockInfo 查询到的信用账户指定证券负债未还信息情况
     * @param errorMessage 查询信用账户指定证券负债未还信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditTickerDebtInfo(XTPCrdDebtStockInfo debtStockInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 请求查询信用账户待还资金的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param remainAmount 查询到的信用账户待还资金
     * @param errorMessage 查询信用账户待还资金发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditAssetDebtInfo(double remainAmount, ErrorMessage errorMessage,int requestId, String sessionId);

    /**
     * 请求查询信用账户可融券头寸信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param assignInfo 查询到的信用账户可融券头寸信息
     * @param errorMessage 查询信用账户可融券头寸信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditTickerAssignInfo(XTPClientQueryCrdPositionStkInfo assignInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 融资融券业务中请求查询指定余券信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockInfo 查询到的余券信息
     * @param errorMessage 查询信用账户余券信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo stockInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 融资融券业务中请求查询余券信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockInfo 查询到的余券信息
     * @param errorMessage 查询信用账户余券信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryMulCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo stockInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 融资融券业务中负债合约展期的通知
     * @param debtExtendInfo 负债合约展期通知的具体信息，用户可以通过debt_extend_info.xtpid来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 负债合约展期订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误。
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 当负债合约展期订单有状态变化的时候，会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的负债合约展期通知。
     */
    void onCreditExtendDebtDate(XTPCreditDebtExtendNotice debtExtendInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 查询融资融券业务中负债合约展期订单响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtExtendInfo 查询到的负债合约展期情况
     * @param errorMessage 查询负债合约展期发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误。当errorMessage.errorId=11000350时，表明没有记录，当为其他非0值时，表明合约发生拒单时的错误原因
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditExtendDebtDateOrders(XTPCreditDebtExtendNotice debtExtendInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 查询融资融券业务中信用账户附加信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundExtraInfo 信用账户附加信息
     * @param errorMessage 查询信用账户附加信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditFundExtraInfo(XTPCrdFundExtraInfo fundExtraInfo, ErrorMessage errorMessage, String sessionId);

    /**
     * 查询融资融券业务中信用账户指定证券的附加信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param positionExtraInfo 信用账户指定证券的附加信息
     * @param errorMessage 查询信用账户附加信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    void onQueryCreditPositionExtraInfo(XTPCrdPositionExtraInfo positionExtraInfo, ErrorMessage errorMessage, String sessionId);

}
