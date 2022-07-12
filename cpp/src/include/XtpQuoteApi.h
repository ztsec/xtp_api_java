#include "xtp/xtp_quote_api.h"
#include "xtp/xquote_api_struct.h"
#include "xtp/xtp_api_data_type.h"
#include <jni.h>
#include <assert.h>

#include <string>
#include <iostream>
#include "glog/logging.h"

#include <queue>
#include <mutex>
#include <thread>

#include <fstream>
#include "RingBuffer.hpp"

#include <time.h>

#include "xquote_market_data_struct.h"

//#define COUNT_QUOTE         //输出queue_deep_log，输出各个缓冲区内的行情数据个数，并计算均值和方差(间隔10s)



class XtpQuote : public XTP::API::QuoteSpi {
private:
    XTP::API::QuoteApi *api_;

    int session_;

private:
    std::string server_ip_;
    uint16_t server_port_;
    std::string username_;
    std::string password_;
    XTP_PROTOCOL_TYPE socket_type_;
    std::string server_local_ip_;

    uint8_t client_id_;
    std::string file_path_;

    JavaVM *jvm_;
    JavaVMAttachArgs att_args_;
    jobject quote_plugin_obj_;

    //OnTickByTick里的缓存
    bool bAttachedTickByTick = false;
    JNIEnv* env = NULL;
    jclass pluginClass = NULL;
    jmethodID jm_onTickByTickEntrust = NULL;
    jmethodID jm_onTickByTickTrade = NULL;

    std::mutex* queue_lock_;
    std::mutex* queue_lock_order_book_;
    std::mutex* queue_lock_ticker_;

    RingBuffer<XTPMD>** queue_;
    RingBuffer<XTPOB>** queue_order_book_;
    RingBuffer<XTPTBT>** queue_ticker_;
    RingBuffer<XTPFMD>** queue_full_;

    bool live_;
    int64_t count_;
    std::thread* thread_;
    std::thread* thread_order_book_;
    std::thread* thread_ticker_;
    XTPMD market_data_for_gene_1_;
    bool marketdata_available_;
    bool quote_thread_available_;


#ifdef COUNT_QUOTE
    std::ofstream queue_deep_file_;
    std::thread count_quote_thread_;
#endif

    //OnDepthMarketData里的缓存
    JNIEnv* envMarketData = NULL;
//    jmethodID jm_eventMarket = NULL;

    //=================

    jclass xtp_error_msg_class_;
    jclass xtp_specific_ticker_class_;
    jclass xtp_market_data_class_;
    jclass xtp_market_data_se_class_;
    jclass xtp_market_data_oe_class_;
    jclass xtp_market_data_be_class_;
    jclass depth_market_data_ext_class_;
    jclass xtp_quote_static_info_class_;
    jclass order_book_class_;
    jclass xtp_tick_by_tick_class_;
    jclass xtp_tick_by_tick_entrust_class_;
    jclass xtp_tick_by_tick_trade_class_;
    jclass xtp_ticker_price_info_class_;
    jclass xtp_quote_static_full_info_class_;

//    uint64_t count_market;
    std::ofstream delay_log_file_;
    std::mutex file_lock_;
    uint64_t thread_num_;
    uint64_t ring_buffer_size_;
    uint64_t thread_num_min_ = 1;
    uint64_t thread_num_max_ = 16;
    uint64_t ring_buffer_size_min_ = 1024;
    uint64_t ring_buffer_size_max_ = 1024 * 256;
    uint16_t full_market_data_available_;

//    struct timeval time_start_;

public:
    void OnDisconnected(int reason);

    void OnError(XTPRI *error_info);

