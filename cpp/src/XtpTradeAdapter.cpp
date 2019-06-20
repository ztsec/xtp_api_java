#include "include/com_zts_xtp_trade_api_TradeApi.h"
#include "include/XtpTradeApi.h"
#include "include/handle.h"
#include "include/InitGlog.h"
#include "include/xtp/xoms_api_struct.h"
#include "include/glog/logging.h"
#include <assert.h>
#ifdef __linux__
#include <cstring>
#endif
#include <iostream>
#include <sstream>

using namespace std;

static Trade* ptrade_=NULL;

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_initGlog
        (JNIEnv *env, jobject obj, jstring logFolder,jstring logSubFolder, jstring jniLogLevel)
{
    const char *char_xtp_data_folder = env->GetStringUTFChars(logFolder, 0);
    string strLogFolder = string(char_xtp_data_folder);
    const char *char_logSubFolder = env->GetStringUTFChars(logSubFolder, 0);
    string strLogSubFolder = string(char_logSubFolder);
	const char *char_jniLogLevel = env->GetStringUTFChars(jniLogLevel, 0);
	string strJniLogLevel = string(char_jniLogLevel);
	init_glog(strLogFolder, strLogSubFolder, strJniLogLevel);
}

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_tradeInit
  (JNIEnv *env, jobject obj, jshort clientId, jstring key, jstring logFolder, jobject jLogLevel, jobject resumeTypeObj)
{
    Trade *ptrade;
    if(NULL == ptrade_)
    {
        ptrade = new Trade();
        ptrade->setClientId(clientId);

        const char *char_serverKey = env->GetStringUTFChars(key, NULL);
        ptrade->setKey(char_serverKey);
        const char *char_xtp_data_folder = env->GetStringUTFChars(logFolder, 0);
        ptrade->setFilePath(char_xtp_data_folder);

        JavaVM* jvm;
        env->GetJavaVM(&jvm);
        ptrade->setJniJvm(jvm);

        jclass tradeOrderInfoClass =  env->FindClass("com/zts/xtp/trade/model/response/OrderResponse");
        ptrade->setTradeOrderInfoClass((jclass)env->NewGlobalRef(tradeOrderInfoClass));

        jclass queryAssetRspClass =  env->FindClass("com/zts/xtp/trade/model/response/AssetResponse");
        ptrade->setQueryAssetRespClass((jclass)env->NewGlobalRef(queryAssetRspClass));

        jclass queryStkPositionRspClass =  env->FindClass("com/zts/xtp/trade/model/response/StockPositionResponse");
        ptrade->setStkPositionRspClass((jclass)env->NewGlobalRef(queryStkPositionRspClass));

        jclass tradeReportClass =  env->FindClass("com/zts/xtp/trade/model/response/TradeResponse");
        ptrade->setTradeReportClass((jclass)env->NewGlobalRef(tradeReportClass));

        jclass xtpErrorMsgClass =  env->FindClass("com/zts/xtp/common/model/ErrorMessage");
        ptrade->setXTPErrorMsgClass((jclass)env->NewGlobalRef(xtpErrorMsgClass));

        jclass structuredFundInfoClass =  env->FindClass("com/zts/xtp/trade/model/response/StructuredFundResponse");
        ptrade->setXTPStructuredFundInfoClass((jclass)env->NewGlobalRef(structuredFundInfoClass));

        jclass fundTransferNoticeClass =  env->FindClass("com/zts/xtp/trade/model/response/FundTransferResponse");
        ptrade->setXTPFundTransferNoticeClass((jclass)env->NewGlobalRef(fundTransferNoticeClass));

        jclass queryETFBaseRspClass =  env->FindClass("com/zts/xtp/trade/model/response/ETFBaseResponse");
        ptrade->setXTPQueryETFBaseRspClass((jclass)env->NewGlobalRef(queryETFBaseRspClass));

        jclass queryETFComponentRspClass =  env->FindClass("com/zts/xtp/trade/model/response/ETFComponentResponse");
        ptrade->setXTPQueryETFComponentRspClass((jclass)env->NewGlobalRef(queryETFComponentRspClass));

        jclass queryIPOTickerRspClass =  env->FindClass("com/zts/xtp/trade/model/response/IPOTickerResponse");
        ptrade->setXTPQueryIPOTickerRspClass((jclass)env->NewGlobalRef(queryIPOTickerRspClass));

        jclass queryIPOQuotaRspClass =  env->FindClass("com/zts/xtp/trade/model/response/IPOQuotaResponse");
        ptrade->setXTPQueryIPOQuotaRspClass((jclass)env->NewGlobalRef(queryIPOQuotaRspClass));

        jclass queryOptionAuctionInfoRspClass = env->FindClass("com/zts/xtp/trade/model/response/OptionAuctionInfoResponse");
        ptrade->setXtpQueryOptionAuctionInfoRspClass((jclass)env->NewGlobalRef(queryOptionAuctionInfoRspClass));

        jclass orderCancelInfoRspClass = env->FindClass("com/zts/xtp/trade/model/response/OrderCancelResponse");
        ptrade->setXTPOrderCancelInfoRspClass((jclass)env->NewGlobalRef(orderCancelInfoRspClass));

        ptrade->setTradePluginJobject(env->NewGlobalRef(obj));

        jclass logLevelClass = env->FindClass("com/zts/xtp/common/enums/XtpLogLevel");
        jmethodID getLogLevelValue = env->GetMethodID(logLevelClass, "getValue", "()I");
        XTP_LOG_LEVEL logLevel = (XTP_LOG_LEVEL)env->CallIntMethod(jLogLevel, getLogLevelValue);

        jclass resumeTypeClass = env->FindClass("com/zts/xtp/common/enums/XtpTeResumeType");
        jmethodID resumeTypeMethod = env->GetMethodID(resumeTypeClass, "getType", "()I");
        XTP_TE_RESUME_TYPE resumeType = (XTP_TE_RESUME_TYPE)env->CallIntMethod(resumeTypeObj, resumeTypeMethod);

        ptrade->Init(logLevel,resumeType);

        ptrade_=ptrade;

    } else
    {
        ptrade=ptrade_;
    }

    setHandle(env, obj, ptrade);
}

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_subscribePublicTopic (JNIEnv *env, jobject obj, jobject resumeTypeObj)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    jclass resumeTypeClass = env->FindClass("com/zts/xtp/common/enums/XtpTeResumeType");
    jmethodID resumeTypeMethod = env->GetMethodID(resumeTypeClass, "getType", "()I");
    XTP_TE_RESUME_TYPE resumeType = (XTP_TE_RESUME_TYPE)env->CallIntMethod(resumeTypeObj, resumeTypeMethod);

    ptrade->SubscribePublicTopic(resumeType);
    return;
}


JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_setHeartBeatInterval (JNIEnv *env, jobject obj, jint interval)
{
    Trade *ptrade = getHandle<Trade>(env, obj);
    ptrade->SetHeartBeatInterval(interval);
}


JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_disconnect (JNIEnv *env, jobject obj)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

//#ifdef __APPLE__
//    LOG(ERROR) << "mac os not release the trade";
//#else
    LOG(INFO) << "release the trade";
    ptrade->Release();
    if(ptrade!=NULL)
        delete ptrade;
//#endif
    ptrade = NULL;
    setHandle(env, obj, ptrade);
    shutdown_glog();

}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_getApiLastError (JNIEnv *env, jobject obj)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    char* errorMsg = ptrade->GetApiLastError();
    return env->NewStringUTF(errorMsg);
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_login
    (JNIEnv *env, jobject obj, jstring ip, jint port, jstring userName, jstring password, jobject jProtocol)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    const char *char_ip = env->GetStringUTFChars(ip, 0);
    std::string strIp(char_ip);

    const char *char_userName = env->GetStringUTFChars(userName, 0);
    std::string strUsername(char_userName);
    const char *char_password = env->GetStringUTFChars(password, 0);
    std::string strPassword(char_password);

    jclass protocolClass = env->FindClass("com/zts/xtp/common/enums/TransferProtocol");
    jmethodID getProtoColValue = env->GetMethodID(protocolClass, "getValue", "()I");
    XTP_PROTOCOL_TYPE protocol = (XTP_PROTOCOL_TYPE)env->CallIntMethod(jProtocol, getProtoColValue);
    uint64_t sessionId = ptrade->Login(strIp, port, strUsername, strPassword, protocol);

    env->ReleaseStringUTFChars(ip, char_ip);
    env->ReleaseStringUTFChars(userName, char_userName);
    env->ReleaseStringUTFChars(password, char_password);

    jstring strSessionId = env->NewStringUTF((std::to_string(sessionId)).c_str());
    return strSessionId;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_logout(JNIEnv *env, jobject obj, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    int result = ptrade->Logout(sessionId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    return result;
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryTradesByXTPID(JNIEnv *env, jobject obj, jstring orderXTPId, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t xtpId;
    const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
    std::stringstream(char_xtpOrderId)>> xtpId;
    int result = ptrade->QueryTradesByXTPID(xtpId, sessionId, requestId);

    env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    return result;

}

jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_insertOrder(JNIEnv *env, jobject obj, jobject tradeOrder, jstring strSessionId)
{

   //build the order
    XTPOrderInsertInfo orderInfo;

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass tradeOrderClass = env->GetObjectClass(tradeOrder);
    assert(tradeOrderClass != NULL);

    jmethodID jm_getOrderXtpId = env->GetMethodID(tradeOrderClass, "getOrderXtpId", "()Ljava/lang/String;");
    assert(jm_getOrderXtpId != NULL);
    jstring jstr_xtpOrderId = (jstring)env->CallObjectMethod(tradeOrder, jm_getOrderXtpId);
    const char *char_xtpOrderId = env->GetStringUTFChars(jstr_xtpOrderId, 0);
    std::stringstream(char_xtpOrderId)>> orderInfo.order_xtp_id;

    jmethodID jm_getOrderClientId = env->GetMethodID(tradeOrderClass, "getOrderClientId", "()I");
    orderInfo.order_client_id = env->CallLongMethod(tradeOrder, jm_getOrderClientId);

    jmethodID jm_getTicker = env->GetMethodID(tradeOrderClass, "getTicker", "()Ljava/lang/String;");
    jstring order_ticker = (jstring)env->CallObjectMethod(tradeOrder, jm_getTicker);
    assert(order_ticker != NULL);
    const char *char_ticker = env->GetStringUTFChars(order_ticker, 0);
    strcpy(orderInfo.ticker, char_ticker);

    jmethodID jm_getMarketType = env->GetMethodID(tradeOrderClass, "getMarketType", "()I");
    orderInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(tradeOrder, jm_getMarketType);

    jmethodID jm_getPrice = env->GetMethodID(tradeOrderClass, "getPrice", "()D");
    orderInfo.price = env->CallDoubleMethod(tradeOrder, jm_getPrice);

    jmethodID jm_getStopPrice = env->GetMethodID(tradeOrderClass, "getStopPrice", "()D");
    orderInfo.stop_price = env->CallDoubleMethod(tradeOrder, jm_getStopPrice);

    jmethodID jm_getQuantity = env->GetMethodID(tradeOrderClass, "getQuantity", "()J");
    orderInfo.quantity = env->CallLongMethod(tradeOrder, jm_getQuantity);

    jmethodID jm_getPriceType = env->GetMethodID(tradeOrderClass, "getPriceType", "()I");
    orderInfo.price_type = (XTP_PRICE_TYPE)env->CallIntMethod(tradeOrder, jm_getPriceType);

    jmethodID jm_getSideType = env->GetMethodID(tradeOrderClass, "getSideType", "()I");
    int  sideType = env->CallIntMethod(tradeOrder, jm_getSideType);
    orderInfo.side = (XTP_SIDE_TYPE)(sideType);

    jmethodID jm_getBusinessType = env->GetMethodID(tradeOrderClass, "getBusinessType", "()I");
    orderInfo.business_type = (XTP_BUSINESS_TYPE)env->CallIntMethod(tradeOrder, jm_getBusinessType);


     jmethodID jm_getPositionEffectType = env->GetMethodID(tradeOrderClass, "getPositionEffectType", "()I");
     int positionEffect =  env->CallIntMethod(tradeOrder, jm_getPositionEffectType);
     orderInfo.position_effect = (XTP_POSITION_EFFECT_TYPE)(positionEffect);
     string str_position_effect;
     str_position_effect = orderInfo.position_effect;
     LOG(INFO)<< "insert order: sessionId: " << sessionId << "; ticker: " << orderInfo.ticker << "; market: " << orderInfo.market \
      << "; price: " << orderInfo.price << "; stop_price: " << orderInfo.stop_price << "; quantity: " << orderInfo.quantity \
       << "; price_type: " << orderInfo.price_type << "; side: " << sideType << "; business_type: " << orderInfo.business_type
        << "; position_effect: " + str_position_effect;

    uint64_t orderId = ptrade->InsertOrder(orderInfo, sessionId);
    env->ReleaseStringUTFChars(order_ticker,char_ticker);
    env->ReleaseStringUTFChars(jstr_xtpOrderId, char_xtpOrderId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    jstring strOrderId = env->NewStringUTF((std::to_string(orderId)).c_str());
    return strOrderId;
}

jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_cancelOrder(JNIEnv *env, jobject obj, jstring orderXTPId, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t xtpId;
    const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
    std::stringstream(char_xtpOrderId) >> xtpId;
    LOG(INFO)<< "cancle order: sessionId: " << sessionId << "; xtpId: " << xtpId;
    uint64_t cancelXTPId = ptrade->CancelOrder(xtpId, sessionId);

    env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    jstring strCancelOrderId = env->NewStringUTF((std::to_string(cancelXTPId)).c_str());
    return strCancelOrderId;

}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryOrderByXtpId(JNIEnv *env, jobject obj, jstring orderXTPId, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t xtpId;
    const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
    std::stringstream(char_xtpOrderId) >> xtpId;

    env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    return ptrade->QueryOrderByXTPID(xtpId, sessionId, requestId);
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryTradesByXtpId(JNIEnv *env, jobject obj, jstring orderXTPId, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t xtpId;
    const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
    std::stringstream(char_xtpOrderId) >> xtpId;

    env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    return ptrade->QueryTradesByXTPID(xtpId, sessionId, requestId);
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryOrders(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    //build the query parameter
    XTPQueryOrderReq orderQueryParam;
    memset(&orderQueryParam,0,sizeof(struct XTPQueryOrderReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    const char *char_ticker = NULL;
    jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    if (strTicker != NULL)
    {
        char_ticker = env->GetStringUTFChars(strTicker, 0);
        strcpy(orderQueryParam.ticker, char_ticker);
    }

    jmethodID jm_getBeginTime = env->GetMethodID(queryParamClass, "getBeginTime", "()J");
    orderQueryParam.begin_time =  env->CallLongMethod(queryParam, jm_getBeginTime);

    jmethodID jm_getEndTime = env->GetMethodID(queryParamClass, "getEndTime", "()J");
    orderQueryParam.end_time =  env->CallLongMethod(queryParam, jm_getEndTime);

    int result = ptrade->QueryOrders(orderQueryParam, sessionId, requestId);
    if (strTicker != NULL)
    {
        env->ReleaseStringUTFChars(strTicker,char_ticker);
    }
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryTrades(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    //build the query parameter
    XTPQueryTraderReq tradeQueryParam;
    memset(&tradeQueryParam,0,sizeof(struct XTPQueryTraderReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    const char *char_ticker = NULL;
    jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    if (strTicker != NULL)
    {
        char_ticker = env->GetStringUTFChars(strTicker, 0);
        strcpy(tradeQueryParam.ticker, char_ticker);
    }
    
    jmethodID jm_getBeginTime = env->GetMethodID(queryParamClass, "getBeginTime", "()J");
    tradeQueryParam.begin_time =  env->CallLongMethod(queryParam, jm_getBeginTime);

    jmethodID jm_getEndTime = env->GetMethodID(queryParamClass, "getEndTime", "()J");
    tradeQueryParam.end_time =  env->CallLongMethod(queryParam, jm_getEndTime);

    int result = ptrade->QueryTrades(tradeQueryParam, sessionId, requestId);
    if (strTicker != NULL)
    {
        env->ReleaseStringUTFChars(strTicker,char_ticker);
    }
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryPosition(JNIEnv *env, jobject obj, jstring ticker, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    const char *char_ticker = NULL;
    if (ticker != NULL) 
    {
        char_ticker = env->GetStringUTFChars(ticker, 0);
    }

    int result = ptrade->QueryPosition(char_ticker, sessionId, requestId);
    if (ticker != NULL) 
    {
        env->ReleaseStringUTFChars(ticker,char_ticker);
    }
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryAsset(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryAsset(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryStructuredFund(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPQueryStructuredFundInfoReq req;
    memset(&req,0,sizeof(struct XTPQueryStructuredFundInfoReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getExchangeType = env->GetMethodID(queryParamClass, "getExchangeType", "()I");
    req.exchange_id = (XTP_EXCHANGE_TYPE)env->CallIntMethod(queryParam, jm_getExchangeType);

    jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (strTicker != NULL) 
    {
        char_ticker = env->GetStringUTFChars(strTicker, 0);
        strcpy(req.sf_ticker, char_ticker);
    }
    

    int result = ptrade->QueryStructuredFund(req, sessionId, requestId);
    if (strTicker != NULL) 
    {
        env->ReleaseStringUTFChars(strTicker, char_ticker);
    }
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_fundTransfer(JNIEnv *env, jobject obj, jobject fundTransfer, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPFundTransferReq req;
    memset(&req,0,sizeof(struct XTPFundTransferReq));

    jclass queryParamClass = env->GetObjectClass(fundTransfer);
    assert(queryParamClass != NULL);

//    jmethodID jm_getSerialId = env->GetMethodID(queryParamClass, "getSerialId", "()Ljava/lang/String;");
//    assert(jm_getSerialId != NULL);
//    jstring jstr_serialId = (jstring)env->CallObjectMethod(fundTransfer, jm_getSerialId);
//    const char *char_serialId = env->GetStringUTFChars(jstr_serialId, 0);
//    std::stringstream(char_serialId)>> req.serial_id;

    jmethodID jm_getFundAccount = env->GetMethodID(queryParamClass, "getFundAccount", "()Ljava/lang/String;");
    assert(jm_getFundAccount != NULL);
    jstring strFundAccount = (jstring)env->CallObjectMethod(fundTransfer, jm_getFundAccount);
    const char *char_fund_account = env->GetStringUTFChars(strFundAccount, 0);
    strcpy(req.fund_account, char_fund_account);

    jmethodID jm_getPassword = env->GetMethodID(queryParamClass, "getPassword", "()Ljava/lang/String;");
    assert(jm_getPassword != NULL);
    jstring strPassword = (jstring)env->CallObjectMethod(fundTransfer, jm_getPassword);
    const char *char_password = env->GetStringUTFChars(strPassword, 0);
    strcpy(req.password, char_password);

    jmethodID jm_getAmount = env->GetMethodID(queryParamClass, "getAmount", "()D");
    req.amount = env->CallDoubleMethod(fundTransfer, jm_getAmount);

    jmethodID jm_getTransferType = env->GetMethodID(queryParamClass, "getFundTransferType", "()I");
    req.transfer_type = (XTP_FUND_TRANSFER_TYPE)env->CallIntMethod(fundTransfer, jm_getTransferType);


    uint64_t result = ptrade->FundTransfer(req, sessionId);
    env->ReleaseStringUTFChars(strPassword, char_password);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    //env->ReleaseStringUTFChars(jstr_serialId, char_serialId);
    env->ReleaseStringUTFChars(strFundAccount, char_fund_account);

    jstring serial_id = env->NewStringUTF((std::to_string(result)).c_str());
    return serial_id;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryFundTransfer(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPQueryFundTransferLogReq req;
    memset(&req,0,sizeof(struct XTPQueryFundTransferLogReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getSerialId = env->GetMethodID(queryParamClass, "getSerialId", "()Ljava/lang/String;");
    assert(jm_getSerialId != NULL);
    jstring jstr_serialId = (jstring)env->CallObjectMethod(queryParam, jm_getSerialId);
    const char *char_serialId = env->GetStringUTFChars(jstr_serialId, 0);
    std::stringstream(char_serialId)>> req.serial_id;

    int result = ptrade->QueryFundTransfer(req, sessionId, requestId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    env->ReleaseStringUTFChars(jstr_serialId, char_serialId);

    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryETF(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPQueryETFBaseReq req;
    memset(&req,0,sizeof(struct XTPQueryETFBaseReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getMarketType = env->GetMethodID(queryParamClass, "getMarketType", "()I");
    if (queryParam != NULL)
    {
        req.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);
    }

    jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (strTicker != NULL)
    {
        char_ticker = env->GetStringUTFChars(strTicker, 0);
        strcpy(req.ticker, char_ticker);
    }


    int result = ptrade->QueryETF(req, sessionId, requestId);
    if (strTicker != NULL) 
    {
        env->ReleaseStringUTFChars(strTicker, char_ticker);
    }
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryETFTickerBasket(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPQueryETFComponentReq req;
    memset(&req,0,sizeof(struct XTPQueryETFComponentReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getMarketType = env->GetMethodID(queryParamClass, "getMarketType", "()I");
    req.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = env->GetStringUTFChars(strTicker, 0);
    strcpy(req.ticker, char_ticker);

    int result = ptrade->QueryETFTickerBasket(req, sessionId, requestId);
    env->ReleaseStringUTFChars(strTicker, char_ticker);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryIPOInfoList(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryIPOInfoList(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryIPOQuotaInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryIPOQuotaInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryOptionAuctionInfo(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer

    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPQueryOptionAuctionInfoReq req;
    memset(&req,0,sizeof(struct XTPQueryOptionAuctionInfoReq));

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    if(queryParam != NULL){
        jclass queryParamClass = env->GetObjectClass(queryParam);
        if(queryParamClass != NULL){
            jmethodID jm_getMarketType = env->GetMethodID(queryParamClass, "getMarketType", "()I");
            if (queryParam != NULL)
            {
                req.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);
            }

            jmethodID jm_getTicker = env->GetMethodID(queryParamClass, "getTicker", "()Ljava/lang/String;");
            assert(jm_getTicker != NULL);
            jstring strTicker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
            const char *char_ticker = NULL;
            if (strTicker != NULL)
            {
                char_ticker = env->GetStringUTFChars(strTicker, 0);
                strcpy(req.ticker, char_ticker);
            }
        }
    }else{
        return ptrade->QueryOptionAuctionInfo(req, sessionId, requestId, true);
    }



      return ptrade->QueryOptionAuctionInfo(req, sessionId, requestId, false);
}