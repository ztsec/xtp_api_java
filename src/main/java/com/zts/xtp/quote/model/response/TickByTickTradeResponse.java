package com.zts.xtp.quote.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

/**
 * 逐笔成交
 *
 * @author roger on 2018/5/22
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"channelNo", "seq"})
@Builder
public class TickByTickTradeResponse {

    /** 频道代码 */
    private int channelNo;

    /** 委托序号(在同一个channel_no内唯一，从1开始连续) */
    private long seq;

    /** 成交价格 */
    private double price;

    /** 成交量 */
    private long qty;

    /** 成交金额(仅适用上交所) */
    private double money;

    /** 买方订单号 */
    private long bidNo;

    /** 卖方订单号 */
    private long askNo;

    /** SH: 内外盘标识('B':主动买; 'S':主动卖; 'N':未知) SZ: 成交标识('4':撤; 'F':成交) */
    private char tradeFlag;
}
