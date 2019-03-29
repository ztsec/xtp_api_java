package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.BusinessType;
import com.zts.xtp.common.enums.MarketType;
import com.zts.xtp.common.enums.PositionEffectType;
import com.zts.xtp.common.enums.SideType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"orderXtpId", "orderClientId", "ticker", "marketType"})
@Builder
public class TradeResponse {

    /**XTP系统订单ID，此成交回报相关的订单ID，在XTP系统中唯一*/
    private String orderXtpId;
    /**报单引用*/
    private int orderClientId;
    /**合约代码*/
    private String ticker;
    /**交易市场*/
    private MarketType marketType;
    /**订单号，引入XTPID后，该字段实际和order_xtp_id重复。接口中暂时保留*/
    private String localOrderId;
    /**成交编号，深交所唯一，上交所每笔交易唯一，当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交 */
    private String execId;
    /**价格，此次成交的价格*/
    private double price;
    /**数量，此次成交的数量，不是累计数量*/
    private long quantity;
    /**成交时间，格式为YYYYMMDDHHMMSSsss*/
    private long tradeTime;
    /**成交金额，此次成交的总金额 = price*quantity*/
    private double tradeAmount;
    /**成交序号 –回报记录号，每个交易所唯一,report_index+market字段可以组成唯一标识表示成交回报*/
    private String reportIndex;
    /**报单编号 –交易所单号，上交所为空，深交所有此字段*/
    private String orderExchId;
    /**成交类型 –成交回报中的执行类型*/
    private char tradeType;
    /**买卖方向*/
    private SideType sideType;
    /**开平标志*/
    private PositionEffectType positionEffectType;
    /**业务类型*/
    private BusinessType businessType;
    /**交易所交易员代码*/
    private String branchPbu;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }

    public void setSideType(int type) {
        sideType = SideType.forType(type);
    }

    public void setPositionEffectType(int type) {
        positionEffectType = PositionEffectType.values()[type];
    }

    public void setBusinessType(int type) {
        businessType = BusinessType.values()[type];
    }
}
