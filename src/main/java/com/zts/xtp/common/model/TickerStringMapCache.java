package com.zts.xtp.common.model;

import gnu.trove.map.TIntObjectMap;
import gnu.trove.map.hash.TIntObjectHashMap;
import java.text.NumberFormat;

public class TickerStringMapCache extends TIntObjectHashMap {
    private static TIntObjectMap tickerStringMapCache = new TIntObjectHashMap();
    private static NumberFormat nTickerFormatToString = NumberFormat.getInstance();

    public static TickerStringMapCache instance = null;
    public TickerStringMapCache() {
        nTickerFormatToString.setGroupingUsed(false);
        nTickerFormatToString.setMinimumIntegerDigits(6);
    }

    private static Integer LOCK = 0;

    public static TickerStringMapCache getInstance() {
        synchronized (LOCK) {
            if (instance == null) {
                instance = new TickerStringMapCache();
            }
            return instance;
        }
    }

    public static String getTickerFromTickerStringMapCache(int nTicker, int nTickerLength, int marketType) {
        //        if (nTickerLength != 6) {
//            nTickerFormatToString.setMinimumIntegerDigits(nTickerLength);
//        }
        int nTickerWithMarket = nTicker*100 + marketType;
        String ticker = (String)tickerStringMapCache.get(nTickerWithMarket);
        if (ticker == null) {

            synchronized (nTickerFormatToString) {
                nTickerFormatToString.setMinimumIntegerDigits(nTickerLength);
                ticker = nTickerFormatToString.format(nTicker);

                try {
                    tickerStringMapCache.put(nTickerWithMarket, ticker);
                } catch (Exception e) {
                    //TODO
                }
            }

        }
        return ticker == null?"":ticker;
    }

}
