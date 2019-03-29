package com.zts.xtp.common.enums;

public enum OptCallOrPutType {
    XTP_OPT_CALL(1),
    XTP_OPT_PUT(2);

    public final int type;

    OptCallOrPutType(int type){
        this.type = type;
    }
}
