#include "xtp/xtp_trader_api.h"
#include "xtp/xoms_api_struct.h"
#include <jni.h>
#include <assert.h>

#include <string>
#include <iostream>
#include "glog/logging.h"


class Trade : public XTP::API::TraderSpi {
private:
    XTP::API::TraderApi* api_;
 //   Trader* spi_;

private:
    uint8_t client_id_;
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
    jclass query_etf_base_rsp_class_;
    jclass query_etf_component_rsp_class_;
    jclass query_ipo_ticker_rsp_class_;
    jclass query_ipo_quota_rsp_class_;
    jclass query_option_auctionInfo_rsp_class_;
    jclass order_cancel_info_class_;

public:

    void OnDisconnected(uint64_t session_id, int reason);

    void OnError(XTPRI *error_info);

    void OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id);

    void OnCancelOrderError(XTPOrderCancelInfo *cancel_info,XTPRI *error_info, uint64_t session_id);

    void OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id);

    void OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) ;

    void OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) ;

    void OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) ;

    void OnQueryStructuredFund(XTPStructuredFundInfo *fund_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, uint64_t session_id);

    void OnQueryETF(XTPQueryETFBaseRsp *etf_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryETFBasket(XTPQueryETFComponentRsp *etf_component_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryIPOInfoList(XTPQueryIPOTickerRsp *ipo_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryIPOQuotaInfo(XTPQueryIPOQuotaRsp *quota_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void OnQueryOptionAuctionInfo(XTPQueryOptionAuctionInfoRsp *option_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id);

    void setJniJvm(JavaVM *env) { jvm_ = env;}
    void setTradePluginJobject(jobject obj) {trade_plugin_obj_ = obj;}

    void setTradeOrderInfoClass(jclass jc){trade_order_info_class_ = jc;}
    void setQueryAssetRespClass(jclass jc){query_asset_rsp_class_ = jc;}
    void setStkPositionRspClass(jclass jc){query_stkposition_rsp_class_ = jc;}
    void setTradeReportClass(jclass jc){trade_report_class_ = jc;}
    void setXTPErrorMsgClass(jclass jc) {xtp_error_msg_class_ = jc;}

    void setXTPStructuredFundInfoClass(jclass jc) {structured_fund_info_class_ = jc;}
    void setXTPFundTransferNoticeClass(jclass jc) {fund_transfer_notice_class_ = jc;}
    void setXTPQueryETFBaseRspClass(jclass jc) {query_etf_base_rsp_class_ = jc;}
    void setXTPQueryETFComponentRspClass(jclass jc) {query_etf_component_rsp_class_ = jc;}
    void setXTPQueryIPOTickerRspClass(jclass jc) {query_ipo_ticker_rsp_class_ = jc;}
    void setXTPQueryIPOQuotaRspClass(jclass jc) {query_ipo_quota_rsp_class_ = jc;}
    void setXtpQueryOptionAuctionInfoRspClass(jclass jc) {query_option_auctionInfo_rsp_class_ = jc;}
    void setXTPOrderCancelInfoRspClass(jclass jc){order_cancel_info_class_= jc;}

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

    uint64_t Login(std::string server_ip, uint16_t server_port, std::string username, std::string password, XTP_PROTOCOL_TYPE protocol)
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
private:

    JNIEnv* preInvoke();
    void  generateOrderInfoObj(JNIEnv* env, jobject& tradeOrderInfoObj,XTPOrderInfo *order_info,int request_id, bool is_last);
    void  generateTradeReportObj(JNIEnv* env, jobject& tradeReportObj,XTPTradeReport *trade_info,int request_id, bool is_last);

    void  generateFundInfoObj(JNIEnv* env, jobject& targetObj, XTPStructuredFundInfo *sourceObj,int request_id, bool is_last);
    void  generateFundTransferNoticeObj(JNIEnv* env, jobject& targetObj,XTPFundTransferNotice *sourceObj,int request_id, bool is_last);
    void  generateETFBaseObj(JNIEnv* env, jobject& targetObj, XTPQueryETFBaseRsp *sourceObj,int request_id, bool is_last);
    void  generateETFComponentObj(JNIEnv* env, jobject& targetObj, XTPQueryETFComponentRsp *sourceObj,int request_id, bool is_last);
    void  generateIPOTickerObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOTickerRsp *sourceObj,int request_id, bool is_last);
    void  generateIPOQuotaObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOQuotaRsp *sourceObj,int request_id, bool is_last);
    void  generateOptionAuctionInfoObj(JNIEnv* env, jobject& targetObj, XTPQueryOptionAuctionInfoRsp *sourceObj, int request_id, bool is_last);
    void  generateCancelInfoObj(JNIEnv* env, jobject& tradeCancelOrderInfoObj,XTPOrderCancelInfo *cancel_info, int request_id, bool is_last);

    void  generateErrorMsgObj(JNIEnv* env, jobject& errorMsgObj,XTPRI *error_info, int request_id);
  };