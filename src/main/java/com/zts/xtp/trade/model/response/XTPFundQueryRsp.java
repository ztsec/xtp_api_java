package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.XTPFundQueryType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPFundQueryRsp {
    /**金额*/
    private double amount;
    /**查询类型*/
    private XTPFundQueryType queryType;
    /**此消息响应函数对应的请求ID*/
    private int requestId;

    public void setXTPFundQueryType(int type) {
        queryType = XTPFundQueryType.values()[type];
    }
}
