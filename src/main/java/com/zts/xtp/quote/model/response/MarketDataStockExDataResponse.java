package com.zts.xtp.quote.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class MarketDataStockExDataResponse {
    /** 委托买入总量(SH,SZ) */
    private long totalBidQty;
    /** 委托卖出总量(SH,SZ) */
    private long totalAskQty;
    /** 加权平均委买价格(SH,SZ) */
    private double maBidPrice;
    /** 加权平均委卖价格(SH,SZ) */
    private double maAskPrice;
    /** 债券加权平均委买价格(SH) */
    private double maBondBidPrice;
    /** 债券加权平均委卖价格(SH) */
    private double maBondAskPrice;
    /** 债券到期收益率(SH) */
    private double yieldToMaturity;
    /** 基金实时参考净值(SH,SZ) */
    private double iopv;
    /** ETF申购笔数(SH) */
    private int etfBuyCount;
    /** ETF赎回笔数(SH) */
    private int etfSellCount;
    /** ETF申购数量(SH) */
    private double etfBuyQty;
    /** ETF申购金额(SH) */
    private double etfBuyMoney;
    /** ETF赎回数量(SH) */
    private double etfSellQty;
    /** ETF赎回金额(SH) */
    private double etfSellMoney;
    /** 权证执行的总数量(SH) */
    private double totalWarrantExecQty;
    /** 权证跌停价格（元）(SH) */
    private double warrantLowerPrice;
    /** 权证涨停价格（元）(SH) */
    private double warrantUpperPrice;
    /** 买入撤单笔数(SH) */
    private int cancelBuyCount;
    /** 卖出撤单笔数(SH) */
    private int cancelSellCount;
    /** 买入撤单数量(SH) */
    private double cancelBuyQty;
    /** 卖出撤单数量(SH) */
    private double cancelSellQty;
    /** 买入撤单金额(SH) */
    private double cancelBuyMoney;
    /** 卖出撤单金额(SH) */
    private double cancelSellMoney;
    /** 买入总笔数(SH) */
    private long totalBuyCount;
    /** 卖出总笔数(SH) */
    private long totalSellCount;
    /** 买入委托成交最大等待时间(SH) */
    private int durationAfterBuy;
    /** 卖出委托成交最大等待时间(SH) */
    private int durationAfterSell;
    /** 买方委托价位数(SH) */
    private int numBidOrders;
    /** 卖方委托价位数(SH) */
    private int numAskOrders;
    /** 基金T-1日净值(SZ) */
    private double preIopv;
}
