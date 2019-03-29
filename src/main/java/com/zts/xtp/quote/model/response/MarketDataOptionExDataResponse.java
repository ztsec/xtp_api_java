package com.zts.xtp.quote.model.response;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class MarketDataOptionExDataResponse {
    /** 波段性中断参考价(SH)*/
    double  auctionPrice;
    /**波段性中断集合竞价虚拟匹配量(SH)*/
    long auctionQty;
    /**最近询价时间(SH)*/
    long lastEnquiryTime;
}
