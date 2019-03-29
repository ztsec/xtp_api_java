package com.zts.xtp.common.enums;

public enum OptExerciseType {
    /**欧式*/
    XTP_OPT_EXERCISE_TYPE_EUR(1),
    /**美式*/
    XTP_OPT_PUT(2);

    public final int type;

    OptExerciseType(int type){
        this.type = type;
    }
}
