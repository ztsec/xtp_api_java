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
#include <string>

#include <stdlib.h>
#include <stdio.h>


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
    env->ReleaseStringUTFChars(logFolder, char_xtp_data_folder);
    env->ReleaseStringUTFChars(logSubFolder, char_logSubFolder);
    env->ReleaseStringUTFChars(jniLogLevel, char_jniLogLevel);
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

        jclass XTPFundQueryRspClass =  env->FindClass("com/zts/xtp/trade/model/response/XTPFundQueryRsp");
        ptrade->setXTPFundQueryRspClass((jclass)env->NewGlobalRef(XTPFundQueryRspClass));

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

        jclass xtpStrategyInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPStrategyInfoStruct");
        ptrade->setXTPStrategyInfoClass((jclass)env->NewGlobalRef(xtpStrategyInfoClass));

        jclass xtpStrategyStateReportClass = env->FindClass("com/zts/xtp/trade/model/response/XTPStrategyStateReportStruct");
        ptrade->setXTPStrategyStateReportClass((jclass)env->NewGlobalRef(xtpStrategyStateReportClass));

        jclass crdFundInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdFundInfo");
        ptrade->setXtpQueryCreditFundInfoRspClass((jclass)env->NewGlobalRef(crdFundInfoClass));

        jclass crdDebtInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdDebtInfo");
        ptrade->setCrdDebtInfoClass((jclass)env->NewGlobalRef(crdDebtInfoClass));

        jclass crdCashRepayRspClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdCashRepayRsp");
        ptrade->setXtpCrdCashRepayRspClass((jclass)env->NewGlobalRef(crdCashRepayRspClass));

        jclass crdCashRepayDebtInterestFeeRspClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdCashRepayDebtInterestFeeRsp");
        ptrade->setCrdCashRepayDebtInterestFeeRspClass((jclass)env->NewGlobalRef(crdCashRepayDebtInterestFeeRspClass));

        jclass crdCashRepayInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdCashRepayInfo");
        ptrade->setCrdCashRepayInfoClass((jclass)env->NewGlobalRef(crdCashRepayInfoClass));

        jclass crdDebtStockInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdDebtStockInfo");
        ptrade->setCrdDebtStockInfoClass((jclass)env->NewGlobalRef(crdDebtStockInfoClass));

        jclass crdPositionStockInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPClientQueryCrdPositionStkInfo");
        ptrade->setCrdPositionStockInfoClass((jclass)env->NewGlobalRef(crdPositionStockInfoClass));

        jclass crdSurplusStockInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPClientQueryCrdSurplusStkRspInfo");
        ptrade->setCrdSurplusStockInfoClass((jclass)env->NewGlobalRef(crdSurplusStockInfoClass));

        jclass crdDebtExtendNoticeClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCreditDebtExtendNotice");
        ptrade->setCrdDebtExtendNoticeClass((jclass)env->NewGlobalRef(crdDebtExtendNoticeClass));

        jclass crdFundExtraInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdFundExtraInfo");
        ptrade->setCrdFundExtraInfoClass((jclass)env->NewGlobalRef(crdFundExtraInfoClass));

        jclass crdPositionExtraInfoClass = env->FindClass("com/zts/xtp/trade/model/response/XTPCrdPositionExtraInfo");
        ptrade->setCrdPositionExtraInfoClass((jclass)env->NewGlobalRef(crdPositionExtraInfoClass));

        ptrade->setTradePluginJobject(env->NewGlobalRef(obj));

        jclass logLevelClass = env->FindClass("com/zts/xtp/common/enums/XtpLogLevel");
        jmethodID getLogLevelValue = env->GetMethodID(logLevelClass, "getValue", "()I");
        XTP_LOG_LEVEL logLevel = (XTP_LOG_LEVEL)env->CallIntMethod(jLogLevel, getLogLevelValue);

        jclass resumeTypeClass = env->FindClass("com/zts/xtp/common/enums/XtpTeResumeType");
        jmethodID resumeTypeMethod = env->GetMethodID(resumeTypeClass, "getType", "()I");
        XTP_TE_RESUME_TYPE resumeType = (XTP_TE_RESUME_TYPE)env->CallIntMethod(resumeTypeObj, resumeTypeMethod);

        ptrade->Init(logLevel,resumeType);

        ptrade_=ptrade;

        env->ReleaseStringUTFChars(key, char_serverKey);
        env->ReleaseStringUTFChars(logFolder, char_xtp_data_folder);

    } else
    {
        ptrade=ptrade_;
    }

    setHandle(env, obj, ptrade);
}

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_subscribePublicTopic (JNIEnv *env, jobject obj, jobject resumeTypeObj)
{
    Trade *ptrade = getHandle<Trade>(env, obj);
    if (ptrade) {
        jclass resumeTypeClass = env->FindClass("com/zts/xtp/common/enums/XtpTeResumeType");
        jmethodID resumeTypeMethod = env->GetMethodID(resumeTypeClass, "getType", "()I");
        XTP_TE_RESUME_TYPE resumeType = (XTP_TE_RESUME_TYPE)env->CallIntMethod(resumeTypeObj, resumeTypeMethod);

        ptrade->SubscribePublicTopic(resumeType);
    }else{
        LOG(ERROR) << "trade subscribePublicTopic failed! It mast be call before trade login and after trade init!";
    }

    return;
}


JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_setHeartBeatInterval (JNIEnv *env, jobject obj, jint interval)
{
    Trade *ptrade = getHandle<Trade>(env, obj);
    if (ptrade) {
        ptrade->SetHeartBeatInterval(interval);
    }else{
        LOG(ERROR) << "trade setHeartBeatInterval failed! It mast be call before trade login and after trade init!";
    }
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
    (JNIEnv *env, jobject obj, jstring ip, jint port, jstring userName, jstring password, jobject jProtocol, jstring localIp)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    const char *char_ip = env->GetStringUTFChars(ip, 0);
    std::string strIp(char_ip);

    const char *char_userName = env->GetStringUTFChars(userName, 0);
    std::string strUsername(char_userName);
    const char *char_password = env->GetStringUTFChars(password, 0);
    std::string strPassword(char_password);

    const char *char_localIp = env->GetStringUTFChars(localIp, 0);
    std::string strLocalIp(char_localIp);

    jclass protocolClass = env->FindClass("com/zts/xtp/common/enums/TransferProtocol");
    jmethodID getProtoColValue = env->GetMethodID(protocolClass, "getValue", "()I");
    XTP_PROTOCOL_TYPE protocol = (XTP_PROTOCOL_TYPE)env->CallIntMethod(jProtocol, getProtoColValue);
    uint64_t sessionId = ptrade->Login(strIp, port, strUsername, strPassword, protocol, strLocalIp);

    env->ReleaseStringUTFChars(ip, char_ip);
    env->ReleaseStringUTFChars(userName, char_userName);
    env->ReleaseStringUTFChars(password, char_password);
    env->ReleaseStringUTFChars(localIp, char_localIp);

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

jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_insertOrder(JNIEnv *env, jobject obj, jint orderXtpId, jint orderClientId, jint tickerL, jint tickerN, jint marketType, jdouble price, jdouble stopPrice, jlong quantity, jint priceType, jint sideType, jint positionEffectType, jint businessType, jlong strSessionIdH, jlong strSessionIdE, jlong tid)
{
    //build the order
    XTPOrderInsertInfo orderInfo;

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    orderInfo.order_xtp_id = orderXtpId;

    orderInfo.order_client_id = orderClientId;

    std::string ticker;
    int tickerNL = 0;
    int tickerNT = tickerN;

    while(tickerNT)
    {
        tickerNT/=10;
        tickerNL++;
    }

    for (int i = 0; i < tickerL-tickerNL; i++) {
        ticker.push_back('0');
    }
    ticker.append(std::to_string(tickerN));

    LOG(INFO) << " insertOrder ticker: " << ticker;
    strcpy(orderInfo.ticker, ticker.data());

    orderInfo.market = (XTP_MARKET_TYPE)(marketType);

    orderInfo.price = price;

    orderInfo.stop_price = stopPrice;

    orderInfo.quantity = quantity;

    orderInfo.price_type = (XTP_PRICE_TYPE)(priceType);

    orderInfo.side = (XTP_SIDE_TYPE)(sideType);

    orderInfo.position_effect = (XTP_POSITION_EFFECT_TYPE)(positionEffectType);

    orderInfo.business_type = (XTP_BUSINESS_TYPE)(businessType);

    uint64_t sessionId = strSessionIdH*10 + strSessionIdE;

    uint64_t orderId = ptrade->InsertOrder(orderInfo, sessionId);

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

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryOrdersByPage(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    //build the query parameter
    XTPQueryOrderByPageReq orderQueryParam;
    memset(&orderQueryParam,0,sizeof(struct XTPQueryOrderByPageReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getReqCount = env->GetMethodID(queryParamClass, "getReqCount", "()J");
    orderQueryParam.req_count =  env->CallLongMethod(queryParam, jm_getReqCount);

    jmethodID jm_getReference = env->GetMethodID(queryParamClass, "getReference", "()J");
    orderQueryParam.reference =  env->CallLongMethod(queryParam, jm_getReference);

    jmethodID jm_getReserved = env->GetMethodID(queryParamClass, "getReserved", "()J");
    orderQueryParam.reserved =  env->CallLongMethod(queryParam, jm_getReserved);

    ptrade->QueryOrdersByPage(orderQueryParam, sessionId, requestId);

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
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

JNIEXPORT void JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryTradesByPage(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    //build the query parameter
    XTPQueryTraderByPageReq traderQueryParam;
    memset(&traderQueryParam,0,sizeof(struct XTPQueryTraderByPageReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getReqCount = env->GetMethodID(queryParamClass, "getReqCount", "()J");
    traderQueryParam.req_count =  env->CallLongMethod(queryParam, jm_getReqCount);

    jmethodID jm_getReference = env->GetMethodID(queryParamClass, "getReference", "()J");
    traderQueryParam.reference =  env->CallLongMethod(queryParam, jm_getReference);

    jmethodID jm_getReserved = env->GetMethodID(queryParamClass, "getReserved", "()J");
    traderQueryParam.reserved =  env->CallLongMethod(queryParam, jm_getReserved);

    ptrade->QueryTradesByPage(traderQueryParam, sessionId, requestId);

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
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

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryOtherServerFund(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    XTPFundQueryReq req;
    memset(&req,0,sizeof(struct XTPFundQueryReq));

    jclass queryParamClass = env->GetObjectClass(queryParam);
    assert(queryParamClass != NULL);

    jmethodID jm_getFundAccount = env->GetMethodID(queryParamClass, "getFundAccount", "()Ljava/lang/String;");
    assert(jm_getFundAccount != NULL);
    jstring strFundAccount = (jstring)env->CallObjectMethod(queryParam, jm_getFundAccount);
    const char *char_fund_account = env->GetStringUTFChars(strFundAccount, 0);
    strcpy(req.fund_account, char_fund_account);

    jmethodID jm_getPassword = env->GetMethodID(queryParamClass, "getPassword", "()Ljava/lang/String;");
    assert(jm_getPassword != NULL);
    jstring strPassword = (jstring)env->CallObjectMethod(queryParam, jm_getPassword);
    const char *char_password = env->GetStringUTFChars(strPassword, 0);
    strcpy(req.password, char_password);

    jmethodID jm_getXTPFundQueryType = env->GetMethodID(queryParamClass, "getXTPFundQueryType", "()I");
    req.query_type = (XTP_FUND_QUERY_TYPE)env->CallIntMethod(queryParam, jm_getXTPFundQueryType);

    int result = ptrade->QueryOtherServerFund(req, sessionId, requestId);
    env->ReleaseStringUTFChars(strPassword, char_password);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    env->ReleaseStringUTFChars(strFundAccount, char_fund_account);

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
            int rtn = ptrade->QueryOptionAuctionInfo(req, sessionId, requestId, false);
            env->ReleaseStringUTFChars(strTicker, char_ticker);
            return rtn;
        }else{
            return ptrade->QueryOptionAuctionInfo(req, sessionId, requestId, false);
        }
    }else{
        return ptrade->QueryOptionAuctionInfo(req, sessionId, requestId, true);
    }
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_getTradingDay(JNIEnv *env, jobject clazz) {
	// get the trader pointer
	Trade *ptrade = getHandle<Trade>(env, clazz);

	const char *tradingDay = ptrade->GetTradingDay();
	return env->NewStringUTF(tradingDay);
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_getApiVersion(JNIEnv *env, jobject clazz) {
	// get the trader pointer
	Trade *ptrade = getHandle<Trade>(env, clazz);

	const char* version = ptrade->GetApiVersion();
	return env->NewStringUTF(version);
}

JNIEXPORT jshort JNICALL Java_com_zts_xtp_trade_api_TradeApi_getClientIDByXTPID(JNIEnv *env, jobject clazz, jstring orderXTPId) {
	// get the trader pointer
	Trade *ptrade = getHandle<Trade>(env, clazz);

	uint64_t xtpId;
	const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
	std::stringstream(char_xtpOrderId) >> xtpId;

	env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
	return ptrade->GetClientIDByXTPID(xtpId);
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_getAccountByXTPID(JNIEnv *env, jobject clazz, jstring orderXTPId) {
	// get the trader pointer
	Trade *ptrade = getHandle<Trade>(env, clazz);

	uint64_t xtpId;
	const char *char_xtpOrderId = env->GetStringUTFChars(orderXTPId, 0);
	std::stringstream(char_xtpOrderId) >> xtpId;
	env->ReleaseStringUTFChars(orderXTPId, char_xtpOrderId);
	const char* account = ptrade->GetAccountByXTPID(xtpId);
	return env->NewStringUTF(account);
}

JNIEXPORT jboolean JNICALL Java_com_zts_xtp_trade_api_TradeApi_isServerRestart(JNIEnv *env, jobject obj, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->IsServerRestart(sessionId);
}

// 算法总线api
JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_loginALGO(JNIEnv *env, jobject obj, jstring ip, jint port, jstring userName, jstring password, jobject jProtocol, jstring localIp)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    assert(ip != NULL);
    const char *char_ip = env->GetStringUTFChars(ip, 0);
    std::string strIp(char_ip);

    assert(userName != NULL);
    const char *char_userName = env->GetStringUTFChars(userName, 0);
    std::string strUsername(char_userName);

    assert(password != NULL);
    const char *char_password = env->GetStringUTFChars(password, 0);
    std::string strPassword(char_password);

    assert(localIp != NULL);
    const char *char_localIp = env->GetStringUTFChars(localIp, 0);
    std::string strLocalIp(char_localIp);

    jclass protocolClass = env->FindClass("com/zts/xtp/common/enums/TransferProtocol");
    jmethodID getProtoColValue = env->GetMethodID(protocolClass, "getValue", "()I");
    XTP_PROTOCOL_TYPE protocol = (XTP_PROTOCOL_TYPE)env->CallIntMethod(jProtocol, getProtoColValue);
    int result = ptrade->LoginALGO(strIp, port, strUsername, strPassword, protocol, strLocalIp);

    env->ReleaseStringUTFChars(ip, char_ip);
    env->ReleaseStringUTFChars(userName, char_userName);
    env->ReleaseStringUTFChars(password, char_password);
    env->ReleaseStringUTFChars(localIp, char_localIp);

    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_insertAlgoOrder(JNIEnv *env, jobject obj, jint strategyType, jstring clientStrategyId, jstring strategyParam, jstring strSessionId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t clientStratId = 0;
    assert(clientStrategyId != NULL);
    const char *char_clientStrategyId = env->GetStringUTFChars(clientStrategyId, 0);
    std::stringstream(char_clientStrategyId)>> clientStratId;

    assert(strategyParam != NULL);
    const char *char_strategyParam = env->GetStringUTFChars(strategyParam, NULL);
    std::string strStrategyParam(char_strategyParam);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(clientStrategyId, char_clientStrategyId);
    env->ReleaseStringUTFChars(strategyParam, char_strategyParam);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->InsertAlgoOrder(strategyType, clientStratId, (char *)strStrategyParam.c_str(), sessionId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_cancelAlgoOrder(JNIEnv *env, jobject obj, jboolean cancelFlag, jstring xtpStrategyId, jstring strSessionId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t xtp_strategy_id = 0;
    assert(xtpStrategyId != NULL);
    const char *char_xtpStrategyId = env->GetStringUTFChars(xtpStrategyId, 0);
    std::stringstream(char_xtpStrategyId)>> xtp_strategy_id;

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(xtpStrategyId, char_xtpStrategyId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->CancelAlgoOrder(cancelFlag, xtp_strategy_id, sessionId);
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_getAlgorithmIDByOrder(JNIEnv *env, jobject obj, jstring orderXtpId, jint orderClientId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t order_xtp_id = 0;
    assert(orderXtpId != NULL);
    const char *char_orderXtpId = env->GetStringUTFChars(orderXtpId, 0);
    std::stringstream(char_orderXtpId)>> order_xtp_id;

    env->ReleaseStringUTFChars(orderXtpId, char_orderXtpId);

    uint64_t xtpStrategyId = ptrade->GetAlgorithmIDByOrder(order_xtp_id, orderClientId);
    jstring strXtpStrategyId = env->NewStringUTF((std::to_string(xtpStrategyId)).c_str());
    return strXtpStrategyId;
}

jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryStrategy(JNIEnv *env, jobject obj, jint strategyType, jstring clientStrategyId, jstring xtpStrategyId, jstring strSessionId, jint requestId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t client_strategy_id = 0;
    assert(clientStrategyId != NULL);
    const char *char_clientStrategyId = env->GetStringUTFChars(clientStrategyId, 0);
    std::stringstream(char_clientStrategyId)>> client_strategy_id;

    uint64_t xtp_strategy_id = 0;
    assert(xtpStrategyId != NULL);
    const char *char_xtpStrategyId = env->GetStringUTFChars(xtpStrategyId, 0);
    std::stringstream(char_xtpStrategyId)>> xtp_strategy_id;

    uint64_t sessionId = 0;
    assert(strSessionId != NULL);
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(clientStrategyId, char_clientStrategyId);
    env->ReleaseStringUTFChars(xtpStrategyId, char_xtpStrategyId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryStrategy(strategyType,client_strategy_id,xtp_strategy_id, sessionId,requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_algoUserEstablishChannel(JNIEnv *env, jobject obj, jstring omsIp, jint omsPort, jstring userName, jstring password, jstring strSessionId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    const char *char_ip = env->GetStringUTFChars(omsIp, 0);
    std::string strIp(char_ip);

    const char *char_userName = env->GetStringUTFChars(userName, 0);
    std::string strUsername(char_userName);

    const char *char_password = env->GetStringUTFChars(password, 0);
    std::string strPassword(char_password);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    int result = ptrade->algoUserEstablishChannel(strIp, omsPort, strUsername, strPassword, sessionId);

    env->ReleaseStringUTFChars(omsIp, char_ip);
    env->ReleaseStringUTFChars(userName, char_userName);
    env->ReleaseStringUTFChars(password, char_password);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_modifyUserTerminalInfo(JNIEnv *env, jobject obj, jobject userTerminalInfo, jstring strSessionId)
{
    Trade *ptrade = getHandle<Trade>(env, obj);

    XTPUserTerminalInfoReq req;
    memset(&req,0,sizeof(struct XTPUserTerminalInfoReq));
    jclass paramClass = env->GetObjectClass(userTerminalInfo);
    assert(paramClass != NULL);

    jmethodID jm_getLocalIp = env->GetMethodID(paramClass, "getLocalIp", "()Ljava/lang/String;");
    assert(jm_getLocalIp != NULL);
    jstring strLocalIp = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getLocalIp);
    const char *local_ip = NULL;
    if (strLocalIp != NULL)
    {
        local_ip = env->GetStringUTFChars(strLocalIp, 0);
        strcpy(req.local_ip, local_ip);
    }

    jmethodID jm_getMacAddr = env->GetMethodID(paramClass, "getMacAddr", "()Ljava/lang/String;");
    assert(jm_getMacAddr != NULL);
    jstring strMacAddr = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getMacAddr);
    const char *mac_addr = NULL;
    if (strMacAddr != NULL)
    {
        mac_addr = env->GetStringUTFChars(strMacAddr, 0);
        strcpy(req.mac_addr, mac_addr);
    }

    jmethodID jm_getHd = env->GetMethodID(paramClass, "getHd", "()Ljava/lang/String;");
    assert(jm_getHd != NULL);
    jstring strHd = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getHd);
    const char *hd = NULL;
    if (strHd != NULL)
    {
        hd = env->GetStringUTFChars(strHd, 0);
        strcpy(req.hd, hd);
    }

    jmethodID jm_getTermType = env->GetMethodID(paramClass, "getTermType", "()I");
    req.term_type = (XTPTerminalType)env->CallIntMethod(userTerminalInfo, jm_getTermType);

    jmethodID jm_getInternetIp = env->GetMethodID(paramClass, "getInternetIp", "()Ljava/lang/String;");
    assert(jm_getInternetIp != NULL);
    jstring strInternetIp = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getInternetIp);
    const char *internet_ip = NULL;
    if (strInternetIp != NULL)
    {
        internet_ip = env->GetStringUTFChars(strInternetIp, 0);
        strcpy(req.internet_ip, internet_ip);
    }

    int32_t internetPort = 0;
    jmethodID jm_getInternetPort = env->GetMethodID(paramClass, "getInternetPort", "()I");
    internetPort = env->CallIntMethod(userTerminalInfo, jm_getInternetPort);
    req.internet_port = internetPort;

    jmethodID jm_getClientVersion = env->GetMethodID(paramClass, "getClientVersion", "()Ljava/lang/String;");
    assert(jm_getClientVersion != NULL);
    jstring strClientVersion = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getClientVersion);
    const char *client_version = NULL;
    if (strClientVersion != NULL)
    {
        client_version = env->GetStringUTFChars(strClientVersion, 0);
        strcpy(req.client_version, client_version);
    }

    jmethodID jm_getMacosSno = env->GetMethodID(paramClass, "getMacosSno", "()Ljava/lang/String;");
    assert(jm_getMacosSno != NULL);
    jstring strMacosSno = (jstring)env->CallObjectMethod(userTerminalInfo, jm_getMacosSno);
    const char *macos_sno = NULL;
    if (strMacosSno != NULL)
    {
        macos_sno = env->GetStringUTFChars(strMacosSno, 0);
        strcpy(req.macos_sno, macos_sno);
    }

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    int result = ptrade->ModifyUserTerminalInfo(req, sessionId);

    if (strLocalIp != NULL)
    {
        env->ReleaseStringUTFChars(strLocalIp, local_ip);
    }

    if (strMacAddr != NULL)
    {
        env->ReleaseStringUTFChars(strMacAddr, mac_addr);
    }

    if (strHd != NULL)
    {
        env->ReleaseStringUTFChars(strHd, hd);
    }

    if (strInternetIp != NULL)
    {
        env->ReleaseStringUTFChars(strInternetIp, internet_ip);
    }

    if (strClientVersion != NULL)
    {
        env->ReleaseStringUTFChars(strClientVersion, client_version);
    }

    if (strMacosSno != NULL)
    {
        env->ReleaseStringUTFChars(strMacosSno, macos_sno);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return result;
}

// 两融业务api
JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_creditCashRepay(JNIEnv *env, jobject obj, jdouble amount, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t orderId = ptrade->CreditCashRepay(amount,sessionId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    jstring strOrderId = env->NewStringUTF((std::to_string(orderId)).c_str());
    return strOrderId;
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_creditCashRepayDebtInterestFee(JNIEnv *env, jobject obj, jstring debtId, jdouble amount, jstring strSessionId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    const char *char_debtId = env->GetStringUTFChars(debtId, 0);
    std::string strDebtId(char_debtId);

    uint64_t orderId = ptrade->CreditCashRepayDebtInterestFee(strDebtId,amount,sessionId);

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    env->ReleaseStringUTFChars(debtId, char_debtId);

    jstring strOrderId = env->NewStringUTF((std::to_string(orderId)).c_str());
    return strOrderId;
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_creditSellStockRepayDebtInterestFee(JNIEnv *env, jobject obj, jobject tradeOrder, jstring debtId, jstring strSessionId)
{
    //build the order
    XTPOrderInsertInfo orderInfo;
    memset(&orderInfo,0,sizeof(XTPOrderInsertInfo));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    const char *char_debtId = env->GetStringUTFChars(debtId, 0);
    std::string strDebtId(char_debtId);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass tradeOrderClass = env->GetObjectClass(tradeOrder);
    assert(tradeOrderClass != NULL);

    jmethodID jm_getOrderXtpId = env->GetMethodID(tradeOrderClass, "getOrderXtpId", "()Ljava/lang/String;");
    assert(jm_getOrderXtpId != NULL);
    jstring jstr_xtpOrderId = (jstring)env->CallObjectMethod(tradeOrder, jm_getOrderXtpId);
    const char *char_xtpOrderId = NULL;
    if (jstr_xtpOrderId != NULL)
    {
        char_xtpOrderId = env->GetStringUTFChars(jstr_xtpOrderId, 0);
        std::stringstream(char_xtpOrderId)>> orderInfo.order_xtp_id;
    }

    jmethodID jm_getOrderClientId = env->GetMethodID(tradeOrderClass, "getOrderClientId", "()I");
    orderInfo.order_client_id = env->CallLongMethod(tradeOrder, jm_getOrderClientId);

    jmethodID jm_getTicker = env->GetMethodID(tradeOrderClass, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    jstring order_ticker = (jstring)env->CallObjectMethod(tradeOrder, jm_getTicker);
    const char *char_ticker = NULL;
    if (order_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(order_ticker, 0);
        strcpy(orderInfo.ticker, char_ticker);
    }

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

    uint64_t orderId = ptrade->CreditSellStockRepayDebtInterestFee(orderInfo, strDebtId, sessionId);

    if (jstr_xtpOrderId != NULL)
    {
        env->ReleaseStringUTFChars(jstr_xtpOrderId, char_xtpOrderId);
    }

    if (order_ticker != NULL)
    {
        env->ReleaseStringUTFChars(order_ticker, char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    env->ReleaseStringUTFChars(debtId, char_debtId);

    jstring strOrderId = env->NewStringUTF((std::to_string(orderId)).c_str());
    return strOrderId;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditCashRepayInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditCashRepayInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditFundInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditFundInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditDebtInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    LOG(INFO) << "trade adapter queryCreditDebtInfo";
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditDebtInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditTickerDebtInfo(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //build the order
    XTPClientQueryCrdDebtStockReq queryCrdDebtStockInfo;
    memset(&queryCrdDebtStockInfo,0,sizeof(XTPClientQueryCrdDebtStockReq));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass queryCrdDebtStockReq = env->GetObjectClass(queryParam);
    assert(queryCrdDebtStockReq != NULL);

    jmethodID jm_getTicker = env->GetMethodID(queryCrdDebtStockReq, "getTicker", "()Ljava/lang/String;");
    assert(jm_getTicker != NULL);
    jstring query_ticker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (query_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(query_ticker, 0);
        strcpy(queryCrdDebtStockInfo.ticker, char_ticker);
    }

    jmethodID jm_getMarketType = env->GetMethodID(queryCrdDebtStockReq, "getMarketType", "()I");
    queryCrdDebtStockInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    if (query_ticker != NULL)
    {
        env->ReleaseStringUTFChars(query_ticker,char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditTickerDebtInfo(queryCrdDebtStockInfo, sessionId,requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditAssetDebtInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditAssetDebtInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditTickerAssignInfo(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //build the order
    XTPClientQueryCrdPositionStockReq queryCrdPositionStockInfo;
    memset(&queryCrdPositionStockInfo,0,sizeof(XTPClientQueryCrdPositionStockReq));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass queryCrdPositionStockReq = env->GetObjectClass(queryParam);
    assert(queryCrdPositionStockReq != NULL);

    jmethodID jm_getTicker = env->GetMethodID(queryCrdPositionStockReq, "getTicker", "()Ljava/lang/String;");
    jstring query_ticker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (query_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(query_ticker, 0);
        strcpy(queryCrdPositionStockInfo.ticker, char_ticker);
    }

    jmethodID jm_getMarketType = env->GetMethodID(queryCrdPositionStockReq, "getMarketType", "()I");
    queryCrdPositionStockInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    if (query_ticker != NULL)
    {
        env->ReleaseStringUTFChars(query_ticker,char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditTickerAssignInfo(queryCrdPositionStockInfo, sessionId,requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditExcessStock(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //build the order
    XTPClientQueryCrdSurplusStkReqInfo queryCrdSurplusStockInfo;
    memset(&queryCrdSurplusStockInfo,0,sizeof(XTPClientQueryCrdSurplusStkReqInfo));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass queryCrdSurplusStockReq = env->GetObjectClass(queryParam);
    assert(queryCrdSurplusStockReq != NULL);


    jmethodID jm_getTicker = env->GetMethodID(queryCrdSurplusStockReq, "getTicker", "()Ljava/lang/String;");
    jstring query_ticker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (query_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(query_ticker, 0);
        strcpy(queryCrdSurplusStockInfo.ticker, char_ticker);
    }

    jmethodID jm_getMarketType = env->GetMethodID(queryCrdSurplusStockReq, "getMarketType", "()I");
    queryCrdSurplusStockInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    if (query_ticker != NULL)
    {
        env->ReleaseStringUTFChars(query_ticker,char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditExcessStock(queryCrdSurplusStockInfo, sessionId,requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryMulCreditExcessStock(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //build the order
    XTPClientQueryCrdSurplusStkReqInfo queryCrdSurplusStockInfo;
    memset(&queryCrdSurplusStockInfo,0,sizeof(XTPClientQueryCrdSurplusStkReqInfo));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass queryCrdSurplusStockReq = env->GetObjectClass(queryParam);
    assert(queryCrdSurplusStockReq != NULL);

    jmethodID jm_getTicker = env->GetMethodID(queryCrdSurplusStockReq, "getTicker", "()Ljava/lang/String;");
    jstring query_ticker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (query_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(query_ticker, 0);
        strcpy(queryCrdSurplusStockInfo.ticker, char_ticker);
    }

    jmethodID jm_getMarketType = env->GetMethodID(queryCrdSurplusStockReq, "getMarketType", "()I");
    queryCrdSurplusStockInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    if (query_ticker != NULL)
    {
        env->ReleaseStringUTFChars(query_ticker,char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    int32_t res = ptrade->QueryMulCreditExcessStock(queryCrdSurplusStockInfo, sessionId,requestId);
    return res;
}

JNIEXPORT jstring JNICALL Java_com_zts_xtp_trade_api_TradeApi_creditExtendDebtDate(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId)
{
    //build the order
    XTPCreditDebtExtendReq xtpCreditDebtExtendReq;
    memset(&xtpCreditDebtExtendReq,0,sizeof(XTPCreditDebtExtendReq));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass creditDebtExtendReq = env->GetObjectClass(queryParam);
    assert(creditDebtExtendReq != NULL);

    jmethodID jm_getOrderXtpId = env->GetMethodID(creditDebtExtendReq, "getOrderXtpId", "()Ljava/lang/String;");
    assert(jm_getOrderXtpId != NULL);
    jstring jstr_xtpOrderId = (jstring)env->CallObjectMethod(queryParam, jm_getOrderXtpId);
    const char *char_xtpOrderId = NULL;
    if (jstr_xtpOrderId != NULL)
    {
        char_xtpOrderId = env->GetStringUTFChars(jstr_xtpOrderId, 0);
        std::stringstream(char_xtpOrderId)>> xtpCreditDebtExtendReq.xtpid;
    }

    jmethodID jm_getDebtId = env->GetMethodID(creditDebtExtendReq, "getDebtId", "()Ljava/lang/String;");
    jstring debtId = (jstring)env->CallObjectMethod(queryParam, jm_getDebtId);
    const char *debt_id = NULL;
    if (debtId != NULL)
    {
        debt_id = env->GetStringUTFChars(debtId, 0);
        strcpy(xtpCreditDebtExtendReq.debt_id, debt_id);
    }

    jmethodID jm_getDeferDays = env->GetMethodID(creditDebtExtendReq, "getDeferDays", "()I");
    xtpCreditDebtExtendReq.defer_days = env->CallLongMethod(queryParam, jm_getDeferDays);

    jmethodID jm_getFundAccount = env->GetMethodID(creditDebtExtendReq, "getFundAccount", "()Ljava/lang/String;");
    jstring fundAccount = (jstring)env->CallObjectMethod(queryParam, jm_getFundAccount);
    const char *fund_account = NULL;
    if (fundAccount != NULL)
    {
        fund_account = env->GetStringUTFChars(fundAccount, 0);
        strcpy(xtpCreditDebtExtendReq.fund_account, fund_account);
    }

    jmethodID jm_getPassword = env->GetMethodID(creditDebtExtendReq, "getPassword", "()Ljava/lang/String;");
    jstring jstr_password = (jstring)env->CallObjectMethod(queryParam, jm_getPassword);
    const char *password = NULL;
    if (jstr_password != NULL)
    {
        password = env->GetStringUTFChars(jstr_password, 0);
        strcpy(xtpCreditDebtExtendReq.password, password);
    }

    uint64_t orderId = ptrade->CreditExtendDebtDate(xtpCreditDebtExtendReq, sessionId);
    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    if (jstr_xtpOrderId != NULL)
    {
        env->ReleaseStringUTFChars(jstr_xtpOrderId,char_xtpOrderId);
    }

    if (debtId != NULL)
    {
        env->ReleaseStringUTFChars(debtId,debt_id);
    }

    if (fundAccount != NULL)
    {
        env->ReleaseStringUTFChars(fundAccount,fund_account);
    }

    if (jstr_password != NULL)
    {
        env->ReleaseStringUTFChars(jstr_password,password);
    }

    jstring strOrderId = env->NewStringUTF((std::to_string(orderId)).c_str());
    return strOrderId;
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditExtendDebtDateOrders(JNIEnv *env, jobject obj, jstring orderXTPId, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    uint64_t xtpId = 0;
    const char *char_xtpId = env->GetStringUTFChars(orderXTPId, 0);
    std::stringstream(char_xtpId)>> xtpId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);
    env->ReleaseStringUTFChars(orderXTPId,char_xtpId);

    return  ptrade->QueryCreditExtendDebtDateOrders(xtpId, sessionId,requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditFundExtraInfo(JNIEnv *env, jobject obj, jstring strSessionId, jint requestId)
{
    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return ptrade->QueryCreditFundExtraInfo(sessionId, requestId);
}

JNIEXPORT jint JNICALL Java_com_zts_xtp_trade_api_TradeApi_queryCreditPositionExtraInfo(JNIEnv *env, jobject obj, jobject queryParam, jstring strSessionId, jint requestId)
{
    //build the order
    XTPClientQueryCrdPositionStockReq queryCrdPositionStockInfo;
    memset(&queryCrdPositionStockInfo,0,sizeof(XTPClientQueryCrdPositionStockReq));

    //get the trader pointer
    Trade *ptrade = getHandle<Trade>(env, obj);

    uint64_t sessionId = 0;
    const char *char_sessionId = env->GetStringUTFChars(strSessionId, 0);
    std::stringstream(char_sessionId)>> sessionId;

    jclass queryCrdPositionStockReq = env->GetObjectClass(queryParam);
    assert(queryCrdPositionStockReq != NULL);


    jmethodID jm_getTicker = env->GetMethodID(queryCrdPositionStockReq, "getTicker", "()Ljava/lang/String;");
    jstring query_ticker = (jstring)env->CallObjectMethod(queryParam, jm_getTicker);
    const char *char_ticker = NULL;
    if (query_ticker != NULL)
    {
        char_ticker = env->GetStringUTFChars(query_ticker, 0);
        strcpy(queryCrdPositionStockInfo.ticker, char_ticker);
    }

    jmethodID jm_getMarketType = env->GetMethodID(queryCrdPositionStockReq, "getMarketType", "()I");
    queryCrdPositionStockInfo.market = (XTP_MARKET_TYPE)env->CallIntMethod(queryParam, jm_getMarketType);

    if (query_ticker != NULL)
    {
        env->ReleaseStringUTFChars(query_ticker,char_ticker);
    }

    env->ReleaseStringUTFChars(strSessionId, char_sessionId);

    return  ptrade->QueryCreditPositionExtraInfo(queryCrdPositionStockInfo, sessionId,requestId);
}
