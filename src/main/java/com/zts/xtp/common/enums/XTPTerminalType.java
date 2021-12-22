package com.zts.xtp.common.enums;

/**
 * 终端类型枚举
 */
public enum XTPTerminalType {
    /**"PC",PC-windows及MacOS*/
    XTP_TERMINAL_PC(1),
    /**"MA",Mobile-Android*/
    XTP_TERMINAL_ANDROID(2),
    /**"MI",Mobile-Ios*/
    XTP_TERMINAL_IOS(3),
    /**"MW",Mobile-Windows Phone*/
    XTP_TERMINAL_WP(4),
    /**"WP",无盘站*/
    XTP_TERMINAL_STATION(5),
    /**"TO",电话委托*/
    XTP_TERMINAL_TEL(6),
    /**"OH",PC-linux及其他终端*/
    XTP_TERMINAL_PC_LINUX(7);


    public final int type;

    XTPTerminalType(int type) {
        this.type = type;
    }

    public static XTPTerminalType forType(int type) {
        switch (type) {
            case 1:
                return XTP_TERMINAL_PC;
            case 2:
                return XTP_TERMINAL_ANDROID;
            case 3:
                return XTP_TERMINAL_IOS;
            case 4:
                return XTP_TERMINAL_WP;
            case 5:
                return XTP_TERMINAL_STATION;
            case 6:
                return XTP_TERMINAL_TEL;
            case 7:
                return XTP_TERMINAL_PC_LINUX;
            default:
                return XTP_TERMINAL_PC_LINUX;
        }
    }

    public int getType(){
        return type;
    }
}
