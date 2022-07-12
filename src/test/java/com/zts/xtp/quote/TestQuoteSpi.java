package com.zts.xtp.quote;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.*;
import com.zts.xtp.quote.spi.QuoteSpi;
//import org.junit.Assert;

/**
 * Created by Guan Jian on 2018/8/15.
 */
public class TestQuoteSpi implements QuoteSpi {

    @Override
    public void onDisconnected(int reason) {
        System.out.println("onDisconnected");
        System.out.println(reason);
//        Assert.assertEquals(0, 0);
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
/*
    @Override
    public void onOrderBook(OrderBookResponse orderBook) {
        if (orderBook.getTicker().equals("300498")) {
            System.out.println("onOrderBook");
            System.out.println(orderBook);
        }
    }
*/
    @Override
    public void onOrderBook(int exchangeId, String ticker, double lastPrice, long qty, double turnOver, long tradesCount,
                        double[] bid, double[] ask, long[] bidQty, long[] askQty, long dataTime) {
        System.out.println("onOrderBook");
        System.out.println("exchangeId: " + exchangeId);
        System.out.println("ticker: " + ticker);
        System.out.println("lastPrice: " + lastPrice);
        System.out.println("qty: " + qty);
        System.out.println("turnOver: " + turnOver);
        System.out.println("tradeCount: " + tradesCount);
        System.out.println("dataTime: " + dataTime);
        for (int i = 0; i < 10; i++) {
            System.out.println("bid " + i + " " + bid[i]);
        }
        for (int i = 0; i < 10; i++) {
            System.out.println("ask " + i + " " + ask[i]);
        }
        for (int i = 0; i < 10; i++) {
            System.out.println("bidQty " + i + " " + bidQty[i]);
        }
        for (int i = 0; i < 10; i++) {
            System.out.println("askQty " + i + " " + askQty[i]);
        }
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
    public void onQueryAllTickersFullInfo(TickerFullInfoResponse tickerInfo, ErrorMessage errorMessage) {
        System.out.println("onQueryAllTickersFullInfo");
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
    public void onDepthMarketData(int exchangeType,String ticker,double lastPrice,double preClosePrice,double openPrice,
                                  double highPrice,double lowPrice,double closePrice, double upperLimitPrice,double lowerLimitPrice,
                                  long dataTime,long qty,double turnover,double avgPrice,double[] bid,
                                  double[] ask,long[] bidQty,long[] askQty,long tradesCount,String tickerStatus,double stkIopv,
                                  int dataType, MarketDataStockExDataResponse stkEx, MarketDataOptionExDataResponse optionEx,
                                  MarketDataBondExDataResponse bondEx) {

        System.out.println("TestQuoteApi onDepthMarketData");
        System.out.println(ticker);
        System.out.println(exchangeType);
        System.out.println(bid[0]);
        System.out.println(ask[0]);
        System.out.println(bidQty[1]);
        System.out.println(askQty[1]);
        System.out.println("stkEx: " + stkEx);
        System.out.println("optionEx: " + optionEx);
        System.out.println("bondEx: " + bondEx);

    }

    @Override
    public void onDepthFullMarketData(int exchangeType,String ticker,double lastPrice,double preClosePrice,double openPrice,
                                      double highPrice,double lowPrice,double closePrice,double upperLimitPrice,
                                      double lowerLimitPrice,long dataTime,long qty,double turnover,double avgPrice,double[] bid,
                                      double[] ask,long[] bidQty,long[] askQty,long tradesCount,String tickerStatus,double stkIopv,
                                      int dataType, MarketDataStockExDataResponse stkEx, MarketDataOptionExDataResponse optionEx,
                                      MarketDataBondExDataResponse bondEx, long[] bid1Qty, int bid1Count, int maxBid1Count,
                                      long[] ask1Qty, int ask1Count, int maxAsk1Count) {
        System.out.println("TestQuoteApi onDepthFullMarketData");
        System.out.println("ticker: " + ticker);
        System.out.println("exchangeType: " + exchangeType);
//        System.out.println(bid[0]);
//        System.out.println(ask[0]);
//        System.out.println(bidQty[1]);
//        System.out.println(askQty[1]);
        System.out.println("bid1Qty[0]: " + bid1Qty[0]);
//        System.out.println("bid1Count: " + bid1Count);
//        System.out.println("maxBid1Count: " + maxBid1Count);
        System.out.println("ask1Qty[0]: " + ask1Qty[0]);
//        System.out.println("ask1Count: " + ask1Count);
//        System.out.println("maxAsk1Count: " + maxAsk1Count);

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