package com.zts.xtp.common.enums;

/**
 * 持仓方向类型
 */
public enum PositionDirectionType {
    /**净*/
    XTP_POSITION_DIRECTION_NET,
    /**多（期权则为权利方）*/
    XTP_POSITION_DIRECTION_LONG,
    /**空（期权则为义务方）*/
    XTP_POSITION_DIRECTION_SHORT,
    /**备兑（期权则为备兑义务方）*/
    XTP_POSITION_DIRECTION_COVERED,

}
