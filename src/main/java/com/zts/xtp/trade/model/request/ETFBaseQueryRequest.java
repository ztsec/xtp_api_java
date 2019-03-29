package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"marketType", "ticker"})
@Builder
public class ETFBaseQueryRequest {

    /**交易市场*/
    private MarketType marketType;
    /**ETF买卖代码*/
    private String ticker;

    public int getMarketType() {
        return marketType.ordinal();
    }

    public void setMarketType(int marketType) {
        this.marketType = MarketType.forType(marketType);
    }


}