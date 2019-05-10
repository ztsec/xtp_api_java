#include "include/XtpQuoteApi.h"
#include "include/glog/logging.h"

XtpQuote::XtpQuote(){
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
}

XtpQuote:: ~XtpQuote(){
    if (api_){
        api_->Release();
    }
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

void XtpQuote::OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) {
    //LOG(INFO) << __PRETTY_FUNCTION__;
    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onDepthMarketData",
    "(Lcom/zts/xtp/quote/model/response/DepthMarketDataResponse;Lcom/zts/xtp/quote/model/response/DepthMarketDataExResponse;)V");


    jobject mdObj = NULL;
    jmethodID mdConstr = env->GetMethodID(xtp_market_data_class_, "<init>","()V");
    if (mdConstr == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }
    mdObj = env->NewObject(xtp_market_data_class_, mdConstr);
    if (mdObj == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }
    generateMarketDataObj(env, mdObj, market_data);

    jobject extObj = NULL;
    jmethodID extConstr = env->GetMethodID(depth_market_data_ext_class_, "<init>","()V");
    if (extConstr == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }
    extObj = env->NewObject(depth_market_data_ext_class_, extConstr);
    if (extObj == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }
    generateDepthMarketDataExtObj(env, extObj, bid1_qty, bid1_count, max_bid1_count, ask1_qty, ask1_count, max_ask1_count);

    env->CallVoidMethod(quote_plugin_obj_, jm_event, mdObj, extObj);
    jvm_->DetachCurrentThread();
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
    //LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
    assert(pluginClass != NULL);
    jmethodID jm_event = env->GetMethodID(pluginClass, "onOrderBook", "(Lcom/zts/xtp/quote/model/response/OrderBookResponse;)V");

    //fetch the default construct
    jmethodID defaultConstr = env->GetMethodID(order_book_class_, "<init>","()V");
    if (defaultConstr == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }

    jobject rspObj=NULL;
    //create the object
    rspObj = env->NewObject(order_book_class_, defaultConstr);
    if (rspObj == NULL) {
       jvm_->DetachCurrentThread();
       return;
    }

    jmethodID jm_setExchangeType= env->GetMethodID(order_book_class_, "setExchangeType", "(I)V");
    assert(jm_setExchangeType != NULL);
    env->CallVoidMethod(rspObj, jm_setExchangeType, order_book->exchange_id);

    jstring jstr_ticker = env->NewStringUTF(order_book->ticker);
    jmethodID jm_setTicker = env->GetMethodID(order_book_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);

    jmethodID jm_setLastPrice = env->GetMethodID(order_book_class_, "setLastPrice", "(D)V");
    assert(jm_setLastPrice != NULL);
    double new_last_price = order_book->last_price;
    env->CallVoidMethod(rspObj, jm_setLastPrice, new_last_price);

    jmethodID jm_setQty= env->GetMethodID(order_book_class_, "setQty", "(J)V");
    env->CallVoidMethod(rspObj, jm_setQty, order_book->qty);

    jmethodID jm_setTurnover = env->GetMethodID(order_book_class_, "setTurnover", "(D)V");
    assert(jm_setTurnover != NULL);
    env->CallVoidMethod(rspObj, jm_setTurnover, order_book->turnover);

    jmethodID jm_setTradesCount= env->GetMethodID(order_book_class_, "setTradesCount", "(J)V");
    env->CallVoidMethod(rspObj, jm_setTradesCount, order_book->trades_count);

    jdoubleArray jarray_setBid = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(jarray_setBid, 0, 10, order_book->bid);
    jmethodID jm_setBid = env->GetMethodID(order_book_class_, "setBid", "([D)V");
    env->CallVoidMethod(rspObj, jm_setBid, jarray_setBid);

    jdoubleArray jarray_setAsk = env->NewDoubleArray(10);
    env->SetDoubleArrayRegion(jarray_setAsk, 0, 10, order_book->ask);
    jmethodID jm_setAsk = env->GetMethodID(order_book_class_, "setAsk", "([D)V");
    env->CallVoidMethod(rspObj, jm_setAsk, jarray_setAsk);

    jlongArray jarray_setBidQty = env->NewLongArray(10);
    env->SetLongArrayRegion(jarray_setBidQty, 0, 10, (jlong *)order_book->bid_qty);
    jmethodID jm_setBidQty = env->GetMethodID(order_book_class_, "setBidQty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setBidQty, jarray_setBidQty);

    jlongArray jarray_setAskQty = env->NewLongArray(10);
    env->SetLongArrayRegion(jarray_setAskQty, 0, 10, (jlong *)order_book->ask_qty);
    jmethodID jm_setAskQty = env->GetMethodID(order_book_class_, "setAskQty", "([J)V");
    env->CallVoidMethod(rspObj, jm_setAskQty, jarray_setAskQty);

    jmethodID jm_setDataTime= env->GetMethodID(order_book_class_, "setDataTime", "(J)V");
    env->CallVoidMethod(rspObj, jm_setDataTime, order_book->data_time);


    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj);
    jvm_->DetachCurrentThread();
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

