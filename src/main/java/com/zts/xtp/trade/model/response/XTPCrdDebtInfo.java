package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.MarketType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/18.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPCrdDebtInfo {
    private int debtType;              // 负债合约类型：0为融资，1为融券，2未知
    private String debtId;             // 负债合约编号
    private String positionId;         // 负债对应两融头寸编号
    private String orderXtpId;         // 生成负债的订单编号，非当日负债无此项
    private int debtStatus;            // 负债合约状态：0为未偿还或部分偿还，1为已偿还，2为过期未平仓，3未知
    private MarketType marketType;     // 市场
    private String ticker;             // 证券代码
    private String orderDate;          // 委托日期
    private String endDate;            // 负债截止日期
    private String origEndDate;        // 负债原始截止日期
    private boolean isExtended;        // 当日是否接收到展期请求：false为没收到，true为收到
    private double remainAmt;          // 未偿还金额
    private String remainQty;          // 未偿还融券数量
    private double remainPrincipal;    // 未偿还本金金额
    private String dueRightQty;		   // 应偿还权益数量
    private long unknown;			   // 保留字段
    private int requestId;             // 此消息响应函数对应的请求ID
    private boolean lastResp;          // 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应

    public void setMarketType(int market) {
        marketType = MarketType.values()[market];
    }

    public void setIsExtended(boolean extended) {
        isExtended = extended;
    }
}
