package com.zts.xtp.common.enums;

/**
 * 交易所类型
 */
public enum ExchangeType {

    /**上海证券交易所 */
    SH(1),
    /**深圳证券交易所 */
    SZ(2),
    /**未知 */
    UNKNOWN(3);

    public final int type;

    ExchangeType(int type) {
        this.type = type;
    }

    public static ExchangeType forType(int type) {
        switch (type) {
            case 1:
                return SH;
            case 2:
                return SZ;
            default:
                return UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
