package com.zts.xtp.trade.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/17.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdCashRepayRsp {
    private String xtpId;               // 直接还款操作的XTPID
    private double requestAmount;       // 直接还款的申请金额
    private double cashRepayAmount;     // 实际还款使用金额
}
