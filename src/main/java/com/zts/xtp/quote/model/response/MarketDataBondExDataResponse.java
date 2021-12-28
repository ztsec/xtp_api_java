package com.zts.xtp.quote.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class MarketDataBondExDataResponse {
    /**委托买入总量(SH,SZ)*/
    private long totalBidQty;
    /**委托卖出总量(SH,SZ)*/
    private long totalAskQty;
    /**加权平均委买价格(SH,SZ)*/
    private double maBidPrice;
    /**加权平均委卖价格(SH,SZ)*/
    private double maAskPrice;
    /**债券加权平均委买价格(SH)*/
    private double maBondBidPrice;
    /**债券加权平均委卖价格(SH)*/
    double maBondAskPrice;
    /**债券到期收益率(SH)*/
    private double yieldToMaturity;
    /**预留*/
    private double r0;
    /**预留*/
    private long r1;
    /**预留*/
    private double r2;
    /**预留*/
    private double r3;
    /**预留*/
    private double r4;
    /**预留*/
    private double r5;
    /**预留*/
    private double r6;
    /**预留*/
    private double r7;
    /**预留*/
    private double r8;
    /**买入撤单笔数(SH)*/
    private int cancelBuyCount;
    /**卖出撤单笔数(SH)*/
    private int cancelSellCount;
    /**买入撤单数量(SH)*/
    private double cancelBuyQty;
    /**卖出撤单数量(SH)*/
    private double cancelSellQty;
    /**买入撤单金额(SH)*/
    private double cancelBuyMoney;
    /**卖出撤单金额(SH)*/
    private double cancelSellMoney;
    /**买入总笔数(SH)*/
    private long totalBuyCount;
    /**卖出总笔数(SH)*/
    private long totalSellCount;
    /**买入委托成交最大等待时间(SH)*/
    private int durationAfterBuy;
    /**卖出委托成交最大等待时间(SH)*/
    private int durationAfterSell;
    /**买方委托价位数(SH)*/
    private int numBidOrders;
    /**卖方委托价位数(SH)*/
    private int numAskOrders;
    /**时段(SHL2)，L1快照数据没有此字段，具体字段说明参阅《上海新债券Level2行情说明.doc》文档*/
    private String instrumentStatus;
}
