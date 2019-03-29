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
    /**未知的开平标识类型*/
    XTP_POSITION_EFFECT_UNKNOWN
}
