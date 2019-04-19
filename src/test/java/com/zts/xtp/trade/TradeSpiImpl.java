package com.zts.xtp.trade;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.trade.model.response.*;
import com.zts.xtp.trade.spi.TradeSpi;

public class TradeSpiImpl implements TradeSpi {

    @Override
    public void onOrderEvent(OrderResponse orderInfo, ErrorMessage errorMessage,
        String sessionId) {
        System.out.println("onOrderEvent orderInfo: " + orderInfo.toString());
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
    public void onFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage,
        String sessionId) {
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
    public void onQueryETFBasket(ETFComponentResponse etfComponentInfo, ErrorMessage errorMessage,
        String sessionId) {
        System.out.println("onQueryETFBasket: " + etfComponentInfo.toString());
    }

    @Override
    public void onQueryFundTransfer(FundTransferResponse fundTransferInfo, ErrorMessage errorMessage,
        String sessionId) {
        if (fundTransferInfo != null) {
            System.out.println("onQueryFundTransfer: " + fundTransferInfo.toString());
        } else {
            System.out.println("onQueryFundTransfer error: " + errorMessage.toString());
        }
    }

    @Override
    public void onQueryIPOInfoList(IPOTickerResponse ipoTickerInfo, ErrorMessage errorMessage,
        String sessionId) {
        if (ipoTickerInfo != null) {
            System.out.println("onQueryIPOInfoList: " + ipoTickerInfo.toString());
        } else {
            System.out.println("onQueryIPOInfoList error: " + errorMessage.toString());
        }

    }

    @Override
    public void onQueryIPOQuotaInfo(IPOQuotaResponse ipoQuotaInfo, ErrorMessage errorMessage,
        String sessionId) {
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
    public void onQueryPosition(StockPositionResponse stockPositionInfo, ErrorMessage errorMessage,
        String sessionId) {
        System.out.println("onQueryPosition: " + stockPositionInfo.toString());
    }

    @Override
    public void onQueryStructuredFund(StructuredFundResponse structuredFundInfo,
        ErrorMessage errorMessage, String sessionId) {
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
    public void onTradeEvent(TradeResponse tradeInfo, String sessionId) {
        System.out.println("onTradeEvent: " + tradeInfo.toString());
    }

    @Override
    public void onCancelOrderError(OrderCancelResponse orderCancelInfo, ErrorMessage errorMessage,
        String sessionId) {
        System.out.println("onCancelOrderError: " + orderCancelInfo.toString());
    }

    @Override
    public void OnQueryOptionAuctionInfo(OptionAuctionInfoResponse optionAuctionInfoResponse, ErrorMessage errorMessage, String sessionId){
        System.out.println("OnQueryOptionAuctionInfo: " + optionAuctionInfoResponse.toString());
    }
}
