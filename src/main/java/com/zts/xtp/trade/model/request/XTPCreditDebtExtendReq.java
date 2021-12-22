package com.zts.xtp.trade.model.request;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/18.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCreditDebtExtendReq {
    private String orderXtpId;		 // xtpid
    private String debtId;	         // 负债合约编号
    private int	deferDays;			 // 展期天数
    private String fundAccount;	     // 资金账号
    private String password;	     // 资金账号密码
}