void XtpQuote::OnTickByTick(XTPTBT *tbt_data) {
    //LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(jvm_ == NULL){
        LOG(ERROR) << "XtpQuoteApi OnTickByTick very fatal error eccored: jvm_ == NULL , this tickByTick Msg dropped." ;
        return;
    }

    if (!bAttachedTickByTick || env==NULL)
    {
        // attach the current thread to the JVM
        jint res = jvm_->AttachCurrentThread((void**)&env, &att_args_);
        if (res != 0) {
            LOG(ERROR) << "XtpQuoteApi OnTickByTick AttachCurrentThread failed." ;
        }
        bAttachedTickByTick = true;
    }

    if (pluginClass == NULL) {
        pluginClass = env->GetObjectClass(quote_plugin_obj_);
        LOG(INFO)  << "OnTickByTick pluginClass is "<< pluginClass ;
    }
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
            if (jm_onTickByTickEntrust == NULL) {
                jm_onTickByTickEntrust = env->GetMethodID(pluginClass, "onTickByTickEntrust", "(ILjava/lang/String;JJIIJDJCC)V");
                LOG(INFO) << "OnTickByTick jm_onTickByTickEntrust is " << jm_onTickByTickEntrust ;
            }
            //cout << "OnTickByTick Entrust:" <<tbt_data->exchange_id<<","<<jstr_ticker<<","<<tbt_data->seq<<","<<tbt_data->data_time<<","<<tbt_data->type<<","<<", subPart="<<
            //	xtp_tbt_entrust->channel_no<<","<<xtp_tbt_entrust->seq<<","<<xtp_tbt_entrust->price<<","<<xtp_tbt_entrust->qty<<","<<xtp_tbt_entrust->side<<","<<xtp_tbt_entrust->ord_type << endl;
            env->CallVoidMethod(quote_plugin_obj_, jm_onTickByTickEntrust,
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
            if (jm_onTickByTickTrade == NULL) {
                jm_onTickByTickTrade = env->GetMethodID(pluginClass, "onTickByTickTrade", "(ILjava/lang/String;JJIIJDJDJJC)V");
                LOG(INFO) << "OnTickByTick jm_onTickByTickTrade is " << jm_onTickByTickTrade ;
            }
            //cout << "OnTickByTick Entrust:" << tbt_data->exchange_id <<","<< jstr_ticker <<","<< tbt_data->seq <<","<< tbt_data->data_time <<","<< tbt_data->type <<","<< ", subPart=" <<
            //	xtp_tbt_trade->channel_no << "," << xtp_tbt_trade->seq<<","<< xtp_tbt_trade->price<<","<< xtp_tbt_trade->qty<<","<< xtp_tbt_trade->money<<","<< xtp_tbt_trade->bid_no<<","<< xtp_tbt_trade->ask_no<<","<< xtp_tbt_trade->trade_flag << endl;
            env->CallVoidMethod(quote_plugin_obj_, jm_onTickByTickTrade,
                                tbt_data->exchange_id, jstr_ticker, tbt_data->seq, tbt_data->data_time, tbt_data->type,
                                xtp_tbt_trade->channel_no, xtp_tbt_trade->seq, xtp_tbt_trade->price, xtp_tbt_trade->qty, xtp_tbt_trade->money, xtp_tbt_trade->bid_no, xtp_tbt_trade->ask_no, xtp_tbt_trade->trade_flag);
            break;
        }
    }















    //==================  原组装对象的方式  效率低  ===================================
