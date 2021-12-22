package com.zts.xtp.common.enums;

public enum DebtExtendOperStatus {
    XTP_DEBT_EXTEND_OPER_PROCESSING,	    // XTP已收到，正在处理中
    XTP_DEBT_EXTEND_OPER_SUBMITTED,			// 已提交到集中柜台处理
    XTP_DEBT_EXTEND_OPER_SUCCESS,			// 成功
    XTP_DEBT_EXTEND_OPER_FAILED,			// 失败
    XTP_DEBT_EXTEND_OPER_UNKNOWN;			// 未知
}
