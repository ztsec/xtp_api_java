package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.XTPFundQueryType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPFundQueryReq {
    /**资金账户代码*/
    private String fundAccount;
    /**资金账户密码*/
    private String password;
    /**查询类型*/
    private XTPFundQueryType queryType;

    public int getXTPFundQueryType() {
        return queryType.ordinal();
    }
}