//    JNIEnv* env;
//     prepare the invocation
//    env = preInvoke();
//
//    jclass pluginClass = env->GetObjectClass(quote_plugin_obj_);
//    assert(pluginClass != NULL);
//    jmethodID jm_event = env->GetMethodID(pluginClass, "onTickByTick", "(Lcom/zts/xtp/quote/model/response/TickByTickResponse;)V");
//
//    jmethodID defaultConstr = env->GetMethodID(xtp_tick_by_tick_class_, "<init>","()V");
//    if (defaultConstr == NULL) {
//       jvm_->DetachCurrentThread();
//       return;
//    }
//
//    jobject rspObj=NULL;
//    //create the object
//    rspObj = env->NewObject(xtp_tick_by_tick_class_, defaultConstr);
//    if (rspObj == NULL) {
//       jvm_->DetachCurrentThread();
//       return;
//    }
//
//    jmethodID jm_setExchangeType = env->GetMethodID(xtp_tick_by_tick_class_, "setExchangeType", "(I)V");
//    assert(jm_setExchangeType != NULL);
//    env->CallVoidMethod(rspObj, jm_setExchangeType, tbt_data->exchange_id);
//
//    jstring jstr_ticker = env->NewStringUTF(tbt_data->ticker);
//    jmethodID jm_setTicker = env->GetMethodID(xtp_tick_by_tick_class_, "setTicker", "(Ljava/lang/String;)V");
//    env->CallVoidMethod(rspObj, jm_setTicker, jstr_ticker);
//
//    jmethodID jm_setSeq = env->GetMethodID(xtp_tick_by_tick_class_, "setSeq", "(J)V");
//    env->CallVoidMethod(rspObj, jm_setSeq, tbt_data->seq);
//
//    jmethodID jm_setDataTime = env->GetMethodID(xtp_tick_by_tick_class_, "setDataTime", "(J)V");
//    env->CallVoidMethod(rspObj, jm_setDataTime, tbt_data->data_time);
//
//    jmethodID jm_setType = env->GetMethodID(xtp_tick_by_tick_class_, "setType", "(I)V");
//    assert(jm_setType != NULL);
//    env->CallVoidMethod(rspObj, jm_setType, tbt_data->type);
//
//    // set entrust or trade according to tbt type
//    jmethodID jm_inner_setChannelNo;
//    jmethodID jm_inner_setSeq;
//    jmethodID jm_inner_setPrice;
//    jmethodID jm_inner_setQty;
//    jmethodID jm_inner_setSide;
//    jmethodID jm_inner_setOrdType;
//    jmethodID jm_inner_setMoney;
//    jmethodID jm_inner_setBidNo;
//    jmethodID jm_inner_setAskNo;
//    jmethodID jm_inner_setTradeFlag;
//
//    switch(tbt_data->type)
//    {
//        // when type is entrust
//        case XTP_TBT_ENTRUST:
//        {
//            // tbt entrust should not be null
//            XTPTickByTickEntrust *xtp_tbt_entrust = &(tbt_data->entrust);
//            if (xtp_tbt_entrust == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jmethodID defaultConstrEntrust = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "<init>", "()V");
//            if (defaultConstr == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jobject entrustObj = NULL;
//            entrustObj = env->NewObject(xtp_tick_by_tick_entrust_class_, defaultConstrEntrust);
//            if (entrustObj == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jm_inner_setChannelNo = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setChannelNo", "(I)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setChannelNo, xtp_tbt_entrust->channel_no);
//
//            jm_inner_setSeq = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setSeq", "(J)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setSeq, xtp_tbt_entrust->seq);
//
//            jm_inner_setPrice = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setPrice", "(D)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setPrice, xtp_tbt_entrust->price);
//
//            jm_inner_setQty = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setQty", "(J)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setQty, xtp_tbt_entrust->qty);
//
//            jm_inner_setSide = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setSide", "(C)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setSide, xtp_tbt_entrust->side);
//
//            jm_inner_setOrdType = env->GetMethodID(xtp_tick_by_tick_entrust_class_, "setOrdType", "(C)V");
//            env->CallVoidMethod(entrustObj, jm_inner_setOrdType, xtp_tbt_entrust->ord_type);
//
//            jmethodID jm_setEntrust = env->GetMethodID(xtp_tick_by_tick_class_, "setEntrust",
//                "(Lcom/zts/xtp/quote/model/response/TickByTickEntrustResponse;)V");
//            env->CallVoidMethod(rspObj, jm_setEntrust, entrustObj);
//            break;
//        }
//        // when type is trade
//        case XTP_TBT_TRADE:
//        {
//            // tbt trade should not be null
//            XTPTickByTickTrade *xtp_tbt_trade = &(tbt_data->trade);
//            if (xtp_tbt_trade == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jmethodID defaultConstrTrade = env->GetMethodID(xtp_tick_by_tick_trade_class_, "<init>", "()V");
//            if (defaultConstrTrade == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jobject tradeObj = NULL;
//            tradeObj = env->NewObject(xtp_tick_by_tick_trade_class_, defaultConstrTrade);
//            if (tradeObj == NULL)
//            {
//                jvm_->DetachCurrentThread();
//                return;
//            }
//
//            jm_inner_setChannelNo = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setChannelNo", "(I)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setChannelNo, xtp_tbt_trade->channel_no);
//
//            jm_inner_setSeq = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setSeq", "(J)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setSeq, xtp_tbt_trade->seq);
//
//            jm_inner_setPrice = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setPrice", "(D)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setPrice, xtp_tbt_trade->price);
//
//            jm_inner_setQty = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setQty", "(J)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setQty, xtp_tbt_trade->qty);
//
//            jm_inner_setMoney = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setMoney", "(D)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setMoney, jm_inner_setMoney, xtp_tbt_trade->money);
//
//            jm_inner_setBidNo = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setBidNo", "(J)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setBidNo, jm_inner_setBidNo, xtp_tbt_trade->bid_no);
//
//            jm_inner_setAskNo = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setAskNo", "(J)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setAskNo, xtp_tbt_trade->ask_no);
//
//            jm_inner_setTradeFlag = env->GetMethodID(xtp_tick_by_tick_trade_class_, "setTradeFlag", "(C)V");
//            env->CallVoidMethod(tradeObj, jm_inner_setTradeFlag, xtp_tbt_trade->trade_flag);
//
//            jmethodID jm_setTrade = env->GetMethodID(xtp_tick_by_tick_class_, "setTrade",
//                "(Lcom/zts/xtp/quote/model/response/TickByTickTradeResponse;)V");
//            env->CallVoidMethod(rspObj, jm_setTrade, tradeObj);
//            break;
//        }
//    }
//
//    env->CallVoidMethod(quote_plugin_obj_, jm_event, rspObj);
//    jvm_->DetachCurrentThread();
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
