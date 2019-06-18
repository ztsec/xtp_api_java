package com.zts.xtp.quote.model.response;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.MarketDataType;
import com.zts.xtp.common.enums.PositionEffectType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"exchangeType", "ticker", "dataTime"})
@Builder
public class DepthMarketDataResponse {
    /** 交易所代码 */
    private ExchangeType exchangeType;
    /** 合约代码（不包含交易所信息） */
    private String ticker;
    /** 最新价 */
    private double lastPrice;
    /** 昨收盘 */
    private double preClosePrice;
    /** 今开盘 */
    private double openPrice;
    /** 最高价 */
    private double highPrice;
    /** 最低价 */
    private double lowPrice;
    /** 今收盘 */
    private double closePrice;
    /** 昨日持仓量(张)(目前未填写) */
    private long preTotalLongPositon;
    /** 持仓量(张) */
    private long totalLongPositon;
    /** 昨日结算价 */
    private double preSettlementPrice;
    /** 今日结算价 */
    private double settlementPrice;
    /** 涨停价 */
    private double upperLimitPrice;
    /** 跌停价 */
    private double lowerLimitPrice;
    /** 预留 */
    private double preDelta;
    /** 预留 */
    private double currDelta;
    /** 时间类，格式为YYYYMMDDHHMMSSsss */
    private long dataTime;
    /** 数量，为总成交量（单位股，与交易所一致） */
    private long qty;
    /** 成交金额，为总成交金额（单位元，与交易所一致） */
    private double turnover;
    /** 当日均价=(turnover/qty) */
    private double avgPrice;
    /** 十档申买价 */
    private double[] bid;
    /** 十档申卖价 */
    private double[] ask;
    /** 十档申买量 */
    private long[] bidQty;
    /** 十档申卖量 */
    private long[] askQty;
    /** 成交笔数 */
    private long tradesCount;
    /** 当前交易状态说明 */
    private String tickerStatus;
    /** 股票、基金、债券等额外数据 */
    private MarketDataStockExDataResponse stk;
    /** 期权额外数据 */
    private MarketDataOptionExDataResponse opt;
    /** 决定了额外数据是哪种数据类型 stk or opt */
    private MarketDataType dataType;

    public void setDataType(int type){
        if(MarketDataType.values().length-1<type){
            this.dataType = MarketDataType.XTP_MARKETDATA_ACTUAL;//如果类型未找到，默认现货
            System.err.println("Error: received MarketDataType value exceed MarketDataType enum size, receive="+type);
        }else{
            this.dataType = MarketDataType.values()[type];
        }
    }

    public void setExchangeType(int type) {
        this.exchangeType = ExchangeType.forType(type);
    }
}
