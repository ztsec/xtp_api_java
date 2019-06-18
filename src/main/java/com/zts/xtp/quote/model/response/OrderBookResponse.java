package com.zts.xtp.quote.model.response;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.MarketDataType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

/**
 * 订单簿模型
 *
 * @author roger on 2018/5/15
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"exchangeType", "ticker", "dataTime"})
@Builder
public class OrderBookResponse {

    /** 交易所代码 */
    private ExchangeType exchangeType;

    /** 合约代码 char[16] */
    private String ticker;

    /** 时间类，格式为 yyyyMMddHHmmssSSS */
    private long dataTime;

    /** 最新价 */
    private double lastPrice;

    /** 数量，为总成交量（单位股，与交易所一致） */
    private long qty;

    /** 成交金额，为总成交金额（单位元，与交易所一致） */
    private double turnover;

    /** 成交笔数 */
    private long tradesCount;

    /** 十档申买价 */
    private double[] bid;

    /** 十档申卖价 */
    private double[] ask;

    /** 十档申买量 */
    private long[] bidQty;

    /** 十档申卖量 */
    private long[] askQty;

    public void setExchangeType(int exchangeType) {
        if(ExchangeType.values().length<exchangeType){
            this.exchangeType = ExchangeType.UNKNOWN;
            System.err.println("Error: OrderBookResponse received ExchangeType value exceed ExchangeType enum size, receive="+exchangeType);
        }else{
            this.exchangeType = ExchangeType.values()[exchangeType - 1];
        }
    }

    public void setExchangeType(ExchangeType exchangeType) {
        this.exchangeType = exchangeType;
    }

}
