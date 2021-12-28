#include "include/XtpQuoteApi.h"
#include "include/glog/logging.h"
#include <chrono>
#include <numeric>
#include <algorithm>
#include <math.h>
#include <exception>

using namespace std;

std::string get_now_date()
{
    char tmp[20];
    memset(tmp,0x00,20);
    time_t nowtime;
    struct tm* p;
    time(&nowtime);
    p = localtime(&nowtime);
    sprintf(tmp, "%04d%02d%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday);
    return std::string(tmp);
}


double get_mean(std::vector<int64_t>& vec)
{
    double sum = std::accumulate(std::begin(vec), std::end(vec), 0.0);
    double mean =  sum / vec.size();
    return mean;
}

double get_stdev(std::vector<int64_t>& vec)
{
    double mean = get_mean(vec);
    double accum  = 0.0;
    std::for_each (std::begin(vec), std::end(vec), [&](const double d) {
        accum  += (d-mean)*(d-mean);
    });
    double stdev = sqrt(accum/(vec.size()));
    return stdev;
}

XtpQuote::XtpQuote(uint64_t thread_num, uint64_t ring_buffer_size)
    :live_(true),count_(0),marketdata_available_(false),quote_thread_available_(false),
     thread_num_(thread_num), ring_buffer_size_(ring_buffer_size)
{
    server_ip_ = "";
    server_port_ = 0;
    username_ = "";
    password_ = "";
    socket_type_ = XTP_PROTOCOL_TYPE::XTP_PROTOCOL_TCP;

    client_id_ = 13;
    file_path_ = "./";

    //initialize the jvm parameters
    att_args_.version = JNI_VERSION_1_8;
    att_args_.name = NULL;
    att_args_.group = NULL;

    std::string delay_file_name = std::string("delay_log_") + get_now_date() + ".log";
    std::string delay_log_file = std::string("/usr/local/project/log/") + delay_file_name;
    delay_log_file_.open(delay_log_file.c_str() , std::ios::app);

#ifdef COUNT_QUOTE
    std::string count_file_name = std::string("queue_deep_log_") + get_now_date() + ".log";
    std::string count_log_file = std::string("/usr/local/project/log/") + count_file_name;
    queue_deep_file_.open(count_log_file.c_str() , std::ios::app);
#endif

    if (thread_num_ < thread_num_min_) {
        thread_num_ = thread_num_min_;
    }
    if (thread_num_ > thread_num_max_) {
        thread_num_ = thread_num_max_;
    }
    if (ring_buffer_size_ < ring_buffer_size_min_) {
        ring_buffer_size_ = ring_buffer_size_min_;
    }
    if (ring_buffer_size_ > ring_buffer_size_max_) {
        ring_buffer_size_ = ring_buffer_size_max_;
    }

    queue_lock_ = new std::mutex[thread_num_];
    queue_lock_order_book_ = new std::mutex[thread_num_];
    queue_ = new RingBuffer<XTPMD>*[thread_num_];
    for(int i=0; i<thread_num_; i++)
    {
        queue_[i] = new RingBuffer<XTPMD>(ring_buffer_size_);
    }

    queue_order_book_ = new RingBuffer<XTPOB>*[thread_num_];
    for (int i = 0; i < thread_num_; i++) {
	    queue_order_book_[i] = new  RingBuffer<XTPOB>(ring_buffer_size_);
    }

    queue_ticker_ = new RingBuffer<XTPTBT>*[thread_num_];
    for (int i = 0; i < thread_num_; i++) {
	queue_ticker_[i] = new RingBuffer<XTPTBT>(ring_buffer_size_);
    }

    thread_ = new std::thread[thread_num_];
    thread_order_book_ = new std::thread[thread_num_];
    thread_ticker_ = new std::thread[thread_num_];
    for(int i=0; i<thread_num_; i++)
    {
        thread_[i] = std::thread([=](){ DealDepthMarketData(i); });
        thread_order_book_[i] = std::thread([=](){ DealOrderBook(i); });
	thread_ticker_[i] = std::thread([=]() { DealTickByTick(i);});
    }

#ifdef COUNT_QUOTE
    count_quote_thread_ = std::thread([=](){ CountDepthMarketData(); });
#endif

}

XtpQuote:: ~XtpQuote(){
    if (api_){
        api_->Release();
    }
    live_ = false;

#ifdef COUNT_QUOTE
    count_quote_thread_.join();
    queue_deep_file_.close();
#endif

    for(int i=0; i<thread_num_; i++) {
        thread_[i].join();
	thread_order_book_[i].join();
	thread_ticker_[i].join();
    }
    for(int i=0; i<thread_num_; i++)
    {
        XTPMD xtpmd;
        while (!queue_[i]->isEmpty())
            queue_[i]->pop(xtpmd);
        delete queue_[i];
    }

    delete [] queue_;
    delete [] thread_;
    delete [] queue_lock_;

    delete [] queue_order_book_;
    delete [] thread_order_book_;
    delete [] queue_lock_order_book_;

    delete [] queue_ticker_;
    delete [] thread_ticker_;
    delete [] queue_lock_ticker_;

    delay_log_file_.close();

}

JNIEnv* XtpQuote::preInvoke() {
       JNIEnv* env;

       if (jvm_ == NULL){
           return NULL;
       }
       // attach the current thread to the JVM
       jint res = jvm_->AttachCurrentThread((void**) &env, NULL);//&att_args_

       if (res !=0) {
          return NULL;
       }
       return env;
}

JNIEnv* XtpQuote::preInvokeMarketDataF() {
    if (jvm_ == NULL) {
        LOG(ERROR) << "XtpQuoteApi jvm_ is null." ;
        return NULL;
    }

    try {
        int status = jvm_->GetEnv((void **)&envMarketData, JNI_VERSION_1_8);
        if ( envMarketData == NULL || status != JNI_OK )
        {
            LOG(ERROR) << " preInvokeMarketDataF status is  " << status;
            jint resMarketData = jvm_->AttachCurrentThread((void**) &envMarketData, &att_args_);
            if (resMarketData != 0) {
                LOG(ERROR) << "XtpQuoteApi OnDepthMarketData AttachCurrentThread failed." ;
                return NULL;
            }

        }
    } catch (exception& e) {
        LOG(ERROR) << "have exception  " ;
    }

    return envMarketData;
}


void XtpQuote::OnDisconnected(int reason) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onDisconnected", "(I)V");
    assert(jm_event != NULL);
    env->CallVoidMethod(quote_plugin_obj_, jm_event, reason);
    jvm_->DetachCurrentThread();

}

void XtpQuote::OnError(XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
}

void XtpQuote::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubMarketData",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
        }
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubMarketData",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}


void XtpQuote::CountDepthMarketData()
{
    std::vector<int64_t> queue_deep;
    int64_t num = 0;
    while(live_)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for(int i=0; i<thread_num_; i++)
        {

            num = queue_[i]->amount();
            queue_deep.push_back(num);
        }

        for(int i=0; i<thread_num_; i++)
        {
#ifdef COUNT_QUOTE
            queue_deep_file_  << queue_deep[i] << ",";
#endif
        }

        auto mean = get_mean(queue_deep);
        auto stdev= get_stdev(queue_deep);
#ifdef COUNT_QUOTE
        queue_deep_file_ << mean << "," << stdev << ",";
        if(mean == 0)
        {
            queue_deep_file_ << 0 << std::endl;
        } else{
            queue_deep_file_ << stdev/mean << std::endl;
        }
#endif
        queue_deep.clear();
    }

}

void XtpQuote::OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) {
    {
        int64_t id = int64_t(market_data->ticker[5]-'0');
        //int64_t id = count_;
        count_++;
        id = id%thread_num_;
        {
            auto ret = true;
            ret = queue_[id]->push(market_data);
            if(!ret)
                LOG(ERROR) << "queue_[" << id << "] push failed for RingBuffer isFull";
            if(queue_[id]->isEmpty())
                LOG(ERROR) << "queue_[" << id << "] push success; and RingBuffer`s rear[" << queue_[id]->rear() << "] pass its front["<< queue_[id]->front() <<"]";
        }
    }

    if(!marketdata_available_)
    {
        memcpy(&market_data_for_gene_1_, market_data, sizeof(XTPMD));
        marketdata_available_ = true;
    }

}

