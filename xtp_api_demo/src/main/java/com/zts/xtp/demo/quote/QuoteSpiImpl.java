package com.zts.xtp.demo.quote;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.quote.model.response.DepthMarketDataResponse;
import com.zts.xtp.quote.model.response.DepthMarketDataExResponse;
import com.zts.xtp.quote.model.response.SpecificTickerResponse;
import com.zts.xtp.quote.spi.QuoteSpi;

public class QuoteSpiImpl implements QuoteSpi{

    @Override
    public void onSubMarketData(SpecificTickerResponse ticker, ErrorMessage errorMessage) {
        System.out.println("onSubMarketData");
        System.out.println(ticker);
        System.out.println(errorMessage);
    }

    @Override
    public void onDepthMarketData(DepthMarketDataResponse depthMarketData,
        DepthMarketDataExResponse depthQuote) {
        System.out.println("onDepthMarketData");
        System.out.println(depthMarketData);
        System.out.println(depthQuote);
    }
}
