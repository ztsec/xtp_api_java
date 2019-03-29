package com.zts.xtp.common.enums;

/**
 * 基金当天拆分合并状态类型
 */
public enum SplitMergeStatus {
    /**允许拆分和合并*/
    XTP_SPLIT_MERGE_STATUS_ALLOW,
    /**只允许拆分，不允许合并*/
    XTP_SPLIT_MERGE_STATUS_ONLY_SPLIT,
    /**只允许合并，不允许拆分*/
    XTP_SPLIT_MERGE_STATUS_ONLY_MERGE,
    /**不允许拆分合并*/
    XTP_SPLIT_MERGE_STATUS_FORBIDDEN;
}
