package com.zts.xtp.trade.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/14.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdFundInfo {
    /**维持担保品比例*/
    private double maintenanceRatio;
    /**总资产*/
    private double allAsset;
    /**总负债*/
    private double allDebt;
    /**两融授信额度*/
    private double lineOfCredit;
    /**两融保证金可用数*/
    private double guaranty;
    /**保留字段*/
    private double reserved;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
}
