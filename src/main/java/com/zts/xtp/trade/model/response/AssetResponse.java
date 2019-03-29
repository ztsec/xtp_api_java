package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.AccountType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class AssetResponse {

    /**总资产(=可用资金 + 证券资产（目前为0）+ 预扣的资金)*/
    private double totalAsset;
    /**可用资金*/
    private double buyingPower;
    /**证券资产*/
    private double securityAsset;
    /**累计买入成交证券占用资金*/
    private double fundBuyAmount;
    /**累计买入成交交易费用*/
    private double fundBuyFee;
    /**累计卖出成交证券所得资金*/
    private double fundSellAmount;
    /**累计卖出成交交易费用*/
    private double fundSellFee;
    /**XTP系统预扣的资金（包括购买卖股票时预扣的交易资金+预扣手续费）*/
    private double withholdingAmount;
    /**账户类型*/
    private AccountType accountType;
    /**冻结的保证金*/
    private double frozenMargin;
    /**行权冻结资金*/
    private double frozenExecCash;
    /**行权费用*/
    private double frozenExecFee;
    /**垫付资金*/
    private double payLater;
    /**预垫付资金*/
    private double preadvaPay;
    /**昨日余额*/
    private double origBanlance;
    /**当前余额*/
    private double banlance;
    /**当天出入金*/
    private double depositWithdraw;
    /**当日交易资金轧差*/
    private double tradeNetting;
    /**资金资产*/
    private double captialAsset;
    /**强锁资金*/
    private double forceFreezeAmount;
    /**可取资金*/
    private double preferredAmount;
    /**保留字段*/
    private long unknown;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setAccountType(int type) {
        accountType = AccountType.values()[type];
    }
}
