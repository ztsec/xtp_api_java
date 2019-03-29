package com.zts.xtp.common.enums;


/**
 * 市场类型
 */
public enum MarketType {
    /**初始化值或者未知*/
    XTP_MKT_INIT(0),
    /**深圳A股*/
    XTP_MKT_SZ_A(1),
    /**上海A股*/
    XTP_MKT_SH_A(2),
    /**未知交易市场类型*/
    XTP_MKT_UNKNOWN(3);

    public final int type;

    MarketType(int type) {
        this.type = type;
    }

    public static MarketType forType(int type) {
        switch (type) {
            case 0:
                return XTP_MKT_INIT;
            case 1:
                return XTP_MKT_SZ_A;
            case 2:
                return XTP_MKT_SZ_A;
            default:
                return XTP_MKT_UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
