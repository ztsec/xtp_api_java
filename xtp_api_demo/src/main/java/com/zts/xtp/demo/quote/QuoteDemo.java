package com.zts.xtp.demo.quote;


import com.zts.xtp.common.enums.JniLogLevel;
import com.zts.xtp.common.enums.TransferProtocol;
import com.zts.xtp.common.enums.XtpLogLevel;
import com.zts.xtp.quote.api.QuoteApi;
import com.zts.xtp.quote.spi.QuoteSpi;
import java.io.File;

public class QuoteDemo {
    private QuoteApi quoteApi;
    private boolean loginFlag;

    public QuoteDemo(String ip, int port, String user, String password, short clientId, String dataFolder){
        QuoteSpi quoteSpi = new QuoteSpiImpl();
        quoteApi = new QuoteApi(quoteSpi);
        quoteApi.init(clientId, dataFolder, XtpLogLevel.XTP_LOG_LEVEL_INFO, JniLogLevel.JNI_LOG_LEVEL_INFO);
        int loginResult = quoteApi.login(ip, port, user, password, TransferProtocol.XTP_PROTOCOL_TCP);

        loginFlag = (loginResult == 0);
    }

    public void shutdown() {
        quoteApi.logout();
        quoteApi.disconnect();
    }

    public boolean isLogin() {
        return loginFlag;
    }

    /**
     * 订阅单个行情，返回的行情数据在QuoteSpiImpl.onDepthMarketData()里获得
     */
    public void testSubscribeMarketData() {
        System.out.println("subscribeMarketData");
        quoteApi.subscribeMarketData(new String[]{"600000"},1,1);

        //wait for response
        try {
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
            System.out.println("sleep error");
        }
    }


}
