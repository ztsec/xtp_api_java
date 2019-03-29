package com.zts.xtp.common.enums;

public enum MarketDataType {

    /**现货(股票/基金/债券等)*/
    XTP_MARKETDATA_ACTUAL(0),
    /**期权*/
    XTP_MARKETDATA_OPTION(1);

    public final int type;

    MarketDataType(int type) {
        this.type = type;
    }

}
