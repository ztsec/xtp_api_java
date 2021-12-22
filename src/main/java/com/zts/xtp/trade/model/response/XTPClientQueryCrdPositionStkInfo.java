package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/21.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPClientQueryCrdPositionStkInfo {
    private MarketType marketType;          // 市场
    private String ticker;                  // 证券代码
    private String limitQty;                // 融券限量
    private String yesterdayQty;            // 昨日日融券数量
    private String leftQty;                 // 剩余可融券数量
    private String frozenQty;               // 冻结融券数量
    private int requestId;
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }
}
