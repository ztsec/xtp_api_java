package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.MarketType;
import com.zts.xtp.common.enums.TickerType;
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
public class IPOTickerResponse {

    /**交易市场*/
    private MarketType marketType;
    /**申购代码*/
    private String ticker;
    /**申购股票名称*/
    private String tickerName;
    /**证券类别*/
    private TickerType tickerType;
    /**申购价格*/
    private double price;
    /**申购单元*/
    private int unit;
    /**最大允许申购数量*/
    private int qtyUpperLimit;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }
    public void setTickerType(int tickerType){
        this.tickerType = TickerType.values()[tickerType];
    }
}