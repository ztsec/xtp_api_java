package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import com.zts.xtp.common.enums.OptCallOrPutType;
import com.zts.xtp.common.enums.OptExerciseType;
import com.zts.xtp.common.enums.TickerType;
import lombok.*;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class OptionAuctionInfoResponse {

    /**合约编码，报单ticker采用本字段*/
    private String ticker;
    /**证券代码源*/
    private MarketType securityIdSource;
    /**合约简称*/
    private String  symbol;
    /**合约交易代码*/
    private String  contractId;
    /**基础证券代码*/
    private String  underlyingSecurityId;
    /**基础证券代码源*/
    private MarketType  underlyingSecurityIdSource;
    /**上市日期，格式为YYYYMMDD*/
    private long listDate;
    /**最后交易日，格式为YYYYMMDD*/
    private long   lastTradeDate;
    /**证券类别*/
    private TickerType tickerType;
    /**是否支持当日回转交易，1-允许，0-不允许*/
    private int dayTrading;
    /**认购或认沽*/
    private OptCallOrPutType callOrPut;
    /**行权交割日，格式为YYYYMMDD*/
    private long  deliveryDay;
    /**交割月份，格式为YYYYMM*/
    private long  deliveryMonth;
    /**行权方式*/
    private OptExerciseType exerciseType;
    /**行权起始日期，格式为YYYYMMDD*/
    private long  exerciseBeginDate;
    /**行权结束日期，格式为YYYYMMDD*/
    private long  exerciseEndDate;
    /**行权价格*/
    private double  exercisePrice;
    /**数量单位，对于某一证券申报的委托，其委托数量字段必须为该证券数量单位的整数倍*/
    private long  qtyUnit;
    /**合约单位*/
    private long  contractUnit;
    /**合约持仓量*/
    private long  contractPosition;
    /**合约前收盘价*/
    private double prevClosePrice;
    /**合约前结算价*/
    private double prevClearingPrice;
    /**限价买最大量*/
    private long lmtBuyMaxQty;
    /**限价买最小量*/
    private long lmtBuyMinQty;
    /**限价卖最大量*/
    private long lmtSellMaxQty;
    /**限价卖最小量*/
    private long lmtSellMinQty;
    /**市价买最大量*/
    private long mktBuyMaxQty;
    /**市价买最小量*/
    private long mktBuyMinQty;
    /**市价卖最大量*/
    private long mktSellMaxQty;
    /**市价卖最小量*/
    private long mktSellMinQty;
    /**最小报价单位*/
    private double  priceTick;
    /**涨停价*/
    private double upperLimitPrice;
    /**跌停价*/
    private double lowerLimitPrice;
    /**今卖开每张保证金*/
    private double sellMargin;
    /**交易所保证金比例计算参数一*/
    private double marginRatioParam1;
    /**交易所保证金比例计算参数二*/
    private double marginRatioParam2;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setSecurityIdSource(int securityIdSource) {
        this.securityIdSource = MarketType.values()[securityIdSource];
    }

    public void setUnderlyingSecurityIdSource(int underlyingSecurityIdSource){
        this.underlyingSecurityIdSource = MarketType.values()[underlyingSecurityIdSource];
    }

    public void setTickerType(int tickerType){
        this.tickerType = TickerType.values()[tickerType];
    }

    public void setCallOrPut(int callOrPut){
        this.callOrPut = OptCallOrPutType.values()[callOrPut - 1];
    }

    public void setExerciseType(int exerciseType){
        this.exerciseType = OptExerciseType.values()[exerciseType - 1];
    }
}
