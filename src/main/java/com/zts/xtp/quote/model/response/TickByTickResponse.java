package com.zts.xtp.quote.model.response;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.PositionDirectionType;
import com.zts.xtp.common.enums.TickByTickType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

/** @author roger on 2018/5/22 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"exchangeType", "ticker", "seq", "dataTime", "type"})
@Builder
public class TickByTickResponse {

    /** 交易所代码 */
    private ExchangeType exchangeType;

    /** 合约代码 char[16] */
    private String ticker;

    /** 预留 */
    private long seq;

    /** 委托时间 or 成交时间，格式为 yyyyMMddHHmmssSSS */
    private long dataTime;

    /** 委托 or 成交 */
    private TickByTickType type;

    /** 逐笔委托 */
    private TickByTickEntrustResponse entrust;

    /** 逐笔成交 */
    private TickByTickTradeResponse trade;

    public void setExchangeType(int exchangeType) {
        this.exchangeType = ExchangeType.forType(exchangeType);
    }
    public void setTickByTickType(int tickByTickType) {
        this.type = TickByTickType.forType(tickByTickType);
    }
}
