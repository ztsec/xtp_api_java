package com.zts.xtp.trade.model.request;


import com.zts.xtp.common.enums.BusinessType;
import com.zts.xtp.common.enums.MarketType;
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
public class OrderInsertRequest {

    /**XTP系统订单ID，无需用户填写，在XTP系统中唯一 */
    private String orderXtpId;
    /**报单引用，由客户自定义*/
    private int orderClientId;
    /**合约代码 客户端请求不带空格*/
    private String ticker;
    /**交易市场*/
    private MarketType marketType;
    /**价格*/
    private double price;
    /**止损价*/
    private double stopPrice;
    /**数量(股票单位为股，逆回购单位为张)*/
    private long quantity;
    /**报单价格*/
    private PriceType priceType;
    /**买卖方向*/
    private SideType sideType;
    /**开平标志*/
    private PositionEffectType positionEffectType;
    /**业务类型*/
    private BusinessType businessType;

    public int getMarketType() {
        return marketType.ordinal();
    }

    public int getPriceType() {
        return priceType.getType();
    }

    public int getSideType() {
        return sideType.getType();
    }

    public int getPositionEffectType() {
        return positionEffectType.ordinal();
    }

    public int getBusinessType() {
        return businessType.ordinal();
    }

}
