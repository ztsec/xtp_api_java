package com.zts.xtp.common.enums;

/**
 * 证券类型
 */
public enum TickerType {

    /** 普通股票 */
    XTP_TICKER_TYPE_STOCK,
    /** 指数 */
    XTP_TICKER_TYPE_INDEX,
    /** 基金 */
    XTP_TICKER_TYPE_FUND,
    /** 债券 */
    XTP_TICKER_TYPE_BOND,
    /** 期权 */
    XTP_TICKER_TYPE_OPTION,
    /** 科创板股票（上海） */
    XTP_TICKER_TYPE_TECH_STOCK,
    /** 未知类型 */
    XTP_TICKER_TYPE_UNKNOWN
}
