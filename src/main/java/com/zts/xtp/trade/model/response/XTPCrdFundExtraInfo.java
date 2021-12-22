package com.zts.xtp.trade.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/15.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdFundExtraInfo {
    private double mfRsAvlUsed;    // 当前资金账户购买货币基金使用的融券卖出所得资金占用
    private String reserve;        // 预留空间
    private int requestId;         // 此消息响应函数对应的请求ID
}
