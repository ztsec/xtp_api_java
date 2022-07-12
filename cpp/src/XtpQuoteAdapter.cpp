#include "include/com_zts_xtp_quote_api_QuoteApi.h"
#include "include/XtpQuoteApi.h"
#include "include/handle.h"
#include "include/xtp/xquote_api_struct.h"
#include "include/InitGlog.h"
#include "include/glog/logging.h"
#include <assert.h>
#ifdef __linux__
#include <cstring>
#endif
#include <iostream>
#include <sstream>

using namespace std;

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_initGlog
        (JNIEnv *env, jobject obj, jstring logFolder,jstring logSubFolder,jstring jniLogLevel)
{
    const char *char_xtp_data_folder = env->GetStringUTFChars(logFolder, 0);
    string strLogFolder = string(char_xtp_data_folder);
    const char *char_logSubFolder = env->GetStringUTFChars(logSubFolder, 0);
    string strLogSubFolder = string(char_logSubFolder);
	const char *char_jniLogLevel = env->GetStringUTFChars(jniLogLevel, 0);
	string strJniLogLevel = string(char_jniLogLevel);
    init_glog(strLogFolder, strLogSubFolder, strJniLogLevel);
    env->ReleaseStringUTFChars(logFolder, char_xtp_data_folder);
    env->ReleaseStringUTFChars(logSubFolder, char_logSubFolder);
    env->ReleaseStringUTFChars(jniLogLevel, char_jniLogLevel);

}

void copy_tickers(JNIEnv *env,jobjectArray tickers,char *pTickers[],int count)
{
    for(int i=0; i<count; i++)
    {
        jstring jstr = (jstring)env->GetObjectArrayElement(tickers, i);
        const char *char_ticker = env->GetStringUTFChars(jstr, 0);
        std::string strTicker(char_ticker);
        strcpy(pTickers[i], strTicker.c_str());
        env->ReleaseStringUTFChars(jstr, char_ticker);
    }
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_quoteInit(JNIEnv *env,
    jobject obj, jshort clientId, jstring logFolder, jobject jLogLevel, jint threadNum, jint ringBufferSize, jint fullMarketDataAvailable)
{
    XtpQuote *pquote = new XtpQuote(threadNum, ringBufferSize, fullMarketDataAvailable);
    pquote->setClientId(clientId);
//    pquote->setFullMarketDataAvailable(fullMarketDataAvailable);
    const char *char_xtp_data_folder = env->GetStringUTFChars(logFolder, 0);
    pquote->setFilePath(char_xtp_data_folder);

    LOG(ERROR) << "threadNum: " << threadNum;
    LOG(ERROR) << "ringBufferSize: " << ringBufferSize;
    LOG(ERROR) << "fullMarketDataAvailable: " << fullMarketDataAvailable;

    JavaVM* jvm;
    env->GetJavaVM(&jvm);
    pquote->setJniJvm(jvm);

    jclass xtpriClass =  env->FindClass("com/zts/xtp/common/model/ErrorMessage");
    pquote->setXTPRIClass((jclass)env->NewGlobalRef(xtpriClass));

    jclass xtpstClass =  env->FindClass("com/zts/xtp/quote/model/response/SpecificTickerResponse");
    pquote->setXTPSTClass((jclass)env->NewGlobalRef(xtpstClass));

    jclass xtpmdClass =  env->FindClass("com/zts/xtp/quote/model/response/DepthMarketDataResponse");
    pquote->setXTPMDClass((jclass)env->NewGlobalRef(xtpmdClass));

    jclass xtpmdseClass =  env->FindClass("com/zts/xtp/quote/model/response/MarketDataStockExDataResponse");
    pquote->setXTPMDSEClass((jclass)env->NewGlobalRef(xtpmdseClass));

    jclass xtpmdoeClass =  env->FindClass("com/zts/xtp/quote/model/response/MarketDataOptionExDataResponse");
    pquote->setXTPMDOEClass((jclass)env->NewGlobalRef(xtpmdoeClass));

    jclass xtpmdbeClass =  env->FindClass("com/zts/xtp/quote/model/response/MarketDataBondExDataResponse");
    pquote->setXTPMDBEClass((jclass)env->NewGlobalRef(xtpmdbeClass));

    jclass depthMarketDataExtClass =  env->FindClass("com/zts/xtp/quote/model/response/DepthMarketDataExResponse");
    pquote->setDepthMarketDataClass((jclass)env->NewGlobalRef(depthMarketDataExtClass));

    jclass xtpqsiClass =  env->FindClass("com/zts/xtp/quote/model/response/TickerInfoResponse");
    pquote->setXTPQSIClass((jclass)env->NewGlobalRef(xtpqsiClass));

    jclass xtpqfiClass =  env->FindClass("com/zts/xtp/quote/model/response/TickerFullInfoResponse");
    pquote->setXTPQFIClass((jclass)env->NewGlobalRef(xtpqfiClass));

    jclass xtpobClass =  env->FindClass("com/zts/xtp/quote/model/response/OrderBookResponse");
    pquote->setXTPOBClass((jclass)env->NewGlobalRef(xtpobClass));

    jclass xtptbtClass =  env->FindClass("com/zts/xtp/quote/model/response/TickByTickResponse");
    pquote->setXTPTBTClass((jclass)env->NewGlobalRef(xtptbtClass));

    jclass xtptbteClass = env->FindClass("com/zts/xtp/quote/model/response/TickByTickEntrustResponse");
    pquote->setXTPTickByTickEntrust((jclass) env->NewGlobalRef(xtptbteClass));

    jclass xtptbttClass = env->FindClass("com/zts/xtp/quote/model/response/TickByTickTradeResponse");
    pquote->setXTPTickByTickTrade((jclass) env->NewGlobalRef(xtptbttClass));

    jclass xtptpiClass =  env->FindClass("com/zts/xtp/quote/model/response/TickerPriceInfoResponse");
    pquote->setXTPTPIClass((jclass)env->NewGlobalRef(xtptpiClass));

    pquote->setPluginJobject(env->NewGlobalRef(obj));

    jclass logLevelClass = env->FindClass("com/zts/xtp/common/enums/XtpLogLevel");
    jmethodID getLogLevelValue = env->GetMethodID(logLevelClass, "getValue", "()I");
    XTP_LOG_LEVEL logLevel = (XTP_LOG_LEVEL)env->CallIntMethod(jLogLevel, getLogLevelValue);
    pquote->Init(logLevel);

    setHandle(env, obj, pquote);
    env->ReleaseStringUTFChars(logFolder, char_xtp_data_folder);

}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_disconnect (JNIEnv *env, jobject obj)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    //pquote->Release();

    LOG(INFO) << "release the quote";
    delete pquote;
    pquote = NULL;
    setHandle(env, obj, pquote);
    //shutdown_glog();
}

