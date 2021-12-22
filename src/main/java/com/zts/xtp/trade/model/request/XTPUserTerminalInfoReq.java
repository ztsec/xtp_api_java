package com.zts.xtp.trade.model.request;

import com.zts.xtp.common.enums.XTPTerminalType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Created by qingfei.hou on 2020/12/18.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class XTPUserTerminalInfoReq {
    private String localIp;               // 本地IP地址
    private String macAddr;               // MAC地址
    private String hd;                    // 硬盘序列号
    private XTPTerminalType termType;	  // 终端类型
    private String  internetIp;		      // 公网IP地址
    private int internetPort;			  // 公网端口号
    private String  clientVersion;		  // 客户端版本号
    private String  macosSno;			  // MacOS系统的序列号，仅为MacOS系统需要填写
    private String  unused;				  // 预留

    public int getTermType() {
        return termType.ordinal();
    }
}
