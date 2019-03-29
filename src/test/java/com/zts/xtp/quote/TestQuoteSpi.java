package com.zts.xtp.quote;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.DepthMarketDataResponse;
import com.zts.xtp.quote.model.response.DepthMarketDataExResponse;
import com.zts.xtp.quote.model.response.OrderBookResponse;
import com.zts.xtp.quote.model.response.SpecificTickerResponse;
import com.zts.xtp.quote.model.response.TickByTickResponse;
import com.zts.xtp.quote.model.response.TickerInfoResponse;
import com.zts.xtp.quote.model.response.TickerPriceInfoResponse;
import com.zts.xtp.quote.spi.QuoteSpi;

/**
 * Created by Guan Jian on 2018/8/15.
 */
public class TestQuoteSpi implements QuoteSpi {

    @Override
    public void onDisconnected(int reason) {
        System.out.println("onDisconnected");
        System.out.println(reason);
    }

    @Override
    public void onSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onSubMarketData");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onUnSubMarketData");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onSubOrderBook");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubOrderBook(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onUnSubOrderBook");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onOrderBook(OrderBookResponse orderBook) {
        System.out.println("onOrderBook");
        System.out.println(orderBook);
    }

    @Override
    public void onSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onSubTickByTick");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubTickByTick(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onUnSubTickByTick");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onTickByTick(TickByTickResponse tickData) {
        System.out.println("onTickByTick");
        System.out.println(tickData);
    }

    @Override
    public void onSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllMarketData");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllMarketData(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllMarketData");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllOrderBook");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllOrderBook(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllOrderBook");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllTickByTick");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllTickByTick(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllTickByTick");
        System.out.println(exchangeId);
        System.out.println(errorMessage);
    }

    @Override
    public void onQueryAllTickers(TickerInfoResponse tickerInfo, ErrorMessage errorMessage) {
        System.out.println("onQueryAllTickers");
        System.out.println(tickerInfo);
        System.out.println(errorMessage);
    }

    @Override
    public void onQueryTickersPriceInfo(TickerPriceInfoResponse tickerInfo, ErrorMessage errorMessage) {
        System.out.println("onQueryTickersPriceInfo");
        System.out.println(tickerInfo);
        System.out.println(errorMessage);
    }

    @Override
    public void onDepthMarketData(DepthMarketDataResponse depthMarketData, DepthMarketDataExResponse depthQuote) {
        System.out.println("onDepthMarketData");
        System.out.println(depthMarketData);
        System.out.println(depthQuote);
    }

    @Override
    public void onError(ErrorMessage errorMessage) {
        System.out.println("onerror:"+errorMessage.getErrorMsg());
    }

    @Override
    public void onSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllOptionMarketData");
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllOptionMarketData(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllOptionMarketData");
        System.out.println(errorMessage);
    }

    @Override
    public void onSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllOptionOrderBook");
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllOptionOrderBook(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllOptionOrderBook");
        System.out.println(errorMessage);
    }

    @Override
    public void onSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onSubscribeAllOptionTickByTick");
        System.out.println(errorMessage);
    }

    @Override
    public void onUnSubscribeAllOptionTickByTick(int exchangeId, ErrorMessage errorMessage) {
        System.out.println("onUnSubscribeAllOptionTickByTick");
        System.out.println(errorMessage);
    }
}