package com.zts.xtp.common.enums;

/**
 * 报单状态类型
 */
public enum OrderStatusType {
    /**初始化*/
    XTP_ORDER_STATUS_INIT,
    /**全部成交*/
    XTP_ORDER_STATUS_ALLTRADED,
    /**部分成交*/
    XTP_ORDER_STATUS_PARTTRADEDQUEUEING,
    /**部分撤单*/
    XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING,
    /**未成交*/
    XTP_ORDER_STATUS_NOTRADEQUEUEING,
    /**已撤单*/
    XTP_ORDER_STATUS_CANCELED,
    /**已拒绝*/
    XTP_ORDER_STATUS_REJECTED,
    /**未知订单状态*/
    XTP_ORDER_STATUS_UNKNOWN

}
