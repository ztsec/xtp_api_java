package com.zts.xtp.common.enums;

/**
 * 报单提交状态类型
 */
public enum OrderSubmitStatusType {
    /**订单已经提交*/
    XTP_ORDER_SUBMIT_STATUS_INSERT_SUBMITTED(1),
    /**订单已经被接受*/
    XTP_ORDER_SUBMIT_STATUS_INSERT_ACCEPTED(2),
    /**订单已经被拒绝*/
    XTP_ORDER_SUBMIT_STATUS_INSERT_REJECTED(3),
    /**撤单已经提交*/
    XTP_ORDER_SUBMIT_STATUS_CANCEL_SUBMITTED(4),
    /**撤单已经被拒绝*/
    XTP_ORDER_SUBMIT_STATUS_CANCEL_REJECTED(5),
    /**撤单已经被接受*/
    XTP_ORDER_SUBMIT_STATUS_CANCEL_ACCEPTED(6);

    public final int type;

    OrderSubmitStatusType(int type) {
        this.type = type;
    }

    public static OrderSubmitStatusType forType(int type) {
        switch (type) {
            case 1:
                return XTP_ORDER_SUBMIT_STATUS_INSERT_SUBMITTED;
            case 2:
                return XTP_ORDER_SUBMIT_STATUS_INSERT_ACCEPTED;
            case 3:
                return XTP_ORDER_SUBMIT_STATUS_INSERT_REJECTED;
            case 4:
                return XTP_ORDER_SUBMIT_STATUS_CANCEL_SUBMITTED;
            case 5:
                return XTP_ORDER_SUBMIT_STATUS_CANCEL_REJECTED;
            default:
                return XTP_ORDER_SUBMIT_STATUS_CANCEL_ACCEPTED;
        }
    }

    public int getType(){
        return type;
    }
}
