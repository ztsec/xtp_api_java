package com.zts.xtp.trade;


import com.zts.xtp.common.enums.*;
import com.zts.xtp.common.jni.JNILoadLibrary;
import com.zts.xtp.trade.api.TradeApi;
import com.zts.xtp.trade.model.request.*;
import com.zts.xtp.trade.spi.TradeSpi;
import org.junit.*;
import org.junit.runners.MethodSorters;
import com.zts.xtp.userInfo.User;

@FixMethodOrder(MethodSorters.JVM)
public class TradeApiTest {
    private static TradeApi tradeApi;
    private static String sessionId;

    static {
        User.readPropToInit();
    }
    private static final String XTP_TRADE_SERVER_IP = User.tradeServerIP;//xtp交易server的ip
    private static final int XTP_TRADE_SERVER_PORT = User.tradeServerPort;//xtp交易server的端口
    private static final String TRADE_KEY = User.serverKey;//xtp交易serverkey
    private static final short CLIENT_ID = User.clientId;//xtp允许的clientid
    private static final String XTP_QUOTE_SERVER_IP = User.quoteServerIP;//xtp行情server的ip
    private static final int XTP_QUOTE_SERVER_PORT = User.quoteServerPort;//xtp行情server的端口
    private static final String ACCOUNT = User.userName;//xtp资金账号
    private static final String PASSWORD = User.userPwd;//xtp密码
    private static final String DATA_FOLDER = User.logFolder;//java api输出日志的本地目录


    /**
     * 初始化xtp api，并登录，在所有case执行前执行一次
     */
    @BeforeClass
    public static void setUp() {
        JNILoadLibrary.loadLibrary();
        TradeSpi tradeSpi = new TradeSpiImpl();
        tradeApi = new TradeApi(tradeSpi);

        tradeApi.init(CLIENT_ID, TRADE_KEY,
                DATA_FOLDER, XtpLogLevel.XTP_LOG_LEVEL_DEBUG, JniLogLevel.JNI_LOG_LEVEL_INFO, XtpTeResumeType.XTP_TERT_RESTART);
        sessionId = tradeApi.login(XTP_TRADE_SERVER_IP, XTP_TRADE_SERVER_PORT,
                ACCOUNT, PASSWORD, TransferProtocol.XTP_PROTOCOL_TCP,"10.10.11.12");

        Assert.assertNotNull("login fail", sessionId);
    }

    @After
    public void waitSomeTime() throws InterruptedException {
        Thread.sleep(5000);
    }

    /**
     * 登出，并等待1秒，保证xtp api资源释放完全
     */
    @AfterClass
    public static void tearDown() {
        try {
            Thread.sleep(10000);
        } catch (InterruptedException ex) {
            System.out.println("sleep error");
        }
        tradeApi.logout(sessionId);
        tradeApi.disconnect();
    }

    /**
     * 下单
     */
    @Test
    public void testInsertOrder() {
        System.out.println("testInsertOrder");
        String orderXtpId = insertOrder();
        System.out.println("testInsertOrder orderXtpId: " + orderXtpId);
        Assert.assertNotNull(orderXtpId);
    }

    /**
     * 撤单
     */
    @Test
    public void testCancelOrder() {
        System.out.println("testCancelOrder");

        String orderXtpId = insertOrder();
        System.out.println("testCancelOrder orderXtpId: " + orderXtpId);
        Assert.assertNotNull(orderXtpId);
        try {
            Thread.sleep(1000);
        } catch (InterruptedException exception) {
            System.out.println("sleep error");
        }
        String cancelOrderXtpId = tradeApi.cancelOrder(orderXtpId, sessionId);
        System.out.println("testCancelOrder cancelOrderXtpId: " + cancelOrderXtpId);
        Assert.assertNotNull(cancelOrderXtpId);
    }

