package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/17.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPClientQueryCrdSurplusStkRspInfo {
    private MarketType marketType;             // 市场
    private String ticker;                     // 证券代码
    private String transferableQuantity;       // 可划转数量
    private String transferredQuantity;        // 已划转数量
    private int requestId;
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }
}
