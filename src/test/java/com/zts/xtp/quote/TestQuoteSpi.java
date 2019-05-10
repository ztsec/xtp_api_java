package com.zts.xtp.quote;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.*;
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
    public void onTickByTickEntrust(int exchange_id, String ticker, long seq, long data_time, int type, int channel_no, long order_seq, double price, long qty, char side, char ord_type) {
        System.out.println("onTickByTickEntrust");
        System.out.println("exchange_id:"+exchange_id+" ticker:"+ticker+" seq:"+seq+" data_time:"+data_time+" type:"+type+" channel_no:"+channel_no+" order_seq:"+order_seq+" price:"+price+" qty:"+qty+" side:"+side+" ord_type:"+ord_type);
    }

    @Override
    public void onTickByTickTrade(int exchange_id, String ticker, long seq, long data_time, int type, int channel_no, long order_seq, double price, long qty, double money, long bid_no, long ask_no, char trade_flag) {
        System.out.println("onTickByTickTrade");
        System.out.println("exchange_id:"+exchange_id+" ticker:"+ticker+" seq:"+seq+" data_time:"+data_time+" type:"+type+" channel_no:"+channel_no+" order_seq:"+order_seq+" price:"+price+" qty:"+qty+" money:"+money+" bid_no:"+bid_no+" ask_no:"+ask_no+" trade_flag:"+trade_flag);
    }

//    @Override
//    public void onTickByTick(TickByTickResponse tickData) {
//        System.out.println("onTickByTick");
//        System.out.println(tickData);
//    }

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