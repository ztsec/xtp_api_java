package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/14.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdDebtStockInfo {
    private MarketType marketType;          // 市场
    private String ticker;                  // 证券代码
    private String stockRepayQuantity;      // 融券负债可还券数量
    private String stockTotalQuantity;      // 融券负债未还总数量
    private int requestId;
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }
}