void XtpQuote::DealDepthMarketData(int64_t id)
{
    JNIEnv* env = NULL;
    jclass pluginClass = NULL;
    jmethodID jm_event = NULL;
    int count = 0;
    int mask = 10000;
    int64_t id_ = id;

    XTPMD market_data;
    while(live_)
    {
        while (!queue_[id_]->isEmpty())
        {
            {
                auto ret = queue_[id_]->pop(market_data);

                if(!ret)
                {
                    LOG(ERROR) << "queue_[" << id << "] pop failed for RingBuffer`s rear[" << queue_[id_]->rear() << "] pass its front["<< queue_[id_]->front() <<"]";
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
            }

            if(count%mask == 0)
            {
                env = preInvoke();
                pluginClass = env->GetObjectClass(quote_plugin_obj_);
                assert(pluginClass != NULL);
                jm_event = env->GetMethodID(pluginClass, "onDepthMarketData","(IIIDDDDDDDDJJDDDDDDDDDDDDDDDDDDDDDDJJJJJJJJJJJJJJJJJJJJJLjava/lang/String;DILcom/zts/xtp/quote/model/response/MarketDataStockExDataResponse;Lcom/zts/xtp/quote/model/response/MarketDataOptionExDataResponse;Lcom/zts/xtp/quote/model/response/MarketDataBondExDataResponse;)V");

            }
            OnDepthMarketData2(&market_data, 0, 0, 0, 0, 0, 0, env, jm_event);
            if(count%mask == mask-1)
            {
                jvm_->DetachCurrentThread();
                env = NULL;
            }
            count++;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    if(env != NULL)
    {
        jvm_->DetachCurrentThread();
    }
}

void XtpQuote::OnDepthMarketData2(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count, JNIEnv* env2, jmethodID jm_event2) {

    JNIEnv* env;
    jmethodID jm_event;
    if(env2==NULL || (jvm_->GetEnv((void **)&env2, JNI_VERSION_1_8) != JNI_OK))
    {
        env = preInvoke();
        jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
        assert(pluginClass != NULL);
        jm_event = env->GetMethodID(pluginClass, "onDepthMarketData","(IIIDDDDDDDDJJDDDDDDDDDDDDDDDDDDDDDDJJJJJJJJJJJJJJJJJJJJJLjava/lang/String;DILcom/zts/xtp/quote/model/response/MarketDataStockExDataResponse;Lcom/zts/xtp/quote/model/response/MarketDataOptionExDataResponse;Lcom/zts/xtp/quote/model/response/MarketDataBondExDataResponse;)V");

    } else {
        env = env2;
        jm_event = jm_event2;
    }

    uint32_t nTicker = atol(market_data->ticker);
    uint32_t nTickerLength = strlen(market_data->ticker);

    jstring jstr_ticker_status = env->NewStringUTF(market_data->ticker_status);

    double bid[10];
    double ask[10];
    long bidQty[10];
    long askQty[10];
    for (int i=0; i<10; i++) {
        bid[i] = market_data->bid[i];
        ask[i] = market_data->ask[i];
        bidQty[i] = market_data->bid_qty[i];
        askQty[i] = market_data->ask_qty[i];
    }

    //md ex data
    jobject mdseObj = NULL;
    jobject mdoeObj = NULL;
    jobject mdbeObj = NULL;
    jmethodID mdseConstr = env->GetMethodID(xtp_market_data_se_class_, "<init>","()V");
    jmethodID mdoeConstr = env->GetMethodID(xtp_market_data_oe_class_, "<init>","()V");
    jmethodID mdbeConstr = env->GetMethodID(xtp_market_data_be_class_, "<init>","()V");
    if (mdseConstr == NULL || mdoeConstr == NULL || mdbeConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }
    mdseObj = env->NewObject(xtp_market_data_se_class_, mdseConstr);
    mdoeObj = env->NewObject(xtp_market_data_oe_class_, mdseConstr);
    mdbeObj = env->NewObject(xtp_market_data_be_class_, mdseConstr);
    if (mdseObj == NULL || mdoeObj == NULL || mdbeObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    if (market_data->data_type_v2 == XTP_MARKETDATA_V2_ACTUAL) {
        generateMarketDataSeObj(env, mdseObj, market_data);
        env->CallVoidMethod(quote_plugin_obj_, jm_event, market_data->exchange_id, nTicker, nTickerLength, market_data->last_price, market_data->pre_close_price,
                            market_data->open_price, market_data->high_price, market_data->low_price, market_data->close_price, market_data->upper_limit_price,
                            market_data->lower_limit_price, market_data->data_time, market_data->qty,
                            market_data->turnover, market_data->avg_price,
                            bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6],bid[7], bid[8], bid[9],
                            ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6],ask[7], ask[8], ask[9],
                            bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6],bidQty[7], bidQty[8], bidQty[9],
                            askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6],askQty[7], askQty[8], askQty[9],
                            market_data->trades_count, jstr_ticker_status, market_data->stk.iopv, market_data->data_type_v2, mdseObj, mdoeObj, mdbeObj);
    }
    if (market_data->data_type_v2 == XTP_MARKETDATA_V2_OPTION) {
        generateMarketDataOeObj(env, mdoeObj, market_data);
        env->CallVoidMethod(quote_plugin_obj_, jm_event, market_data->exchange_id, nTicker, nTickerLength, market_data->last_price, market_data->pre_close_price,
                            market_data->open_price, market_data->high_price, market_data->low_price, market_data->close_price, market_data->upper_limit_price,
                            market_data->lower_limit_price, market_data->data_time, market_data->qty,
                            market_data->turnover, market_data->avg_price,
                            bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6],bid[7], bid[8], bid[9],
                            ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6],ask[7], ask[8], ask[9],
                            bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6],bidQty[7], bidQty[8], bidQty[9],
                            askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6],askQty[7], askQty[8], askQty[9],
                            market_data->trades_count, jstr_ticker_status, 0.0, market_data->data_type_v2, mdseObj, mdoeObj, mdbeObj);
    }
    if (market_data->data_type_v2 == XTP_MARKETDATA_V2_BOND && market_data->exchange_id == XTP_EXCHANGE_SH) {
        //L2
        if (market_data->bond.instrument_status != NULL) {
            if (0 == strcmp(market_data->bond.instrument_status, "ADD")) {
                jstr_ticker_status = env->NewStringUTF("  0");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "START")) {
                jstr_ticker_status = env->NewStringUTF("S 1");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "OCALL")) {
                jstr_ticker_status = env->NewStringUTF("C11");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "TRADE")) {
                jstr_ticker_status = env->NewStringUTF("T11");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "SUSP")) {
                jstr_ticker_status = env->NewStringUTF("P01");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "CLOSE")) {
                jstr_ticker_status = env->NewStringUTF("E01");
            }
            if (0 == strcmp(market_data->bond.instrument_status, "ENDTR")) {
                jstr_ticker_status = env->NewStringUTF("E01");
            }
            generateMarketDataBeObj(env, mdbeObj, market_data);

            env->CallVoidMethod(quote_plugin_obj_, jm_event, market_data->exchange_id, nTicker, nTickerLength, market_data->last_price, market_data->pre_close_price,
                                market_data->open_price, market_data->high_price, market_data->low_price, market_data->close_price, market_data->upper_limit_price,
                                market_data->lower_limit_price, market_data->data_time, market_data->qty,
                                market_data->turnover, market_data->avg_price,
                                bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6],bid[7], bid[8], bid[9],
                                ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6],ask[7], ask[8], ask[9],
                                bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6],bidQty[7], bidQty[8], bidQty[9],
                                askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6],askQty[7], askQty[8], askQty[9],
                                market_data->trades_count, jstr_ticker_status, 0.0, market_data->data_type_v2, mdseObj, mdoeObj, mdbeObj);

        } else {  //L1
            generateMarketDataSeObj(env, mdseObj, market_data);
            env->CallVoidMethod(quote_plugin_obj_, jm_event, market_data->exchange_id, nTicker, nTickerLength, market_data->last_price, market_data->pre_close_price,
                                market_data->open_price, market_data->high_price, market_data->low_price, market_data->close_price, market_data->upper_limit_price,
                                market_data->lower_limit_price, market_data->data_time, market_data->qty,
                                market_data->turnover, market_data->avg_price,
                                bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6],bid[7], bid[8], bid[9],
                                ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6],ask[7], ask[8], ask[9],
                                bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6],bidQty[7], bidQty[8], bidQty[9],
                                askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6],askQty[7], askQty[8], askQty[9],
                                market_data->trades_count, jstr_ticker_status, 0.0, market_data->data_type_v2, mdseObj, mdoeObj, mdbeObj);
        }
    }
    if (market_data->data_type_v2 == XTP_MARKETDATA_V2_INDEX) {
        generateMarketDataSeObj(env, mdseObj, market_data);
        env->CallVoidMethod(quote_plugin_obj_, jm_event, market_data->exchange_id, nTicker, nTickerLength, market_data->last_price, market_data->pre_close_price,
                            market_data->open_price, market_data->high_price, market_data->low_price, market_data->close_price, market_data->upper_limit_price,
                            market_data->lower_limit_price, market_data->data_time, market_data->qty,
                            market_data->turnover, market_data->avg_price,
                            bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6],bid[7], bid[8], bid[9],
                            ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6],ask[7], ask[8], ask[9],
                            bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6],bidQty[7], bidQty[8], bidQty[9],
                            askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6],askQty[7], askQty[8], askQty[9],
                            market_data->trades_count, jstr_ticker_status, 0.0, market_data->data_type_v2, mdseObj, mdoeObj, mdbeObj);
    }

    if (env2==NULL) {
        jvm_->DetachCurrentThread();
    }
}

void XtpQuote::OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubOrderBook",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubOrderBook",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnOrderBook(XTPOB *order_book) {
    int64_t id = int64_t(order_book->ticker[5] - '0');
    id = id % thread_num_;

    {
	bool ret = true;
	ret = queue_order_book_[id]->push(order_book);
	if (!ret)
	    LOG(ERROR) << "queue_order_book_" << id << "] push failed for RingBuffer is Full";
	if (queue_order_book_[id]->isEmpty())
	    LOG(ERROR) << "queue_order_book_[" << id << "] push success; and RingBuffer's rear[" << queue_order_book_[id] << "] pass its front[" << queue_order_book_[id]->front() << "]";
    }
}

