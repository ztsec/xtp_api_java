package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.XTPStrategyStateType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/23.
 */

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPStrategyStateReportStruct {
    private int	mStrategyType;		                    // 策略类型
    private String mClientStrategyId;	                // 客户策略id
    private String mXtpStrategyId;		                // xtp策略id
    private XTPStrategyStateType mStrategyState;	    // 策略状态
    private long mStrategyQty;				            // 策略总量
    private long mStrategyOrderedQty;		            // 策略已委托数量
    private long mStrategyCancelledQty;	                // 策略已撤单数量
    private long mStrategyExecutionQty;	                // 策略已成交数量
    private long mStrategyUnclosedQty;	                // 策略未平仓数量(T0卖出数量-买入数量)
    private double	mStrategyAsset;			            // 策略总金额
    private double	mStrategyOrderedAsset;	            // 策略已委托金额
    private double	mStrategyExecutionAsset;	        // 策略已成交金额
    private double	mStrategyExecutionPrice;	        // 策略执行价格
    private double	mStrategyMarketPrice;	            // 策略市场价
    private double	mStrategyPriceDiff;		            // 策略执行价差
    private double	mStrategyAssetDiff;		            // 策略执行绩效(T0资金预净收入)

    public void setMStrategyState(int type) {
        mStrategyState = XTPStrategyStateType.values()[type];
    }
}