JNIEXPORT jint JNICALL JNICALL Java_com_zts_xtp_quote_api_QuoteApi_login (JNIEnv *env, jobject obj, jstring ip,
        jint port, jstring userName, jstring password, jobject jProtocol, jstring localIp)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

    const char *char_ip = env->GetStringUTFChars(ip, 0);
    std::string strIp(char_ip);
    const char *char_userName = env->GetStringUTFChars(userName, 0);
    std::string strUsername(char_userName);
    const char *char_password = env->GetStringUTFChars(password, 0);
    std::string strPassword(char_password);
    const char *local_ip = env->GetStringUTFChars(localIp, 0);
    std::string strLocalIp(local_ip);

    pquote->setServerIp(strIp.c_str());
    pquote->setServerPort(port);
    pquote->setUsername(strUsername.c_str());
    pquote->setPassword(strPassword.c_str());
    pquote->setServerLocalIp(strLocalIp.c_str());


    jclass protocolClass = env->FindClass("com/zts/xtp/common/enums/TransferProtocol");
    jmethodID getProtoColValue = env->GetMethodID(protocolClass, "getValue", "()I");
    XTP_PROTOCOL_TYPE protocol = (XTP_PROTOCOL_TYPE)env->CallIntMethod(jProtocol, getProtoColValue);

    pquote->setSocketType(protocol);

    int sessionId = pquote->Login();

    env->ReleaseStringUTFChars(ip, char_ip);
    env->ReleaseStringUTFChars(userName, char_userName);
    env->ReleaseStringUTFChars(password, char_password);
    env->ReleaseStringUTFChars(localIp, local_ip);

    return sessionId;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_logout (JNIEnv *env, jobject obj)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    int result = pquote->Logout();
    return result;
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_setHeartBeatInterval (JNIEnv *env, jobject obj, jint interval)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    if (pquote) {
        pquote->SetHeartBeatInterval(interval);
    }else{
        LOG(ERROR) << "quote setHeartBeatInterval failed! It mast be call before quote login and after quote init!";
    }
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_setUDPBufferSize(JNIEnv *env, jobject obj, jint size)
{
	XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
	if (pquote)
	{
		pquote->SetUDPBufferSize(size);
	}else{
        LOG(ERROR) << "quote SetUDPBufferSize failed! It mast be call before quote login and after quote init!";
    }
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_setUDPRecvThreadAffinity(JNIEnv *env, jobject obj, jint cpuNo)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    if (pquote)
    {
        pquote->SetUDPRecvThreadAffinity(cpuNo);
    }else{
        LOG(ERROR) << "quote SetUDPRecvThreadAffinity failed! It mast be call before quote login!";
    }
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_setUDPParseThreadAffinity(JNIEnv *env, jobject obj, jint cpuNo)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    if (pquote)
    {
        pquote->SetUDPParseThreadAffinity(cpuNo);
    }else{
        LOG(ERROR) << "quote SetUDPParseThreadAffinity failed! It mast be call before quote login!";
    }
}

