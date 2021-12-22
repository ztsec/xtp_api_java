package com.zts.xtp.common.enums;

/**
 * Created by qingfei.hou on 2020/1/21.
 */
public enum XTPStrategyStateType {
    XTP_STRATEGY_STATE_CREATING(0),
    XTP_STRATEGY_STATE_CREATED(1),
    XTP_STRATEGY_STATE_STARTING(2),
    XTP_STRATEGY_STATE_STARTED(3),
    XTP_STRATEGY_STATE_STOPPING(4),
    XTP_STRATEGY_STATE_STOPPED(5),
    XTP_STRATEGY_STATE_DESTROYING(6),
    XTP_STRATEGY_STATE_DESTROYED(7),
    XTP_STRATEGY_STATE_ERROR(8);

    public final int type;

    XTPStrategyStateType(int type) {
        this.type = type;
    }

}