void XtpQuote::DoOrderBook(XTPOB *order_book, JNIEnv* env, jmethodID jm_event) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;

    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);

    uint32_t nTicker = atol(order_book->ticker);
    uint32_t nTickerLength = strlen(order_book->ticker);

    double bid[10];
    double ask[10];
    long bidQty[10];
    long askQty[10];
    for (int i=0; i<10; i++) {
        bid[i] = order_book->bid[i];
        ask[i] = order_book->ask[i];
        bidQty[i] = order_book->bid_qty[i];
        askQty[i] = order_book->ask_qty[i];
    }

    env->CallVoidMethod(quote_plugin_obj_, jm_event, order_book->exchange_id, nTicker, nTickerLength, 
	    order_book->last_price, order_book->qty, order_book->turnover, order_book->trades_count,
	    bid[0], bid[1], bid[2], bid[3], bid[4], bid[5], bid[6], bid[7], bid[8], bid[9],
	    ask[0], ask[1], ask[2], ask[3], ask[4], ask[5], ask[6], ask[7], ask[8], ask[9],
	    bidQty[0], bidQty[1], bidQty[2], bidQty[3], bidQty[4], bidQty[5], bidQty[6], bidQty[7], bidQty[8], bidQty[9],
	    askQty[0], askQty[1], askQty[2], askQty[3], askQty[4], askQty[5], askQty[6], askQty[7], askQty[8], askQty[9],
	    order_book->data_time);
}

void XtpQuote::DealOrderBook(int64_t id) {
    JNIEnv* env = NULL;
//    env = preInvoke();
    jclass pluginClass = NULL;
    jmethodID jm_event = NULL;
    int count = 0;
    int mask = 10000;
//    bool flag = true;
    int64_t id_ = id;

    XTPOB order_book;

    while (live_) {

        while (!queue_order_book_[id]->isEmpty()) {
	    {

		bool ret = queue_order_book_[id]->pop(order_book);
		if (!ret) {
		    LOG(ERROR) << "queue_order_book[" << id << "] pop failed for RingBuffer's rear[" << queue_order_book_[id]->rear() << "] pass its front[" << queue_order_book_[id]->front() << "]";
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
		}

		if (count % mask == 0 || (jvm_->GetEnv((void **)&env, JNI_VERSION_1_8) != JNI_OK)) {
		    env = preInvoke();
		    pluginClass = env->GetObjectClass(quote_plugin_obj_);
		    assert(pluginClass != NULL);
		    jm_event = env->GetMethodID(pluginClass, "onOrderBook", "(IIIDJDJDDDDDDDDDDDDDDDDDDDDJJJJJJJJJJJJJJJJJJJJJ)V");
		}

		DoOrderBook(&order_book, env, jm_event);

		if (count % mask == mask - 1) {
		    jvm_->DetachCurrentThread();
		    env = NULL;
		}
		count++;
	    }
	}
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    if (env != NULL) {
	jvm_->DetachCurrentThread();
    }
}

void XtpQuote::OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubTickByTick",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubTickByTick",
        "(Lcom/zts/xtp/quote/model/response/SpecificTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

     jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_specific_ticker_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_specific_ticker_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_specific_ticker_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_specific_ticker_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_specific_ticker_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::DoTickByTick(XTPTBT *tbt_data, JNIEnv* env, jmethodID jm_event) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;

    if (jm_event == NULL) {
	LOG(ERROR) << "DoTickByTick : jm_event is NULL, return";
    }

    if(jvm_ == NULL){
        LOG(ERROR) << "XtpQuoteApi OnTickByTick very fatal error eccored: jvm_ == NULL , this tickByTick Msg dropped." ;
        return;
    }

    //恢复每次attach  最后detach  解决同时订阅marketdata时  会造成NewStringUTF 崩溃
    pluginClass = env->GetObjectClass(quote_plugin_obj_);

    jstring jstr_ticker = env->NewStringUTF(tbt_data->ticker);

    switch(tbt_data->type)
    {
        // when type is entrust
        case XTP_TBT_ENTRUST:
        {
            // tbt entrust should not be null
            XTPTickByTickEntrust *xtp_tbt_entrust = &(tbt_data->entrust);
            if (xtp_tbt_entrust == NULL)
            {
                LOG(ERROR) << "OnTickByTick xtp_tbt_entrust is NULL," << tbt_data->seq ;
                return;
            }

            env->CallVoidMethod(quote_plugin_obj_, jm_event,
                                tbt_data->exchange_id, jstr_ticker, tbt_data->seq, tbt_data->data_time, tbt_data->type,
                                xtp_tbt_entrust->channel_no, xtp_tbt_entrust->seq, xtp_tbt_entrust->price, xtp_tbt_entrust->qty, xtp_tbt_entrust->side, xtp_tbt_entrust->ord_type);
            break;
        }
        // when type is trade
        case XTP_TBT_TRADE:
        {
            // tbt trade should not be null
            XTPTickByTickTrade *xtp_tbt_trade = &(tbt_data->trade);
            if (xtp_tbt_trade == NULL)
            {
                LOG(ERROR) << "OnTickByTick xtp_tbt_trade is NULL," << tbt_data->seq ;
                return;
            }

            env->CallVoidMethod(quote_plugin_obj_, jm_event,
                                tbt_data->exchange_id, jstr_ticker, tbt_data->seq, tbt_data->data_time, tbt_data->type,
                                xtp_tbt_trade->channel_no, xtp_tbt_trade->seq, xtp_tbt_trade->price, xtp_tbt_trade->qty, xtp_tbt_trade->money, xtp_tbt_trade->bid_no, xtp_tbt_trade->ask_no, xtp_tbt_trade->trade_flag);
            break;
        }
    }

}

void XtpQuote::OnTickByTick(XTPTBT *tbt_data) {
    int64_t id = int64_t(tbt_data->ticker[5] - '0');
    id = id % thread_num_;

    {
	bool ret = true;
	ret = queue_ticker_[id]->push(tbt_data);
	if (!ret)
	    LOG(ERROR) << "queue_ticker_" << id << "] push failed for RingBuffer is Full";
	if (queue_ticker_[id]->isEmpty())
	    LOG(ERROR) << "queue_ticker_[" << id << "] push success; and RingBuffer's rear[" << queue_ticker_[id] << "] pass its front[" << queue_ticker_[id]->front() << "]";
    }
}

