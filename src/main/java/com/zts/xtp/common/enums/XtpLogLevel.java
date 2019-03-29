package com.zts.xtp.common.enums;

/**
 * XTP日志输出级别类型
 */
public enum XtpLogLevel {
    /** 严重错误级别*/
    XTP_LOG_LEVEL_FATAL(0),
    /** 错误级别*/
    XTP_LOG_LEVEL_ERROR(1),
    /** 警告级别*/
    XTP_LOG_LEVEL_WARNING(2),
    /** info级别*/
    XTP_LOG_LEVEL_INFO(3),
    /** debug级别*/
    XTP_LOG_LEVEL_DEBUG(4),
    /** trace级别*/
    XTP_LOG_LEVEL_TRACE(5);

    private int value;

    private XtpLogLevel(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

}
