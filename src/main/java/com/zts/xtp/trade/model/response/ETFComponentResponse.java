package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.EtfReplaceType;
import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;


@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"marketType", "ticker", "componentTicker", "componentName"})
@Builder
public class ETFComponentResponse {

    /**交易市场*/
    private MarketType marketType;
    /**ETF代码*/
    private String ticker;
    /**成份股代码*/
    private String componentTicker;
    /**成份股名称*/
    private String componentName;
    /**成份股数量*/
    private long quantity;
    /**成份股交易市场*/
    private MarketType componentMarketType;
    /**成份股替代标识*/
    private EtfReplaceType etfReplaceType;
    /**溢价比例*/
    private double premiumRatio;
    /**成分股替代标识为必须现金替代时候的总金额*/
    private double amount;
    /**申购溢价比例*/
    private double creationPremiumRatio;
    /**赎回溢价比例*/
    private double redemptionDiscountRatio;
    /**申购时，成分股替代标识为必须现金替代时候的总金额*/
    private double creationAmount;
    /**赎回时，成分股替代标识为必须现金替代时候的总金额*/
    private double redemptionAmount;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }

    public void setComponentMarketType(int type) {
        componentMarketType = MarketType.values()[type];
    }

    public void setEtfReplaceType(int type) {
        etfReplaceType = EtfReplaceType.values()[type];
    }
}