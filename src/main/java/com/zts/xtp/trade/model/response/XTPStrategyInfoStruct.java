package com.zts.xtp.trade.model.response;

import com.zts.xtp.common.enums.XTPStrategyStateType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/22.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPStrategyInfoStruct {
    private int	mStrategyType;		                   // 策略类型
    private String mClientStrategyId;	               // 客户策略id
    private String mXtpStrategyId;		               // xtp策略id
    private XTPStrategyStateType mStrategyState;	   // 策略状态
    private String childOrderXtpId;                    // 用户策略所报的子单
    private int requestId;
    private boolean lastResp;

    public void setMStrategyState(int type) {
        mStrategyState = XTPStrategyStateType.values()[type];
    }
}
