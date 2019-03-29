package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"marketType", "etf"})
@Builder
public class ETFBaseResponse {

    /**交易市场*/
    private MarketType marketType;
    /**etf代码,买卖,申赎统一使用该代码*/
    private String etf;
    /**etf申购赎回代码*/
    private String subscribeRedemptionTicker;
    /**最小申购赎回单位对应的ETF份数,例如上证"50ETF"就是900000*/
    private int unit;
    /**是否允许申购,1-允许,0-禁止*/
    private int subscribeStatus;
    /**是否允许赎回,1-允许,0-禁止*/
    private int redemptionStatus;
    /**最大现金替代比例,小于1的数值 TODO 是否采用double*/
    private double maxCashRatio;
    /**T日预估金额*/
    private double estimateAmount;
    /**T-X日现金差额 */
    private double cashComponent;
    /**基金单位净值*/
    private double netValue;
    /**最小申赎单位净值总金额=net_value*unit*/
    private double totalAmount;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }

}