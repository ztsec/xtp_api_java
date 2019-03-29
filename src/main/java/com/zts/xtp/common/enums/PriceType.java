package com.zts.xtp.common.enums;

/**
 * 价格类型
 */
public enum PriceType {
    /**限价单-沪深（除普通股票业务外，其余业务均使用此种类型）*/
    XTP_PRICE_LIMIT(1),
    /**即时成交剩余转撤销，市价单-深*/
    XTP_PRICE_BEST_OR_CANCEL(2),
    /**最优五档即时成交剩余转限价，市价单-沪*/
    XTP_PRICE_BEST5_OR_LIMIT(3),
    /**最优5档即时成交剩余转撤销，市价单-沪深*/
    XTP_PRICE_BEST5_OR_CANCEL(4),
    /**全部成交或撤销,市价单-深*/
    XTP_PRICE_ALL_OR_CANCEL(5),
    /**本方最优，市价单-深*/
    XTP_PRICE_FORWARD_BEST(6),
    /**对方最优剩余转限价，市价单-深*/
    XTP_PRICE_REVERSE_BEST_LIMIT(7),
    /**期权限价申报FOK*/
    XTP_PRICE_LIMIT_OR_CANCEL(8),
    /**未知或者无效价格类型*/
    XTP_PRICE_TYPE_UNKNOWN(9);

    public final int type;

    PriceType(int type) {
        this.type = type;
    }

    public static PriceType forType(int type) {
        switch (type) {
            case 1:
                return XTP_PRICE_LIMIT;
            case 2:
                return XTP_PRICE_BEST_OR_CANCEL;
            case 3:
                return XTP_PRICE_BEST5_OR_LIMIT;
            case 4:
                return XTP_PRICE_BEST5_OR_CANCEL;
            case 5:
                return XTP_PRICE_ALL_OR_CANCEL;
            case 6:
                return XTP_PRICE_FORWARD_BEST;
            case 7:
                return XTP_PRICE_REVERSE_BEST_LIMIT;
            case 8:
                return XTP_PRICE_LIMIT_OR_CANCEL;
            default:
                return XTP_PRICE_TYPE_UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
