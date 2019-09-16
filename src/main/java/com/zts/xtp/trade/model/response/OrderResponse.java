package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.BusinessType;
import com.zts.xtp.common.enums.MarketType;
import com.zts.xtp.common.enums.OrderStatusType;
import com.zts.xtp.common.enums.OrderSubmitStatusType;
import com.zts.xtp.common.enums.PositionEffectType;
import com.zts.xtp.common.enums.PriceType;
import com.zts.xtp.common.enums.SideType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"orderXtpId"})
@Builder
public class OrderResponse {

    /**XTP系统订单ID，在XTP系统中唯一*/
    private String orderXtpId;
    /**报单引用，用户自定义*/
    private int orderClientId;
    /**报单操作引用，用户自定义*/
    private int orderCancelClientId;
    /**撤单在XTP系统中的id，在XTP系统中唯一*/
    private String orderCancelXtpId;
    /**合约代码*/
    private String ticker;
    /**交易市场*/
    private MarketType marketType;
    /**价格*/
    private double price;
    /**数量，此订单的报单数量*/
    private long quantity;
    /**报单价格条件*/
    private PriceType priceType;
    /**买卖方向*/
    private SideType sideType;
    /**开平标志*/
    private PositionEffectType positionEffectType;
    /**业务类型*/
    private BusinessType businessType;
    /**今成交数量，为此订单累计成交数量*/
    private long qtyTraded;
    /**剩余数量，当撤单成功时，表示撤单数量*/
    private long qtyLeft;
    /**委托时间，格式为YYYYMMDDHHMMSSsss*/
    private long insertTime;
    /**最后修改时间，格式为YYYYMMDDHHMMSSsss*/
    private long updateTime;
    /**撤销时间，格式为YYYYMMDDHHMMSSsss*/
    private long cancelTime;
    /**成交金额，为此订单的成交总金额*/
    private double tradeAmount;
    /**本地报单编号 OMS生成的单号，不等同于order_xtp_id，为服务器传到报盘的单号*/
    private String orderLocalId;
    /**报单状态，订单响应中没有部分成交状态的推送，在查询订单结果中，会有部分成交状态*/
    private OrderStatusType orderStatusType;
    /**报单提交状态，OMS内部使用，用户无需关心*/
    private OrderSubmitStatusType orderSubmitStatusType;
    /**报单类型 */
    private char txtPOrderTypeType;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    public void setMarketType(int type) {
        marketType = MarketType.values()[type];
    }

    public void setPriceType(int type) {
        priceType = PriceType.forType(type);
    }

    public void setSideType(int type) {
        sideType = SideType.forType(type);
    }

    public void setPositionEffectType(int type) {
        if(type>7) type = 7;
        positionEffectType = PositionEffectType.values()[type];
    }

    public void setBusinessType(int type) {
        businessType = BusinessType.values()[type];
    }

    public void setOrderStatusType(int type) {
        orderStatusType = OrderStatusType.values()[type];
    }

    public void setOrderSubmitStatusType(int type) {
        orderSubmitStatusType = OrderSubmitStatusType.forType(type);
    }
}
