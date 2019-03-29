package com.zts.xtp.trade.model.request;



import com.zts.xtp.common.enums.FundTransferType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class FundTransferRequest {
    /**资金内转编号，无需用户填写，类似于xtp_id*/
    private String serialId;
    /**资金账户代码*/
    private String fundAccount;
    /**资金账户密码*/
    private String password;
    /**金额*/
    private double amount;
    /**内转类型*/
    private FundTransferType fundTransferType;

    public int getFundTransferType() {
        return fundTransferType.ordinal();
    }


}