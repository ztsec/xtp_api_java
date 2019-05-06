package com.zts.xtp.common.enums;


/**
 * 开平标识类型
 */
public enum PositionEffectType {
    /**初始值或未知值开平标识，现货适用*/
    XTP_POSITION_EFFECT_INIT,
    /**开*/
    XTP_POSITION_EFFECT_OPEN,
    /**平*/
    XTP_POSITION_EFFECT_CLOSE,
    /**强平*/
    XTP_POSITION_EFFECT_FORCECLOSE,
    /**平今*/
    XTP_POSITION_EFFECT_CLOSETODAY,
    /**平昨*/
    XTP_POSITION_EFFECT_CLOSEYESTERDAY,
    /**强减*/
    XTP_POSITION_EFFECT_FORCEOFF,
    /**本地强平*/
    XTP_POSITION_EFFECT_LOCALFORCECLOSE,
    /**信用业务追保强平*/
    XTP_POSITION_EFFECT_CREDIT_FORCE_COVER,
    /**信用业务清偿强平*/
    XTP_POSITION_EFFECT_CREDIT_FORCE_CLEAR,
    /**信用业务合约到期强平*/
    XTP_POSITION_EFFECT_CREDIT_FORCE_DEBT,
    /**信用业务无条件强平*/
    XTP_POSITION_EFFECT_CREDIT_FORCE_UNCOND,
    /**未知的开平标识类型*/
    XTP_POSITION_EFFECT_UNKNOWN
}
