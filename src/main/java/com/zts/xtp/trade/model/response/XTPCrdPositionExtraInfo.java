package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/18.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdPositionExtraInfo {
    private MarketType marketType;      // 证券市场
    private String ticker;              // 证券代码
    private double mfRsAvlUsed;         // 购买货币基金使用的融券卖出所得资金占用
    private String reserve;             // 预留空间
    private int requestId;              // 此消息响应函数对应的请求ID
    private boolean lastResp;           // 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应

    public void setMarketType(int market) {
        marketType = MarketType.values()[market];
    }
}
