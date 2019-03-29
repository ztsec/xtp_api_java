package com.zts.xtp.demo.trade;

import com.zts.xtp.common.model.ErrorMessage;
import com.zts.xtp.trade.model.response.OrderResponse;
import com.zts.xtp.trade.model.response.TradeResponse;
import com.zts.xtp.trade.spi.TradeSpi;

public class TradeSpiImpl implements TradeSpi{

    public void onOrderEvent(OrderResponse orderInfo, ErrorMessage errorMessage, String sessionId) {
        System.out.println("onOrderEvent: " + orderInfo);
    }

    @Override
    public void onTradeEvent(TradeResponse tradeInfo, String sessionId) {
        System.out.println("onTradeEvent: " + tradeInfo);
    }
}
