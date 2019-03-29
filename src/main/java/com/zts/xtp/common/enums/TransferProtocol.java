package com.zts.xtp.common.enums;


/**
 * 通讯传输协议方式
 */
public enum TransferProtocol {

    /** 采用TCP方式传输 */
    XTP_PROTOCOL_TCP(1),
    /** 采用UDP方式传输,XTP 18.13版本支持  */
    XTP_PROTOCOL_UDP(2);

    /** Socket type. */
    public final int type;

    TransferProtocol(int type) {
        this.type = type;
    }

    public static TransferProtocol forType(int type) {
        switch (type) {
            case 1:
                return XTP_PROTOCOL_TCP;
            case 2:
                return XTP_PROTOCOL_UDP;
            default:
                throw new IllegalArgumentException("Unknown transferProtocol type: " + type);
        }
    }

    public int getValue() {
        return type;
    }
}

