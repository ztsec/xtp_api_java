package com.zts.xtp.trade.model.request;

import lombok.*;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
@Getter
public class OrderQueryByPageReq {
    /**需要查询的订单条数*/
    private long reqCount;
    /**上一次收到的查询订单结果中带回来的索引，如果是从头查询，请置0*/
    private long reference;
    /**保留字段*/
    private long reserved;
}
