package com.zts.xtp.common.enums;

public enum XtpTeResumeType {
    /**从本交易日开始重传*/
    XTP_TERT_RESTART(0),
    /**从从上次收到的续传（暂未支持）*/
    XTP_TERT_RESUME(1),
    /**只传送登录后公有流（订单响应、成交回报）的内容*/
    XTP_TERT_QUICK(2);

    public final int type;

    XtpTeResumeType(int type) {
        this.type = type;
    }

    public static XtpTeResumeType forType(int type) {
        switch (type) {
            case 0:
                return XTP_TERT_RESTART;
            case 1:
                return XTP_TERT_RESUME;
            case 2:
                return XTP_TERT_QUICK;
            default:
                return XTP_TERT_QUICK;
        }
    }

    public int getType(){
        return type;
    }
}
