package com.zts.xtp.common.enums;

/**
 * 买卖方向类型
 */
public enum SideType {

    /**买（新股申购、ETF买等）*/
    XTP_SIDE_BUY(1),
    /**卖（逆回购）*/
    XTP_SIDE_SELL(2),
    /**买开*/
    XTP_SIDE_BUY_OPEN(3),
    /**卖开*/
    XTP_SIDE_SELL_OPEN(4),
    /**买平*/
    XTP_SIDE_BUY_CLOSE(5),
    /**卖平*/
    XTP_SIDE_SELL_CLOSE(6),
    /**申购*/
    XTP_SIDE_PURCHASE(7),
    /**赎回*/
    XTP_SIDE_REDEMPTION(8),
    /**拆分*/
    XTP_SIDE_SPLIT(9),
    /**合并*/
    XTP_SIDE_MERGE(10),
    /**未知或者无效买卖方向*/
    XTP_SIDE_UNKNOWN(11);

    public final int type;

    SideType(int type) {
        this.type = type;
    }

    public static SideType forType(int type) {
        switch (type) {
            case 1:
                return XTP_SIDE_BUY;
            case 2:
                return XTP_SIDE_SELL;
            case 3:
                return XTP_SIDE_BUY_OPEN;
            case 4:
                return XTP_SIDE_SELL_OPEN;
            case 5:
                return XTP_SIDE_BUY_CLOSE;
            case 6:
                return XTP_SIDE_SELL_CLOSE;
            case 7:
                return XTP_SIDE_PURCHASE;
            case 8:
                return XTP_SIDE_REDEMPTION;
            case 9:
                return XTP_SIDE_SPLIT;
            case 10:
                return XTP_SIDE_MERGE;
            default:
                return XTP_SIDE_UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
