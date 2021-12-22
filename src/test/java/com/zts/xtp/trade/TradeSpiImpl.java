package com.zts.xtp.trade;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.trade.model.response.*;
import com.zts.xtp.trade.spi.TradeSpi;

public class TradeSpiImpl implements TradeSpi {

    @Override
    public void onOrderEvent(String orderXtpId, int orderClientId, int orderCancelClientId, String orderCancelXtpId,
                             String ticker, int marketType, double price, long quantity, int priceType, int sideType,
                             int positionEffectType, int businessType, long qtyTraded, long qtyLeft, long insertTime,
                             long updateTime, long cancelTime, double tradeAmount, String orderLocalId,
                             int orderStatusType, int orderSubmitStatusType, char txtPOrderTypeType, int requestId,
                             boolean lastResp,  ErrorMessage errorMessage, String sessionId) {
        System.out.println("onOrderEvent orderInfo: " + orderXtpId);
    }

    @Override
    public void onDisconnect(String sessionId, int reason) {
        System.out.println("onDisconnect");
    }

    @Override
    public void onError(ErrorMessage errorMessage) {
        System.out.println("onError: " + errorMessage.toString());
    }

    @Override
    public void onFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) {
        if (fundTransferInfo != null) {
            System.out.println("onFundTransfer: " + fundTransferInfo.toString());
        } else {
            System.out.println("onFundTransfer error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryAsset(AssetResponse assetInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onQueryAsset: " + assetInfo.toString());
    }

    @Override
    public void onQueryETF(ETFBaseResponse etfBaseInfo, ErrorMessage errorMessage, String sessionId) {
        if (etfBaseInfo != null) {
            System.out.println("onQueryETF: " + etfBaseInfo.toString());
        } else {
            System.out.println("onQueryETF error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryETFBasket(ETFComponentResponse etfComponentInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onQueryETFBasket: " + etfComponentInfo.toString());
    }

    @Override
    public void onQueryFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage, String sessionId) {
        if (fundTransferInfo != null) {
            System.out.println("onQueryFundTransfer: " + fundTransferInfo.toString());
        } else {
            System.out.println("onQueryFundTransfer error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryOtherServerFund(XTPFundQueryRsp rsp, ErrorMessage errorMessage, String sessionId) {
        if (rsp != null) {
            System.out.println("onQueryOtherServerFund: " + rsp.toString());
        } else {
            System.out.println("onQueryOtherServerFund error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryIPOInfoList(IPOTickerResponse ipoTickerInfo, ErrorMessage errorMessage, String sessionId) {
        if (ipoTickerInfo != null) {
            System.out.println("onQueryIPOInfoList: " + ipoTickerInfo.toString());
        } else {
            System.out.println("onQueryIPOInfoList error: " + errorMessage.toString());
        }

    }

    @Override
    public void onQueryIPOQuotaInfo(IPOQuotaResponse ipoQuotaInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onQueryIPOQuotaInfo: " + ipoQuotaInfo.toString());
    }

    @Override
    public void onQueryOrder(OrderResponse orderInfo, ErrorMessage errorMessage, String sessionId) {
        if (orderInfo != null) {
            System.out.println("onQueryOrder: " + orderInfo.toString());
        } else {
            System.out.println("onQueryOrder error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryOrderByPage(OrderResponse orderInfo, long reqCount, long orderSequence, long queryReference, String sessionId) {
        if (orderInfo != null) {
            System.out.println("onQueryOrderByPage: " + orderInfo.toString() + "  reqCount="+reqCount+"  orderSequence="+orderSequence+"  queryReference="+queryReference+"  sessionId="+sessionId);
        } else {
            System.err.println("onQueryOrderByPage error");
        }
    }

    @Override
    public void onQueryTradeByPage(TradeResponse tradeInfo, long reqCount, long tradeSequence, long queryReference, String sessionId) {
        if (tradeInfo != null) {
            System.out.println("onQueryTradeByPage: " + tradeInfo.toString() + "  reqCount="+reqCount+"  tradeSequence="+tradeSequence+"  queryReference="+queryReference+"  sessionId="+sessionId);
        } else {
            System.err.println("onQueryTradeByPage error");
        }
    }

    @Override
    public void onQueryPosition(StockPositionResponse stockPositionInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onQueryPosition: " + stockPositionInfo.toString());
    }


    @Override
    public void onQueryStructuredFund(StructuredFundResponse structuredFundInfo, ErrorMessage errorMessage, String sessionId) {
        if (structuredFundInfo != null ) {
            System.out.println("onQueryStructuredFund: " + structuredFundInfo.toString());
        } else {
            System.out.println("onQueryStructuredFund error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryTrade(TradeResponse tradeInfo, ErrorMessage errorMessage, String sessionId) {
        if (tradeInfo != null) {
            System.out.println("onQueryTrade: " + tradeInfo.toString());
        } else {
            System.out.println("onQueryTrade error: " + errorMessage.toString());
        }
    }


    @Override
    public void onTradeEvent(String orderXtpId, int orderClientId, String ticker, int marketType, String localOrderId, String execId,
                             double price, long quantity, long tradeTime, double tradeAmount, String reportIndex, String orderExchId,
                             char tradeType, int sideType, int positionEffectType, int businessType, String branchPbu, int requestId,
                             boolean lastResp, String sessionId) {
        System.out.println("onTradeEvent: " + orderXtpId);
    }

    @Override
    public void onCancelOrderError(OrderCancelResponse orderCancelInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onCancelOrderError: " + orderCancelInfo.toString());
    }

    @Override
    public void OnQueryOptionAuctionInfo(OptionAuctionInfoResponse optionAuctionInfoResponse, ErrorMessage errorMessage, String sessionId){
        System.out.println("OnQueryOptionAuctionInfo: " + optionAuctionInfoResponse.toString());
    }

    @Override
    public void onInsertAlgoOrder(XTPStrategyInfoStruct strategyInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onInsertAlgoOrder XTPStrategyInfoStruct: " + strategyInfo.toString());
        System.out.println("errorMessage: " + errorMessage.toString());
    }

    @Override
    public void onCancelAlgoOrder(XTPStrategyInfoStruct strategyInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onCancelAlgoOrder XTPStrategyInfoStruct: " + strategyInfo.toString());
        System.out.println("errorMessage: " + errorMessage.toString());
    }

    @Override
    public void onQueryStrategy(XTPStrategyInfoStruct strategyInfo, String strategyParam, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onQueryStrategy XTPStrategyInfoStruct: " + strategyInfo.toString());
        System.out.println("onQueryStrategy strategyParam: " + strategyParam);
        System.out.println("onQueryStrategy errorMessage: " + errorMessage.toString());
    }

    @Override
    public void onAlgoUserEstablishChannel(String user, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onAIGOUserVerification user: " + user);
        System.out.println("onAIGOUserVerification: " + errorMessage.toString());
        System.out.println("onAIGOUserVerification sessionId: " + sessionId);
    }

    @Override
    public void onStrategyStateReport(XTPStrategyStateReportStruct strategyInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onStrategyStateReport strategyInfo: " + strategyInfo);
        System.out.println("onStrategyStateReport errorMessage: " + errorMessage.toString());
        System.out.println("onStrategyStateReport sessionId: " + sessionId);
    }

    @Override
    public void onAlgoDisconnected(int reason) {
        System.out.println("algo disconnected reason: " + reason);
    }

    @Override
    public void onAlgoConnected() {
        System.out.println("algo connected ");
    }

    /**
     * 融资融券业务中现金直接还款的响应
     * @param cashRepayRsp 现金直接还款通知的具体信息，用户可以通过cashRepayRsp.orderXtpId来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 现金还款发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onCreditCashRepay(XTPCrdCashRepayRsp cashRepayRsp, ErrorMessage errorMessage, String sessionId){
        System.out.println("onCreditCashRepay cashRepayRsp: " + cashRepayRsp);
        System.out.println("onCreditCashRepay errorMessage: " + errorMessage.toString());
        System.out.println("onCreditCashRepay sessionId: " + sessionId);
    }

    /**
     * 融资融券业务中现金还息的响应
     * @param cashRepayDebtInterestFeeRsp 现金还息通知的具体信息，用户可以通过cashRepayDebtInterestFeeRsp.orderXtpId来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 现金还息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onCreditCashRepayDebtInterestFee(XTPCrdCashRepayDebtInterestFeeRsp cashRepayDebtInterestFeeRsp, ErrorMessage errorMessage, String sessionId){
        System.out.println("onCreditCashRepayDebtInterestFee cashRepayDebtInterestFeeRsp: " + cashRepayDebtInterestFeeRsp);
        System.out.println("onCreditCashRepayDebtInterestFee errorMessage: " + errorMessage.toString());
        System.out.println("onCreditCashRepayDebtInterestFee sessionId: " + sessionId);
    }

    /**
     * 请求查询融资融券业务中的现金直接还款报单的响应
     * @param cashRepayInfo 查询到的某一笔现金直接还款通知的具体信息
     * @param errorMessage 查询现金直接报单发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditCashRepayInfo(XTPCrdCashRepayInfo cashRepayInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditCashRepayInfo cashRepayInfo: " + cashRepayInfo);
        System.out.println("onQueryCreditCashRepayInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditCashRepayInfo sessionId: " + sessionId);
    }

    /**
     * 请求查询信用账户额外信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundInfo 查询到的信用账户额外信息情况
     * @param errorMessage 查询信用账户额外信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditFundInfo(XTPCrdFundInfo fundInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditFundInfo fundInfo: " + fundInfo);
        System.out.println("onQueryCreditFundInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditFundInfo sessionId: " + sessionId);
    }

    /**
     * 请求查询信用账户负债信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtInfo 查询到的信用账户合约负债情况
     * @param errorMessage 查询信用账户负债信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditDebtInfo(XTPCrdDebtInfo debtInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditDebtInfo debtInfo: " + debtInfo);
        System.out.println("onQueryCreditDebtInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditDebtInfo sessionId: " + sessionId);
    }

    /**
     * 请求查询信用账户指定证券负债未还信息响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtStockInfo 查询到的信用账户指定证券负债未还信息情况
     * @param errorMessage 查询信用账户指定证券负债未还信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditTickerDebtInfo(XTPCrdDebtStockInfo debtStockInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditTickerDebtInfo debtStockInfo: " + debtStockInfo);
        System.out.println("onQueryCreditTickerDebtInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditTickerDebtInfo sessionId: " + sessionId);
    }

    /**
     * 请求查询信用账户待还资金的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param remainAmount 查询到的信用账户待还资金
     * @param errorMessage 查询信用账户待还资金发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditAssetDebtInfo(double remainAmount, ErrorMessage errorMessage,int requestId, String sessionId){
        System.out.println("onQueryCreditAssetDebtInfo remainAmount: " + remainAmount);
        System.out.println("onQueryCreditAssetDebtInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditAssetDebtInfo sessionId: " + sessionId);
    }

    /**
     * 请求查询信用账户可融券头寸信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param assignInfo 查询到的信用账户可融券头寸信息
     * @param errorMessage 查询信用账户可融券头寸信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditTickerAssignInfo(XTPClientQueryCrdPositionStkInfo assignInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditTickerAssignInfo assignInfo: " + assignInfo);
        System.out.println("onQueryCreditTickerAssignInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditTickerAssignInfo sessionId: " + sessionId);
    }

    /**
     * 融资融券业务中请求查询指定余券信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockInfo 查询到的余券信息
     * @param errorMessage 查询信用账户余券信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo stockInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditExcessStock stockInfo: " + stockInfo);
        System.out.println("onQueryCreditExcessStock errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditExcessStock sessionId: " + sessionId);
    }

    /**
     * 融资融券业务中请求查询余券信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param stockInfo 查询到的余券信息
     * @param errorMessage 查询信用账户余券信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryMulCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo stockInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryMulCreditExcessStock stockInfo: " + stockInfo);
        System.out.println("onQueryMulCreditExcessStock errorMessage: " + errorMessage.toString());
        System.out.println("onQueryMulCreditExcessStock sessionId: " + sessionId);
    }

    /**
     * 融资融券业务中负债合约展期的通知
     * @param debtExtendInfo 负债合约展期通知的具体信息，用户可以通过debt_extend_info.xtpid来管理订单，通过GetClientIDByXTPID() == clientId来过滤自己的订单。
     * @param errorMessage 负债合约展期订单被拒绝或者发生错误时错误代码和错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误。
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 当负债合约展期订单有状态变化的时候，会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的负债合约展期通知。
     */
    @Override
    public void onCreditExtendDebtDate(XTPCreditDebtExtendNotice debtExtendInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onCreditExtendDebtDate debtExtendInfo: " + debtExtendInfo);
        System.out.println("onCreditExtendDebtDate errorMessage: " + errorMessage.toString());
        System.out.println("onCreditExtendDebtDate sessionId: " + sessionId);
    }

    /**
     * 查询融资融券业务中负债合约展期订单响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param debtExtendInfo 查询到的负债合约展期情况
     * @param errorMessage 查询负债合约展期发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误。当errorMessage.errorId=11000350时，表明没有记录，当为其他非0值时，表明合约发生拒单时的错误原因
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditExtendDebtDateOrders(XTPCreditDebtExtendNotice debtExtendInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditExtendDebtDateOrders debtExtendInfo: " + debtExtendInfo);
        System.out.println("onQueryCreditExtendDebtDateOrders errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditExtendDebtDateOrders sessionId: " + sessionId);
    }

    /**
     * 查询融资融券业务中信用账户附加信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param fundExtraInfo 信用账户附加信息
     * @param errorMessage 查询信用账户附加信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditFundExtraInfo(XTPCrdFundExtraInfo fundExtraInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditFundExtraInfo fundExtraInfo: " + fundExtraInfo);
        System.out.println("onQueryCreditFundExtraInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditFundExtraInfo sessionId: " + sessionId);
    }

    /**
     * 查询融资融券业务中信用账户指定证券的附加信息的响应，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     * @param positionExtraInfo 信用账户指定证券的附加信息
     * @param errorMessage 查询信用账户附加信息发生错误时返回的错误信息，当errorMessage为空，或者errorMessage.errorId为0时，表明没有错误
     * @param sessionId 资金账户对应的sessionId，登录时得到
     * @remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
     */
    @Override
    public void onQueryCreditPositionExtraInfo(XTPCrdPositionExtraInfo positionExtraInfo, ErrorMessage errorMessage, String sessionId){
        System.out.println("onQueryCreditPositionExtraInfo positionExtraInfo: " + positionExtraInfo);
        System.out.println("onQueryCreditPositionExtraInfo errorMessage: " + errorMessage.toString());
        System.out.println("onQueryCreditPositionExtraInfo sessionId: " + sessionId);
    }

}