    /**
     * 委托查询
     */
    @Test
    public void testQueryOrders() {
        System.out.println("testQueryOrders");
        OrderQueryRequest req = OrderQueryRequest.builder().beginTime(0)
            .endTime(0).build();

        int result = tradeApi.queryOrders(req, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 成交查询
     */
    @Test
    public void testQueryTrades() {
        System.out.println("testQueryTrades");
        TraderQueryRequest req = TraderQueryRequest.builder().beginTime(0)
            .endTime(0).build();

        int result = tradeApi.queryTrades(req, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 根据报单ID请求查询报单
     */
    @Test
    public void testQueryOrderByXtpId() {
        System.out.println("testQueryOrderByXtpId");
        String orderXtpId = insertOrder();
        Assert.assertNotNull(orderXtpId);

        int result = tradeApi.queryOrderByXtpId(orderXtpId, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 根据委托编号请求查询相关成交
     */
    @Test
    public void testQueryTradesByXtpId() {
        System.out.println("testQueryTradesByXtpId");
        String orderXtpId = insertOrder();
        Assert.assertNotNull(orderXtpId);

        int result = tradeApi.queryTradesByXtpId(orderXtpId, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 查询投资者持仓
     */
    @Test
    public void testQueryPosition() {
        System.out.println("testQueryPosition");
        int result = tradeApi.queryPosition(null, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 查询资产
     */
    @Test
    public void testQueryAsset() {
        System.out.println("testQueryAsset");
        int result = tradeApi.queryAsset(sessionId, 13);
        System.out.println("testQueryAsset result: " + result);
    }

    /**
     * 查询分级基金
     */
    @Test
    public void testQueryStructuredFund() {
        System.out.println("testQueryStructuredFund");
        StructuredFundInfoQueryRequest req = StructuredFundInfoQueryRequest.builder()
            .exchangeType(ExchangeType.SZ).build();

        int result = tradeApi.queryStructuredFund(req, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 资金划拨请求
     */
    @Test
    public void testFundTransfer() {
        System.out.println("testFundTransfer");
        String serialId = fundTransfer();
        System.out.println("testFundTransfer serialId: " + serialId);
        Assert.assertNotNull(serialId);
    }

    /**
     * 查询资金划拨
     */
    @Test
    public void testQueryFundTransfer() {
        System.out.println("testQueryFundTransfer");
        FundTransferLogQueryRequest req = FundTransferLogQueryRequest.builder()
            .serialId("0").build();
        int result = tradeApi.queryFundTransfer(req, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 查询ETF清单文件
     */
    @Test
    public void testQueryETF() {
        System.out.println("testQueryETF");
        ETFBaseQueryRequest req = ETFBaseQueryRequest.builder().marketType(MarketType.XTP_MKT_SH_A).build();

        int result = tradeApi.queryETF(req, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 请求查询ETF股票篮
     */
    @Test
    public void testQueryETFTickerBasket(){
        System.out.println("testQueryETFTickerBasket");
        ETFComponentQueryRequest queryETFBaseReq = ETFComponentQueryRequest.builder()
            .marketType(MarketType.XTP_MKT_SH_A).ticker("510050").build();
        int result = tradeApi.queryETFTickerBasket(queryETFBaseReq, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 请求查询今日新股申购信息列表
     */
    @Test
    public void testQueryIPOInfoList(){
        System.out.println("testQueryIPOInfoList");
        int result = tradeApi.queryIPOInfoList(sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 请求查询用户新股申购额度信息
     */
    @Test
    public void testQueryIPOQuotaInfo(){
        System.out.println("testQueryIPOQuotaInfo");
        int result = tradeApi.queryIPOQuotaInfo(sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 查询期权信息
     */
    @Test
    public void testQueryOptionAuctionInfo(){
        System.out.println("testQueryOptionAuctionInfo");
        int result = tradeApi.queryOptionAuctionInfo(null, sessionId, 13);
        Assert.assertEquals(result, 0);
    }

    /**
     * 获取当前交易日
     */
	@Test
	public void testGetTradingDay() {
		System.out.println("testGetTradingDay");
		String tradingDay = tradeApi.getTradingDay();
		System.out.println("testGetTradingDay tradingDay: " + tradingDay);
		Assert.assertNotNull(tradingDay);
	}

    /**
     * 获取API的发行版本号
     */
    @Test
    public void testGetApiVersion() {
        System.out.println("testGetApiVersion");
        String version = tradeApi.getApiVersion();
        System.out.println("testGetApiVersion version: " + version);
    }

    /**
     * 通过报单在xtp系统中的ID获取下单的客户端id
     */
    @Test
    public void testGetClientIDByXTPID() {
        System.out.println("testGetClientIDByXTPID");
        short clientId = tradeApi.getClientIDByXTPID("1009073519666951445");
        System.out.println("testGetClientIDByXTPID clientId: " + clientId);
        Assert.assertNotNull(clientId);
    }

    /**
     * 通过报单在xtp系统中的ID获取相关资金账户名
     */
	@Test
	public void testGetAccountByXTPID() {
		System.out.println("testGetAccountByXTPID");
		String account = tradeApi.getAccountByXTPID("396628509490944857");
		System.out.println("testGetAccountByXTPID account: " + account);
		Assert.assertNotNull(account);
	}

    /**
     * 服务器是否重启过
     */
    @Test
    public void testIsServerRestart() {
        System.out.println("testIsServerRestart");
        boolean result = tradeApi.isServerRestart(sessionId);
        System.out.println("testIsServerRestart: " + result);
    }

    /**
     * 分页请求查询报单
     */
    @Test
    public void testQueryOrdersByPage() {
        System.out.println("testQueryOrdersByPage");
        OrderQueryByPageReq req = OrderQueryByPageReq.builder().reqCount(10).reserved(0).build();
        tradeApi.queryOrdersByPage(req, sessionId,13);
    }

    /**
     *
     */
    @Test
    public void testQueryTradesByPage() {
        System.out.println("testQueryTradesByPage");
        TradeQueryByPageReq req = TradeQueryByPageReq.builder().reqCount(10).reserved(0).build();
        tradeApi.queryTradesByPage(req, sessionId,13);
    }

    /**
     * xtp节点之间资金划拨请求
     */
    @Test
    public void testFundTransfer2() {

//        System.out.println("begin queryAsset");
//        int result5 = tradeApi.queryAsset(sessionId, 13);
//        System.out.println("queryAsset status: " + result5);

        System.out.println("begin FundTransfer");
        FundTransferRequest req = FundTransferRequest.builder()
                .fundAccount(ACCOUNT).password(PASSWORD).amount(10000.0)
                .fundTransferType(FundTransferType.XTP_FUND_INTER_TRANSFER_OUT).build();
        String serialId = tradeApi.fundTransfer(req, sessionId);
        System.out.println("FundTransfer serialId: " + serialId);
        Assert.assertNotNull(serialId);

//        System.out.println("begin QueryFundTransfer");
//        FundTransferLogQueryRequest req2 = FundTransferLogQueryRequest.builder()
//                .serialId("973240483297893215").build();
//        int result4 = tradeApi.queryFundTransfer(req2, sessionId, 13);
//        System.out.println("QueryFundTransfer result: " + result4);

//        System.out.println("begin queryAsset again");
//        int result3 = tradeApi.queryAsset(sessionId, 13);
//        System.out.println("queryAsset again result: " + result3);

        System.out.println("testGetApiLastError");
        String result = tradeApi.getApiLastError();
        System.out.println("testGetApiLastError: " + result);
    }

    /**
     * 查询其他节点可用资金
     */
    @Test
    public void testQueryOtherServerFund() {
//        System.out.println("begin queryAsset");
//        int result5 = tradeApi.queryAsset(sessionId, 13);
//        System.out.println("queryAsset status: " + result5);


        System.out.println("testQueryOtherServerFund");
        XTPFundQueryReq req = XTPFundQueryReq.builder()
                .fundAccount(ACCOUNT).password(PASSWORD).queryType(XTPFundQueryType.XTP_FUND_QUERY_INTERNAL).build();
        int result = tradeApi.queryOtherServerFund(req, sessionId, 29);
        System.out.println("testQueryOtherServerFund result:" + result);
    }

    /**算法平台接口 */

    /**
     * 登录xalgo服务
     */
    @Test
    public void testLoginALGO() {
        System.out.println("testLoginALGO2");
        int result = tradeApi.loginALGO("10.25.24.75",8611,"internal1","123456",TransferProtocol.XTP_PROTOCOL_TCP,"127.0.1.1");
        System.out.println("testLoginALGO: " + result);
    }

    /**
     * 获取API的系统错误
     */
    @Test
    public void testGetApiLastError() {
        System.out.println("testGetApiLastError");
        String result = tradeApi.getApiLastError();
        System.out.println("testGetApiLastError: " + result);
    }

    /**
     * 建立算法通道
     */
    @Test
    public void  testAlgoUserEstablishChannel() {
        System.out.println("algoUserVerification");
        System.out.println("algoUserVerification sessionId: " + sessionId);
        int result = tradeApi.algoUserEstablishChannel(XTP_TRADE_SERVER_IP,XTP_TRADE_SERVER_PORT,ACCOUNT,PASSWORD,sessionId);
        System.out.println("algoUserVerification: " + result);
    }

    /**
     * 报算法单
     */
    @Test
    public void testInsertAlgoOrder() {
        System.out.println("insertAlgoOrder");
        int result = tradeApi.insertAlgoOrder(2001,"111779","test111779",sessionId);
        System.out.println("insertAlgoOrder: " + result);
    }

    /**
     * 撤销算法单请求
     */
    @Test
    public void testCancelAlgoOrder() {
        System.out.println("cancelAlgoOrder");
        int result = tradeApi.cancelAlgoOrder(false,"3526168215553",sessionId);
        System.out.println("cancelAlgoOrder: " + result);
    }

    /**
     * 查询用户策略请求
     */
    @Test
    public void  testQueryStrategy() {
        System.out.println("queryStrategy");
        int result = tradeApi.queryStrategy(1,"0","0",sessionId,13);
        System.out.println("queryStrategy: " + result);
    }

    /**xalgo test case*/
    @Test
    public void testXalgo() {
        System.out.println("testLoginXALGO");
        int result1 = tradeApi.loginALGO("10.25.24.75",8611,"internal1","123456",TransferProtocol.XTP_PROTOCOL_TCP,"127.0.1.1");
        System.out.println("testLoginXALGO: " + result1);

        System.out.println("xalgoUserVerification");
        System.out.println("xalgoUserVerification sessionId: " + sessionId);
        int result2 = tradeApi.algoUserEstablishChannel(XTP_TRADE_SERVER_IP,XTP_TRADE_SERVER_PORT,"100062002543","123456",sessionId);
        System.out.println("xalgoUserVerification: " + result2);

        try {
            Thread.sleep(1000);
        } catch (InterruptedException exception) {
            System.out.println("sleep error");
        }

        System.out.println("insertAlgoOrder");
        int result3 = tradeApi.insertAlgoOrder(3103,"16327158123","{\"ticker\":\"600000\",\"quantity\":800,\"market\":\"SH\",\"amount\":null,\"limit_action\":false,\"expire_action\":false,\"side\":\"BUY\",\"price\":0,\"start_time\":\"10:50:00\",\"end_time\":\"14:55:00\",\"business_type\":\"CASH\",\"loadtime\":1632796646030,\"task_type\":\"KFTWAPCORE\"}",sessionId);
        System.out.println("insertAlgoOrder: " + result3);

        try {
            Thread.sleep(1000);
        } catch (InterruptedException exception) {
            System.out.println("sleep error");
        }

        System.out.println("cancelAlgoOrder");
        int result4 = tradeApi.cancelAlgoOrder(true,"678808190977",sessionId);
        System.out.println("cancelAlgoOrder: " + result4);

        System.out.println("queryStrategy");
        int result5 = tradeApi.queryStrategy(0,"0","678808190977",sessionId,13);
        System.out.println("queryStrategy: " + result5);


        System.out.println("getAlgorithmIDByOrder");
        String result6 = tradeApi.getAlgorithmIDByOrder("54217631598642153",203360200);
        System.out.println("getAlgorithmIDByOrder: " + result6);

    }

    /**
     * 修改已登录用户的硬件信息
     */
    @Test
    public void  testModifyUserTerminalInfo() {
        System.out.println("modifyUserTerminalInfo");
        XTPUserTerminalInfoReq req = XTPUserTerminalInfoReq.builder().localIp("127.0.9.1").macAddr("123").hd("123")
                .termType(XTPTerminalType.XTP_TERMINAL_PC).internetIp("127.125.125.1").internetPort(6666).macosSno("5453").build();
        int result = tradeApi.modifyUserTerminalInfo(req,sessionId);
        System.out.println("modifyUserTerminalInfo: " + result);
    }

    /** 两融接口 */

    /**
     * 融资融券业务中现金直接还款请求
     */
    @Test
    public void testCreditCashRepay() {
        System.out.println("testQueryCreditFundInfo");
        String orderXtpId = tradeApi.creditCashRepay(7.3,sessionId);
        System.out.println("orderXtpId: " + orderXtpId);
    }

    /**
     * 融资融券业务中现金还指定负债合约息费请求
     */
    @Test
    public void testCreditCashRepayDebtInterestFee() {
        System.out.println("testQueryCreditFundInfo");
        String orderXtpId = tradeApi.creditCashRepayDebtInterestFee("00001",7.7,sessionId);
        System.out.println("orderXtpId: " + orderXtpId);
    }

    /**
     * 请求查询融资融券业务中的现金直接还款报单
     */
    @Test
    public void testQueryCreditCashRepayInfo() {
        System.out.println("testQueryCreditCashRepayInfo");
        int result = tradeApi.queryCreditCashRepayInfo(sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询信用账户特有信息，除资金账户以外的信息
     */
    @Test
    public void testQueryCreditFundInfo() {
        System.out.println("testQueryCreditFundInfo");
        int result = tradeApi.queryCreditFundInfo(sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询信用账户负债合约信息
     */
    @Test
    public void testQueryCreditDebtInfo() {
        System.out.println("testQueryCreditDebtInfo");
        int result = tradeApi.queryCreditDebtInfo(sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询信用账户待还资金信息
     */
    @Test
    public void testQueryCreditAssetDebtInfo() {
        System.out.println("testQueryCreditAssetDebtInfo");
        int result = tradeApi.queryCreditAssetDebtInfo( sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询信用账户可融券头寸信息
     */
    @Test
    public void testQueryCreditTickerAssignInfo() {
        System.out.println("testQueryCreditTickerAssignInfo");
        XTPClientQueryCrdPositionStockReq req = XTPClientQueryCrdPositionStockReq.builder().
                marketType(MarketType.XTP_MKT_SH_A).ticker("600007").build();
        int result = tradeApi.queryCreditTickerAssignInfo(req, sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 融资融券业务中请求查询指定证券的余券
     */
    @Test
    public void testQueryCreditExcessStock() {
        System.out.println("testQueryCreditExcessStock");
        XTPClientQueryCrdSurplusStkReqInfo req = XTPClientQueryCrdSurplusStkReqInfo.builder().
                marketType(MarketType.XTP_MKT_SH_A).ticker("600007").build();
        int result = tradeApi.queryCreditExcessStock(req, sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 融资融券业务中请求负债合约展期
     */
    @Test
    public void testCreditExtendDebtDate() {
        System.out.println("testCreditExtendDebtDate");
        XTPCreditDebtExtendReq req = XTPCreditDebtExtendReq.builder().
                orderXtpId("123456789").debtId("11111").deferDays(20200119).fundAccount(ACCOUNT).password(PASSWORD).build();
        String result = tradeApi.creditExtendDebtDate(req, sessionId);
        System.out.println("result: " + result);
    }

    /**
     * 融资融券业务中请求查询负债合约展期
     */
    @Test
    public void testQueryCreditExtendDebtDateOrders() {
        System.out.println("testCreditExtendDebtDate");
        int result = tradeApi.queryCreditExtendDebtDateOrders("973522439176718291", sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询融资融券业务中账戶的附加信息
     */
    @Test
    public void testQueryCreditFundExtraInfo() {
        System.out.println("testQueryCreditFundExtraInfo");
        int result = tradeApi.queryCreditFundExtraInfo( sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询融资融券业务中账戶指定证券的附加信息
     */
    @Test
    public void testQueryCreditPositionExtraInfo() {
        System.out.println("testQueryCreditPositionExtraInfo");
        XTPClientQueryCrdPositionStockReq req = XTPClientQueryCrdPositionStockReq.builder().
                marketType(MarketType.XTP_MKT_SH_A).ticker("600007").build();
        int result = tradeApi.queryCreditPositionExtraInfo(req, sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 请求查询指定证券负债未还信息
     */
    @Test
    public void testQueryCreditTickerDebtInfo() {
        System.out.println("testQueryCreditTickerDebtInfo");
        XTPClientQueryCrdDebtStockReq req = XTPClientQueryCrdDebtStockReq.builder().
                marketType(MarketType.XTP_MKT_SH_A).ticker("600007").build();
        int result = tradeApi.queryCreditTickerDebtInfo(req, sessionId,13);
        System.out.println("result: " + result);
    }

    /**
     * 融资融券业务中卖券还指定负债合约息费请求
     */
    @Test
    public void testCreditSellStockRepayDebtInterestFee() {
        OrderInsertRequest req = OrderInsertRequest.builder()
                .orderXtpId("0").orderClientId(2).ticker("600007").marketType(MarketType.XTP_MKT_SH_A)
                .price(26).stopPrice(0).quantity(1).priceType(PriceType.XTP_PRICE_ALL_OR_CANCEL)
                .sideType(SideType.XTP_SIDE_SELL).businessType(BusinessType.XTP_BUSINESS_TYPE_MARGIN)
                .positionEffectType(PositionEffectType.XTP_POSITION_EFFECT_OPEN).build();

        System.out.println("testCreditSellStockRepayDebtInterestFee");
        String orderXtpId = tradeApi.creditSellStockRepayDebtInterestFee(req ,"1234567",sessionId);
        System.out.println("orderXtpId77: " + orderXtpId);
    }

    /**
     * 融资融券业务中请求查询余券
     */
    @Test
    public void  testQueryMulCreditExcessStock() {
        System.out.println("queryMulCreditExcessStock");
        XTPClientQueryCrdSurplusStkReqInfo req = XTPClientQueryCrdSurplusStkReqInfo.builder().ticker("").build();
        int result = tradeApi.queryMulCreditExcessStock(req,sessionId,13);
        System.out.println("queryMulCreditExcessStock: " + result);
    }

    //==============Common Functions=========

    private String insertOrder() {
        long sessionIdH = Long.valueOf(sessionId)/10;
        long sessionIdL = Long.valueOf(sessionId)%10;
        return tradeApi.insertOrder(0, 1,6,600918,2,9.07,
                0,1,5,1,1,2,
                sessionIdH,sessionIdL,88);
    }

    private String fundTransfer() {
        FundTransferRequest req = FundTransferRequest.builder()
            .fundAccount(ACCOUNT).password(PASSWORD).amount(100.0)
            .fundTransferType(FundTransferType.XTP_FUND_TRANSFER_IN).build();

        return tradeApi.fundTransfer(req, sessionId);
    }

}
