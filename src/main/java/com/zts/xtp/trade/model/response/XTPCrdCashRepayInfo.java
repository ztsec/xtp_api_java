package com.zts.xtp.trade.model.response;


import com.zts.xtp.common.enums.CrdCrStatus;
import com.zts.xtp.common.enums.PositionEffectType;
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
public class XTPCrdCashRepayInfo {
    private String xtpId;                       // 直接还款操作的XTPID
    private CrdCrStatus crdCrStatus;            // 直接还款处理状态
    private double requestAmount;               // 直接还款的申请金额
    private double cashRepayAmount;             // 实际还款使用金额
    private PositionEffectType positionEffect;  // 强平标志
    private int requestId;                      // 此消息响应函数对应的请求ID
    private boolean lastResp;                   // 此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应

    public void setCrdCrStatus(int status) {
        crdCrStatus = CrdCrStatus.values()[status];
    }

    public void setPositionEffectType(int type) {
        if(PositionEffectType.values().length-1<type){
            positionEffect = PositionEffectType.XTP_POSITION_EFFECT_UNKNOWN;
            System.err.println("Error: received PositionEffectType value exceed positionEffectType enum size, receive="+type);
        }else{
            positionEffect = PositionEffectType.values()[type];
        }
    }
}
