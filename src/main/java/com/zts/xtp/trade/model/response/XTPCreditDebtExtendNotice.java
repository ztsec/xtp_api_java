package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.DebtExtendOperStatus;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/21.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCreditDebtExtendNotice {
    private String xtpId;					    // XTP系统订单ID，无需用户填写，在XTP系统中唯一
    private String debtId;	                    // 负债合约编号
    private DebtExtendOperStatus operStatus;	// 展期请求操作状态
    private String operTime;					// 操作时间
    private int requestId;                      // 此消息响应函数对应的请求ID
    private boolean lastResp;                   // 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应


    public void setOperStatus(int status) {
        operStatus = DebtExtendOperStatus.values()[status];
    }
}
