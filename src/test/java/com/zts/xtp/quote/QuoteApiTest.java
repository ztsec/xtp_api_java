package com.zts.xtp.quote;

import com.zts.xtp.common.jni.JNILoadLibrary;
import com.zts.xtp.quote.api.QuoteApi;
import org.junit.*;
import org.junit.runners.MethodSorters;

import java.io.File;

import static org.junit.Assert.*;


@FixMethodOrder(MethodSorters.JVM)
public class QuoteApiTest {

    private static QuoteApi quoteApi;

    @BeforeClass
    public static void setUp() throws Exception {
        JNILoadLibrary.loadLibrary();
        TestQuoteSpi testspi = new TestQuoteSpi();
        quoteApi = new QuoteApi(testspi);
        quoteApi.setUDPBufferSize(512);

        short clientId = 18;
        quoteApi.init(clientId,"/var/log/zts/xtp",XtpLogLevel.XTP_LOG_LEVEL_INFO);

        int login_result = quoteApi.login("xx.xx.xx.xx",1234,"xxxxxx","xxxxxx",1);
        Assert.assertEquals(login_result, 0);
    }

    @AfterClass
    public static void tearDown() throws Exception {
        int logout_result = quoteApi.logout();
        Assert.assertEquals(logout_result,0);
        quoteApi.disconnect();
    }

    @After
    public void waitSomeTime() throws InterruptedException {
        Thread.sleep(2000);
    }


    @Test
    public void setHeartBeatInterval() {
        System.out.println("setHeartBeatInterval");
        quoteApi.setHeartBeatInterval(1000);
    }

    @Test
    public void subscribeMarketData() {
        System.out.println("subscribeMarketData");
        quoteApi.subscribeMarketData(new String[]{"300498"},1,2);//20008025
    }

    @Test
    public void unSubscribeMarketData() {
        System.out.println("unSubscribeMarketData");
        quoteApi.unSubscribeMarketData(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void subscribeOrderBook() {
        System.out.println("subscribeOrderBook");
        quoteApi.subscribeOrderBook(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void unSubscribeOrderBook() {
        System.out.println("unSubscribeOrderBook");
        quoteApi.unSubscribeOrderBook(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void subscribeTickByTick() {
        System.out.println("subscribeTickByTick");
        quoteApi.subscribeTickByTick(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void unSubscribeTickByTick() {
        System.out.println("unSubscribeTickByTick");
        quoteApi.unSubscribeTickByTick(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void subscribeAllMarketData() {
        System.out.println("subscribeAllMarketData");
        quoteApi.subscribeAllMarketData(2);
    }

    @Test
    public void unSubscribeAllMarketData() {
        System.out.println("unSubscribeAllMarketData");
        quoteApi.unSubscribeAllMarketData(2);
    }

    @Test
    public void subscribeAllOrderBook() {
        System.out.println("subscribeAllOrderBook");
        quoteApi.subscribeAllOrderBook(2);
    }

    @Test
    public void unSubscribeAllOrderBook() {
        System.out.println("unSubscribeAllOrderBook");
        quoteApi.unSubscribeAllOrderBook(2);
    }

    @Test
    public void subscribeAllTickByTick() {
        System.out.println("subscribeAllTickByTick");
        quoteApi.subscribeAllTickByTick(2);
    }

    @Test
    public void unSubscribeAllTickByTick() {
        System.out.println("unSubscribeAllTickByTick");
        quoteApi.unSubscribeAllTickByTick(2);
    }

    @Test
    public void queryAllTickers() {
        System.out.println("queryAllTickers");
        quoteApi.queryAllTickers(2);
    }

    @Test
    public void queryTickersPriceInfo() {
        System.out.println("queryTickersPriceInfo");
        quoteApi.queryTickersPriceInfo(new String[]{"300498"},1,2);//11011077
    }

    @Test
    public void subscribeAllOptionMarketData(){
        System.out.println("subscribeAllOptionMarketData");
        quoteApi.subscribeAllOptionMarketData(2);
    }

    @Test
    public void unSubscribeAllOptionMarketData(){
        System.out.println("unSubscribeAllOptionMarketData");
        quoteApi.unSubscribeAllOptionMarketData(2);
    }

    @Test
    public void subscribeAllOptionOrderBook(){
        System.out.println("subscribeAllOptionOrderBook");
        quoteApi.subscribeAllOptionOrderBook(2);
    }

    @Test
    public void unSubscribeAllOptionOrderBook(){
        System.out.println("unSubscribeAllOptionOrderBook");
        quoteApi.unSubscribeAllOptionOrderBook(2);
    }

    @Test
    public void subscribeAllOptionTickByTick(){
        System.out.println("subscribeAllOptionTickByTick");
        quoteApi.subscribeAllOptionTickByTick(2);
    }

    @Test
    public void unSubscribeAllOptionTickByTick(){
        System.out.println("unSubscribeAllOptionTickByTick");
        quoteApi.unSubscribeAllOptionTickByTick(2);
    }
}