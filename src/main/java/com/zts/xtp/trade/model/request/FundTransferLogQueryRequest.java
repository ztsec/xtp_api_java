package com.zts.xtp.trade.model.request;


import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@EqualsAndHashCode(of = {"serialId"})
@Builder
public class FundTransferLogQueryRequest {

    /**资金内转编号*/
    private String serialId;
}