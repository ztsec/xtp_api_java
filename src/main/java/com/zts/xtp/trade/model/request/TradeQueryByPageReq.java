package com.zts.xtp.trade.model.request;

import lombok.*;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class TradeQueryByPageReq {
    /**需要查询的成交回报条数*/
    private long reqCount;
    /**上一次收到的查询成交回报结果中带回来的索引，如果是从头查询，请置0*/
    private long reference;
    /**保留字段*/
    private long reserved;
}
