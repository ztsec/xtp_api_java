package src.main.java.com.zts.xtp.demo.trade;

import com.zts.xtp.common.enums.*;
import com.zts.xtp.trade.api.TradeApi;
import com.zts.xtp.trade.spi.TradeSpi;


public class TradeDemo {
    private TradeApi tradeApi;
    private String sessionId;

    public TradeDemo(String ip, int port, String user, String password, short clientId, String key, String dataFolder){
        TradeSpi tradeSpi = new src.main.java.com.zts.xtp.demo.trade.TradeSpiImpl();
        tradeApi = new TradeApi(tradeSpi);
        tradeApi.init(clientId, key, dataFolder, XtpLogLevel.XTP_LOG_LEVEL_INFO, JniLogLevel.JNI_LOG_LEVEL_INFO, XtpTeResumeType.XTP_TERT_RESTART);
        sessionId = tradeApi.login(ip, port, user, password, TransferProtocol.XTP_PROTOCOL_TCP, "10.10.11.12");
        System.out.println("login OMS result: " + sessionId);
    }

    public void shutdown() {
        tradeApi.logout(sessionId);
        tradeApi.disconnect();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            System.out.println("sleep error");
        }
    }

    public boolean isLogin() {
        return (sessionId != null && !sessionId.equals("0"));
    }

    /**
     * 下单请求，返回的委托数据在TradeSpiImpl.onOrderEvent()里获得，成交数据在TradeSpiImpl.onTradeEvent()里获得
     */
    public void testInsertOrder() {
        System.out.println("testInsertOrder");
//        OrderInsertRequest req = OrderInsertRequest.builder()
//            .orderXtpId("0").orderClientId(13).ticker("000002").marketType(MarketType.XTP_MKT_SZ_A)
//            .price(22.44).stopPrice(0).quantity(200).priceType(PriceType.XTP_PRICE_LIMIT)
//            .sideType(SideType.XTP_SIDE_BUY).businessType(BusinessType.XTP_BUSINESS_TYPE_CASH).build();
//
//        String orderXtpId = tradeApi.insertOrder(req, sessionId);

        long sessionIdH = Long.valueOf(sessionId)/10;
        long sessionIdL = Long.valueOf(sessionId)%10;
        String orderXtpId = tradeApi.insertOrder(0, 1,6,600918,2,9.07,
                0,1,5,1,1,2,
                sessionIdH,sessionIdL,88);

        System.out.println("testInsertOrder orderXtpId: " + orderXtpId);

        //wait for response
        try {
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
            System.out.println("sleep error");
        }
    }


}
