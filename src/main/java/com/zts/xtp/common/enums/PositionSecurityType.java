package com.zts.xtp.common.enums;

/**
 * 持仓证券枚举类型
 */
public enum PositionSecurityType {
    /**普通持仓*/
    XTP_POSITION_SECURITY_NORMAL,
    /**配售类型的持仓，包含配股、配债等*/
    XTP_POSITION_SECURITY_PLACEMENT,
    /**未知类型*/
    XTP_POSITION_SECURITY_UNKNOWN,
}
