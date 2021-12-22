package com.zts.xtp.quote.model.response;

import com.zts.xtp.common.enums.ExchangeType;
import com.zts.xtp.common.enums.QualificationType;
import com.zts.xtp.common.enums.SecurityStatus;
import com.zts.xtp.common.enums.SecurityType;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;


@Data
@NoArgsConstructor
@AllArgsConstructor
public class TickerFullInfoResponse {
    /** 交易所代码 */
    private ExchangeType exchangeType;
    /** 合约代码 */
    private String ticker;
    /** 合约名称  */
    private String tickerName;
    /** 合约详细类型 */
    private SecurityType securityType;
    /** 合约适当性类别 */
    private QualificationType tickerQualificationType;
    /** 是否注册制(仅适用创业板股票，创新企业股票及存托凭证) */
    private boolean isRegistration;
    /** 是否具有协议控制架构(仅适用创业板股票，创新企业股票及存托凭证) */
    private boolean isVIE;
    /** 是否尚未盈利(仅适用创业板股票，创新企业股票及存托凭证) */
    private boolean isNoprofit;
    /** 是否存在投票权差异(仅适用创业板股票，创新企业股票及存托凭证) */
    private boolean isWeightedVotingRights;
    /** 是否有涨跌幅限制(注：不提供具体幅度，可通过涨跌停价和昨收价来计算幅度) */
    private boolean isHavePriceLimit;
    /** 昨收盘 */
    private double preClosePrice;
    /** 涨停板价 */
    private double upperLimitPrice;
    /** 跌停板价 */
    private double lowerLimitPrice;
    /** 最小变动价位 */
    private double priceTick;
    /** 限价买委托数量上限 */
    private int bidQtyUpperLimit;
    /** 限价买委托数量下限 */
    private int bidQtyLowerLimit;
    /** 限价买数量单位 */
    private int bidQtyUnit;
    /** 限价卖委托数量上限 */
    private int askQtyUpperLimit;
    /** 限价卖委托数量下限 */
    private int askQtyLowerLimit;
    /** 限价卖数量单位 */
    private int askQtyUnit;
    /** 市价买委托数量上限 */
    private int marketBidQtyUpperLimit;
    /** 市价买委托数量下限 */
    private int marketBidQtyLowerLimit;
    /** 市价买数量单位 */
    private int marketBidQtyUnit;
    /** 市价卖委托数量上限 */
    private int marketAskQtyUpperLimit;
    /** 市价卖委托数量上限 */
    private int marketAskQtyLowerLimit;
    /** 市价卖数量单位 */
    private int marketAskQtyUnit;
    /**证券状态*/
    private SecurityStatus securityStatus;
    /** 保留字段 */
    private int unknown1;
    /** 保留字段 */
    private int unknown;
    /** 是否最后一条记录 */
    private boolean lastResp;

    public void setExchangeType(int exchangeType) {
        if(ExchangeType.values().length<exchangeType){
            this.exchangeType = ExchangeType.UNKNOWN;
            System.err.println("Error: TickerInfoResponse received ExchangeType value exceed ExchangeType enum size, receive="+exchangeType);
        }else{
            this.exchangeType = ExchangeType.values()[exchangeType - 1];
        }
    }

    public void setSecurityType (int type) {
        this.securityType = SecurityType.forType(type);
    }
    public void setQualificationType(int type) {
        this.tickerQualificationType = QualificationType.forType(type);
    }

    public void setIsRegistration(boolean registration) {
        this.isRegistration = registration;
    }

    public void setIsVIE(boolean vie) {
        this.isVIE = vie;
    }

    public void setIsNoprofit(boolean noprofit) {
        this.isNoprofit = noprofit;
    }

    public void setIsWeightedVotingRights(boolean weightedVotingRights) {
        this.isWeightedVotingRights = weightedVotingRights;
    }

    public void setIsHavePriceLimit(boolean havePriceLimit) {
        this.isHavePriceLimit = havePriceLimit;
    }

    public void setSecurityStatus (int type) {
        this.securityStatus = SecurityStatus.forType(type);
    }
}
