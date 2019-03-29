package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.ExchangeType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"exchangeType", "ticker"})
@Builder
public class StructuredFundInfoQueryRequest {

    /**交易所代码，不可为空*/
    private ExchangeType exchangeType;
    /**分级基金母基金代码，可以为空，如果为空，则默认查询所有的分级基金*/
    private String ticker;

    public int getExchangeType() {
        return exchangeType.getType();
    }
}