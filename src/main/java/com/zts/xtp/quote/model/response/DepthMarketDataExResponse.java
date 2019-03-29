package com.zts.xtp.quote.model.response;


import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class DepthMarketDataExResponse {

    /** 买一队列数据 */
    private long[] bid1Qty;

    /** 买一队列的有效委托笔数 */
    private int bid1Count;

    /** 买一队列总委托笔数 */
    private int maxBid1Count;

    /** 卖一队列数据 */
    private long[] ask1Qty;

    /** 卖一队列的有效委托笔数 */
    private int ask1Count;

    /** 卖一队列总委托笔数 */
    private int maxAsk1Count;

}
