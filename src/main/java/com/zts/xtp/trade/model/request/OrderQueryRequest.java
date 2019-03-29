package com.zts.xtp.trade.model.request;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"ticker"})
@Builder
@Getter
public class OrderQueryRequest {

    /**证券代码，可以为空，如果为空，则默认查询时间段内的所有成交回报*/
    private String ticker;
    /**格式为YYYYMMDDHHMMSSsss，为0则默认当前交易日0点*/
    private long beginTime;
    /**格式为YYYYMMDDHHMMSSsss，为0则默认当前时间*/
    private long endTime;

}
