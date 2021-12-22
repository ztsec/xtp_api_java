package com.zts.xtp.common.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.Getter;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Getter
public class ErrorMessage {

    /** 错误代码 */
    private int errorId;

    /** 错误信息 */
    private String errorMsg;

    /** 请求代码，用于定位该返回值对应的请求 */
    private int requestId;

	public int getErrorId() {
		return errorId;
	}

	public String getErrorMsg() {
	    return errorMsg;
    }
}
