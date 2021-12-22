package com.zts.xtp.common.enums;


/**
 * 现金替代标识定义
 */
public enum EtfReplaceType {
    /**禁止现金替代*/
    ERT_CASH_FORBIDDEN,
    /**可以现金替代*/
    ERT_CASH_OPTIONAL,
    /**必须现金替代*/
    ERT_CASH_MUST,
    /**深市退补现金替代*/
    ERT_CASH_RECOMPUTE_INTER_SZ,
    /**深市必须现金替代*/
    ERT_CASH_MUST_INTER_SZ,
    /**非沪深市场成分证券退补现金替代*/
    ERT_CASH_RECOMPUTE_INTER_OTHER,
    /**表示非沪深市场成份证券必须现金替代*/
    ERT_CASH_MUST_INTER_OTHER,
    /**港市退补现金替代（仅适用于跨沪深港ETF产品）*/
    ERT_CASH_RECOMPUTE_INTER_HK,
    /**港市必须现金替代（仅适用于跨沪深港ETF产品）*/
    ERT_CASH_MUST_INTER_HK,
    /**无效值*/
    EPT_INVALID;

}
