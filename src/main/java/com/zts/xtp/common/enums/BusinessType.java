package com.zts.xtp.common.enums;

/**
 * 证券业务类型
 */
public enum BusinessType {
    /**普通股票业务（股票买卖，ETF买卖，沪市交易型货币基金等）*/
    XTP_BUSINESS_TYPE_CASH,
    /**新股申购业务（对应的price type需选择限价类型）*/
    XTP_BUSINESS_TYPE_IPOS,
    /**回购业务 ( 国债逆回购业务对应的price type填为限价，side填为卖 )*/
    XTP_BUSINESS_TYPE_REPO,
    /**ETF申赎业务*/
    XTP_BUSINESS_TYPE_ETF,
    /**融资融券业务*/
    XTP_BUSINESS_TYPE_MARGIN,
    /**转托管（未支持）*/
    XTP_BUSINESS_TYPE_DESIGNATION,
    /**配股业务（对应的price type需选择限价类型,side填为买）*/
    XTP_BUSINESS_TYPE_ALLOTMENT,
    /**分级基金申赎业务*/
    XTP_BUSINESS_TYPE_STRUCTURED_FUND_PURCHASE_REDEMPTION,
    /**分级基金拆分合并业务*/
    XTP_BUSINESS_TYPE_STRUCTURED_FUND_SPLIT_MERGE,
    /**货币基金申赎业务（暂未支持，沪市交易型货币基金的买卖请使用普通股票业务）*/
    XTP_BUSINESS_TYPE_MONEY_FUND,
    /**期权业务*/
    XTP_BUSINESS_TYPE_OPTION,
    /**行权*/
    XTP_BUSINESS_TYPE_EXECUTE,
    /**锁定解锁，暂不支持*/
    XTP_BUSINESS_TYPE_FREEZE,
    /**期权组合策略 组合和拆分业务*/
    XTP_BUSINESS_TYPE_OPTION_COMBINE,
    /**期权行权合并业务*/
    XTP_BUSINESS_TYPE_EXECUTE_COMBINE,
    /**未知类型*/
    XTP_BUSINESS_TYPE_UNKNOWN;
}
