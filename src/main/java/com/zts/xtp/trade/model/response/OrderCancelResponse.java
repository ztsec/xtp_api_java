package com.zts.xtp.trade.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"orderCancelXtpId", "orderXtpId"})
@Builder
public class OrderCancelResponse {

    /**撤单XTPID*/
    private String orderCancelXtpId;
    /**原始订单XTPID*/
    private String orderXtpId;
    /**此消息响应函数对应的请求ID*/
    private int requestId;
    /**此消息响应函数是否为request_id这条请求所对应的最后一个响应，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应*/
    private boolean lastResp;

    @Override
    public String toString(){
        return "OrderCancelResponse [ "+
                "orderXtpId:"+orderXtpId+
                "orderCancelXtpId"+orderCancelXtpId+
                "requestId"+requestId+
                "lastResp"+lastResp;
    }
}