void XtpQuote::DealTickByTick(int64_t id) {
    JNIEnv* env = NULL;
//    env = preInvoke();
    jclass pluginClass = NULL;
    jmethodID jm_event_entrust = NULL;
    jmethodID jm_event_trade = NULL;
    int count = 0;
    int mask = 10000;
//    bool flag = true;
    int64_t id_ = id;

    XTPTBT ticker;

    while (live_) {
        while (!queue_ticker_[id]->isEmpty()) {
	    {
		bool ret = queue_ticker_[id]->pop(ticker);
		if (!ret) {
		    LOG(ERROR) << "queue_ticker_[" << id << "] pop failed for RingBuffer's rear[" << queue_ticker_[id]->rear() << "] pass its front[" << queue_ticker_[id]->front() << "]";
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
		}

		if (count % mask == 0 || (jvm_->GetEnv((void **)&env, JNI_VERSION_1_8) != JNI_OK)) {
		    jm_event_entrust = jm_event_trade = NULL;
		    env = preInvoke();
		    pluginClass = env->GetObjectClass(quote_plugin_obj_);
		    jm_event_entrust = env->GetMethodID(pluginClass, "onTickByTickEntrust", "(ILjava/lang/String;JJIIJDJCC)V");
		    jm_event_trade = env->GetMethodID(pluginClass, "onTickByTickTrade", "(ILjava/lang/String;JJIIJDJDJJC)V");
		}

		if (ticker.type == XTP_TBT_ENTRUST) {
		    DoTickByTick(&ticker, env, jm_event_entrust);
		} else if (ticker.type == XTP_TBT_TRADE) {
		    DoTickByTick(&ticker, env, jm_event_trade);
		}

		if (count % mask == mask - 1) {
		    jvm_->DetachCurrentThread();
		    env = NULL;
		}
		count++;
	    }
	}
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    if (env != NULL) {
	jvm_->DetachCurrentThread();
    }
}

void XtpQuote::OnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllMarketData",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllMarketData",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllOrderBook",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllOrderBook",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllTickByTick",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllTickByTick",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnQueryAllTickers(XTPQSI* ticker_info, XTPRI *error_info, bool is_last) {
//    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onQueryAllTickers",
    "(Lcom/zts/xtp/quote/model/response/TickerInfoResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_quote_static_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_quote_static_info_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_quote_static_info_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker_info->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker_info->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_quote_static_info_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jstring jstr_tickerName = env->NewStringUTF(ticker_info->ticker_name);
        jmethodID jm_setTickerName = env->GetMethodID(xtp_quote_static_info_class_, "setTickerName", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTickerName, jstr_tickerName);

        jmethodID jm_setTickerType= env->GetMethodID(xtp_quote_static_info_class_, "setTickerType", "(I)V");
        assert(jm_setTickerType != NULL);
        env->CallVoidMethod(rspObj, jm_setTickerType, ticker_info->ticker_type);

        jmethodID jm_setPreClosePrice = env->GetMethodID(xtp_quote_static_info_class_, "setPreClosePrice", "(D)V");
        assert(jm_setPreClosePrice != NULL);
        double new_pre_close_price = ticker_info->pre_close_price;
        env->CallVoidMethod(rspObj, jm_setPreClosePrice, new_pre_close_price);

        jmethodID jm_setUpperLimitPrice = env->GetMethodID(xtp_quote_static_info_class_, "setUpperLimitPrice", "(D)V");
        assert(jm_setUpperLimitPrice != NULL);
        double new_upper_limit_price = ticker_info->upper_limit_price;
        env->CallVoidMethod(rspObj, jm_setUpperLimitPrice, new_upper_limit_price);

        jmethodID jm_setLowerLimitPrice = env->GetMethodID(xtp_quote_static_info_class_, "setLowerLimitPrice", "(D)V");
        assert(jm_setLowerLimitPrice != NULL);
        double new_lower_limit_price = ticker_info->lower_limit_price;
        env->CallVoidMethod(rspObj, jm_setLowerLimitPrice, new_lower_limit_price);

        jmethodID jm_setPriceTick = env->GetMethodID(xtp_quote_static_info_class_, "setPriceTick", "(D)V");
        assert(jm_setPriceTick != NULL);
        double new_price_tick = ticker_info->price_tick;
        env->CallVoidMethod(rspObj, jm_setPriceTick, new_price_tick);

        jmethodID jm_setBuyQtyUnit = env->GetMethodID(xtp_quote_static_info_class_, "setBuyQtyUnit", "(I)V");
        assert(jm_setBuyQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setBuyQtyUnit, ticker_info->buy_qty_unit);

        jmethodID jm_setSellQtyUnit = env->GetMethodID(xtp_quote_static_info_class_, "setSellQtyUnit", "(I)V");
        assert(jm_setSellQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setSellQtyUnit, ticker_info->sell_qty_unit);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_quote_static_info_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnQueryTickersPriceInfo(XTPTPI* ticker_info, XTPRI *error_info, bool is_last) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onQueryTickersPriceInfo",
        "(Lcom/zts/xtp/quote/model/response/TickerPriceInfoResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_ticker_price_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        rspObj = env->NewObject(xtp_ticker_price_info_class_, defaultConstr);
        if (rspObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setExchangeType= env->GetMethodID(xtp_ticker_price_info_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker_info->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker_info->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_ticker_price_info_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jmethodID jm_setLastPrice = env->GetMethodID(xtp_ticker_price_info_class_, "setLastPrice", "(D)V");
        assert(jm_setLastPrice != NULL);
        double new_last_price = ticker_info->last_price;
        env->CallVoidMethod(rspObj, jm_setLastPrice, new_last_price);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_ticker_price_info_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllOptionMarketData",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllOptionMarketData",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllOptionOrderBook",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllOptionOrderBook",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onSubscribeAllOptionTickByTick",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnUnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info){
    //LOG(INFO) << __PRETTY_FUNCTION__ ;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
        	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
				}
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onUnSubscribeAllOptionTickByTick",
        "(ILcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, (int)exchange_id, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::OnQueryAllTickersFullInfo(XTPQFI* ticker_info, XTPRI *error_info, bool is_last) {
//    LOG(INFO) << __PRETTY_FUNCTION__;
    if (error_info != NULL) {
        if(error_info->error_id!=0){
            LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
        }
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onQueryAllTickersFullInfo",
                                          "(Lcom/zts/xtp/quote/model/response/TickerFullInfoResponse;Lcom/zts/xtp/common/model/ErrorMessage;)V");

    //generate the error msg object
    //fetch the errormsg default construct
    jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
    if (defaultErrorConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the errormsg object
    jobject errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
    if (errorMsgObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateErrorMsgObj(env, errorMsgObj, error_info, 0);

    jobject rspObj=NULL;
    //error_info = null or error_id =0 means successful
    if (error_info == NULL || error_info->error_id == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_quote_static_full_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        rspObj = env->NewObject(xtp_quote_static_full_info_class_, defaultConstr);
        if (rspObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        jmethodID jm_setExchangeType = env->GetMethodID(xtp_quote_static_full_info_class_, "setExchangeType", "(I)V");
        assert(jm_setExchangeType != NULL);
        env->CallVoidMethod(rspObj, jm_setExchangeType, ticker_info->exchange_id);

        jstring jstr_ticker = env->NewStringUTF(ticker_info->ticker);
        jmethodID jm_setTicker = env->GetMethodID(xtp_quote_static_full_info_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

        jstring jstr_tickerName = env->NewStringUTF(ticker_info->ticker_name);
        jmethodID jm_setTickerName = env->GetMethodID(xtp_quote_static_full_info_class_, "setTickerName", "(Ljava/lang/String;)V");
        env->CallVoidMethod(rspObj, jm_setTickerName, jstr_tickerName);

        jmethodID jm_setSecurityType = env->GetMethodID(xtp_quote_static_full_info_class_, "setSecurityType", "(I)V");
        assert(jm_setSecurityType != NULL);
        env->CallVoidMethod(rspObj, jm_setSecurityType, ticker_info->security_type);

        jmethodID jm_setQualificationType = env->GetMethodID(xtp_quote_static_full_info_class_, "setQualificationType", "(I)V");
        assert(jm_setQualificationType != NULL);
        env->CallVoidMethod(rspObj, jm_setQualificationType, ticker_info->ticker_qualification_class);

        jmethodID jm_setIsRegistration = env->GetMethodID(xtp_quote_static_full_info_class_, "setIsRegistration", "(Z)V");
        assert(jm_setIsRegistration != NULL);
        env->CallVoidMethod(rspObj, jm_setIsRegistration, ticker_info->is_registration);

        jmethodID jm_setIsVIE = env->GetMethodID(xtp_quote_static_full_info_class_, "setIsVIE", "(Z)V");
        assert(jm_setIsVIE != NULL);
        env->CallVoidMethod(rspObj, jm_setIsVIE, ticker_info->is_VIE);

        jmethodID jm_setIsNoprofit = env->GetMethodID(xtp_quote_static_full_info_class_, "setIsNoprofit", "(Z)V");
        assert(jm_setIsNoprofit != NULL);
        env->CallVoidMethod(rspObj, jm_setIsNoprofit, ticker_info->is_noprofit);

        jmethodID jm_setIsWeightedVotingRights = env->GetMethodID(xtp_quote_static_full_info_class_, "setIsWeightedVotingRights", "(Z)V");
        assert(jm_setIsWeightedVotingRights != NULL);
        env->CallVoidMethod(rspObj, jm_setIsWeightedVotingRights, ticker_info->is_weighted_voting_rights);

        jmethodID jm_setIsHavePriceLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setIsHavePriceLimit", "(Z)V");
        assert(jm_setIsHavePriceLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setIsHavePriceLimit, ticker_info->is_have_price_limit);

        jmethodID jm_setPreClosePrice = env->GetMethodID(xtp_quote_static_full_info_class_, "setPreClosePrice", "(D)V");
        assert(jm_setPreClosePrice != NULL);
        double new_pre_close_price = ticker_info->pre_close_price;
        env->CallVoidMethod(rspObj, jm_setPreClosePrice, new_pre_close_price);

        jmethodID jm_setUpperLimitPrice = env->GetMethodID(xtp_quote_static_full_info_class_, "setUpperLimitPrice", "(D)V");
        assert(jm_setUpperLimitPrice != NULL);
        double new_upper_limit_price = ticker_info->upper_limit_price;
        env->CallVoidMethod(rspObj, jm_setUpperLimitPrice, new_upper_limit_price);

        jmethodID jm_setLowerLimitPrice = env->GetMethodID(xtp_quote_static_full_info_class_, "setLowerLimitPrice", "(D)V");
        assert(jm_setLowerLimitPrice != NULL);
        double new_lower_limit_price = ticker_info->lower_limit_price;
        env->CallVoidMethod(rspObj, jm_setLowerLimitPrice, new_lower_limit_price);

        jmethodID jm_setPriceTick = env->GetMethodID(xtp_quote_static_full_info_class_, "setPriceTick", "(D)V");
        assert(jm_setPriceTick != NULL);
        double new_price_tick = ticker_info->price_tick;
        env->CallVoidMethod(rspObj, jm_setPriceTick, new_price_tick);

        jmethodID jm_setBidQtyUpperLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setBidQtyUpperLimit", "(I)V");
        assert(jm_setBidQtyUpperLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setBidQtyUpperLimit, ticker_info->bid_qty_upper_limit);

        jmethodID jm_setBidQtyLowerLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setBidQtyLowerLimit", "(I)V");
        assert(jm_setBidQtyLowerLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setBidQtyLowerLimit, ticker_info->bid_qty_lower_limit);

        jmethodID jm_setBidQtyUnit = env->GetMethodID(xtp_quote_static_full_info_class_, "setBidQtyUnit", "(I)V");
        assert(jm_setBidQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setBidQtyUnit, ticker_info->bid_qty_unit);

        jmethodID jm_setAskQtyUpperLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setAskQtyUpperLimit", "(I)V");
        assert(jm_setAskQtyUpperLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setAskQtyUpperLimit, ticker_info->ask_qty_upper_limit);

        jmethodID jm_setAskQtyLowerLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setAskQtyLowerLimit", "(I)V");
        assert(jm_setAskQtyLowerLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setAskQtyLowerLimit, ticker_info->ask_qty_lower_limit);

        jmethodID jm_setAskQtyUnit = env->GetMethodID(xtp_quote_static_full_info_class_, "setAskQtyUnit", "(I)V");
        assert(jm_setAskQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setAskQtyUnit, ticker_info->ask_qty_unit);

        jmethodID jm_setMarketBidQtyUpperLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketBidQtyUpperLimit", "(I)V");
        assert(jm_setMarketBidQtyUpperLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketBidQtyUpperLimit, ticker_info->market_bid_qty_upper_limit);

        jmethodID jm_setMarketBidQtyLowerLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketBidQtyLowerLimit", "(I)V");
        assert(jm_setMarketBidQtyLowerLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketBidQtyLowerLimit, ticker_info->market_bid_qty_lower_limit);

        jmethodID jm_setMarketBidQtyUnit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketBidQtyUnit", "(I)V");
        assert(jm_setMarketBidQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketBidQtyUnit, ticker_info->market_bid_qty_unit);

        jmethodID jm_setMarketAskQtyUpperLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketAskQtyUpperLimit", "(I)V");
        assert(jm_setMarketAskQtyUpperLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketAskQtyUpperLimit, ticker_info->market_ask_qty_upper_limit);

        jmethodID jm_setMarketAskQtyLowerLimit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketAskQtyLowerLimit", "(I)V");
        assert(jm_setMarketAskQtyLowerLimit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketAskQtyLowerLimit, ticker_info->market_ask_qty_lower_limit);

        jmethodID jm_setMarketAskQtyUnit = env->GetMethodID(xtp_quote_static_full_info_class_, "setMarketAskQtyUnit", "(I)V");
        assert(jm_setMarketAskQtyUnit != NULL);
        env->CallVoidMethod(rspObj, jm_setMarketAskQtyUnit, ticker_info->market_ask_qty_unit);

        jmethodID jm_setSecurityStatus = env->GetMethodID(xtp_quote_static_full_info_class_, "setSecurityStatus", "(I)V");
        assert(jm_setSecurityStatus != NULL);
        env->CallVoidMethod(rspObj, jm_setSecurityStatus, ticker_info->security_status);

        jmethodID jm_setUnknown1 = env->GetMethodID(xtp_quote_static_full_info_class_, "setUnknown1", "(I)V");
        env->CallVoidMethod(rspObj, jm_setUnknown1, ticker_info->unknown1);

        jmethodID jm_setUnknown = env->GetMethodID(xtp_quote_static_full_info_class_, "setUnknown", "(I)V");
        env->CallVoidMethod(rspObj, jm_setUnknown, ticker_info->unknown);

        jmethodID jm_setLastResp = env->GetMethodID(xtp_quote_static_full_info_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(rspObj, jm_setLastResp, is_last);
    }
    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj, errorMsgObj);
    jvm_->DetachCurrentThread();
}

void XtpQuote::generateErrorMsgObj(JNIEnv* env, jobject& errorMsgObj,XTPRI *error_info, int request_id) {

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(xtp_error_msg_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(errorMsgObj, jm_setRequestId, request_id);

    //call setErrorId
    if (error_info != NULL) {
        jmethodID jm_setErrorId = env->GetMethodID(xtp_error_msg_class_, "setErrorId", "(I)V");
        assert(jm_setErrorId != NULL);
        env->CallVoidMethod(errorMsgObj, jm_setErrorId, error_info->error_id);

        //call setErrorMsg
        jstring jerrorMsgStr = env->NewStringUTF(error_info->error_msg);
        jmethodID jm_setErrorMsg = env->GetMethodID(xtp_error_msg_class_, "setErrorMsg", "(Ljava/lang/String;)V");
        env->CallVoidMethod(errorMsgObj, jm_setErrorMsg, jerrorMsgStr);
    }
}

void XtpQuote::generateMarketDataObj(JNIEnv* env, jobject& rspObj, XTPMD *sourceObj) {
    jmethodID jm_setExchangeType= env->GetMethodID(xtp_market_data_class_, "setExchangeType", "(I)V");
    assert(jm_setExchangeType != NULL);
    env->CallVoidMethod(rspObj, jm_setExchangeType, sourceObj->exchange_id);

    jstring jstr_ticker = env->NewStringUTF(sourceObj->ticker);
    jmethodID jm_setTicker = env->GetMethodID(xtp_market_data_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

    jmethodID jm_setLastPrice = env->GetMethodID(xtp_market_data_class_, "setLastPrice", "(D)V");
    assert(jm_setLastPrice != NULL);
    double new_last_price = sourceObj->last_price;
    env->CallVoidMethod(rspObj, jm_setLastPrice, new_last_price);

    jmethodID jm_setPreClosePrice = env->GetMethodID(xtp_market_data_class_, "setPreClosePrice", "(D)V");
    assert(jm_setPreClosePrice != NULL);
    double new_pre_close_price = sourceObj->pre_close_price;
    env->CallVoidMethod(rspObj, jm_setPreClosePrice, new_pre_close_price);

    jmethodID jm_setOpenPrice = env->GetMethodID(xtp_market_data_class_, "setOpenPrice", "(D)V");
    assert(jm_setOpenPrice != NULL);
    double new_open_price = sourceObj->open_price;
    env->CallVoidMethod(rspObj, jm_setOpenPrice, new_open_price);

    jmethodID jm_setHighPrice = env->GetMethodID(xtp_market_data_class_, "setHighPrice", "(D)V");
    assert(jm_setHighPrice != NULL);
    double new_high_price = sourceObj->high_price;
    env->CallVoidMethod(rspObj, jm_setHighPrice, new_high_price);

    jmethodID jm_setLowPrice = env->GetMethodID(xtp_market_data_class_, "setLowPrice", "(D)V");
    assert(jm_setLowPrice != NULL);
    double new_low_price = sourceObj->low_price;
    env->CallVoidMethod(rspObj, jm_setLowPrice, new_low_price);

    jmethodID jm_setClosePrice = env->GetMethodID(xtp_market_data_class_, "setClosePrice", "(D)V");
    assert(jm_setClosePrice != NULL);
    double new_close_price = sourceObj->close_price;
    env->CallVoidMethod(rspObj, jm_setClosePrice, new_close_price);

    jmethodID jm_setPreTotalLongPositon = env->GetMethodID(xtp_market_data_class_, "setPreTotalLongPositon", "(J)V");
    assert(jm_setPreTotalLongPositon != NULL);
    env->CallVoidMethod(rspObj, jm_setPreTotalLongPositon, sourceObj->pre_total_long_positon);

    jmethodID jm_setTotalLongPositon = env->GetMethodID(xtp_market_data_class_, "setTotalLongPositon", "(J)V");
    assert(jm_setTotalLongPositon != NULL);
    env->CallVoidMethod(rspObj, jm_setTotalLongPositon, sourceObj->total_long_positon);

    jmethodID jm_setPreSettlementPrice = env->GetMethodID(xtp_market_data_class_, "setPreSettlementPrice", "(D)V");
    assert(jm_setPreSettlementPrice != NULL);
    double new_pre_settlement_price = sourceObj->pre_settl_price;
    env->CallVoidMethod(rspObj, jm_setPreSettlementPrice, new_pre_settlement_price);

    jmethodID jm_setSettlementPrice = env->GetMethodID(xtp_market_data_class_, "setSettlementPrice", "(D)V");
    assert(jm_setSettlementPrice != NULL);
    double new_settlement_price = sourceObj->settl_price;
    env->CallVoidMethod(rspObj, jm_setSettlementPrice, new_settlement_price);

    jmethodID jm_setUpperLimitPrice = env->GetMethodID(xtp_market_data_class_, "setUpperLimitPrice", "(D)V");
    assert(jm_setUpperLimitPrice != NULL);
    double new_upper_limit_price = sourceObj->upper_limit_price;
    env->CallVoidMethod(rspObj, jm_setUpperLimitPrice, new_upper_limit_price);

    jmethodID jm_setLowerLimitPrice = env->GetMethodID(xtp_market_data_class_, "setLowerLimitPrice", "(D)V");
    assert(jm_setLowerLimitPrice != NULL);
    double new_lower_limit_price = sourceObj->lower_limit_price;
    env->CallVoidMethod(rspObj, jm_setLowerLimitPrice, new_lower_limit_price);

    jmethodID jm_setPreDelta = env->GetMethodID(xtp_market_data_class_, "setPreDelta", "(D)V");
    assert(jm_setPreDelta != NULL);
    env->CallVoidMethod(rspObj, jm_setPreDelta, sourceObj->pre_delta);

    jmethodID jm_setCurrDelta = env->GetMethodID(xtp_market_data_class_, "setCurrDelta", "(D)V");
    assert(jm_setCurrDelta != NULL);
    env->CallVoidMethod(rspObj, jm_setCurrDelta, sourceObj->curr_delta);

    jmethodID jm_setDataTime = env->GetMethodID(xtp_market_data_class_, "setDataTime", "(J)V");
    env->CallVoidMethod(rspObj, jm_setDataTime, sourceObj->data_time);

    jmethodID jm_setQty = env->GetMethodID(xtp_market_data_class_, "setQty", "(J)V");
    env->CallVoidMethod(rspObj, jm_setQty, sourceObj->qty);

    jmethodID jm_setTurnover = env->GetMethodID(xtp_market_data_class_, "setTurnover", "(D)V");
    assert(jm_setTurnover != NULL);
    env->CallVoidMethod(rspObj, jm_setTurnover, sourceObj->turnover);

    jmethodID jm_setAvgPrice = env->GetMethodID(xtp_market_data_class_, "setAvgPrice", "(D)V");
    assert(jm_setAvgPrice != NULL);
    double new_avg_price = sourceObj->avg_price;
    env->CallVoidMethod(rspObj, jm_setAvgPrice, new_avg_price);

    jdoubleArray jarray_setBid = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(jarray_setBid, 0, 10, sourceObj->bid);
    jmethodID jm_setBid = env->GetMethodID(xtp_market_data_class_, "setBid", "([D)V");
    env->CallVoidMethod(rspObj, jm_setBid, jarray_setBid);

    jdoubleArray jarray_setAsk = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(jarray_setAsk, 0, 10, sourceObj->ask);
    jmethodID jm_setAsk = env->GetMethodID(xtp_market_data_class_, "setAsk", "([D)V");
    env->CallVoidMethod(rspObj, jm_setAsk, jarray_setAsk);

    jlongArray jarray_setBidQty = env->NewLongArray(10);
    env->SetLongArrayRegion(jarray_setBidQty, 0, 10, (jlong *)sourceObj->bid_qty);
    jmethodID jm_setBidQty = env->GetMethodID(xtp_market_data_class_, "setBidQty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setBidQty, jarray_setBidQty);

    jlongArray jarray_setAskQty = env->NewLongArray(10);
    env->SetLongArrayRegion(jarray_setAskQty, 0, 10, (jlong *)sourceObj->ask_qty);
    jmethodID jm_setAskQty = env->GetMethodID(xtp_market_data_class_, "setAskQty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setAskQty, jarray_setAskQty);

    jmethodID jm_setTradesCount = env->GetMethodID(xtp_market_data_class_, "setTradesCount", "(J)V");
    env->CallVoidMethod(rspObj, jm_setTradesCount, sourceObj->trades_count);

    jstring jstr_setTickerStatus = env->NewStringUTF(sourceObj->ticker_status);
    jmethodID jm_setTickerStatus = env->GetMethodID(xtp_market_data_class_, "setTickerStatus", "(Ljava/lang/String;)V");
    env->CallVoidMethod(rspObj, jm_setTickerStatus, jstr_setTickerStatus);

    //only use XTPMarketDataStockExData
    if (sourceObj->data_type == XTP_MARKETDATA_ACTUAL) {

        jobject mdseObj = NULL;
        jmethodID mdseConstr = env->GetMethodID(xtp_market_data_se_class_, "<init>","()V");
        if (mdseConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }
        mdseObj = env->NewObject(xtp_market_data_se_class_, mdseConstr);
        if (mdseObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        jmethodID jm_setTotalBidQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalBidQty", "(J)V");
        env->CallVoidMethod(mdseObj, jm_setTotalBidQty, sourceObj->stk.total_bid_qty);

        jmethodID jm_setTotalAskQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalAskQty", "(J)V");
        env->CallVoidMethod(mdseObj, jm_setTotalAskQty, sourceObj->stk.total_ask_qty);

        jmethodID jm_setMaBidPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBidPrice", "(D)V");
        assert(jm_setMaBidPrice != NULL);
        double new_ma_bid_price = sourceObj->stk.ma_bid_price;
        env->CallVoidMethod(mdseObj, jm_setMaBidPrice, new_ma_bid_price);

        jmethodID jm_setMaAskPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaAskPrice", "(D)V");
        assert(jm_setMaAskPrice != NULL);
        double new_ma_ask_price = sourceObj->stk.ma_ask_price;
        env->CallVoidMethod(mdseObj, jm_setMaAskPrice, new_ma_ask_price);

        jmethodID jm_setMaBondBidPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBondBidPrice", "(D)V");
        assert(jm_setMaBondBidPrice != NULL);
        double new_ma_bond_bid_price = sourceObj->stk.ma_bond_bid_price;
        env->CallVoidMethod(mdseObj, jm_setMaBondBidPrice, new_ma_bond_bid_price);

        jmethodID jm_setMaBondAskPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBondAskPrice", "(D)V");
        assert(jm_setMaBondAskPrice != NULL);
        double new_ma_bond_ask_price = sourceObj->stk.ma_bond_ask_price;
        env->CallVoidMethod(mdseObj, jm_setMaBondAskPrice, new_ma_bond_ask_price);

        jmethodID jm_setYieldToMaturity = env->GetMethodID(xtp_market_data_se_class_, "setYieldToMaturity", "(D)V");
        assert(jm_setYieldToMaturity != NULL);
        env->CallVoidMethod(mdseObj, jm_setYieldToMaturity, sourceObj->stk.yield_to_maturity);

        jmethodID jm_setIopv = env->GetMethodID(xtp_market_data_se_class_, "setIopv", "(D)V");
        assert(jm_setIopv != NULL);
        env->CallVoidMethod(mdseObj, jm_setIopv, sourceObj->stk.iopv);

        jmethodID jm_setEtfBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyCount", "(I)V");
        assert(jm_setEtfBuyCount != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfBuyCount, sourceObj->stk.etf_buy_count);

        jmethodID jm_setEtfSellCount = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellCount", "(I)V");
        assert(jm_setEtfSellCount != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfSellCount, sourceObj->stk.etf_sell_count);

        jmethodID jm_setEtfBuyQty = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyQty", "(D)V");
        assert(jm_setEtfBuyQty != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfBuyQty, sourceObj->stk.etf_buy_qty);

        jmethodID jm_setEtfBuyMoney = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyMoney", "(D)V");
        assert(jm_setEtfBuyMoney != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfBuyMoney, sourceObj->stk.etf_buy_money);

        jmethodID jm_setEtfSellQty = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellQty", "(D)V");
        assert(jm_setEtfSellQty != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfSellQty, sourceObj->stk.etf_sell_qty);

        jmethodID jm_setEtfSellMoney = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellMoney", "(D)V");
        assert(jm_setEtfSellMoney != NULL);
        env->CallVoidMethod(mdseObj, jm_setEtfSellMoney, sourceObj->stk.etf_sell_money);

        jmethodID jm_setTotalWarrantExecQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalWarrantExecQty", "(D)V");
        assert(jm_setTotalWarrantExecQty != NULL);
        env->CallVoidMethod(mdseObj, jm_setTotalWarrantExecQty, sourceObj->stk.total_warrant_exec_qty);

        jmethodID jm_setWarrantLowerPrice = env->GetMethodID(xtp_market_data_se_class_, "setWarrantLowerPrice", "(D)V");
        assert(jm_setWarrantLowerPrice != NULL);
        double new_warrant_lower_price = sourceObj->stk.warrant_lower_price;
        env->CallVoidMethod(mdseObj, jm_setWarrantLowerPrice, new_warrant_lower_price);

        jmethodID jm_setWarrantUpperPrice = env->GetMethodID(xtp_market_data_se_class_, "setWarrantUpperPrice", "(D)V");
        assert(jm_setWarrantUpperPrice != NULL);
        double new_warrant_upper_price = sourceObj->stk.warrant_upper_price;
        env->CallVoidMethod(mdseObj, jm_setWarrantUpperPrice, new_warrant_upper_price);

        jmethodID jm_setCancelBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyCount", "(I)V");
        assert(jm_setCancelBuyCount != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelBuyCount, sourceObj->stk.cancel_buy_count);

        jmethodID jm_setCancelSellCount = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellCount", "(I)V");
        assert(jm_setCancelSellCount != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelSellCount, sourceObj->stk.cancel_sell_count);

        jmethodID jm_setCancelBuyQty = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyQty", "(D)V");
        assert(jm_setCancelBuyQty != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelBuyQty, sourceObj->stk.cancel_buy_qty);

        jmethodID jm_setCancelSellQty = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellQty", "(D)V");
        assert(jm_setCancelSellQty != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelSellQty, sourceObj->stk.cancel_sell_qty);

        jmethodID jm_setCancelBuyMoney = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyMoney", "(D)V");
        assert(jm_setCancelBuyMoney != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelBuyMoney, sourceObj->stk.cancel_buy_money);

        jmethodID jm_setCancelSellMoney = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellMoney", "(D)V");
        assert(jm_setCancelSellMoney != NULL);
        env->CallVoidMethod(mdseObj, jm_setCancelSellMoney, sourceObj->stk.cancel_sell_money);

        jmethodID jm_setTotalBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setTotalBuyCount", "(J)V");
        env->CallVoidMethod(mdseObj, jm_setTotalBuyCount, sourceObj->stk.total_buy_count);

        jmethodID jm_setTotalSellCount = env->GetMethodID(xtp_market_data_se_class_, "setTotalSellCount", "(J)V");
        env->CallVoidMethod(mdseObj, jm_setTotalSellCount, sourceObj->stk.total_sell_count);

        jmethodID jm_setDurationAfterBuy = env->GetMethodID(xtp_market_data_se_class_, "setDurationAfterBuy", "(I)V");
        assert(jm_setDurationAfterBuy != NULL);
        env->CallVoidMethod(mdseObj, jm_setDurationAfterBuy, sourceObj->stk.duration_after_buy);

        jmethodID jm_setDurationAfterSell = env->GetMethodID(xtp_market_data_se_class_, "setDurationAfterSell", "(I)V");
        assert(jm_setDurationAfterSell != NULL);
        env->CallVoidMethod(mdseObj, jm_setDurationAfterSell, sourceObj->stk.duration_after_sell);

        jmethodID jm_setNumBidOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumBidOrders", "(I)V");
        assert(jm_setNumBidOrders != NULL);
        env->CallVoidMethod(mdseObj, jm_setNumBidOrders, sourceObj->stk.num_bid_orders);

        jmethodID jm_setNumAskOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumAskOrders", "(I)V");
        assert(jm_setNumAskOrders != NULL);
        env->CallVoidMethod(mdseObj, jm_setNumAskOrders, sourceObj->stk.num_ask_orders);

        jmethodID jm_setPreIopv = env->GetMethodID(xtp_market_data_se_class_, "setPreIopv", "(D)V");
        assert(jm_setPreIopv != NULL);
        env->CallVoidMethod(mdseObj, jm_setPreIopv, sourceObj->stk.pre_iopv);

        jmethodID jm_setStk = env->GetMethodID(xtp_market_data_class_, "setStk", "(Lcom/zts/xtp/quote/model/response/MarketDataStockExDataResponse;)V");
        env->CallVoidMethod(rspObj, jm_setStk, mdseObj);

    }
    else{

        jobject mdoeObj = NULL;
        jmethodID mdoeConstr = env->GetMethodID(xtp_market_data_oe_class_, "<init>","()V");
        if (mdoeConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        mdoeObj = env->NewObject(xtp_market_data_oe_class_, mdoeConstr);
        if (mdoeObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        jmethodID jm_setAuctionPrice = env->GetMethodID(xtp_market_data_oe_class_, "setAuctionPrice", "(D)V");
        assert(jm_setAuctionPrice != NULL);
        env->CallVoidMethod(mdoeObj, jm_setAuctionPrice, sourceObj->opt.auction_price);

        jmethodID jm_setAuctionQty = env->GetMethodID(xtp_market_data_oe_class_, "setAuctionQty", "(J)V");
        assert(jm_setAuctionQty != NULL);
        env->CallVoidMethod(mdoeObj, jm_setAuctionQty, sourceObj->opt.auction_qty);

        jmethodID jm_setLastEnquiryTime = env->GetMethodID(xtp_market_data_oe_class_, "setLastEnquiryTime", "(J)V");
        assert(jm_setLastEnquiryTime != NULL);
        env->CallVoidMethod(mdoeObj, jm_setLastEnquiryTime, sourceObj->opt.last_enquiry_time);

        jmethodID jm_setOpt = env->GetMethodID(xtp_market_data_class_, "setOpt", "(Lcom/zts/xtp/quote/model/response/MarketDataOptionExDataResponse;)V");
        env->CallVoidMethod(rspObj, jm_setOpt, mdoeObj);

    }

    jmethodID jm_setDataType = env->GetMethodID(xtp_market_data_class_, "setDataType", "(I)V");
    env->CallVoidMethod(rspObj, jm_setDataType, sourceObj->data_type);

}

void XtpQuote::generateMarketDataSeObj(JNIEnv* env, jobject& mdseObj, XTPMD *sourceObj) {
    jmethodID jm_setTotalBidQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalBidQty", "(J)V");
    env->CallVoidMethod(mdseObj, jm_setTotalBidQty, sourceObj->stk.total_bid_qty);

    jmethodID jm_setTotalAskQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalAskQty", "(J)V");
    env->CallVoidMethod(mdseObj, jm_setTotalAskQty, sourceObj->stk.total_ask_qty);

    jmethodID jm_setMaBidPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBidPrice", "(D)V");
    assert(jm_setMaBidPrice != NULL);
    double new_ma_bid_price = sourceObj->stk.ma_bid_price;
    env->CallVoidMethod(mdseObj, jm_setMaBidPrice, new_ma_bid_price);

    jmethodID jm_setMaAskPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaAskPrice", "(D)V");
    assert(jm_setMaAskPrice != NULL);
    double new_ma_ask_price = sourceObj->stk.ma_ask_price;
    env->CallVoidMethod(mdseObj, jm_setMaAskPrice, new_ma_ask_price);

    jmethodID jm_setMaBondBidPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBondBidPrice", "(D)V");
    assert(jm_setMaBondBidPrice != NULL);
    double new_ma_bond_bid_price = sourceObj->stk.ma_bond_bid_price;
    env->CallVoidMethod(mdseObj, jm_setMaBondBidPrice, new_ma_bond_bid_price);

    jmethodID jm_setMaBondAskPrice = env->GetMethodID(xtp_market_data_se_class_, "setMaBondAskPrice", "(D)V");
    assert(jm_setMaBondAskPrice != NULL);
    double new_ma_bond_ask_price = sourceObj->stk.ma_bond_ask_price;
    env->CallVoidMethod(mdseObj, jm_setMaBondAskPrice, new_ma_bond_ask_price);

    jmethodID jm_setYieldToMaturity = env->GetMethodID(xtp_market_data_se_class_, "setYieldToMaturity", "(D)V");
    assert(jm_setYieldToMaturity != NULL);
    env->CallVoidMethod(mdseObj, jm_setYieldToMaturity, sourceObj->stk.yield_to_maturity);

    jmethodID jm_setIopv = env->GetMethodID(xtp_market_data_se_class_, "setIopv", "(D)V");
    assert(jm_setIopv != NULL);
    env->CallVoidMethod(mdseObj, jm_setIopv, sourceObj->stk.iopv);

    jmethodID jm_setEtfBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyCount", "(I)V");
    assert(jm_setEtfBuyCount != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfBuyCount, sourceObj->stk.etf_buy_count);

    jmethodID jm_setEtfSellCount = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellCount", "(I)V");
    assert(jm_setEtfSellCount != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfSellCount, sourceObj->stk.etf_sell_count);

    jmethodID jm_setEtfBuyQty = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyQty", "(D)V");
    assert(jm_setEtfBuyQty != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfBuyQty, sourceObj->stk.etf_buy_qty);

    jmethodID jm_setEtfBuyMoney = env->GetMethodID(xtp_market_data_se_class_, "setEtfBuyMoney", "(D)V");
    assert(jm_setEtfBuyMoney != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfBuyMoney, sourceObj->stk.etf_buy_money);

    jmethodID jm_setEtfSellQty = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellQty", "(D)V");
    assert(jm_setEtfSellQty != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfSellQty, sourceObj->stk.etf_sell_qty);

    jmethodID jm_setEtfSellMoney = env->GetMethodID(xtp_market_data_se_class_, "setEtfSellMoney", "(D)V");
    assert(jm_setEtfSellMoney != NULL);
    env->CallVoidMethod(mdseObj, jm_setEtfSellMoney, sourceObj->stk.etf_sell_money);

    jmethodID jm_setTotalWarrantExecQty = env->GetMethodID(xtp_market_data_se_class_, "setTotalWarrantExecQty", "(D)V");
    assert(jm_setTotalWarrantExecQty != NULL);
    env->CallVoidMethod(mdseObj, jm_setTotalWarrantExecQty, sourceObj->stk.total_warrant_exec_qty);

    jmethodID jm_setWarrantLowerPrice = env->GetMethodID(xtp_market_data_se_class_, "setWarrantLowerPrice", "(D)V");
    assert(jm_setWarrantLowerPrice != NULL);
    double new_warrant_lower_price = sourceObj->stk.warrant_lower_price;
    env->CallVoidMethod(mdseObj, jm_setWarrantLowerPrice, new_warrant_lower_price);

    jmethodID jm_setWarrantUpperPrice = env->GetMethodID(xtp_market_data_se_class_, "setWarrantUpperPrice", "(D)V");
    assert(jm_setWarrantUpperPrice != NULL);
    double new_warrant_upper_price = sourceObj->stk.warrant_upper_price;
    env->CallVoidMethod(mdseObj, jm_setWarrantUpperPrice, new_warrant_upper_price);

    jmethodID jm_setCancelBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyCount", "(I)V");
    assert(jm_setCancelBuyCount != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelBuyCount, sourceObj->stk.cancel_buy_count);

    jmethodID jm_setCancelSellCount = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellCount", "(I)V");
    assert(jm_setCancelSellCount != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelSellCount, sourceObj->stk.cancel_sell_count);

    jmethodID jm_setCancelBuyQty = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyQty", "(D)V");
    assert(jm_setCancelBuyQty != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelBuyQty, sourceObj->stk.cancel_buy_qty);

    jmethodID jm_setCancelSellQty = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellQty", "(D)V");
    assert(jm_setCancelSellQty != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelSellQty, sourceObj->stk.cancel_sell_qty);

    jmethodID jm_setCancelBuyMoney = env->GetMethodID(xtp_market_data_se_class_, "setCancelBuyMoney", "(D)V");
    assert(jm_setCancelBuyMoney != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelBuyMoney, sourceObj->stk.cancel_buy_money);

    jmethodID jm_setCancelSellMoney = env->GetMethodID(xtp_market_data_se_class_, "setCancelSellMoney", "(D)V");
    assert(jm_setCancelSellMoney != NULL);
    env->CallVoidMethod(mdseObj, jm_setCancelSellMoney, sourceObj->stk.cancel_sell_money);

    jmethodID jm_setTotalBuyCount = env->GetMethodID(xtp_market_data_se_class_, "setTotalBuyCount", "(J)V");
    env->CallVoidMethod(mdseObj, jm_setTotalBuyCount, sourceObj->stk.total_buy_count);

    jmethodID jm_setTotalSellCount = env->GetMethodID(xtp_market_data_se_class_, "setTotalSellCount", "(J)V");
    env->CallVoidMethod(mdseObj, jm_setTotalSellCount, sourceObj->stk.total_sell_count);

    jmethodID jm_setDurationAfterBuy = env->GetMethodID(xtp_market_data_se_class_, "setDurationAfterBuy", "(I)V");
    assert(jm_setDurationAfterBuy != NULL);
    env->CallVoidMethod(mdseObj, jm_setDurationAfterBuy, sourceObj->stk.duration_after_buy);

    jmethodID jm_setDurationAfterSell = env->GetMethodID(xtp_market_data_se_class_, "setDurationAfterSell", "(I)V");
    assert(jm_setDurationAfterSell != NULL);
    env->CallVoidMethod(mdseObj, jm_setDurationAfterSell, sourceObj->stk.duration_after_sell);

    jmethodID jm_setNumBidOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumBidOrders", "(I)V");
    assert(jm_setNumBidOrders != NULL);
    env->CallVoidMethod(mdseObj, jm_setNumBidOrders, sourceObj->stk.num_bid_orders);

    jmethodID jm_setNumAskOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumAskOrders", "(I)V");
    assert(jm_setNumAskOrders != NULL);
    env->CallVoidMethod(mdseObj, jm_setNumAskOrders, sourceObj->stk.num_ask_orders);

    jmethodID jm_setPreIopv = env->GetMethodID(xtp_market_data_se_class_, "setPreIopv", "(D)V");
    assert(jm_setPreIopv != NULL);
    env->CallVoidMethod(mdseObj, jm_setPreIopv, sourceObj->stk.pre_iopv);
}

void XtpQuote::generateMarketDataOeObj(JNIEnv* env, jobject& mdoeObj, XTPMD *sourceObj) {
    jmethodID jm_setAuctionPrice = env->GetMethodID(xtp_market_data_oe_class_, "setAuctionPrice", "(D)V");
    assert(jm_setAuctionPrice != NULL);
    env->CallVoidMethod(mdoeObj, jm_setAuctionPrice, sourceObj->opt.auction_price);

    jmethodID jm_setAuctionQty = env->GetMethodID(xtp_market_data_oe_class_, "setAuctionQty", "(J)V");
    assert(jm_setAuctionQty != NULL);
    env->CallVoidMethod(mdoeObj, jm_setAuctionQty, sourceObj->opt.auction_qty);

    jmethodID jm_setLastEnquiryTime = env->GetMethodID(xtp_market_data_oe_class_, "setLastEnquiryTime", "(J)V");
    assert(jm_setLastEnquiryTime != NULL);
    env->CallVoidMethod(mdoeObj, jm_setLastEnquiryTime, sourceObj->opt.last_enquiry_time);
}

void XtpQuote::generateMarketDataBeObj(JNIEnv* env, jobject& mdbeObj, XTPMD *sourceObj) {
    jmethodID jm_setTotalBidQty = env->GetMethodID(xtp_market_data_be_class_, "setTotalBidQty", "(J)V");
    env->CallVoidMethod(mdbeObj, jm_setTotalBidQty, sourceObj->bond.total_bid_qty);

    jmethodID jm_setTotalAskQty = env->GetMethodID(xtp_market_data_be_class_, "setTotalAskQty", "(J)V");
    env->CallVoidMethod(mdbeObj, jm_setTotalAskQty, sourceObj->bond.total_ask_qty);

    jmethodID jm_setMaBidPrice = env->GetMethodID(xtp_market_data_be_class_, "setMaBidPrice", "(D)V");
    assert(jm_setMaBidPrice != NULL);
    double new_ma_bid_price = sourceObj->bond.ma_bid_price;
    env->CallVoidMethod(mdbeObj, jm_setMaBidPrice, new_ma_bid_price);

    jmethodID jm_setMaAskPrice = env->GetMethodID(xtp_market_data_be_class_, "setMaAskPrice", "(D)V");
    assert(jm_setMaAskPrice != NULL);
    double new_ma_ask_price = sourceObj->bond.ma_ask_price;
    env->CallVoidMethod(mdbeObj, jm_setMaAskPrice, new_ma_ask_price);

    jmethodID jm_setMaBondBidPrice = env->GetMethodID(xtp_market_data_be_class_, "setMaBondBidPrice", "(D)V");
    assert(jm_setMaBondBidPrice != NULL);
    double new_ma_bond_bid_price = sourceObj->bond.ma_bond_bid_price;
    env->CallVoidMethod(mdbeObj, jm_setMaBondBidPrice, new_ma_bond_bid_price);

    jmethodID jm_setMaBondAskPrice = env->GetMethodID(xtp_market_data_be_class_, "setMaBondAskPrice", "(D)V");
    assert(jm_setMaBondAskPrice != NULL);
    double new_ma_bond_ask_price = sourceObj->bond.ma_bond_ask_price;
    env->CallVoidMethod(mdbeObj, jm_setMaBondAskPrice, new_ma_bond_ask_price);

    jmethodID jm_setYieldToMaturity = env->GetMethodID(xtp_market_data_be_class_, "setYieldToMaturity", "(D)V");
    assert(jm_setYieldToMaturity != NULL);
    env->CallVoidMethod(mdbeObj, jm_setYieldToMaturity, sourceObj->stk.yield_to_maturity);

    jmethodID jm_setCancelBuyCount = env->GetMethodID(xtp_market_data_be_class_, "setCancelBuyCount", "(I)V");
    assert(jm_setCancelBuyCount != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelBuyCount, sourceObj->bond.cancel_buy_count);

    jmethodID jm_setCancelSellCount = env->GetMethodID(xtp_market_data_be_class_, "setCancelSellCount", "(I)V");
    assert(jm_setCancelSellCount != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelSellCount, sourceObj->bond.cancel_sell_count);

    jmethodID jm_setCancelBuyQty = env->GetMethodID(xtp_market_data_be_class_, "setCancelBuyQty", "(D)V");
    assert(jm_setCancelBuyQty != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelBuyQty, sourceObj->bond.cancel_buy_qty);

    jmethodID jm_setCancelSellQty = env->GetMethodID(xtp_market_data_be_class_, "setCancelSellQty", "(D)V");
    assert(jm_setCancelSellQty != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelSellQty, sourceObj->bond.cancel_sell_qty);

    jmethodID jm_setCancelBuyMoney = env->GetMethodID(xtp_market_data_be_class_, "setCancelBuyMoney", "(D)V");
    assert(jm_setCancelBuyMoney != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelBuyMoney, sourceObj->bond.cancel_buy_money);

    jmethodID jm_setCancelSellMoney = env->GetMethodID(xtp_market_data_be_class_, "setCancelSellMoney", "(D)V");
    assert(jm_setCancelSellMoney != NULL);
    env->CallVoidMethod(mdbeObj, jm_setCancelSellMoney, sourceObj->bond.cancel_sell_money);

    jmethodID jm_setTotalBuyCount = env->GetMethodID(xtp_market_data_be_class_, "setTotalBuyCount", "(J)V");
    env->CallVoidMethod(mdbeObj, jm_setTotalBuyCount, sourceObj->bond.total_buy_count);

    jmethodID jm_setTotalSellCount = env->GetMethodID(xtp_market_data_be_class_, "setTotalSellCount", "(J)V");
    env->CallVoidMethod(mdbeObj, jm_setTotalSellCount, sourceObj->bond.total_sell_count);

    jmethodID jm_setDurationAfterBuy = env->GetMethodID(xtp_market_data_be_class_, "setDurationAfterBuy", "(I)V");
    assert(jm_setDurationAfterBuy != NULL);
    env->CallVoidMethod(mdbeObj, jm_setDurationAfterBuy, sourceObj->bond.duration_after_buy);

    jmethodID jm_setDurationAfterSell = env->GetMethodID(xtp_market_data_be_class_, "setDurationAfterSell", "(I)V");
    assert(jm_setDurationAfterSell != NULL);
    env->CallVoidMethod(mdbeObj, jm_setDurationAfterSell, sourceObj->bond.duration_after_sell);

    jmethodID jm_setNumBidOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumBidOrders", "(I)V");
    assert(jm_setNumBidOrders != NULL);
    env->CallVoidMethod(mdbeObj, jm_setNumBidOrders, sourceObj->bond.num_bid_orders);

    jmethodID jm_setNumAskOrders = env->GetMethodID(xtp_market_data_se_class_, "setNumAskOrders", "(I)V");
    assert(jm_setNumAskOrders != NULL);
    env->CallVoidMethod(mdbeObj, jm_setNumAskOrders, sourceObj->bond.num_ask_orders);

    //call setErrorMsg
    jstring jinstrumentStatusStr = env->NewStringUTF(sourceObj->bond.instrument_status);
    jmethodID jm_setInstrumentStatus = env->GetMethodID(xtp_market_data_se_class_, "setInstrumentStatus", "(Ljava/lang/String;)V");
    env->CallVoidMethod(mdbeObj, jm_setInstrumentStatus, jinstrumentStatusStr);
}

void XtpQuote::generateDepthMarketDataExtObj(JNIEnv* env, jobject& rspObj, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
    jlongArray jarray_setBid1Qty = env->NewLongArray(bid1_count);
    env->SetLongArrayRegion(jarray_setBid1Qty, 0, bid1_count, (jlong *)bid1_qty);
    jmethodID jm_setBid1Qty = env->GetMethodID(depth_market_data_ext_class_, "setBid1Qty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setBid1Qty, jarray_setBid1Qty);

    jmethodID jm_setBid1Count = env->GetMethodID(depth_market_data_ext_class_, "setBid1Count", "(I)V");
    assert(jm_setBid1Count != NULL);
    env->CallVoidMethod(rspObj, jm_setBid1Count, bid1_count);

    jmethodID jm_setMaxBid1Count = env->GetMethodID(depth_market_data_ext_class_, "setMaxBid1Count", "(I)V");
    assert(jm_setMaxBid1Count != NULL);
    env->CallVoidMethod(rspObj, jm_setMaxBid1Count, max_bid1_count);

    jlongArray jarray_setAsk1Qty = env->NewLongArray(ask1_count);
    env->SetLongArrayRegion(jarray_setAsk1Qty, 0, ask1_count, (jlong *)ask1_qty);
    jmethodID jm_setAsk1Qty = env->GetMethodID(depth_market_data_ext_class_, "setAsk1Qty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setAsk1Qty, jarray_setAsk1Qty);

    jmethodID jm_setAsk1Count = env->GetMethodID(depth_market_data_ext_class_, "setAsk1Count", "(I)V");
    assert(jm_setAsk1Count != NULL);
    env->CallVoidMethod(rspObj, jm_setAsk1Count, ask1_count);

    jmethodID jm_setMaxAsk1Count = env->GetMethodID(depth_market_data_ext_class_, "setMaxAsk1Count", "(I)V");
    assert(jm_setMaxAsk1Count != NULL);
    env->CallVoidMethod(rspObj, jm_setMaxAsk1Count, max_ask1_count);
}
