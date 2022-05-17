package src.main.java.com.zts.xtp.userInfo;

import com.zts.xtp.common.enums.TransferProtocol;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

public class User {

    private static String propFilePath = "user.config.properites";


    public static short clientId;
    public static String serverKey;
    public static String logFolder;
    public static String tradeServerIP;
    public static int tradeServerPort;
    public static String userName;
    public static String userPwd;
    public static TransferProtocol tradeTransferProtocol;
    public static String quoteServerIP;
    public static int quoteServerPort;
    public static TransferProtocol quoteTransferProtocol;
    public static int udpBufferSize;
    public static int threadNum;
    public static int ringBufferSize;


    //    public static void main(String args[]) {
    public static void readPropToInit() {
        String isInner = User.getValue("inner");
        if(isInner!=null){
            if(isInner.equals("")){
                isInner = User.getValue("inner");
                if(isInner!=null){
                    if(isInner.equals("1"))
                        propFilePath = propFilePath.replaceAll("user","user-noCommit");
                }
            }else if(isInner.equals("1"))
                propFilePath = propFilePath.replaceAll("user","user-noCommit");
        }

        clientId = Short.parseShort(User.getValue("clientId"));
        serverKey = User.getValue("serverKey");
        logFolder = User.getValue("logFolder");
        tradeServerIP = User.getValue("tradeServerIP");
        tradeServerPort = Integer.parseInt(User.getValue("tradeServerPort"));
        userName = User.getValue("userName");
        userPwd = User.getValue("userPwd");
        String protocol = User.getValue("tradeTransferProtocol");
        if("udp".equals(protocol))
            tradeTransferProtocol = TransferProtocol.XTP_PROTOCOL_UDP;
        else
            tradeTransferProtocol = TransferProtocol.XTP_PROTOCOL_TCP;
        quoteServerIP = User.getValue("quoteServerIP");
        quoteServerPort = Integer.parseInt(User.getValue("quoteServerPort"));
        protocol = User.getValue("quoteTransferProtocol");
        if("udp".equals(protocol))
            quoteTransferProtocol = TransferProtocol.XTP_PROTOCOL_UDP;
        else
            quoteTransferProtocol = TransferProtocol.XTP_PROTOCOL_TCP;
        udpBufferSize = Integer.parseInt(User.getValue("udpBufferSize"));
        threadNum = Integer.parseInt(User.getValue("threadNum"));
        ringBufferSize = Integer.parseInt(User.getValue("ringBufferSize"));

    }


    private static Properties prop = new Properties();

    /**
     * 取出值
     * @param k
     * @return
     */
    private static String getValue(String k){
        InputStream in;
        try {
            in = new BufferedInputStream(new FileInputStream(propFilePath));
            prop.load(in);     ///加载属性列表
            Iterator<String> it = prop.stringPropertyNames().iterator();
            while (it.hasNext()) {
                String key = it.next();
                if (key.equals(k)) {
                    return prop.getProperty(key);
                }
            }
            in.close();
        }catch (java.io.FileNotFoundException e) {
            propFilePath = "../" + propFilePath;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
    /**
     * 设置键值
     * @param map
     */
    private static void setValue(Map<String, String> map){
        ///保存属性到b.properties文件
        FileOutputStream oFile ;
        try {
            System.out.println("s"+propFilePath);
            oFile = new FileOutputStream(propFilePath, false);
            //true表示追加打开
            System.out.println(map.get("key")+",,,"+map.get("value"));
            prop.setProperty(map.get("key"), map.get("value"));
            //prop.put(map.get("key"), map.get("value"));
            prop.store(oFile, "The New properties file");
            oFile.close();
        } catch (Exception e) {
            // TODO 自动生成的 catch 块
            e.printStackTrace();
        }
    }


}
