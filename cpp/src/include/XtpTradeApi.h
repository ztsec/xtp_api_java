#include "xtp/xtp_trader_api.h"
#include "xtp/xoms_api_struct.h"
#include <jni.h>
#include <assert.h>

#include <string>
#include <iostream>
#include "glog/logging.h"
#include <map>
#include <thread>

class Trade : public XTP::API::TraderSpi {
private:
    XTP::API::TraderApi* api_;
 //   Trader* spi_;

private:
    uint16_t client_id_;
    std::string key_;
    std::string file_path_;
    //keep the jni VM pointer
    JavaVM* jvm_;
    //the jvm attr
    JavaVMAttachArgs att_args_;
    //keep the referenced the trader plugin jobject
    jobject trade_plugin_obj_;
    jclass trade_order_info_class_;
    jclass query_asset_rsp_class_;
    jclass query_stkposition_rsp_class_;
    jclass trade_report_class_;
    jclass xtp_error_msg_class_;

    jclass structured_fund_info_class_;
    jclass fund_transfer_notice_class_;
    jclass fund_query_rsp_class_;
    jclass query_etf_base_rsp_class_;
    jclass query_etf_component_rsp_class_;
    jclass query_ipo_ticker_rsp_class_;
    jclass query_ipo_quota_rsp_class_;
    jclass query_option_auctionInfo_rsp_class_;
    jclass xtp_strategy_info_class_;
    jclass xtp_strategy_state_report_class_;

    jclass query_credit_fund_rsp_class_;
    jclass crd_debt_info_class_;
    jclass crd_cash_repay_rsp_class_;
    jclass crd_cash_repay_debt_interest_fee_rsp_class_;
    jclass crd_cash_repay_info_class_;
    jclass crd_debt_stock_info_class_;
    jclass crd_position_stock_info_class_;
    jclass crd_surplus_stock_info_class_;
    jclass crd_debt_extend_notice_class_;
    jclass crd_fund_extra_info_class_;
    jclass crd_position_extra_info_class_;

    //使用map存储session所在线程和env的对应关系
    typedef struct EnvCatchStruct
    {
        //缓存env对象
        JNIEnv*	  env;
        //缓存tradePluginClass对象
        jclass	  tradePluginClass;
        //缓存成交回报方法jm_eventTrade对象
        jmethodID jm_eventTrade;
        //缓存委托回报方法jm_eventOrder对象
        jmethodID jm_eventOrder;
        //缓存撤单失败方法jm_eventCancelOrder对象
        jmethodID jm_eventCancelOrderError;
        //缓存查询资产方法jm_eventAsset对象
        jmethodID jm_eventAsset;
        //缓存查询持仓方法jm_eventPosition对象
        jmethodID jm_eventPosition;
    } EnvCatchStruct;
    std::map<std::thread::id,EnvCatchStruct*> envCatchMap;

public:

    void OnDisconnected(uint64_t session_id, int reason);

    void OnError(XTPRI *error_info);

