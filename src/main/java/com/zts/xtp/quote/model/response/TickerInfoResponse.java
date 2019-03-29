package com.zts.xtp.quote.model.response;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.TickerType;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;


@Data
@NoArgsConstructor
@AllArgsConstructor
public class TickerInfoResponse {

    /** 交易所代码 */
    private ExchangeType exchangeType;

    /** 合约代码 */
    private String ticker;

    /** 合约名称  */
    private String tickerName;

    /** 合约类型 */
    private TickerType tickerType;

    /** 昨收盘 */
    private double preClosePrice;

    /** 涨停板价 */
    private double upperLimitPrice;

    /** 跌停板价 */
    private double lowerLimitPrice;

    /** 最小变动价位 */
    private double priceTick;

    /** 合约最小交易量(买)  */
    private int buyQtyUnit;

    /** 合约最小交易量(卖) */
    private int sellQtyUnit;

    /** 是否最后一条记录 */
    private boolean lastResp;

    public void setExchangeType(int exchangeType) {
        this.exchangeType = ExchangeType.values()[exchangeType - 1];
    }

    public void setTickerType(int tickerType) {
        this.tickerType = TickerType.values()[tickerType];
    }
}
