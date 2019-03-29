# 中泰证券量化交易平台XTP JAVA API接口
    本项目是中泰证券XTP极速交易JAVA接口的开源实现，供客户在量化交易中使用JAVA接口快速接入XTP系统。中泰证券XTP是为股票交易而生的极速交易系统，为投资者提供极速交易、极速行情、Level2行情。

    目前支持xtp版本为1.1.18.13，支持win、linux、mac平台运行
    
    请先到中泰证券xtp官方网站申请测试账号 https://xtp.zts.com.cn/register 及测试环境的连接ip、端口等信息
                                                                                        
    API参考官方C++版本的接口文档https://xtp.zts.com.cn/home
   

##如何使用：
* **方式一：直接使用成品**
    * 1）在项目根目录下手工拷贝动态库到系统目录 
    
        > linux：
        
            cp cpp/linux/* /usr/local/lib/
            cp cpp/buildcpp/linux/libtradeplugin.so /usr/local/lib/
            cp cpp/buildcpp/linux/libquoteplugin.so /usr/local/lib/
            
        > mac：
        
            cp cpp/mac/* /usr/local/lib/
            cp cpp/buildcpp/macosx/libtradeplugin.dylib /usr/local/lib/
            cp cpp/buildcpp/macosx/libquoteplugin.dylib /usr/local/lib/
            
        > win32：
        
            copy cpp\lib\win32\dll\*  c:\windows\system32\
            copy cpp\buildcpp\win32\libtradeplugin.dll  c:\windows\system32\
            copy cpp\buildcpp\win32\libquoteplugin.dll  c:\windows\system32\
            
         > win64：
                 
            copy cpp\lib\win64\dll\*  c:\windows\system32\
            copy cpp\buildcpp\win64\libtradeplugin.dll  c:\windows\system32\
            copy cpp\buildcpp\win64\libquoteplugin.dll  c:\windows\system32\
                     
                     
    * 2）在量化交易java代码中引入xtpapi-1.1.18.13.jar并使用
    
         > 如需进行单元测试：
                           
            src/test/java/com.zts.xtp/trade/TradeApiTest.java是交易的单元测试：
            修改TradeApiTest.java：
            tradeApi.init((short)18, "23a71733bba3sd78722319b212e",
                       "/var/log/zts/xtp", XtpLogLevel.XTP_LOG_LEVEL_INFO);
            sessionId = tradeApi.login("xx.xx.xx.xx", 1234,
                       "15001030", "xxxxxx", TransferProtocol.XTP_PROTOCOL_TCP);
            init方法的第1个参数是xtp client id（不超过255），实盘环境配置请联系官方人员获取，
                     第2个参数为测试环境的serverkey，实盘环境请联系官方人员获取，
                     第3个参数为java api产生的日志路径，
                     第4个参数为java api交易日志级别，
            login方法的参数分别是测试xtp交易ip、xtp交易端口、资金账号、密码、传输方式(测试只支持TCP，实盘可配置TCP或UDP），
                     请分别填入测试环境的参数及申请到的测试账号口令，实盘环境配置请联系官方人员获取。
                                          
            src/test/java/com.zts.xtp/quote/QuoteApiTest.java是行情的单元测试：
            修改QuoteApiTest.java：
            int login_result = quoteApi.login("xx.xx.xx.xx",1234,"xxxxx","xxxxxx",1);
            login方法第1个参数为xtp行情ip、第2个参数为xtp行情端口、第3个参数为申请到的测试账号、第4个参数为账号密码，
                    实盘环境的环境信息及资金账号请联系官方人员获取。
                    
            执行./gradlew build -x test进行重新编译java生成xtpapi-1.1.18.13.jar
            分别执行TradeApiTest、QuoteApiTest中的junit单元测试。
                  
         > 如需进行demo测试：
                                     
            使用java ide导入xtp_api_demo工程，修改xtp_api_demo/src/main/java/com/zts/xtp/demo/Application.java
            修改如下参数（参考上面如需进行单元测试的说明）：
                    private static final String XTP_TRADE_SERVER_IP = "xx.xx.xx.xx";//xtp交易server的ip
                    private static final int XTP_TRADE_SERVER_PORT = 1234;//xtp交易server的端口
                    private static final String TRADE_KEY = "23a71733bba3sd78722319b212e";//xtp交易serverkey
                    private static final short CLIENT_ID = 18;//xtp允许的clientid
                    private static final String XTP_QUOTE_SERVER_IP = "xx.xx.xx.xx";//xtp行情server的ip
                    private static final int XTP_QUOTE_SERVER_PORT = 1234;//xtp行情server的端口
                    private static final String ACCOUNT = "xxxxxx";//xtp资金账号
                    private static final String PASSWORD = "xxxxxx";//xtp密码
                    private static final String DATA_FOLDER = "/var/log/zts/xtp";//java api输出日志的本地目录
            运行Application.java即可

 
<br/>

* **方式二：源码编译后使用（用于当方式一无法正常使用或需要修改源码时）**
    * 1）安装C++编译工具
    
         > linux：yum install -y gcc 或apt-get install -y gcc
         
         > mac: xcode-select --install 或 安装brew并用brew install gcc@7
         
         > win：安装visual stuido 2015
         
    * 2）安装cmake
    
         > linux：yum install -y cmake 或apt-get install -y cmake
         
         > mac:  http://www.cmake.org/download/下载cmake-3.13.2.tar.gz 解压tar-zxvf cmake-3.13.2.tar.gz，进入解压目录后，sudo ./bootstrap && sudo make && sudo make install
         
         > win：下载安装https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-win32-x86.msi、安装visual studio(2005及以上)
         
    * 3）安装jdk8
    
        > 根据系统安装对应jdk https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html
   
    * 4）在工程根目录下的mkdir cpp/build,进入到cpp/build目录执行cmake ..
         如果是linux或mac：则继续执行 make&&make install 进行编译和安装 
         如果是win：则在cpp/build目录会生成visual studio工程，打开执行编译
         则在/usr/local/lib(win下需要手工将编译生成的dll从cpp\build\Release拷贝到c:\windows\system32\)下会新增5个动态库：
    
        > linux：libglog.so、libxtptraderapi.so、libxtpquoteapi.so、libtradeplugin.so、libquoteplugin.so
   
        > mac：libglog.dylib、libxtptraderapi.dylib、libxtpquoteapi.dylib、libtradeplugin.dylib、libquoteplugin.dylib
        
        > win：libglog.dll、libxtptraderapi.dll、libxtpquoteapi.dll、libtradeplugin.dll、libquoteplugin.dll
    
    * 5）安装并配置gradle
    
    * 6）在工程根目录下执行./gradlew build -x test 执行成功后在项目根目录生成build/libs/xtpapi-1.1.18.13.jar
    
    * 7）在量化交易java代码中引入xtpapi-1.1.18.13.jar并使用
           
         > 如需进行单元测试：
                   
          src/test/java/com.zts.xtp/trade/TradeApiTest.java是交易的单元测试：
          修改TradeApiTest.java：
          tradeApi.init((short)18, "23a71733bba3sd78722319b212e",
                                  "/var/log/zts/xtp", XtpLogLevel.XTP_LOG_LEVEL_INFO);
          sessionId = tradeApi.login("xx.xx.xx.xx", 1234,
                                            "xxxxxx", "xxxxxx", TransferProtocol.XTP_PROTOCOL_TCP);
          init方法的第1个参数是xtp client id（不超过255），实盘环境配置请联系官方人员获取，
                   第2个参数为测试环境的serverkey，实盘环境请联系官方人员获取，
                   第3个参数为java api产生的日志路径，
                   第4个参数为java api交易日志级别，
          login方法的参数分别是测试xtp交易ip、xtp交易端口、资金账号、密码、传输方式(测试只支持TCP，实盘可配置TCP或UDP），请分别
                   填入测试环境的参数及申请到的测试账号口令，实盘环境配置请联系官方人员获取。
                                  
          src/test/java/com.zts.xtp/quote/QuoteApiTest.java是行情的单元测试：
          修改QuoteApiTest.java：
          int login_result = quoteApi.login("23a71733bba3sd78722319b212e",1234,"xxxxxx","xxxxxx",1);
          login方法第1个参数为xtp行情ip、第2个参数为xtp行情端口、第3个参数为申请到的测试账号、第4个参数为账号密码，
                   实盘环境的环境信息及资金账号请联系官方人员获取。
                   
          执行./gradlew build -x test进行重新编译java生成xtpapi-1.1.18.13.jar
          分别执行TradeApiTest、QuoteApiTest中的junit单元测试。
          
         > 如需进行demo测试：
                             
          使用java ide导入xtp_api_demo工程，修改xtp_api_demo/src/main/java/com/zts/xtp/demo/Application.java
          修改如下参数（参考上面如需进行单元测试的说明）：
                   private static final String XTP_TRADE_SERVER_IP = "xx.xx.xx.xx";//xtp交易server的ip
                   private static final int XTP_TRADE_SERVER_PORT = 1234;//xtp交易server的端口
                   private static final String TRADE_KEY = "23a71733bba3sd78722319b212e";//xtp交易serverkey
                   private static final short CLIENT_ID = 18;//xtp允许的clientid
                   private static final String XTP_QUOTE_SERVER_IP = "xx.xx.xx.xx";//xtp行情server的ip
                   private static final int XTP_QUOTE_SERVER_PORT = 1234;//xtp行情server的端口
                   private static final String ACCOUNT = "xxxxxx";//xtp资金账号
                   private static final String PASSWORD = "xxxxxx";//xtp密码
                   private static final String DATA_FOLDER = "/var/log/zts/xtp";//java api输出日志的本地目录
          运行Application.java即可
          
          