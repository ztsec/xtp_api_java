package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class IPOQuotaResponse {

    /**交易市场*/
    private MarketType marketType;
    /**可申购额度*/
    private int quantity;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }
}