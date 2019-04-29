package com.zts.xtp.common.enums;

/**
 * JNI日志输出级别类型
 */
public enum JniLogLevel {
    /** info级别*/
    JNI_LOG_LEVEL_INFO(0),
    /** 警告级别*/
    JNI_LOG_LEVEL_WARNING(1),
    /** 错误级别*/
    JNI_LOG_LEVEL_ERROR(2);

    private int value;

    private JniLogLevel(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

}
