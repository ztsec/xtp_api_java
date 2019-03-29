package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.SplitMergeStatus;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"exchangeType", "sfTicker", "ticker"})
@Builder
public class StructuredFundResponse {

    /**交易所代码*/
    private ExchangeType exchangeType;
    /**分级基金母基金代码*/
    private String sfTicker;
    /**分级基金母基金名称*/
    private String sfTickerName;
    /**分级基金子基金代码*/
    private String ticker;
    /**分级基金子基金名称*/
    private String tickerName;
    /**基金允许拆分合并状态*/
    private SplitMergeStatus splitMergeStatus;
    /**拆分合并比例*/
    private int ratio;
    /**最小拆分数量*/
    private int minSplitQty;
    /**最小合并数量*/
    private int minMergeQty;
    /**基金净值*/
    private double netPrice;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setExchangeType(int type) {
        exchangeType = ExchangeType.forType(type);
    }

    public void setSplitMergeStatus(int type) {
        splitMergeStatus = SplitMergeStatus.values()[type];
    }
}
