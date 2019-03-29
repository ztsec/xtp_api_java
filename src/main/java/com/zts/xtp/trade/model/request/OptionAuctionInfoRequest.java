package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class OptionAuctionInfoRequest {
    private MarketType marketType;
    private String ticker;

    public int getMarketType() {
        return marketType.ordinal();
    }
}
