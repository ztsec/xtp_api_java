package com.zts.xtp.common.enums;


/**
 * 资金流转方向类型
 */
public enum FundTransferType {
    /**转出 从XTP转出到柜台*/
    XTP_FUND_TRANSFER_OUT,
    /**转入 从柜台转入XTP*/
    XTP_FUND_TRANSFER_IN,
    /**跨节点转出 从本XTP节点1，转出到对端XTP节点2，XTP服务器之间划拨，只能跨节点用户使用*/
    XTP_FUND_INTER_TRANSFER_OUT,
    /**跨节点转入 从对端XTP节点2，转入到本XTP节点1，XTP服务器之间划拨，只能跨节点用户使用*/
    XTP_FUND_INTER_TRANSFER_IN,
    /**未知类型*/
    XTP_FUND_TRANSFER_UNKNOWN
}