    void OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id);
    void OnOrderEvent2(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id, JNIEnv* env = NULL, jmethodID jm_event = NULL);

    void OnCancelOrderError(XTPOrderCancelInfo *cancel_info,XTPRI *error_info, uint64_t session_id);
    void OnCancelOrderError2(XTPOrderCancelInfo *cancel_info,XTPRI *error_info, uint64_t session_id, JNIEnv* env = NULL, jmethodID jm_event = NULL);

    void OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id);
    void OnTradeEvent2(XTPTradeReport *trade_info, uint64_t session_id, JNIEnv* env = NULL, jmethodID jm_event = NULL);

    void OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) ;

    void OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) ;

    void OnQueryOrderByPage(XTPQueryOrderRsp *order_info, int64_t req_count, int64_t order_sequence, int64_t query_reference, int request_id, bool is_last, uint64_t session_id);

    void OnQueryTradeByPage(XTPQueryTradeRsp *trade_info, int64_t req_count, int64_t trade_sequence, int64_t query_reference, int request_id, bool is_last, uint64_t session_id);

    void OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);
    void OnQueryPosition2(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id, JNIEnv* env = NULL, jmethodID jm_event = NULL);

    void OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);
    void OnQueryAsset2(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id, JNIEnv* env = NULL, jmethodID jm_event = NULL);

    void OnQueryStructuredFund(XTPStructuredFundInfo *fund_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryOtherServerFund(XTPFundQueryRsp *fund_info, XTPRI *error_info, int request_id, uint64_t session_id);

    void OnFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, uint64_t session_id);

    void OnQueryETF(XTPQueryETFBaseRsp *etf_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryETFBasket(XTPQueryETFComponentRsp *etf_component_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryIPOInfoList(XTPQueryIPOTickerRsp *ipo_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryIPOQuotaInfo(XTPQueryIPOQuotaRsp *quota_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryOptionAuctionInfo(XTPQueryOptionAuctionInfoRsp *option_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnInsertAlgoOrder(XTPStrategyInfoStruct* strategy_info, XTPRI *error_info, uint64_t session_id);

    void OnCancelAlgoOrder(XTPStrategyInfoStruct* strategy_info, XTPRI *error_info, uint64_t session_id);

    void OnQueryStrategy(XTPStrategyInfoStruct *strategy_info, char* strategy_param, XTPRI *error_info, int32_t request_id, bool is_last, uint64_t session_id);

    void OnStrategyStateReport(XTPStrategyStateReportStruct* strategy_state, uint64_t session_id);

    void OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id);

    void OnAlgoDisconnected(int reason);

    void OnAlgoConnected();

    // 新增融资融券业务spi接口
    void OnCreditCashRepay(XTPCrdCashRepayRsp *cash_repay_info, XTPRI *error_info, uint64_t session_id);

    void OnCreditCashRepayDebtInterestFee(XTPCrdCashRepayDebtInterestFeeRsp *cash_repay_info, XTPRI *error_info, uint64_t session_id);

    void OnQueryCreditCashRepayInfo(XTPCrdCashRepayInfo *cash_repay_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryCreditFundInfo(XTPCrdFundInfo *fund_info, XTPRI *error_info, int request_id, uint64_t session_id);

    void OnQueryCreditDebtInfo(XTPCrdDebtInfo *debt_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryCreditTickerDebtInfo(XTPCrdDebtStockInfo *debt_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryCreditAssetDebtInfo(double remain_amount, XTPRI *error_info, int request_id, uint64_t session_id);

    void OnQueryCreditTickerAssignInfo(XTPClientQueryCrdPositionStkInfo *assign_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo *stock_info, XTPRI *error_info, int request_id, uint64_t session_id);

    void OnQueryMulCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo *stock_info, XTPRI *error_info, int request_id, uint64_t session_id, bool is_last);

    void OnCreditExtendDebtDate(XTPCreditDebtExtendNotice *debt_extend_info, XTPRI *error_info, uint64_t session_id);

    void OnQueryCreditExtendDebtDateOrders(XTPCreditDebtExtendNotice *debt_extend_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryCreditFundExtraInfo(XTPCrdFundExtraInfo *fund_info, XTPRI *error_info, int request_id, uint64_t session_id);

    void OnQueryCreditPositionExtraInfo(XTPCrdPositionExtraInfo *fund_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void setJniJvm(JavaVM *env) { jvm_ = env;}
    void setTradePluginJobject(jobject obj) {trade_plugin_obj_ = obj;}

    void setTradeOrderInfoClass(jclass jc){trade_order_info_class_ = jc;}
    void setQueryAssetRespClass(jclass jc){query_asset_rsp_class_ = jc;}
    void setStkPositionRspClass(jclass jc){query_stkposition_rsp_class_ = jc;}
    void setTradeReportClass(jclass jc){trade_report_class_ = jc;}
    void setXTPErrorMsgClass(jclass jc) {xtp_error_msg_class_ = jc;}

    void setXTPStructuredFundInfoClass(jclass jc) {structured_fund_info_class_ = jc;}
    void setXTPFundTransferNoticeClass(jclass jc) {fund_transfer_notice_class_ = jc;}
    void setXTPFundQueryRspClass(jclass jc) {fund_query_rsp_class_ = jc;}
    void setXTPQueryETFBaseRspClass(jclass jc) {query_etf_base_rsp_class_ = jc;}
    void setXTPQueryETFComponentRspClass(jclass jc) {query_etf_component_rsp_class_ = jc;}
    void setXTPQueryIPOTickerRspClass(jclass jc) {query_ipo_ticker_rsp_class_ = jc;}
    void setXTPQueryIPOQuotaRspClass(jclass jc) {query_ipo_quota_rsp_class_ = jc;}
    void setXtpQueryOptionAuctionInfoRspClass(jclass jc) {query_option_auctionInfo_rsp_class_ = jc;}
    void setXTPStrategyInfoClass(jclass jc) {xtp_strategy_info_class_ = jc;}
    void setXTPStrategyStateReportClass(jclass jc) {xtp_strategy_state_report_class_ = jc;}

    void setXtpQueryCreditFundInfoRspClass(jclass jc) {query_credit_fund_rsp_class_ = jc;}
    void setCrdDebtInfoClass(jclass jc) {crd_debt_info_class_ = jc;}
    void setXtpCrdCashRepayRspClass(jclass jc) {crd_cash_repay_rsp_class_ = jc;}
    void setCrdCashRepayDebtInterestFeeRspClass(jclass jc) {crd_cash_repay_debt_interest_fee_rsp_class_ = jc;}
    void setCrdCashRepayInfoClass(jclass jc) {crd_cash_repay_info_class_ = jc;}
    void setCrdDebtStockInfoClass(jclass jc) {crd_debt_stock_info_class_ = jc;}
    void setCrdPositionStockInfoClass(jclass jc) {crd_position_stock_info_class_ = jc;}
    void setCrdSurplusStockInfoClass(jclass jc) {crd_surplus_stock_info_class_ = jc;}
    void setCrdDebtExtendNoticeClass(jclass jc) {crd_debt_extend_notice_class_ = jc;}
    void setCrdFundExtraInfoClass(jclass jc) {crd_fund_extra_info_class_ = jc;}
    void setCrdPositionExtraInfoClass(jclass jc) {crd_position_extra_info_class_ = jc;}

    void setClientId(uint16_t client_id) { client_id_ = client_id; }
    uint16_t getClientId() { return(client_id_); }
    void setKey(const char* key) { key_ = key; }
    const char* getKey() { return(key_.c_str()); }
    void setFilePath(const char* file_path) { file_path_ = file_path; }
    const char* getFilePath() { return(file_path_.c_str()); }

    Trade();

    ~Trade();

    void Init(XTP_LOG_LEVEL logLevel,XTP_TE_RESUME_TYPE resumeType)
    {
        api_ = XTP::API::TraderApi::CreateTraderApi(client_id_, file_path_.c_str(), logLevel);
        api_->SubscribePublicTopic(resumeType);
        api_->SetSoftwareKey(key_.c_str());
        api_->SetSoftwareVersion("1.1.0");
        api_->RegisterSpi(this);
    }

    void Release()
    {
        if(api_)
            api_->Release();
    }

    void SubscribePublicTopic(XTP_TE_RESUME_TYPE resumeType)
    {
        if(api_!=NULL){
            api_->SubscribePublicTopic(resumeType);
        }
        else{
            LOG(ERROR) << __PRETTY_FUNCTION__<<"subscribePublicTopic mast call after init" ;
        }
    }

    void SetHeartBeatInterval(uint32_t interval){
        if(api_==NULL){
            LOG(ERROR) << __PRETTY_FUNCTION__<<"setHeartBeatInterval mast call after init" ;
        }
        api_->SetHeartBeatInterval(interval);
    }

    uint64_t Login(std::string server_ip, uint16_t server_port, std::string username, std::string password, XTP_PROTOCOL_TYPE protocol, std::string local_ip)
    {
        return api_->Login(server_ip.c_str(), server_port, username.c_str(), password.c_str(), protocol);
    }

    int Logout(uint64_t session_id) {
        return api_->Logout(session_id);
    }

    char* GetApiLastError() {
        return api_->GetApiLastError()->error_msg;
    }

    uint64_t InsertOrder(XTPOrderInsertInfo order, uint64_t session_id)
    {
        //LOG(INFO) << __PRETTY_FUNCTION__ ;
        return api_->InsertOrder(&order, session_id);
    }

    uint64_t CancelOrder(const uint64_t order_xtp_id, uint64_t session_id)
    {

        return api_->CancelOrder(order_xtp_id, session_id);
    }

    int QueryOrderByXTPID(const uint64_t order_xtp_id, uint64_t session_id, int request_id)
    {
       return api_->QueryOrderByXTPID(order_xtp_id, session_id, request_id);
    }

    int QueryOrders(const XTPQueryOrderReq query_param, uint64_t session_id, int request_id)
    {
       return api_->QueryOrders(&query_param, session_id, request_id);
    }

    int QueryTradesByXTPID(const uint64_t order_xtp_id, uint64_t session_id, int request_id)
    {
       return api_->QueryTradesByXTPID(order_xtp_id, session_id, request_id);
    }

    int QueryTrades(XTPQueryTraderReq query_param, uint64_t session_id, int request_id)
    {
       return api_->QueryTrades(&query_param, session_id, request_id);

    }

    void QueryOrdersByPage(const XTPQueryOrderByPageReq query_param, uint64_t session_id, int request_id)
    {
        api_->QueryOrdersByPage(&query_param, session_id, request_id);
    }

    void QueryTradesByPage(const XTPQueryTraderByPageReq query_param, uint64_t session_id, int request_id)
    {
        api_->QueryTradesByPage(&query_param, session_id, request_id);
    }

    int QueryPosition(const char *ticker, uint64_t session_id, int request_id)
    {
       return api_->QueryPosition(ticker, session_id, request_id);

    }

    int QueryAsset(uint64_t session_id, int request_id)
    {
        return api_->QueryAsset(session_id, request_id);
    }

    int QueryStructuredFund(XTPQueryStructuredFundInfoReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryStructuredFund(&query_param, session_id, request_id);
    }

    uint64_t FundTransfer(XTPFundTransferReq fund_transfer, uint64_t session_id)
    {
        return api_->FundTransfer(&fund_transfer, session_id);
    }

    int QueryFundTransfer(XTPQueryFundTransferLogReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryFundTransfer(&query_param, session_id, request_id);
    }

    int QueryOtherServerFund(XTPFundQueryReq query_param, uint64_t session_id, jint request_id)
    {
        return api_->QueryOtherServerFund(&query_param, session_id, request_id);
    }

    int QueryETF(XTPQueryETFBaseReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryETF(&query_param, session_id, request_id);
    }

    int QueryETFTickerBasket(XTPQueryETFComponentReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryETFTickerBasket(&query_param, session_id, request_id);
    }

    int QueryIPOInfoList(uint64_t session_id, int request_id)
    {
        return api_->QueryIPOInfoList(session_id, request_id);
    }

    int QueryIPOQuotaInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryIPOQuotaInfo(session_id, request_id);
    }

    int QueryOptionAuctionInfo(XTPQueryOptionAuctionInfoReq query_param, uint64_t session_id, int request_id, bool isParamNUll)
    {
        if(isParamNUll){
            return api_->QueryOptionAuctionInfo(NULL, session_id, request_id);
        }
        return api_->QueryOptionAuctionInfo(&query_param, session_id, request_id);
    }

	const char* GetTradingDay()
	{
		return api_->GetTradingDay();
	}

	const char* GetApiVersion()
	{
		return api_->GetApiVersion();
	}

	uint8_t GetClientIDByXTPID(uint64_t order_xtp_id)
	{
		return api_->GetClientIDByXTPID(order_xtp_id);
	}

	const char* GetAccountByXTPID(uint64_t order_xtp_id)
	{
		return api_->GetAccountByXTPID(order_xtp_id);
	}

	bool IsServerRestart(uint64_t session_id)
    {
        return api_->IsServerRestart(session_id);
    }

	// 新增算法总线api接口
    int LoginALGO(std::string server_ip, int server_port, std::string username, std::string password, XTP_PROTOCOL_TYPE protocol, std::string local_ip)
    {
        return api_->LoginALGO(server_ip.c_str(), server_port, username.c_str(), password.c_str(), protocol, local_ip.c_str());
    }

    int InsertAlgoOrder(uint32_t strategy_type, uint64_t client_strategy_id, char *strategy_param, uint64_t session_id)
    {
        return api_->InsertAlgoOrder(strategy_type, client_strategy_id, strategy_param,session_id);
    }

    int CancelAlgoOrder(bool cancel_flag, uint64_t xtp_strategy_id, uint64_t session_id)
    {
        return api_->CancelAlgoOrder(cancel_flag, xtp_strategy_id, session_id);
    }

    uint64_t GetAlgorithmIDByOrder(uint64_t order_xtp_id, uint32_t orderClientId)
    {
        return api_->GetAlgorithmIDByOrder(order_xtp_id, orderClientId);
    }

    int QueryStrategy(uint32_t strategy_type, uint64_t client_strategy_id, uint64_t xtp_strategy_id, uint64_t session_id, int32_t request_id)
    {
        return api_->QueryStrategy(strategy_type,client_strategy_id,xtp_strategy_id,session_id,request_id);
    }

    int algoUserEstablishChannel(std::string oms_ip, int oms_port, std::string user, std::string password, uint64_t session_id)
    {
        return api_->ALGOUserEstablishChannel(oms_ip.c_str(), oms_port, user.c_str(), password.c_str(), session_id);
    }

    int ModifyUserTerminalInfo(XTPUserTerminalInfoReq req, uint64_t session_id)
    {
        return api_->ModifyUserTerminalInfo(&req, session_id);
    }

    // 新增融资融券业务api接口
    uint64_t CreditCashRepay(double amount, uint64_t session_id)
    {
        return api_->CreditCashRepay(amount, session_id);
    }

    uint64_t CreditCashRepayDebtInterestFee(std::string debt_id, double amount, uint64_t session_id)
    {
        return api_->CreditCashRepayDebtInterestFee(debt_id.c_str(), amount, session_id);
    }

    uint64_t CreditSellStockRepayDebtInterestFee(XTPOrderInsertInfo order, std::string debt_id, uint64_t session_id)
    {
        return api_->CreditSellStockRepayDebtInterestFee(&order, debt_id.c_str(), session_id);
    }

    int QueryCreditCashRepayInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryCreditCashRepayInfo(session_id, request_id);
    }

    int QueryCreditFundInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryCreditFundInfo(session_id, request_id);
    }

    int QueryCreditDebtInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryCreditDebtInfo(session_id, request_id);
    }

    int QueryCreditTickerDebtInfo(XTPClientQueryCrdDebtStockReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryCreditTickerDebtInfo(&query_param, session_id, request_id);
    }

    int QueryCreditAssetDebtInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryCreditAssetDebtInfo(session_id, request_id);
    }

    int QueryCreditTickerAssignInfo(XTPClientQueryCrdPositionStockReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryCreditTickerAssignInfo(&query_param, session_id, request_id);
    }

    int QueryCreditExcessStock(XTPClientQueryCrdSurplusStkReqInfo query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryCreditExcessStock(&query_param, session_id, request_id);
    }

    int QueryMulCreditExcessStock(XTPClientQueryCrdSurplusStkReqInfo query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryMulCreditExcessStock(&query_param, session_id, request_id);
    }

    uint64_t CreditExtendDebtDate(XTPCreditDebtExtendReq debt_extend, uint64_t session_id)
    {
        return api_->CreditExtendDebtDate(&debt_extend, session_id);
    }

    int QueryCreditExtendDebtDateOrders(uint64_t xtp_id, uint64_t session_id, int request_id)
    {
        return api_->QueryCreditExtendDebtDateOrders(xtp_id, session_id, request_id);
    }

    int QueryCreditFundExtraInfo(uint64_t session_id, int request_id)
    {
        return api_->QueryCreditFundExtraInfo(session_id, request_id);
    }

    int QueryCreditPositionExtraInfo(XTPClientQueryCrdPositionStockReq query_param, uint64_t session_id, int request_id)
    {
        return api_->QueryCreditPositionExtraInfo(&query_param, session_id, request_id);
    }

private:

    JNIEnv* preInvoke();
    EnvCatchStruct* getCache(std::thread::id tid);

    void  generateOrderInfoObj(JNIEnv* env, jobject& tradeOrderInfoObj,XTPOrderInfo *order_info,int request_id, bool is_last);
    void  generateTradeReportObj(JNIEnv* env, jobject& tradeReportObj,XTPTradeReport *trade_info,int request_id, bool is_last);

    void generateFundInfoObj(JNIEnv* env, jobject& targetObj, XTPStructuredFundInfo *sourceObj,int request_id, bool is_last);
    void generateFundTransferNoticeObj(JNIEnv* env, jobject& targetObj,XTPFundTransferNotice *sourceObj,int request_id, bool is_last);
    void generateXTPFundQueryRspObj(JNIEnv*env, jobject& resultObj, XTPFundQueryRsp *sourceObj, int request_id);
    void generateETFBaseObj(JNIEnv* env, jobject& targetObj, XTPQueryETFBaseRsp *sourceObj,int request_id, bool is_last);
    void generateETFComponentObj(JNIEnv* env, jobject& targetObj, XTPQueryETFComponentRsp *sourceObj,int request_id, bool is_last);
    void generateIPOTickerObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOTickerRsp *sourceObj,int request_id, bool is_last);
    void generateIPOQuotaObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOQuotaRsp *sourceObj,int request_id, bool is_last);
    void generateOptionAuctionInfoObj(JNIEnv* env, jobject& targetObj, XTPQueryOptionAuctionInfoRsp *sourceObj, int request_id, bool is_last);
    void generateXTPStrategyInfoObj(JNIEnv* env, jobject& targetObj,XTPStrategyInfoStruct *strategy_info,uint64_t child_order_xtp_id, int request_id, bool is_last);
    void generateXTPStrategyStateReportObj(JNIEnv* env, jobject& targetObj, XTPStrategyStateReportStruct *strategy_state);

    void generateCrdCashRepayRspObj(JNIEnv* env, jobject& crdCashRepayRspObj, XTPCrdCashRepayRsp *crd_cash_repay_rsp);
    void generateCrdCashRepayDebtInterestFeeRspObj(JNIEnv* env, jobject& targetObj, XTPCrdCashRepayDebtInterestFeeRsp *crd_cash_repay_debt_rsp);
    void generateCrdCashRepayInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdCashRepayInfo *cash_repay_info,int request_id, bool is_last);
    void generateCrdDebtStockInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdDebtStockInfo *debt_info,int request_id, bool is_last);
    void generateCrdPositionStockInfoObj(JNIEnv* env, jobject& targetObj,XTPClientQueryCrdPositionStkInfo *debt_info,int request_id, bool is_last);
    void generateCrdSurplusStockInfoObj(JNIEnv* env, jobject& targetObj,XTPClientQueryCrdSurplusStkRspInfo *stock_info,int request_id, bool is_last);
    void generateCrdDebtExtendNoticeObj(JNIEnv* env, jobject& targetObj,XTPCreditDebtExtendNotice *debt_extend_info, int request_id, bool is_last);
    void generateCrdFundExtraInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdFundExtraInfo *fund_info, int request_id);
    void generateCrdPositionExtraObj(JNIEnv* env, jobject& targetObj,XTPCrdPositionExtraInfo *position_extra_info, int request_id, bool is_last);
    void generateCreditFundInfoObj(JNIEnv* env, jobject& crdFundInfoObj, XTPCrdFundInfo *fund_info, int request_id);
    void generateCrdDebtInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdDebtInfo *debt_info,int request_id, bool is_last);

    void generateErrorMsgObj(JNIEnv* env, jobject& errorMsgObj,XTPRI *error_info, int request_id);
  };
