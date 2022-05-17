package src.main.java.com.zts.xtp.demo;

import com.zts.xtp.common.jni.JNILoadLibrary;
import src.main.java.com.zts.xtp.demo.quote.QuoteDemo;
import src.main.java.com.zts.xtp.demo.trade.TradeDemo;
import src.main.java.com.zts.xtp.userInfo.User;

public class Application {

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
    private static final int THREAD_NUM = User.threadNum;//接收行情的线程数
    private static final int RING_BUFFER_SIZE = User.ringBufferSize;//接收行情每一个线程数据缓冲区大小

    private static TradeDemo tradeDemo;
    private static QuoteDemo quoteDemo;


    private static boolean init() {
        JNILoadLibrary.loadLibrary();
        tradeDemo = new TradeDemo(XTP_TRADE_SERVER_IP, XTP_TRADE_SERVER_PORT,
            ACCOUNT, PASSWORD, CLIENT_ID, TRADE_KEY, DATA_FOLDER);

        quoteDemo = new QuoteDemo(XTP_QUOTE_SERVER_IP, XTP_QUOTE_SERVER_PORT,
            ACCOUNT, PASSWORD, CLIENT_ID, DATA_FOLDER, THREAD_NUM, RING_BUFFER_SIZE);

        return tradeDemo.isLogin() && quoteDemo.isLogin();
    }

    private static void shutdown() {
        if (tradeDemo.isLogin()) {
            tradeDemo.shutdown();
        }

        if (quoteDemo.isLogin()) {
            quoteDemo.shutdown();
        }
    }

    public static void main(String[] args) {
        boolean loginFlag = init();
        if (!loginFlag) {
            return;
        }

        quoteDemo.testSubscribeMarketData();
//        tradeDemo.testInsertOrder();

        shutdown();
    }
}