JNIEXPORT void JNICALL Java_com_zts_xtp_quote_api_QuoteApi_setUDPSeqLogOutPutFlag(JNIEnv *env, jobject obj, jboolean flag)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    if (pquote)
    {
        pquote->SetUDPSeqLogOutPutFlag(flag);
    }else{
        LOG(ERROR) << "quote SetUDPSeqLogOutPutFlag failed! It mast be call before quote login!";
    }
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeMarketData (JNIEnv *env, jobject obj,
        jobjectArray tickers, jint count, jint exchangeType)
{
    LOG(INFO) << __PRETTY_FUNCTION__;
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
   //char *pTickers[count]; //remove by jiangch 2019.3.4
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
	if (nullptr == pTickers)
	{
		return -1;
	}
		
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }
    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->SubscribeMarketData(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        //LOG(INFO) << pTickers[i] << "; " << exchangeType;
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeMarketData (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

	//char *pTickers[count]; //remove by jiangch
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
	if (nullptr == pTickers)
	{
		return -1;
	}
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->UnSubscribeMarketData(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeOrderBook (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

	//char *pTickers[count]; //remove by jiangch
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->SubscribeOrderBook(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }

	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeOrderBook (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

	//char *pTickers[count]; //remove by jiangch
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
	if (nullptr == pTickers)
	{
		return -1;
	}
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->UnSubscribeOrderBook(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeTickByTick (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

	//char *pTickers[count]; //remove by jiangch
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->SubscribeTickByTick(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeTickByTick (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

	//char *pTickers[count]; //remove by jiangch
	char **pTickers = new char*[count]; //add by jiangch 2019.3.4
	if (nullptr == pTickers)
	{
		return -1;
	}
    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->UnSubscribeTickByTick(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllMarketData(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->SubscribeAllMarketData((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllMarketData(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->UnSubscribeAllMarketData((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllOrderBook(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->SubscribeAllOrderBook((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllOrderBook(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->UnSubscribeAllOrderBook((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllTickByTick(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->SubscribeAllTickByTick((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllTickByTick(JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->UnSubscribeAllTickByTick((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_queryAllTickers (JNIEnv *env, jobject obj, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->QueryAllTickers((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_queryTickersPriceInfo (JNIEnv *env, jobject obj, jobjectArray tickers, jint count, jint exchangeType)
{
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);

    //char *pTickers[count]; //remove by jiangch  2019.3.4
	char **pTickers = new char*[count];  //add by jiangch 2019.3.4
	if (nullptr == pTickers)
	{
		return -1;
	}

    for(int i=0; i<count; i++)
    {
        pTickers[i] = new char[XTP_TICKER_LEN];
        memset(pTickers[i],0,XTP_TICKER_LEN);
    }

    copy_tickers(env,tickers,pTickers,count);
    int result = pquote->QueryTickersPriceInfo(pTickers, count, (XTP_EXCHANGE_TYPE)exchangeType);

    for(int i=0; i<count; i++)
    {
        delete[] pTickers[i];
    }
	delete[] pTickers; //add by jiangch 2019.3.4
    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllOptionMarketData(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->SubscribeAllOptionMarketData((XTP_EXCHANGE_TYPE)exchangeType);
}
  
JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllOptionMarketData(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->UnSubscribeAllOptionMarketData((XTP_EXCHANGE_TYPE)exchangeType);
}
  
JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllOptionOrderBook(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->SubscribeAllOptionOrderBook((XTP_EXCHANGE_TYPE)exchangeType);
}
  
JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllOptionOrderBook(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->UnSubscribeAllOptionOrderBook((XTP_EXCHANGE_TYPE)exchangeType);
}
  
JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_subscribeAllOptionTickByTick(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->SubscribeAllOptionTickByTick((XTP_EXCHANGE_TYPE)exchangeType);
}
  
JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_unSubscribeAllOptionTickByTick(JNIEnv *env, jobject obj, jint exchangeType){
  XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
  return pquote->UnSubscribeAllOptionTickByTick((XTP_EXCHANGE_TYPE)exchangeType);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_quote_api_QuoteApi_queryAllTickersFullInfo(JNIEnv *env, jobject obj, jint exchangeType){
    XtpQuote *pquote = getHandle<XtpQuote>(env, obj);
    return pquote->QueryAllTickersFullInfo((XTP_EXCHANGE_TYPE)exchangeType);
}
