package com.zts.xtp.common.enums;


/**
 * 柜台资金操作结果
 */
public enum FundOperStatus {
    /**XOMS已收到，正在处理中*/
    XTP_FUND_OPER_PROCESSING,
    /**成功*/
    XTP_FUND_OPER_SUCCESS,
    /**失败*/
    XTP_FUND_OPER_FAILED,
    /**已提交到集中柜台处理*/
    XTP_FUND_OPER_SUBMITTED,
    /**未知*/
    XTP_FUND_OPER_UNKNOWN
}
