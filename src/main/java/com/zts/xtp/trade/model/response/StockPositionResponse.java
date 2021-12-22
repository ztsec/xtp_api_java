package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.MarketType;
import com.zts.xtp.common.enums.PositionDirectionType;
import com.zts.xtp.common.enums.PositionSecurityType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"ticker", "marketType"})
@Builder
public class StockPositionResponse {

    /**证券代码*/
    private String ticker;
    /**证券名称*/
    private String tickerName;
    /**交易市场*/
    private MarketType marketType;
    /**总持仓*/
    private long totalQty;
    /**可卖持仓*/
    private long sellableQty;
    /**持仓成本*/
    private double avgPrice;
    /**浮动盈亏*/
    private double unrealizedPnl;
    /**昨日持仓*/
    private long yesterdayPosition;
    /**今日申购赎回数量*/
    private long purchaseRedeemableQty;
    /**持仓方向*/
    private PositionDirectionType positionDirectionType;
    /**保留字段1*/
    private PositionSecurityType positionSecurityType;
    /**可行权合约*/
    private long executableOption;
    /**可锁定标的*/
    private long lockablePosition;
    /**可行权标的*/
    private long executableUnderlying;
    /**已锁定标的*/
    private long lockedPosition;
    /**可用已锁定标的*/
    private long usableLockedPosition;
    /**盈亏成本价*/
    private double profitPrice;
    /**保留字段*/
    private long unknown;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }

    public void setPositionDirectionType(int type) {
        positionDirectionType = PositionDirectionType.values()[type];
    }

    public void setPositionSecurityType(int type) {
        positionSecurityType = PositionSecurityType.values()[type];
    }
}
