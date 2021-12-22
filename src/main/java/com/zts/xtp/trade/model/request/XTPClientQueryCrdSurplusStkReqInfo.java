package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.MarketType;
import lombok.*;

/**
 * Created by qingfei.hou on 2020/12/14.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"marketType", "ticker"})
@Builder
public class XTPClientQueryCrdSurplusStkReqInfo {
    /**交易市场*/
    private MarketType marketType;
    /**证券代码*/
    private String ticker;

    public int getMarketType() {
        if (this.marketType == null) {
            return 0;
        }
        return marketType.ordinal();
    }

    public void setMarketType(int marketType) {
        this.marketType = MarketType.forType(marketType);
    }
}
