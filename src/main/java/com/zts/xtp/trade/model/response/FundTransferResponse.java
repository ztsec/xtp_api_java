package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.FundOperStatus;
import com.zts.xtp.common.enums.FundTransferType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"serialId", "fundTransferType"})
@Builder
public class FundTransferResponse {

    /**资金内转编号*/
    private String serialId;
    /**内转类型*/
    FundTransferType fundTransferType;
    /**金额*/
    private double amount;
    /**操作结果*/
    FundOperStatus fundOperStatus;
    /**操作时间*/
    private String transferTime;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setFundTransferType(int type) {
        fundTransferType = FundTransferType.values()[type];
    }

    public void setFundOperStatus(int type) {
        fundOperStatus = FundOperStatus.values()[type];
    }
}
