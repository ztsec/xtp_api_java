package com.zts.xtp.common.enums;

/**
 * 证券状态枚举类型
 */
public enum SecurityStatus {
    /**风险警示板*/
    XTP_SECURITY_STATUS_ST(0),
    /**首日上市*/
    XTP_SECURITY_STATUS_N_IPO(1),
    /**普通*/
    XTP_SECURITY_STATUS_COMMON(2),
    /**恢复上市*/
    XTP_SECURITY_STATUS_RESUME(3),
    /**退市整理期*/
    XTP_SECURITY_STATUS_DELISTING(10),
    /**其他*/
    XTP_SECURITY_STATUS_OTHERS(255);

    public final int type;

    SecurityStatus(int type) {
        this.type = type;
    }

    public static SecurityStatus forType(int type) {
        switch (type) {
            case 0:
                return XTP_SECURITY_STATUS_ST;
            case 1:
                return XTP_SECURITY_STATUS_N_IPO;
            case 2:
                return XTP_SECURITY_STATUS_COMMON;
            case 3:
                return XTP_SECURITY_STATUS_RESUME;
            case 10:
                return XTP_SECURITY_STATUS_DELISTING;
            default:
                return XTP_SECURITY_STATUS_OTHERS;
        }
    }

    public int getType(){
        return type;
    }
}
