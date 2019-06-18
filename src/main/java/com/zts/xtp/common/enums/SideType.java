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
    /**改版之后的side的备兑，暂不支持*/
    XTP_SIDE_COVER(11),
    /**改版之后的side锁定（对应开平标识为开）/解锁（对应开平标识为平）*/
    XTP_SIDE_FREEZE(12),
    /**融资买入*/
    XTP_SIDE_MARGIN_TRADE(21),
    /**融券卖出*/
    XTP_SIDE_SHORT_SELL(22),
    /**卖券还款*/
    XTP_SIDE_REPAY_MARGIN(23),
    /**买券还券*/
    XTP_SIDE_REPAY_STOCK(24),
    /// 现金还款（不放在普通订单协议，另加请求和查询协议）
    //#define XTP_SIDE_CASH_REPAY_MARGIN	25
    /**现券还券*/
    XTP_SIDE_STOCK_REPAY_STOCK(26),
    /**余券划转*/
    XTP_SIDE_SURSTK_TRANS(27),
    /**担保品转入*/
    XTP_SIDE_GRTSTK_TRANSIN(28),
    /**担保品转出*/
    XTP_SIDE_GRTSTK_TRANSOUT(29),
    /**未知或者无效买卖方向*/
    XTP_SIDE_UNKNOWN(30);

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
            case 11:
                return XTP_SIDE_COVER;
            case 12:
                return XTP_SIDE_FREEZE;
            case 21:
                return XTP_SIDE_MARGIN_TRADE;
            case 22:
                return XTP_SIDE_SHORT_SELL;
            case 23:
                return XTP_SIDE_REPAY_MARGIN;
            case 24:
                return XTP_SIDE_REPAY_STOCK;
            case 26:
                return XTP_SIDE_STOCK_REPAY_STOCK;
            case 27:
                return XTP_SIDE_SURSTK_TRANS;
            case 28:
                return XTP_SIDE_GRTSTK_TRANSIN;
            case 29:
                return XTP_SIDE_GRTSTK_TRANSOUT;
            case 30:
                return XTP_SIDE_UNKNOWN;
            default:
                return XTP_SIDE_UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
