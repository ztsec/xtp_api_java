package com.zts.xtp.quote.model.response;

import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

/**
 * 逐笔委托(仅适用深交所)
 *
 * @author roger on 2018/5/22
 */
@Data
@NoArgsConstructor
@EqualsAndHashCode(of = {"channelNo", "seq"})
public class TickByTickEntrustResponse {

    /** 频道代码 */
    private int channelNo;

    /** 委托序号(在同一个channel_no内唯一，从1开始连续) */
    private long seq;

    /** 委托价格 */
    private double price;

    /** 委托数量 */
    private long qty;

    /** '1':买; '2':卖; 'G':借入; 'F':出借 */
    private char side;

    /** 订单类别: '1': 市价; '2': 限价; '3': 本方最优 */
    private char ordType;
}
