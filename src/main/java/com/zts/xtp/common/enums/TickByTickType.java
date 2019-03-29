package com.zts.xtp.common.enums;

/**
 * 逐笔回报类型
 */
public enum TickByTickType {
    /**逐笔委托 */
    ORDER(1),
    /**逐笔成交 */
    TRADE(2),
    /**未知 */
    UNKNOWN(0);

    private int type;

    TickByTickType(int type) {
        this.type = type;
    }

    public static TickByTickType forType(int type) {
        switch (type) {
            case 1:
                return ORDER;
            case 2:
                return TRADE;
            default:
                return UNKNOWN;
        }
    }

    public int getTbtType() {
        return type;
    }
}