    void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count,
                           int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count);

    void OnDepthMarketData1(XTPMD *market_data);

    void OnDepthFullMarketData(XTPFMD *full_market_data, JNIEnv* env2 = NULL, jmethodID jm_event = NULL);

    void OnDepthMarketData2(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count,
                            int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count, JNIEnv* env2 = NULL,
                            jmethodID jm_event = NULL);

    void DealDepthMarketData(int64_t id);

    void GenerateDepthMarketData();

    void CountDepthMarketData();

    void OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnOrderBook(XTPOB *order_book);
    void DealOrderBook(int64_t id);
    void DoOrderBook(XTPOB *order_book, JNIEnv* env, jmethodID jm_event);

    void OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);

    void OnTickByTick(XTPTBT *tbt_data);
    void DealTickByTick(int64_t id);
    void DoTickByTick(XTPTBT *tbt_data, JNIEnv* env, jmethodID jm_event);

    void OnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnQueryAllTickers(XTPQSI *ticker_info, XTPRI *error_info, bool is_last);

    void OnQueryTickersPriceInfo(XTPTPI *ticker_info, XTPRI *error_info, bool is_last);

    void OnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnUnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);

    void OnQueryAllTickersFullInfo(XTPQFI *ticker_info, XTPRI *error_info, bool is_last);

    //=================================================
    void setServerIp(const char *server_ip) { server_ip_ = server_ip; }

    const char *getServerIp() { return (server_ip_.c_str()); }

    void setServerLocalIp(const char *server_local_ip) { server_local_ip_ = server_local_ip; }

    const char *getServerLocalIp() { return (server_local_ip_.c_str()); }

    void setServerPort(uint16_t server_port) { server_port_ = server_port; }

    uint16_t getServerPort() { return (server_port_); }

    void setUsername(const char *username) { username_ = username; }

    const char *getUsername() { return (username_.c_str()); }

    void setPassword(const char *password) { password_ = password; }

    const char *getPassword() { return (password_.c_str()); }

    void setSocketType(XTP_PROTOCOL_TYPE socket_type) { socket_type_ = socket_type; }

    void setJniJvm(JavaVM *env) { jvm_ = env; }

    void setPluginJobject(jobject obj) { quote_plugin_obj_ = obj; }

    void setXTPRIClass(jclass jc) { xtp_error_msg_class_ = jc; }

    void setXTPSTClass(jclass jc) { xtp_specific_ticker_class_ = jc; }

    void setXTPMDClass(jclass jc) { xtp_market_data_class_ = jc; }

    void setXTPMDSEClass(jclass jc) { xtp_market_data_se_class_ = jc; }

    void setXTPMDOEClass(jclass jc) { xtp_market_data_oe_class_ = jc; }

    void setXTPMDBEClass(jclass jc) { xtp_market_data_be_class_ = jc; }

    void setDepthMarketDataClass(jclass jc) { depth_market_data_ext_class_ = jc; }

    void setXTPQSIClass(jclass jc) { xtp_quote_static_info_class_ = jc; }

    void setXTPQFIClass(jclass jc) { xtp_quote_static_full_info_class_ = jc; }

    void setXTPOBClass(jclass jc) { order_book_class_ = jc; }

    void setXTPTBTClass(jclass jc) { xtp_tick_by_tick_class_ = jc; }

    void setXTPTickByTickEntrust(jclass jc) { xtp_tick_by_tick_entrust_class_ = jc; }

    void setXTPTickByTickTrade(jclass jc) { xtp_tick_by_tick_trade_class_ = jc; }

    void setXTPTPIClass(jclass jc) { xtp_ticker_price_info_class_ = jc; }

    void setClientId(uint16_t client_id) { client_id_ = client_id; }

    uint16_t getClientId() { return client_id_; }

    void setFilePath(const char *file_path) { file_path_ = file_path; }

    const char *getFilePath() { return (file_path_.c_str()); }

    void setFullMarketDataAvailable(uint16_t full_market_data_available) { full_market_data_available_ = full_market_data_available; }

    uint8_t getFullMarketDataAvailabled() { return full_market_data_available_; }

    XtpQuote(uint64_t thread_num, uint64_t ring_buffer_size, uint16_t ful_marke_data_available);

    ~XtpQuote();

    void Init(XTP_LOG_LEVEL logLevel) {
        api_ = XTP::API::QuoteApi::CreateQuoteApi(client_id_, file_path_.c_str(),logLevel);
        api_->RegisterSpi(this);
    }

    void Release() {
        api_->Release();
    }

    int Login() {
        session_ = api_->Login(server_ip_.c_str(), server_port_, username_.c_str(), password_.c_str(), socket_type_);
        return session_;
    }

    int Logout() {
        return api_->Logout();
    }

    void SetHeartBeatInterval(int interval) {
        if(api_!=NULL){
            api_->SetHeartBeatInterval(interval);
        }
        else{
            LOG(ERROR) << __PRETTY_FUNCTION__<<"setHeartBeatInterval mast call after init" ;
        }
    }

    int SubscribeMarketData(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeMarketData(ticker, count, exchange_id);
    }

    int UnSubscribeMarketData(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeMarketData(ticker, count, exchange_id);
    }

    int SubscribeOrderBook(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeOrderBook(ticker, count, exchange_id);
    }

    int UnSubscribeOrderBook(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeOrderBook(ticker, count, exchange_id);
    }

    int SubscribeTickByTick(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeTickByTick(ticker, count, exchange_id);
    }

    int UnSubscribeTickByTick(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeTickByTick(ticker, count, exchange_id);
    }

    int SubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeAllMarketData(exchange_id);
    }

    int UnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeAllMarketData(exchange_id);
    }

    int SubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeAllOrderBook(exchange_id);
    }

    int UnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeAllOrderBook(exchange_id);
    }

    int SubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->SubscribeAllTickByTick(exchange_id);
    }

    int UnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->UnSubscribeAllTickByTick(exchange_id);
    }

    int QueryAllTickers(XTP_EXCHANGE_TYPE exchange_id) {
        return api_->QueryAllTickers(exchange_id);
    }

    int QueryTickersPriceInfo(char *ticker[], int count, XTP_EXCHANGE_TYPE exchange_id) {
        return api_->QueryTickersPriceInfo(ticker, count, exchange_id);
    }

    int QueryAllTickersPriceInfo() {
        return api_->QueryAllTickersPriceInfo();
    }

    int SubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id){
        return api_->SubscribeAllOptionMarketData(exchange_id);
    }

    int UnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id){
        return api_->UnSubscribeAllOptionMarketData(exchange_id);
    }

    int SubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id){
        return api_->SubscribeAllOptionOrderBook(exchange_id);
    }

    int UnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id){
        return api_->UnSubscribeAllOptionOrderBook(exchange_id);
    }

    int SubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id){
        return api_->SubscribeAllOptionTickByTick(exchange_id);
    }

    int UnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id){
        return api_->UnSubscribeAllOptionTickByTick(exchange_id);
    }

    int QueryAllTickersFullInfo(XTP_EXCHANGE_TYPE exchange_id){
        return api_->QueryAllTickersFullInfo(exchange_id);
    }

    const char * GetTradingDay(){
        return api_->GetTradingDay();
    }

    const char * GetApiVersion(){
        return api_->GetApiVersion();
    }

    XTPRI * GetApiLastError(){
        return api_->GetApiLastError();
    }

    void SetUDPBufferSize(uint32_t buff_size){
        if(api_==NULL){
            LOG(ERROR) << __PRETTY_FUNCTION__<<"SetUDPBufferSize mast call after init" ;
        }
        api_->SetUDPBufferSize(buff_size);
    }

    void SetUDPRecvThreadAffinity(uint32_t cpu_no){
        if(api_==NULL){
            LOG(ERROR) << __PRETTY_FUNCTION__<<"SetUDPRecvThreadAffinity mast call before login" ;
        }
        api_->SetUDPRecvThreadAffinity(cpu_no);
    }

    void SetUDPParseThreadAffinity(uint32_t cpu_no){
        if(api_==NULL){
            LOG(ERROR) << __PRETTY_FUNCTION__<<"SetUDPParseThreadAffinity mast call before login" ;
        }
        api_->SetUDPParseThreadAffinity(cpu_no);
    }

    void SetUDPSeqLogOutPutFlag(bool flag){
        if(api_==NULL){
            LOG(ERROR) << __PRETTY_FUNCTION__<<"SetUDPSeqLogOutPutFlag mast call before login" ;
        }
        api_->SetUDPSeqLogOutPutFlag(flag);
    }


private:

    JNIEnv *preInvoke();

    JNIEnv* preInvokeMarketDataF();

    void generateErrorMsgObj(JNIEnv *env, jobject &errorMsgObj, XTPRI *error_info, int request_id);

    void generateMarketDataObj(JNIEnv *env, jobject &rspObj, XTPMD *sourceObj);

    void generateMarketDataSeObj(JNIEnv *env, jobject &rspObj, XTPMD *sourceObj);

    void generateMarketDataOeObj(JNIEnv *env, jobject &rspObj, XTPMD *sourceObj);

    void generateMarketDataBeObj(JNIEnv *env, jobject &rspObj, XTPMD *sourceObj);

    void generateDepthMarketDataExtObj(JNIEnv *env, jobject &rspObj, int64_t bid1_qty[], int32_t bid1_count,
                                       int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count,
                                       int32_t max_ask1_count);
};
