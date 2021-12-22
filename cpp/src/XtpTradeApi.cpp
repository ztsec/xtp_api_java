#include "include/XtpTradeApi.h"
#include "include/glog/logging.h"

Trade::Trade(){
    client_id_ = 255;
    key_ = "";
    file_path_ = "./";

    //initialize the jvm parameters
    att_args_.version = JNI_VERSION_1_8;
    att_args_.name = NULL;
    att_args_.group = NULL;
}

Trade:: ~Trade(){
    if (api_){
       api_->Release();
    }
}

void Trade::OnDisconnected(uint64_t session_id, int reason) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;
    JNIEnv* env;
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_onDisconnect = env->GetMethodID(tradePluginClass_, "onDisconnect", "(Ljava/lang/String;I)V");
    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_onDisconnect, strSessionId, reason);
    jvm_->DetachCurrentThread();
}

void Trade::OnError(XTPRI *error_info) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;
}

void Trade::OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id) {
    std::thread::id tid = std::this_thread::get_id();
    Trade::EnvCatchStruct* envCatchStruct = getCache(tid);
    OnTradeEvent2(trade_info, session_id, envCatchStruct->env, envCatchStruct->jm_eventTrade);
}

void Trade::OnTradeEvent2(XTPTradeReport *trade_info, uint64_t session_id, JNIEnv* env2, jmethodID jm_event2) {
    JNIEnv* envTrade2 = NULL;
    jmethodID jm_eventTrade2 = NULL;

    envTrade2 = env2;
    jm_eventTrade2 = jm_event2;

    long orderXtpIdH = trade_info->order_xtp_id/10;
    long orderXtpIdE = trade_info->order_xtp_id%10;

    uint32_t nTicker = atol(trade_info->ticker);
    uint32_t nTickerLength = strlen(trade_info->ticker);

    long localOrderIdH = trade_info->local_order_id/10;
    long localOrderIdE = trade_info->local_order_id%10;

    jstring execId = envTrade2->NewStringUTF(trade_info->exec_id);

    long reportIndexH = trade_info->report_index/10;
    long reportIndexE = trade_info->report_index%10;

    jstring orderExchId = envTrade2->NewStringUTF(trade_info->order_exch_id);
    jstring branchPbu = envTrade2->NewStringUTF(trade_info->branch_pbu);

    long strSessionIdH = session_id/10;
    long strSessionIdE = session_id%10;

    envTrade2->CallVoidMethod(trade_plugin_obj_, jm_eventTrade2, orderXtpIdH, orderXtpIdE, trade_info->order_client_id, nTicker, nTickerLength, trade_info->market,
                             localOrderIdH, localOrderIdE, execId, trade_info->price, trade_info->quantity, trade_info->trade_time, trade_info->trade_amount,
                             reportIndexH, reportIndexE, orderExchId, trade_info->trade_type, trade_info->side, trade_info->position_effect, trade_info->business_type,
                             branchPbu, 0, true, strSessionIdH, strSessionIdE);

}

void Trade::OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

     JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryOrderResult = env->GetMethodID(tradePluginClass_, "onQueryOrder",
    "(Lcom/zts/xtp/trade/model/response/OrderResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject tradeOrderInfoObj=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(trade_order_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        tradeOrderInfoObj = env->NewObject(trade_order_info_class_, defaultConstr);
        if (tradeOrderInfoObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

       generateOrderInfoObj(env, tradeOrderInfoObj,order_info,request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryOrderResult, tradeOrderInfoObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

     JNIEnv* env;
     // prepare the invocation
     env = preInvoke();
     jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
     assert(tradePluginClass_ != NULL);
     jmethodID jm_queryTradeResult = env->GetMethodID(tradePluginClass_, "onQueryTrade",
     "(Lcom/zts/xtp/trade/model/response/TradeResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject tradeReportObj=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0) {
          //fetch the default construct
         jmethodID defaultConstr = env->GetMethodID(trade_report_class_, "<init>","()V");
         if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
         }

         //create the object
         tradeReportObj = env->NewObject(trade_report_class_, defaultConstr);
         if (tradeReportObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
         }
         generateTradeReportObj(env, tradeReportObj,trade_info,request_id, is_last);
     }

     jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
     env->CallVoidMethod(trade_plugin_obj_, jm_queryTradeResult, tradeReportObj,errorMsgObj, strSessionId);
     jvm_->DetachCurrentThread();

}

void Trade::OnQueryOrderByPage(XTPQueryOrderRsp *order_info, int64_t req_count, int64_t order_sequence, int64_t query_reference, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

     JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryOrderResult = env->GetMethodID(tradePluginClass_, "onQueryOrderByPage",
    "(Lcom/zts/xtp/trade/model/response/OrderResponse;JJJLjava/lang/String;)V");

    jobject tradeOrderInfoObj=NULL;
    //fetch the default construct
    jmethodID defaultConstr = env->GetMethodID(trade_order_info_class_, "<init>","()V");
    if (defaultConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }
    //create the object
    tradeOrderInfoObj = env->NewObject(trade_order_info_class_, defaultConstr);
    if (tradeOrderInfoObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }
    generateOrderInfoObj(env, tradeOrderInfoObj,order_info,request_id, is_last);

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryOrderResult, tradeOrderInfoObj, req_count, order_sequence, query_reference , strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryTradeByPage(XTPQueryTradeRsp *trade_info, int64_t req_count, int64_t trade_sequence, int64_t query_reference, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

     JNIEnv* env;
     // prepare the invocation
     env = preInvoke();
     jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
     assert(tradePluginClass_ != NULL);
     jmethodID jm_queryTradeResult = env->GetMethodID(tradePluginClass_, "onQueryTradeByPage",
     "(Lcom/zts/xtp/trade/model/response/TradeResponse;JJJLjava/lang/String;)V");

     jobject tradeReportObj=NULL;
     //fetch the default construct
     jmethodID defaultConstr = env->GetMethodID(trade_report_class_, "<init>","()V");
     if (defaultConstr == NULL) {
         jvm_->DetachCurrentThread();
         return;
     }
     //create the object
     tradeReportObj = env->NewObject(trade_report_class_, defaultConstr);
     if (tradeReportObj == NULL) {
         jvm_->DetachCurrentThread();
         return;
     }
     generateTradeReportObj(env, tradeReportObj,trade_info,request_id, is_last);

     jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
     env->CallVoidMethod(trade_plugin_obj_, jm_queryTradeResult, tradeReportObj, req_count, trade_sequence, query_reference, strSessionId);
     jvm_->DetachCurrentThread();

}

void Trade::OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    std::thread::id tid = std::this_thread::get_id();
    Trade::EnvCatchStruct* envCatchStruct = getCache(tid);
    OnQueryPosition2(position, error_info, request_id, is_last, session_id, envCatchStruct->env, envCatchStruct->jm_eventPosition);
}

void Trade::OnQueryPosition2(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id, JNIEnv* env2 , jmethodID jm_queryStkPositionResultNew) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    env = env2;

    long strSessionIdH = session_id/10;
    long strSessionIdE = session_id%10;

    int errorCode = error_info->error_id;
    jstring errorMsg = env->NewStringUTF(error_info->error_msg);
    //error_id =0 means successful
    if (errorCode == 0) {

        uint32_t nTicker = atol(position->ticker);
        uint32_t nTickerLength = strlen(position->ticker);

        jstring jtickernamestr = env->NewStringUTF(position->ticker_name);
        int xtpMarketType = (int)position->market;

        env->CallVoidMethod(trade_plugin_obj_, jm_queryStkPositionResultNew, nTicker, nTickerLength, jtickernamestr, xtpMarketType, position->total_qty,
                            position->sellable_qty, position->avg_price, position->unrealized_pnl, position->yesterday_position,
                            position->purchase_redeemable_qty, position->position_direction,
                            position->executable_option, position->lockable_position, position->executable_underlying, position->locked_position,
                            position->usable_locked_position, position->profit_price, position->buy_cost, position->profit_cost,
                            request_id, is_last, errorCode, errorMsg, strSessionIdH, strSessionIdE);
    } else {
        jstring jtickername = env->NewStringUTF("");
        env->CallVoidMethod(trade_plugin_obj_, jm_queryStkPositionResultNew, 0, 0, jtickername, 0, 0, 0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0.0, 0.0, 0.0,
                            request_id, is_last, errorCode, errorMsg, strSessionIdH, strSessionIdE);
    }

}

void Trade::OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    std::thread::id tid = std::this_thread::get_id();
    Trade::EnvCatchStruct* envCatchStruct = getCache(tid);
    OnQueryAsset2(asset, error_info, request_id, is_last, session_id, envCatchStruct->env, envCatchStruct->jm_eventAsset);
}

void Trade::OnQueryAsset2(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id, JNIEnv* env2, jmethodID jm_queryAssetResultNew) {

    JNIEnv* env;
    env = env2;

    long strSessionIdH = session_id/10;
    long strSessionIdE = session_id%10;

    int errorCode = error_info->error_id;
    jstring errorMsg = env->NewStringUTF(error_info->error_msg);
    //error_id =0 means successful
    if (errorCode == 0) {

        env->CallVoidMethod(trade_plugin_obj_, jm_queryAssetResultNew, asset->total_asset, asset->buying_power,
                            asset->security_asset, asset->fund_buy_amount, asset->fund_buy_fee, asset->fund_sell_amount,
                            asset->fund_sell_fee, asset->withholding_amount, asset->account_type, asset->frozen_margin,
                            asset->frozen_exec_cash, asset->frozen_exec_fee, asset->pay_later, asset->preadva_pay, asset->orig_banlance,
                            asset->banlance, asset->deposit_withdraw, asset->trade_netting, asset->captial_asset,
                            asset->force_freeze_amount, asset->preferred_amount, asset->repay_stock_aval_banlance,
                            asset->fund_order_data_charges, asset->fund_cancel_data_charges, request_id, is_last, errorCode, errorMsg, strSessionIdH, strSessionIdE);
    } else {
        env->CallVoidMethod(trade_plugin_obj_, jm_queryAssetResultNew, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0,
                           0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                           request_id, is_last, errorCode, errorMsg, strSessionIdH, strSessionIdE);
    }

}

void Trade::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id) {
    std::thread::id tid = std::this_thread::get_id();
    Trade::EnvCatchStruct* envCatchStruct = getCache(tid);
    OnOrderEvent2(order_info, error_info, session_id, envCatchStruct->env, envCatchStruct->jm_eventOrder);
}

void Trade::OnOrderEvent2(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id, JNIEnv* env2, jmethodID jm_event2) {

    JNIEnv* envOrder2 = NULL;
    jmethodID jm_eventOrder2 = NULL;

    envOrder2 = env2;
    jm_eventOrder2 = jm_event2;

    int errorCode = error_info->error_id;
    jstring errorMsg = env2->NewStringUTF(error_info->error_msg);

    //error_id =0 means successful
    if (errorCode == 0 || NULL != order_info)
    {

        long orderXtpIdH = order_info->order_xtp_id/10;
        long orderXtpIdE = order_info->order_xtp_id%10;

        long orderCancelXtpIdH = order_info->order_cancel_xtp_id/10;
        long orderCancelXtpIdE = order_info->order_cancel_xtp_id%10;

        uint32_t nTicker = atol(order_info->ticker);
        uint32_t nTickerLength = strlen(order_info->ticker);

        jstring orderLocalId = envOrder2->NewStringUTF(order_info->order_local_id);

        long strSessionIdH = session_id/10;
        long strSessionIdE = session_id%10;

        envOrder2->CallVoidMethod(trade_plugin_obj_, jm_eventOrder2, orderXtpIdH, orderXtpIdE, order_info->order_client_id, order_info->order_cancel_client_id,
                                  orderCancelXtpIdH, orderCancelXtpIdE, nTicker, nTickerLength, order_info->market, order_info->price, order_info->quantity, order_info->price_type,
                                  order_info->side, order_info->position_effect, order_info->business_type, order_info->qty_traded, order_info->qty_left,
                                  order_info->insert_time, order_info->update_time, order_info->cancel_time, order_info->trade_amount, orderLocalId,
                                  order_info->order_status, order_info->order_submit_status, order_info->order_type, 0, true, errorCode, errorMsg, strSessionIdH, strSessionIdE);
    } else {
        return;
    }

}

void Trade::OnCancelOrderError(XTPOrderCancelInfo *cancel_info,XTPRI *error_info, uint64_t session_id){
    std::thread::id tid = std::this_thread::get_id();
    Trade::EnvCatchStruct* envCatchStruct = getCache(tid);
    OnCancelOrderError2(cancel_info, error_info, session_id, envCatchStruct->env, envCatchStruct->jm_eventCancelOrderError);
}

void Trade::OnCancelOrderError2(XTPOrderCancelInfo *cancel_info,XTPRI *error_info, uint64_t session_id, JNIEnv* env2, jmethodID jm_event2){
    JNIEnv * env = env2;
    jmethodID jm_onCancelOrderError = jm_event2;

    long orderXtpIdH = 0;
    long orderXtpIdE = 0;

    long orderCancelXtpIdH = 0;
    long orderCancelXtpIdE = 0;

    long strSessionIdH = session_id/10;
    long strSessionIdE = session_id%10;

    int errorCode = error_info->error_id;
    jstring errorMsg = env2->NewStringUTF(error_info->error_msg);

    if (errorCode == 0 || NULL != cancel_info) {
        orderXtpIdH = cancel_info->order_xtp_id/10;
        orderXtpIdE = cancel_info->order_xtp_id%10;

        orderCancelXtpIdH = cancel_info->order_cancel_xtp_id/10;
        orderCancelXtpIdE = cancel_info->order_cancel_xtp_id%10;
    }

    env->CallVoidMethod(trade_plugin_obj_, jm_onCancelOrderError, orderXtpIdH, orderXtpIdE, orderCancelXtpIdH, orderCancelXtpIdE,
                        errorCode, errorMsg, strSessionIdH, strSessionIdE);

}

void Trade::OnInsertAlgoOrder(XTPStrategyInfoStruct* strategy_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onInsertAlgoOrder",
                                          "(Lcom/zts/xtp/trade/model/response/XTPStrategyInfoStruct;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject strategyInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != strategy_info) {
//        LOG(ERROR) << "trade api OnCancelStrategy errorCode == 0";
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_strategy_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        strategyInfo = env->NewObject(xtp_strategy_info_class_, defaultConstr);
        if (strategyInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateXTPStrategyInfoObj(env, strategyInfo, strategy_info, 0, 0, true);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, strategyInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnCancelAlgoOrder(XTPStrategyInfoStruct* strategy_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onCancelAlgoOrder",
                                          "(Lcom/zts/xtp/trade/model/response/XTPStrategyInfoStruct;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject strategyInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != strategy_info) {
//        LOG(ERROR) << "trade api OnCancelStrategy errorCode == 0";
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_strategy_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        strategyInfo = env->NewObject(xtp_strategy_info_class_, defaultConstr);
        if (strategyInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateXTPStrategyInfoObj(env, strategyInfo, strategy_info, 0, 0, true);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, strategyInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryStrategy(XTPStrategyInfoStruct* strategy_info, char* strategy_param, XTPRI *error_info, int32_t request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryStrategy",
                                          "(Lcom/zts/xtp/trade/model/response/XTPStrategyInfoStruct;Ljava/lang/String;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject strategyInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != strategy_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_strategy_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        strategyInfo = env->NewObject(xtp_strategy_info_class_, defaultConstr);
        if (strategyInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateXTPStrategyInfoObj(env, strategyInfo, strategy_info, 0, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    jstring strategyParam = env->NewStringUTF(strategy_param);
    env->CallVoidMethod(trade_plugin_obj_, jm_event, strategyInfo, strategyParam, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnStrategyStateReport(XTPStrategyStateReportStruct *strategy_state, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onStrategyStateReport",
                                          "(Lcom/zts/xtp/trade/model/response/XTPStrategyStateReportStruct;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

    jobject strategyStateReportInfo = NULL;
    jobject errorMsgObj = NULL;

    //error_id =0 means successful
    if (NULL != strategy_state) {
        //generate the error msg object
        //fetch the errormsg default construct
        jmethodID defaultErrorConstr = env->GetMethodID(xtp_error_msg_class_, "<init>","()V");
        if (defaultErrorConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the errormsg object
        errorMsgObj = env->NewObject(xtp_error_msg_class_, defaultErrorConstr);
        if (errorMsgObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateErrorMsgObj(env, errorMsgObj,&strategy_state->m_error_info,0);

        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(xtp_strategy_state_report_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        strategyStateReportInfo = env->NewObject(xtp_strategy_state_report_class_, defaultConstr);
        if (xtp_strategy_state_report_class_ == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateXTPStrategyStateReportObj(env, strategyStateReportInfo, strategy_state);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, strategyStateReportInfo, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if (error_info->error_id != 0) {
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onAlgoUserEstablishChannel",
                                          "(Ljava/lang/String;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jstring juser = env->NewStringUTF(user);
    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, juser,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnAlgoDisconnected(int reason) {
    LOG(ERROR) << __PRETTY_FUNCTION__ << " reason: " << reason;
    JNIEnv* env;
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_onAlgoDisconnect = env->GetMethodID(tradePluginClass_, "onAlgoDisconnected", "(I)V");
    env->CallVoidMethod(trade_plugin_obj_, jm_onAlgoDisconnect, reason);
    jvm_->DetachCurrentThread();
}

void Trade::OnAlgoConnected() {
    LOG(INFO) << __PRETTY_FUNCTION__ ;
    JNIEnv* env;
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_onAlgoConnect = env->GetMethodID(tradePluginClass_, "onAlgoConnected", "()V");
    env->CallVoidMethod(trade_plugin_obj_, jm_onAlgoConnect);
    jvm_->DetachCurrentThread();
}

// 两融业务spi
void Trade::OnCreditCashRepay(XTPCrdCashRepayRsp *cash_repay_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onCreditCashRepay",
     "(Lcom/zts/xtp/trade/model/response/XTPCrdCashRepayRsp;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject crdCashRepayRsp = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != cash_repay_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_cash_repay_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdCashRepayRsp = env->NewObject(crd_cash_repay_rsp_class_, defaultConstr);
        if (crdCashRepayRsp == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdCashRepayRspObj(env, crdCashRepayRsp,cash_repay_info);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdCashRepayRsp,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnCreditCashRepayDebtInterestFee(XTPCrdCashRepayDebtInterestFeeRsp *cash_repay_debt_inter_fee_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onCreditCashRepayDebtInterestFee",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdCashRepayDebtInterestFeeRsp;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject crdCashRepayDebtInterestFeeRsp = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != cash_repay_debt_inter_fee_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdCashRepayDebtInterestFeeRsp = env->NewObject(crd_cash_repay_debt_interest_fee_rsp_class_, defaultConstr);
        if (crdCashRepayDebtInterestFeeRsp == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdCashRepayDebtInterestFeeRspObj(env, crdCashRepayDebtInterestFeeRsp,cash_repay_debt_inter_fee_info);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdCashRepayDebtInterestFeeRsp,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditCashRepayInfo(XTPCrdCashRepayInfo *cash_repay_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditCashRepayInfo",
                                          "(Lcom/zts/xtp/trade/model/response/XTPCrdCashRepayInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdCashRepayInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != cash_repay_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_cash_repay_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdCashRepayInfo = env->NewObject(crd_cash_repay_info_class_, defaultConstr);
        if (crdCashRepayInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdCashRepayInfoObj(env, crdCashRepayInfo,cash_repay_info,request_id,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdCashRepayInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditFundInfo(XTPCrdFundInfo *fund_info, XTPRI *error_info, int request_id, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryCreditFundResult = env->GetMethodID(tradePluginClass_, "onQueryCreditFundInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdFundInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdFundInfo=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != fund_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_credit_fund_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdFundInfo = env->NewObject(query_credit_fund_rsp_class_, defaultConstr);
        if (crdFundInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCreditFundInfoObj(env, crdFundInfo,fund_info,request_id);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryCreditFundResult, crdFundInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditDebtInfo(XTPCrdDebtInfo *debt_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditDebtInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdDebtInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdDebtInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != debt_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_debt_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdDebtInfo = env->NewObject(crd_debt_info_class_, defaultConstr);
        if (crdDebtInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdDebtInfoObj(env, crdDebtInfo,debt_info,request_id,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdDebtInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditTickerDebtInfo(XTPCrdDebtStockInfo *debt_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditTickerDebtInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdDebtStockInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdDebtStockInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != debt_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_debt_stock_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdDebtStockInfo = env->NewObject(crd_debt_stock_info_class_, defaultConstr);
        if (crdDebtStockInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdDebtStockInfoObj(env, crdDebtStockInfo,debt_info,request_id,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdDebtStockInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditAssetDebtInfo(double remain_amount, XTPRI *error_info, int request_id, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryCreditAssetDebtResult = env->GetMethodID(tradePluginClass_, "onQueryCreditAssetDebtInfo",
                                                               "(DLcom/zts/xtp/common/model/ErrorMessage;ILjava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryCreditAssetDebtResult, remain_amount,errorMsgObj, request_id, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditTickerAssignInfo(XTPClientQueryCrdPositionStkInfo *assign_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditTickerAssignInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPClientQueryCrdPositionStkInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdPositionStkInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != assign_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_position_stock_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdPositionStkInfo = env->NewObject(crd_position_stock_info_class_, defaultConstr);
        if (crdPositionStkInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdPositionStockInfoObj(env, crdPositionStkInfo,assign_info,request_id,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdPositionStkInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo *stock_info, XTPRI *error_info, int request_id, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditExcessStock",
    "(Lcom/zts/xtp/trade/model/response/XTPClientQueryCrdSurplusStkRspInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdSurplusStkInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != stock_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_surplus_stock_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdSurplusStkInfo = env->NewObject(crd_surplus_stock_info_class_, defaultConstr);
        if (crdSurplusStkInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdSurplusStockInfoObj(env, crdSurplusStkInfo,stock_info,request_id,true);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdSurplusStkInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryMulCreditExcessStock(XTPClientQueryCrdSurplusStkRspInfo *stock_info, XTPRI *error_info, int request_id, uint64_t session_id, bool is_last) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryMulCreditExcessStock",
    "(Lcom/zts/xtp/trade/model/response/XTPClientQueryCrdSurplusStkRspInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdSurplusStkInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != stock_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_surplus_stock_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdSurplusStkInfo = env->NewObject(crd_surplus_stock_info_class_, defaultConstr);
        if (crdSurplusStkInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdSurplusStockInfoObj(env, crdSurplusStkInfo,stock_info,request_id,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdSurplusStkInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnCreditExtendDebtDate(XTPCreditDebtExtendNotice *debt_extend_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onCreditExtendDebtDate",
    "(Lcom/zts/xtp/trade/model/response/XTPCreditDebtExtendNotice;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject creditDebtExtendNotice = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != debt_extend_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_debt_extend_notice_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        creditDebtExtendNotice = env->NewObject(crd_debt_extend_notice_class_, defaultConstr);
        if (creditDebtExtendNotice == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdDebtExtendNoticeObj(env, creditDebtExtendNotice,debt_extend_info,0,true);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, creditDebtExtendNotice,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditExtendDebtDateOrders(XTPCreditDebtExtendNotice *debt_extend_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditExtendDebtDateOrders",
    "(Lcom/zts/xtp/trade/model/response/XTPCreditDebtExtendNotice;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject creditDebtExtendNotice = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != debt_extend_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_debt_extend_notice_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        creditDebtExtendNotice = env->NewObject(crd_debt_extend_notice_class_, defaultConstr);
        if (creditDebtExtendNotice == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdDebtExtendNoticeObj(env, creditDebtExtendNotice,debt_extend_info,request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, creditDebtExtendNotice,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditFundExtraInfo(XTPCrdFundExtraInfo *fund_info, XTPRI *error_info, int request_id, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditFundExtraInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdFundExtraInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdFundExtraInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != fund_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_fund_extra_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdFundExtraInfo = env->NewObject(crd_fund_extra_info_class_, defaultConstr);
        if (crdFundExtraInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdFundExtraInfoObj(env, crdFundExtraInfo,fund_info,request_id);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdFundExtraInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryCreditPositionExtraInfo(XTPCrdPositionExtraInfo *position_extra_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryCreditPositionExtraInfo",
    "(Lcom/zts/xtp/trade/model/response/XTPCrdPositionExtraInfo;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

    //generate the error msg object
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

    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject crdPositionExtraInfo = NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != position_extra_info) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(crd_position_extra_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        crdPositionExtraInfo = env->NewObject(crd_position_extra_info_class_, defaultConstr);
        if (crdPositionExtraInfo == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }
        generateCrdPositionExtraObj(env, crdPositionExtraInfo,position_extra_info,request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, crdPositionExtraInfo,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

//==================For XTP API 1.16===========================
void Trade::OnQueryStructuredFund(XTPStructuredFundInfo *fund_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (fund_info == NULL) {
        LOG(INFO) << "The fund info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryStructuredFund",
    "(Lcom/zts/xtp/trade/model/response/StructuredFundResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(structured_fund_info_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(structured_fund_info_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateFundInfoObj(env, resultObj, fund_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (fund_transfer_info == NULL) {
        LOG(INFO) << "The fund transfer info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryFundTransfer",
    "(Lcom/zts/xtp/trade/model/response/FundTransferResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
//    if (errorCode == 0) {//必须注释这里
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(fund_transfer_notice_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(fund_transfer_notice_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateFundTransferNoticeObj(env, resultObj, fund_transfer_info, request_id, is_last);
//    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryOtherServerFund(XTPFundQueryRsp *fund_info, XTPRI *error_info, int request_id, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;
    LOG(ERROR) << __PRETTY_FUNCTION__ << " fund_info amount: " << fund_info->amount << " ,query_type: " << fund_info->query_type;

    if(error_info->error_id!=0){
        LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    if (fund_info == NULL) {
        LOG(INFO) << "Query other server fund info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryOtherServerFund",
                                          "(Lcom/zts/xtp/trade/model/response/XTPFundQueryRsp;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

    //generate the error msg object
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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    //fetch the default construct
    jmethodID defaultConstr = env->GetMethodID(fund_query_rsp_class_, "<init>","()V");
    if (defaultConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the object
    resultObj = env->NewObject(fund_query_rsp_class_, defaultConstr);
    if (resultObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateXTPFundQueryRspObj(env, resultObj, fund_info, request_id);

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnFundTransfer(XTPFundTransferNotice *fund_transfer_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (fund_transfer_info == NULL) {
        LOG(INFO) << "The fund transfer info is empty for session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onFundTransfer",
    "(Lcom/zts/xtp/trade/model/response/FundTransferResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
//    if (errorCode == 0) {//这里必须注释掉
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(fund_transfer_notice_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(fund_transfer_notice_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateFundTransferNoticeObj(env, resultObj, fund_transfer_info, 0, true);
//    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryETF(XTPQueryETFBaseRsp *etf_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (etf_info == NULL) {
        LOG(INFO) << "The etf info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryETF",
    "(Lcom/zts/xtp/trade/model/response/ETFBaseResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_etf_base_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(query_etf_base_rsp_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateETFBaseObj(env, resultObj, etf_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryETFBasket(XTPQueryETFComponentRsp *etf_component_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (etf_component_info == NULL) {
        LOG(INFO) << "The component info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryETFBasket",
    "(Lcom/zts/xtp/trade/model/response/ETFComponentResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_etf_component_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(query_etf_component_rsp_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateETFComponentObj(env, resultObj, etf_component_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryIPOInfoList(XTPQueryIPOTickerRsp *ipo_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (ipo_info == NULL) {
        LOG(INFO) << "The ipo info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryIPOInfoList",
    "(Lcom/zts/xtp/trade/model/response/IPOTickerResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj, error_info, request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_ipo_ticker_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(query_ipo_ticker_rsp_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateIPOTickerObj(env, resultObj, ipo_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryIPOQuotaInfo(XTPQueryIPOQuotaRsp *quota_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    if (quota_info == NULL) {
        LOG(INFO) << "The quota info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "onQueryIPOQuotaInfo",
    "(Lcom/zts/xtp/trade/model/response/IPOQuotaResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_ipo_quota_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(query_ipo_quota_rsp_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateIPOQuotaObj(env, resultObj, quota_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryOptionAuctionInfo(XTPQueryOptionAuctionInfoRsp *option_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id){
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
      LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
    }

    if (option_info == NULL) {
        LOG(INFO) << "The option info is empty for request: " << request_id << "; session: " << session_id ;
        return;
    }

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_event = env->GetMethodID(tradePluginClass_, "OnQueryOptionAuctionInfo",
    "(Lcom/zts/xtp/trade/model/response/OptionAuctionInfoResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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
    generateErrorMsgObj(env, errorMsgObj,error_info,request_id);

    jobject resultObj = NULL;
    int errorCode = error_info->error_id;
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_option_auctionInfo_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        //create the object
        resultObj = env->NewObject(query_option_auctionInfo_rsp_class_, defaultConstr);
        if (resultObj == NULL) {
            jvm_->DetachCurrentThread();
            return;
        }

        generateOptionAuctionInfoObj(env, resultObj, option_info, request_id, is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_event, resultObj, errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

//========================================================================

JNIEnv* Trade::preInvoke() {
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

Trade::EnvCatchStruct* Trade::getCache(std::thread::id tid) {
    if(envCatchMap.find(tid) != envCatchMap.end())
    {
        Trade::EnvCatchStruct* catchStruct = envCatchMap.at(tid);
        JNIEnv* envIn;
        envIn = catchStruct->env;
        if (jvm_->GetEnv((void **)&envIn, JNI_VERSION_1_8) != JNI_OK)
        {
            LOG(ERROR) << tid << " envCache find but not ok";
            envIn = preInvoke();
            catchStruct->env = envIn;

            catchStruct->tradePluginClass = envIn->GetObjectClass(trade_plugin_obj_);

            catchStruct->jm_eventTrade = envIn->GetMethodID(catchStruct->tradePluginClass, "onTradeEvent","(JJIIIIJJLjava/lang/String;DJJDJJLjava/lang/String;CIIILjava/lang/String;IZJJ)V");

            catchStruct->jm_eventOrder = envIn->GetMethodID(catchStruct->tradePluginClass, "onOrderEvent","(JJIIJJIIIDJIIIIJJJJJDLjava/lang/String;IICIZILjava/lang/String;JJ)V");

            catchStruct->jm_eventCancelOrderError = envIn->GetMethodID(catchStruct->tradePluginClass, "onCancelOrderError","(JJJJILjava/lang/String;JJ)V");

            catchStruct->jm_eventAsset = envIn->GetMethodID(catchStruct->tradePluginClass, "onQueryAsset","(DDDDDDDDIDDDDDDDDDDDDDDDIZILjava/lang/String;JJ)V");

            catchStruct->jm_eventPosition = envIn->GetMethodID(catchStruct->tradePluginClass, "onQueryPosition","(IILjava/lang/String;IJJDDJJIJJJJJDDDIZILjava/lang/String;JJ)V");

            envCatchMap.at(tid) = catchStruct;
        }
        return catchStruct;
    } else {
        LOG(ERROR) << tid << " envCache not find";
        JNIEnv* envIn = preInvoke();

        jclass tradePluginClassIn = envIn->GetObjectClass(trade_plugin_obj_);

        jmethodID jm_eventTradeIn = envIn->GetMethodID(tradePluginClassIn, "onTradeEvent","(JJIIIIJJLjava/lang/String;DJJDJJLjava/lang/String;CIIILjava/lang/String;IZJJ)V");

        jmethodID jm_eventOrderIn = envIn->GetMethodID(tradePluginClassIn, "onOrderEvent","(JJIIJJIIIDJIIIIJJJJJDLjava/lang/String;IICIZILjava/lang/String;JJ)V");

        jmethodID jm_eventCancelOrderErrorIn = envIn->GetMethodID(tradePluginClassIn, "onCancelOrderError","(JJJJILjava/lang/String;JJ)V");

        jmethodID jm_eventAssetIn = envIn->GetMethodID(tradePluginClassIn, "onQueryAsset","(DDDDDDDDIDDDDDDDDDDDDDDDIZILjava/lang/String;JJ)V");

        jmethodID jm_eventPositionIn = envIn->GetMethodID(tradePluginClassIn, "onQueryPosition","(IILjava/lang/String;IJJDDJJIJJJJJDDDIZILjava/lang/String;JJ)V");

        Trade::EnvCatchStruct* catchStruct = new Trade::EnvCatchStruct();
        catchStruct->env = envIn;
        catchStruct->tradePluginClass = tradePluginClassIn;
        catchStruct->jm_eventTrade = jm_eventTradeIn;
        catchStruct->jm_eventOrder = jm_eventOrderIn;
        catchStruct->jm_eventCancelOrderError = jm_eventCancelOrderErrorIn;
        catchStruct->jm_eventAsset = jm_eventAssetIn;
        catchStruct->jm_eventPosition = jm_eventPositionIn;
        envCatchMap[tid] = catchStruct;
        return catchStruct;
    }

}

void Trade::generateOrderInfoObj(JNIEnv* env, jobject& tradeOrderInfoObj,XTPOrderInfo *order_info,int request_id, bool is_last) {
    //call setOrderXtpId
    jmethodID jm_setOrderXtpId = env->GetMethodID(trade_order_info_class_, "setOrderXtpId", "(Ljava/lang/String;)V");
    assert(jm_setOrderXtpId != NULL);
    jstring orderXtpIdStr = env->NewStringUTF((std::to_string(order_info->order_xtp_id)).c_str());
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderXtpId, orderXtpIdStr);

    //call setOrderClientId
    jmethodID jm_setOrderClientId = env->GetMethodID(trade_order_info_class_, "setOrderClientId", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderClientId, order_info->order_client_id);

    //call setOrderClientId
    jmethodID jm_setOrderCancelClientId = env->GetMethodID(trade_order_info_class_, "setOrderCancelClientId", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderCancelClientId, order_info->order_cancel_client_id);

    //call setOrderCancelXtpId
    jmethodID jm_setOrderCancelXtpId = env->GetMethodID(trade_order_info_class_, "setOrderCancelXtpId", "(Ljava/lang/String;)V");
    jstring orderCancelXtpIdStr = env->NewStringUTF((std::to_string(order_info->order_cancel_xtp_id)).c_str());
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderCancelXtpId, orderCancelXtpIdStr);

    //call setTicker
    jmethodID jm_setTicker = env->GetMethodID(trade_order_info_class_, "setTicker", "(Ljava/lang/String;)V");
    jstring jticker = env->NewStringUTF(order_info->ticker);
    env->CallVoidMethod(tradeOrderInfoObj, jm_setTicker, jticker);

    //call setMarketType
    int xtpMarketType = (int)order_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(trade_order_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setMarketType, xtpMarketType);

    //call setPrice
    jmethodID jm_setPrice = env->GetMethodID(trade_order_info_class_, "setPrice", "(D)V");
    double new_price = order_info->price;
    env->CallVoidMethod(tradeOrderInfoObj, jm_setPrice, new_price);

    //call setQuantity
    jmethodID jm_setQuantity = env->GetMethodID(trade_order_info_class_, "setQuantity", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setQuantity, order_info->quantity);

    //call setPriceType
    int xtpPriceType = (int)order_info->price_type;
    jmethodID jm_setPriceType = env->GetMethodID(trade_order_info_class_, "setPriceType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setPriceType, xtpPriceType);

    //call setSideType
    int xtpSideType = (int)order_info->side;
    jmethodID jm_setSideType = env->GetMethodID(trade_order_info_class_, "setSideType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setSideType, xtpSideType);

    //call setPositionEffectType
    int positionEffect = (int)order_info->position_effect;
//    LOG(INFO)<< "generateOrderInfoObj: position_effect" << order_info->position_effect << "; positionEffect: " << positionEffect;
    jmethodID jm_setPositionEffectType = env->GetMethodID(trade_order_info_class_, "setPositionEffectType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setPositionEffectType, positionEffect);

    //call setBusinessType
    int xtpBusinessType = (int)order_info->business_type;
    jmethodID jm_setBusinessType = env->GetMethodID(trade_order_info_class_, "setBusinessType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setBusinessType, xtpBusinessType);

    //call setQtyTraded
    jmethodID jm_setQtyTraded = env->GetMethodID(trade_order_info_class_, "setQtyTraded", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setQtyTraded, order_info->qty_traded);

    //call setQtyLeft
    jmethodID jm_setQtyLeft = env->GetMethodID(trade_order_info_class_, "setQtyLeft", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setQtyLeft, order_info->qty_left);

    //call setInsertTime
    jmethodID jm_setInsertTime = env->GetMethodID(trade_order_info_class_, "setInsertTime", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setInsertTime, order_info->insert_time);

    //call setUpdateTime
    jmethodID jm_setUpdateTime = env->GetMethodID(trade_order_info_class_, "setUpdateTime", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setUpdateTime, order_info->update_time);

    //call setCancelTime
    jmethodID jm_setCancelTime = env->GetMethodID(trade_order_info_class_, "setCancelTime", "(J)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setCancelTime, order_info->cancel_time);

    //call setTradeAmount
    jmethodID jm_setTradeAmount = env->GetMethodID(trade_order_info_class_, "setTradeAmount", "(D)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setTradeAmount, order_info->trade_amount);

    //call setOrderLocalId
    jstring jorderLocalId = env->NewStringUTF(order_info->order_local_id);
    jmethodID jm_setOrderLocalId = env->GetMethodID(trade_order_info_class_, "setOrderLocalId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderLocalId, jorderLocalId);

    //call setOrderStatusType
    int xtpOrderStatus = (int)order_info->order_status;
    jmethodID jm_setOrderStatusType = env->GetMethodID(trade_order_info_class_, "setOrderStatusType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderStatusType, xtpOrderStatus);

    //call setOrderSubmitStatusType
    int xtpOrderSubmitStatusType = (int)order_info->order_submit_status;
    jmethodID jm_setOrderSubmitStatusType = env->GetMethodID(trade_order_info_class_, "setOrderSubmitStatusType", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderSubmitStatusType, xtpOrderSubmitStatusType);

    //call setTxtPOrderTypeType
    jmethodID jm_setTxtPOrderTypeType = env->GetMethodID(trade_order_info_class_, "setTxtPOrderTypeType", "(C)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setTxtPOrderTypeType, order_info->order_type);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(trade_order_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(tradeOrderInfoObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(trade_order_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(tradeOrderInfoObj, jm_setLastResp,is_last);

}

void  Trade::generateTradeReportObj(JNIEnv* env, jobject& tradeReportObj,XTPTradeReport *trade_info,int request_id, bool is_last) {
    //call setOrderXtpId
    jmethodID jm_setOrderXtp = env->GetMethodID(trade_report_class_, "setOrderXtpId", "(Ljava/lang/String;)V");
    assert(jm_setOrderXtp != NULL);
    jstring orderXtpIdStr = env->NewStringUTF((std::to_string(trade_info->order_xtp_id)).c_str());
    env->CallVoidMethod(tradeReportObj, jm_setOrderXtp, orderXtpIdStr);

    //call setOrderClientId
    jmethodID jm_setOrderClient = env->GetMethodID(trade_report_class_, "setOrderClientId", "(I)V");
    assert(jm_setOrderClient != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setOrderClient, trade_info->order_client_id);

    //call setTicker
    jstring jtickerstr = env->NewStringUTF(trade_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(trade_report_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(tradeReportObj, jm_setTicker, jtickerstr);

    //call setMarketType
    int xtpMarketType = (int)trade_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(trade_report_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(tradeReportObj, jm_setMarketType, xtpMarketType);

    //call setLocalOrder
    jmethodID jm_setLocalOrder = env->GetMethodID(trade_report_class_, "setLocalOrderId", "(Ljava/lang/String;)V");
    assert(jm_setLocalOrder != NULL);
    jstring localOrderXtpIdStr = env->NewStringUTF((std::to_string(trade_info->local_order_id)).c_str());
    env->CallVoidMethod(tradeReportObj, jm_setLocalOrder, localOrderXtpIdStr);

    //call setExecId
    jstring jExecIdstr = env->NewStringUTF(trade_info->exec_id);
    jmethodID jm_setExecId = env->GetMethodID(trade_report_class_, "setExecId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(tradeReportObj, jm_setExecId, jExecIdstr);

    //call setPrice
    jmethodID jm_setPrice = env->GetMethodID(trade_report_class_, "setPrice", "(D)V");
    assert(jm_setPrice != NULL);
    double new_price = trade_info->price;
    env->CallVoidMethod(tradeReportObj, jm_setPrice, new_price);

    //call setQuantity
    jmethodID jm_setQuantity = env->GetMethodID(trade_report_class_, "setQuantity", "(J)V");
    assert(jm_setQuantity != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setQuantity, trade_info->quantity);

    //call setTradeTime
    jmethodID jm_setTradeTime = env->GetMethodID(trade_report_class_, "setTradeTime", "(J)V");
    assert(jm_setTradeTime != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setTradeTime, trade_info->trade_time);

    //call setTradeAmount
    jmethodID jm_setTradeAmount = env->GetMethodID(trade_report_class_, "setTradeAmount", "(D)V");
    assert(jm_setTradeAmount != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setTradeAmount, trade_info->trade_amount);

    //call setReportIndex
    jmethodID jm_setReportIndex = env->GetMethodID(trade_report_class_, "setReportIndex", "(Ljava/lang/String;)V");
    assert(jm_setReportIndex != NULL);
    jstring reportIndexStr = env->NewStringUTF((std::to_string(trade_info->report_index)).c_str());
    env->CallVoidMethod(tradeReportObj, jm_setReportIndex, reportIndexStr);

    //call setOrderExchId
    jstring jOrderExecIdstr = env->NewStringUTF(trade_info->order_exch_id);
    jmethodID jm_setOrderExchId = env->GetMethodID(trade_report_class_, "setOrderExchId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(tradeReportObj, jm_setOrderExchId, jOrderExecIdstr);

    //call setTradeType
    jmethodID jm_setTradeType = env->GetMethodID(trade_report_class_, "setTradeType", "(C)V");
    env->CallVoidMethod(tradeReportObj, jm_setTradeType, trade_info->trade_type);

    //call setSideType
    int xtpSideType = (int)trade_info->side;
    jmethodID jm_setSide = env->GetMethodID(trade_report_class_, "setSideType", "(I)V");
    env->CallVoidMethod(tradeReportObj, jm_setSide, xtpSideType);

    //call setPositionEffectType
    int positionEffect = (int)trade_info->position_effect;
    jmethodID jm_setPositionEffectType = env->GetMethodID(trade_report_class_, "setPositionEffectType", "(I)V");
    env->CallVoidMethod(tradeReportObj, jm_setPositionEffectType, positionEffect);

    //call setBusinessType
    int xtpBusinessType = (int)trade_info->business_type;
    jmethodID jm_setBusiness = env->GetMethodID(trade_report_class_, "setBusinessType", "(I)V");
    env->CallVoidMethod(tradeReportObj, jm_setBusiness, xtpBusinessType);

    //call setBranchPbu
    jstring jBranchPbustr = env->NewStringUTF(trade_info->branch_pbu);
    jmethodID jm_setBranchPbu = env->GetMethodID(trade_report_class_, "setBranchPbu", "(Ljava/lang/String;)V");
    env->CallVoidMethod(tradeReportObj, jm_setBranchPbu, jBranchPbustr);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(trade_report_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(trade_report_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(tradeReportObj, jm_setLastResp,is_last);

}

void Trade::generateXTPStrategyInfoObj(JNIEnv* env, jobject& targetObj,XTPStrategyInfoStruct *strategy_info,uint64_t child_order_xtp_id, int request_id, bool is_last) {

    //call setMStrategyType
    jmethodID jm_setMStrategyType = env->GetMethodID(xtp_strategy_info_class_, "setMStrategyType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyType, strategy_info->m_strategy_type);

    //call setMClientStrategyId
    jmethodID jm_setMClientStrategyId = env->GetMethodID(xtp_strategy_info_class_, "setMClientStrategyId", "(Ljava/lang/String;)V");
    jstring mClientStrategyId = env->NewStringUTF((std::to_string(strategy_info->m_client_strategy_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setMClientStrategyId, mClientStrategyId);

    //call setMXtpStrategyId
    jmethodID jm_setMXtpStrategyId = env->GetMethodID(xtp_strategy_info_class_, "setMXtpStrategyId", "(Ljava/lang/String;)V");
    jstring mXtpStrategyId = env->NewStringUTF((std::to_string(strategy_info->m_xtp_strategy_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setMXtpStrategyId, mXtpStrategyId);

    //call setMStrategyState
    jmethodID jm_setMStrategyState = env->GetMethodID(xtp_strategy_info_class_, "setMStrategyState", "(I)V");
    int strategyState = (int)strategy_info->m_strategy_state;
    env->CallVoidMethod(targetObj, jm_setMStrategyState, strategyState);

    //call setChildOrderXtpId
    jmethodID jm_setChildOrderXtpId = env->GetMethodID(xtp_strategy_info_class_, "setChildOrderXtpId", "(Ljava/lang/String;)V");
    jstring childOrderXtpId = env->NewStringUTF((std::to_string(child_order_xtp_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setChildOrderXtpId, childOrderXtpId);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(xtp_strategy_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(xtp_strategy_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateXTPStrategyStateReportObj(JNIEnv* env, jobject& targetObj,XTPStrategyStateReportStruct *strategy_state) {

    //call setMStrategyType
    jmethodID jm_setMStrategyType = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyType, strategy_state->m_strategy_info.m_strategy_type);

    //call setMClientStrategyId
    jmethodID jm_setMClientStrategyId = env->GetMethodID(xtp_strategy_state_report_class_, "setMClientStrategyId", "(Ljava/lang/String;)V");
    jstring mClientStrategyId = env->NewStringUTF((std::to_string(strategy_state->m_strategy_info.m_client_strategy_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setMClientStrategyId, mClientStrategyId);

    //call setMXtpStrategyId
    jmethodID jm_setMXtpStrategyId = env->GetMethodID(xtp_strategy_state_report_class_, "setMXtpStrategyId", "(Ljava/lang/String;)V");
    jstring mXtpStrategyId = env->NewStringUTF((std::to_string(strategy_state->m_strategy_info.m_xtp_strategy_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setMXtpStrategyId, mXtpStrategyId);

    //call setMStrategyState
    jmethodID jm_setMStrategyState = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyState", "(I)V");
    int strategyState = (int)strategy_state->m_strategy_info.m_strategy_state;
    env->CallVoidMethod(targetObj, jm_setMStrategyState, strategyState);

    //call setMStrategyQty
    jmethodID jm_setMStrategyQty = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyQty, strategy_state->m_strategy_qty);

    //call setMStrategyOrderedQty
    jmethodID jm_setMStrategyOrderedQty = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyOrderedQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyOrderedQty, strategy_state->m_strategy_ordered_qty);

    //call setMStrategyCancelledQty
    jmethodID jm_setMStrategyCancelledQty = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyCancelledQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyCancelledQty, strategy_state->m_strategy_cancelled_qty);

    //call setMStrategyExecutionQty
    jmethodID jm_setMStrategyExecutionQty = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyExecutionQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyExecutionQty, strategy_state->m_strategy_execution_qty);

    //call setMStrategyUnclosedQty
    jmethodID jm_setMStrategyUnclosedQty = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyUnclosedQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyUnclosedQty, strategy_state->m_strategy_unclosed_qty);

    //call setMStrategyAsset
    jmethodID jm_setMStrategyAsset = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyAsset", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyAsset, strategy_state->m_strategy_asset);

    //call setMStrategyOrderedAsset
    jmethodID jm_setMStrategyOrderedAsset = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyOrderedAsset", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyOrderedAsset, strategy_state->m_strategy_ordered_asset);

    //call setMStrategyExecutionAsset
    jmethodID jm_setMStrategyExecutionAsset = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyExecutionAsset", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyExecutionAsset, strategy_state->m_strategy_execution_asset);

    //call setMStrategyExecutionPrice
    jmethodID jm_setMStrategyExecutionPrice = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyExecutionPrice", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyExecutionPrice, strategy_state->m_strategy_execution_price);

    //call setMStrategyMarketPrice
    jmethodID jm_setMStrategyMarketPrice = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyMarketPrice", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyMarketPrice, strategy_state->m_strategy_market_price);

    //call setMStrategyPriceDiff
    jmethodID jm_setMStrategyPriceDiff = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyPriceDiff", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyPriceDiff, strategy_state->m_strategy_price_diff);

    //call setMStrategyAssetDiff
    jmethodID jm_setMStrategyAssetDiff = env->GetMethodID(xtp_strategy_state_report_class_, "setMStrategyAssetDiff", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMStrategyAssetDiff, strategy_state->m_strategy_asset_diff);

}

void Trade::generateCrdCashRepayRspObj(JNIEnv* env, jobject& crdCashRepayRspObj, XTPCrdCashRepayRsp *crd_cash_repay_rsp) {
    //call setXtpId
    jmethodID jm_setXtpId = env->GetMethodID(crd_cash_repay_rsp_class_, "setXtpId", "(Ljava/lang/String;)V");
    jstring xtpId = env->NewStringUTF((std::to_string(crd_cash_repay_rsp->xtp_id)).c_str());
    env->CallVoidMethod(crdCashRepayRspObj, jm_setXtpId, xtpId);

    //call setRequestAmount
    jmethodID jm_setRequestAmount = env->GetMethodID(crd_cash_repay_rsp_class_, "setRequestAmount", "(D)V");
    env->CallVoidMethod(crdCashRepayRspObj, jm_setRequestAmount, crd_cash_repay_rsp->request_amount);

    //call setCashRepayAmount
    jmethodID jm_setCashRepayAmount = env->GetMethodID(crd_cash_repay_rsp_class_, "setCashRepayAmount", "(D)V");
    env->CallVoidMethod(crdCashRepayRspObj, jm_setCashRepayAmount, crd_cash_repay_rsp->cash_repay_amount);

}

void Trade::generateCrdCashRepayDebtInterestFeeRspObj(JNIEnv* env, jobject& targetObj, XTPCrdCashRepayDebtInterestFeeRsp *crd_cash_repay_debt_rsp) {
    //call setXtpId
    jmethodID jm_setXtpId = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "setXtpId", "(Ljava/lang/String;)V");
    jstring xtpId = env->NewStringUTF((std::to_string(crd_cash_repay_debt_rsp->xtp_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setXtpId, xtpId);

    //call setRequestAmount
    jmethodID jm_setRequestAmount = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "setRequestAmount", "(D)V");
    env->CallVoidMethod(targetObj, jm_setRequestAmount, crd_cash_repay_debt_rsp->request_amount);

    //call setCashRepayAmount
    jmethodID jm_setCashRepayAmount = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "setCashRepayAmount", "(D)V");
    env->CallVoidMethod(targetObj, jm_setCashRepayAmount, crd_cash_repay_debt_rsp->cash_repay_amount);

    //call setDebtCompactId
    jstring jdebtCompactId = env->NewStringUTF(crd_cash_repay_debt_rsp->debt_compact_id);
    jmethodID jm_setDebtCompactId = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "setDebtCompactId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setDebtCompactId, jdebtCompactId);

    //call setUnknow
    jstring junknow = env->NewStringUTF(crd_cash_repay_debt_rsp->unknow);
    jmethodID jm_setUnknow = env->GetMethodID(crd_cash_repay_debt_interest_fee_rsp_class_, "setUnknow", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setUnknow, junknow);

}

void Trade::generateCrdCashRepayInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdCashRepayInfo *cash_repay_info,int request_id, bool is_last) {
    //call setXtpId
    jmethodID jm_setXtpId = env->GetMethodID(crd_cash_repay_info_class_, "setXtpId", "(Ljava/lang/String;)V");
    assert(jm_setXtpId != NULL);
    jstring xtpId = env->NewStringUTF((std::to_string(cash_repay_info->xtp_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setXtpId, xtpId);

    //call setCrdCrStatus
    int crdCrStatus = (int)cash_repay_info->status;
    jmethodID jm_setCrdCrStatus = env->GetMethodID(crd_cash_repay_info_class_, "setCrdCrStatus", "(I)V");
    env->CallVoidMethod(targetObj, jm_setCrdCrStatus, crdCrStatus);

    //call setRequestAmount
    jmethodID jm_setRequestAmount = env->GetMethodID(crd_cash_repay_info_class_, "setRequestAmount", "(D)V");
    env->CallVoidMethod(targetObj, jm_setRequestAmount, cash_repay_info->request_amount);

    //call setCashRepayAmount
    jmethodID jm_setCashRepayAmount = env->GetMethodID(crd_cash_repay_info_class_, "setCashRepayAmount", "(D)V");
    env->CallVoidMethod(targetObj, jm_setCashRepayAmount, cash_repay_info->cash_repay_amount);

    //call setPositionEffectType
    int positionEffect = (int)cash_repay_info->position_effect;
    jmethodID jm_setPositionEffectType = env->GetMethodID(crd_cash_repay_info_class_, "setPositionEffectType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setPositionEffectType, positionEffect);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_cash_repay_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_cash_repay_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateCreditFundInfoObj(JNIEnv* env, jobject& crdFundInfoObj, XTPCrdFundInfo *fund_info, int request_id) {
    //call setMaintenanceRatio
    jmethodID jm_setMaintenanceRatio = env->GetMethodID(query_credit_fund_rsp_class_, "setMaintenanceRatio", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setMaintenanceRatio, fund_info->maintenance_ratio);

    //call setAllAsset
    jmethodID jm_setAllAsset = env->GetMethodID(query_credit_fund_rsp_class_, "setAllAsset", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setAllAsset, fund_info->all_asset);

    //call setAllDebt
    jmethodID jm_setAllDebt = env->GetMethodID(query_credit_fund_rsp_class_, "setAllDebt", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setAllDebt, fund_info->all_debt);

    //call setLineOfCredit
    jmethodID jm_setLineOfCredit = env->GetMethodID(query_credit_fund_rsp_class_, "setLineOfCredit", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setLineOfCredit, fund_info->line_of_credit);

    //call setGuaranty
    jmethodID jm_setGuaranty = env->GetMethodID(query_credit_fund_rsp_class_, "setGuaranty", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setGuaranty, fund_info->guaranty);

    //call setReserved
    jmethodID jm_setReserved = env->GetMethodID(query_credit_fund_rsp_class_, "setReserved", "(D)V");
    env->CallVoidMethod(crdFundInfoObj, jm_setReserved, fund_info->reserved);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(query_credit_fund_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(crdFundInfoObj,jm_setRequestId, request_id);

}

void Trade::generateCrdDebtInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdDebtInfo *debt_info,int request_id, bool is_last) {
    //call setDebtType
    jmethodID jm_setDebtType = env->GetMethodID(crd_debt_info_class_, "setDebtType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setDebtType, debt_info->debt_type);

    //call setDebtId
    jstring jdebtIdstr = env->NewStringUTF(debt_info->debt_id);
    jmethodID jm_setDebtId = env->GetMethodID(crd_debt_info_class_, "setDebtId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setDebtId, jdebtIdstr);

    //call setPositionId
    jmethodID jm_setPositionId = env->GetMethodID(crd_debt_info_class_, "setPositionId", "(Ljava/lang/String;)V");
    jstring positionId = env->NewStringUTF((std::to_string(debt_info->position_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setPositionId, positionId);

    //call setOrderXtpId
    jmethodID jm_setOrderXtpId = env->GetMethodID(crd_debt_info_class_, "setOrderXtpId", "(Ljava/lang/String;)V");
    assert(jm_setOrderXtpId != NULL);
    jstring orderXtpIdStr = env->NewStringUTF((std::to_string(debt_info->order_xtp_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setOrderXtpId, orderXtpIdStr);

    //call setDebtStatus
    jmethodID jm_setDebtStatus = env->GetMethodID(crd_debt_info_class_, "setDebtStatus", "(I)V");
    env->CallVoidMethod(targetObj, jm_setDebtStatus, debt_info->debt_status);

    //call setMarketType
    int marketType = (int)debt_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(crd_debt_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, marketType);

    //call setTicker
    jstring jticker = env->NewStringUTF(debt_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(crd_debt_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jticker);

    //call setOrderDate
    jmethodID jm_setOrderDate = env->GetMethodID(crd_debt_info_class_, "setOrderDate", "(Ljava/lang/String;)V");
    jstring orderDate = env->NewStringUTF((std::to_string(debt_info->order_date)).c_str());
    env->CallVoidMethod(targetObj, jm_setOrderDate, orderDate);

    //call setEndDate
    jmethodID jm_setEndDate = env->GetMethodID(crd_debt_info_class_, "setEndDate", "(Ljava/lang/String;)V");
    jstring endDate = env->NewStringUTF((std::to_string(debt_info->end_date)).c_str());
    env->CallVoidMethod(targetObj, jm_setEndDate, endDate);

    //call setOrigEndDate
    jmethodID jm_setOrigEndDate = env->GetMethodID(crd_debt_info_class_, "setOrigEndDate", "(Ljava/lang/String;)V");
    jstring origEndDate = env->NewStringUTF((std::to_string(debt_info->orig_end_date)).c_str());
    env->CallVoidMethod(targetObj, jm_setOrigEndDate, origEndDate);

    //call setIsExtended isExtended
    bool isExtended = (bool)debt_info->is_extended;
    jmethodID jm_setIsExtended = env->GetMethodID(crd_debt_info_class_, "setIsExtended", "(Z)V");
    env->CallVoidMethod(targetObj, jm_setIsExtended, isExtended);

    //call setRemainAmt
    jmethodID jm_setRemainAmt = env->GetMethodID(crd_debt_info_class_, "setRemainAmt", "(D)V");
    env->CallVoidMethod(targetObj, jm_setRemainAmt, debt_info->remain_amt);

    //call setRemainQty
    jmethodID jm_setRemainQty = env->GetMethodID(crd_debt_info_class_, "setRemainQty", "(Ljava/lang/String;)V");
    jstring remainQty = env->NewStringUTF((std::to_string(debt_info->remain_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setRemainQty, remainQty);

    //call setRemainPrincipal
    jmethodID jm_setRemainPrincipal = env->GetMethodID(crd_debt_info_class_, "setRemainPrincipal", "(D)V");
    env->CallVoidMethod(targetObj, jm_setRemainPrincipal, debt_info->remain_principal);

    //call setDueRightQty
    jmethodID jm_setDueRightQty = env->GetMethodID(crd_debt_info_class_, "setDueRightQty", "(Ljava/lang/String;)V");
    jstring dueRightQty = env->NewStringUTF((std::to_string(debt_info->due_right_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setDueRightQty, dueRightQty);

    //call setUnknown
    jmethodID jm_setUnknown = env->GetMethodID(crd_debt_info_class_, "setUnknown", "(J)V");
    env->CallVoidMethod(targetObj, jm_setUnknown, debt_info->unknown);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_debt_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_debt_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateCrdDebtStockInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdDebtStockInfo *debt_info,int request_id, bool is_last) {

    //call setMarketType
    int marketType = (int)debt_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(crd_debt_stock_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, marketType);

    //call setTicker
    jstring jtickerstr = env->NewStringUTF(debt_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(crd_debt_stock_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    //call setStockRepayQuantity
    jmethodID jm_setStockRepayQuantity = env->GetMethodID(crd_debt_stock_info_class_, "setStockRepayQuantity", "(Ljava/lang/String;)V");
    jstring stockRepayQuantity = env->NewStringUTF((std::to_string(debt_info->stock_repay_quantity)).c_str());
    env->CallVoidMethod(targetObj, jm_setStockRepayQuantity, stockRepayQuantity);

    //call setStockTotalQuantity
    jmethodID jm_setStockTotalQuantity = env->GetMethodID(crd_debt_stock_info_class_, "setStockTotalQuantity", "(Ljava/lang/String;)V");
    jstring stockTotalQuantity = env->NewStringUTF((std::to_string(debt_info->stock_total_quantity)).c_str());
    env->CallVoidMethod(targetObj, jm_setStockTotalQuantity, stockTotalQuantity);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_debt_stock_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_debt_stock_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateCrdPositionStockInfoObj(JNIEnv* env, jobject& targetObj,XTPClientQueryCrdPositionStkInfo *assign_info,int request_id, bool is_last) {

    //call setMarketType
    int marketType = (int)assign_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(crd_position_stock_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, marketType);

    //call setTicker
    jstring jtickerstr = env->NewStringUTF(assign_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(crd_position_stock_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    //call setLimitQty
    jmethodID jm_setLimitQty = env->GetMethodID(crd_position_stock_info_class_, "setLimitQty", "(Ljava/lang/String;)V");
    assert(jm_setLimitQty != NULL);
    jstring limitQty = env->NewStringUTF((std::to_string(assign_info->limit_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setLimitQty, limitQty);

    //call setYesterdayQty
    jmethodID jm_setYesterdayQty = env->GetMethodID(crd_position_stock_info_class_, "setYesterdayQty", "(Ljava/lang/String;)V");
    jstring yesterdayQty = env->NewStringUTF((std::to_string(assign_info->yesterday_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setYesterdayQty, yesterdayQty);

    //call setLeftQty
    jmethodID jm_setLeftQty = env->GetMethodID(crd_position_stock_info_class_, "setLeftQty", "(Ljava/lang/String;)V");
    jstring leftQty = env->NewStringUTF((std::to_string(assign_info->left_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setLeftQty, leftQty);

    //call setFrozenQty
    jmethodID jm_setFrozenQty = env->GetMethodID(crd_position_stock_info_class_, "setFrozenQty", "(Ljava/lang/String;)V");
    jstring frozenQty = env->NewStringUTF((std::to_string(assign_info->frozen_qty)).c_str());
    env->CallVoidMethod(targetObj, jm_setFrozenQty, frozenQty);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_position_stock_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_position_stock_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateCrdSurplusStockInfoObj(JNIEnv* env, jobject& targetObj,XTPClientQueryCrdSurplusStkRspInfo *stock_info,int request_id,bool is_last) {

    //call setMarketType
    int marketType = (int)stock_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(crd_surplus_stock_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, marketType);

    //call setTicker
    jstring jtickerstr = env->NewStringUTF(stock_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(crd_surplus_stock_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    //call setTransferableQuantity
    jmethodID jm_setTransferableQuantity = env->GetMethodID(crd_surplus_stock_info_class_, "setTransferableQuantity", "(Ljava/lang/String;)V");
    jstring transferableQuantity = env->NewStringUTF((std::to_string(stock_info->transferable_quantity)).c_str());
    env->CallVoidMethod(targetObj, jm_setTransferableQuantity, transferableQuantity);

    //call setTransferredQuantity
    jmethodID jm_setTransferredQuantity = env->GetMethodID(crd_surplus_stock_info_class_, "setTransferredQuantity", "(Ljava/lang/String;)V");
    jstring transferredQuantity = env->NewStringUTF((std::to_string(stock_info->transferred_quantity)).c_str());
    env->CallVoidMethod(targetObj, jm_setTransferredQuantity, transferredQuantity);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_surplus_stock_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_surplus_stock_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void Trade::generateCrdDebtExtendNoticeObj(JNIEnv* env, jobject& targetObj,XTPCreditDebtExtendNotice *debt_extend_info, int request_id, bool is_last) {
    //call setXtpId
    jmethodID jm_setXtpId = env->GetMethodID(crd_debt_extend_notice_class_, "setXtpId", "(Ljava/lang/String;)V");
    assert(jm_setXtpId != NULL);
    jstring orderXtpIdStr = env->NewStringUTF((std::to_string(debt_extend_info->xtpid)).c_str());
    env->CallVoidMethod(targetObj, jm_setXtpId, orderXtpIdStr);

    //call setDebtId
    jstring jdebtIdstr = env->NewStringUTF(debt_extend_info->debt_id);
    jmethodID jm_setDebtId = env->GetMethodID(crd_debt_extend_notice_class_, "setDebtId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setDebtId, jdebtIdstr);

    //call setOperStatus
    int operStatus = (int)debt_extend_info->oper_status;
    jmethodID jm_setOperStatus = env->GetMethodID(crd_debt_extend_notice_class_, "setOperStatus", "(I)V");
    env->CallVoidMethod(targetObj, jm_setOperStatus, operStatus);

    //call setOperTime
    jmethodID jm_setOperTime = env->GetMethodID(crd_debt_extend_notice_class_, "setOperTime", "(Ljava/lang/String;)V");
    jstring operTime = env->NewStringUTF((std::to_string(debt_extend_info->oper_time)).c_str());
    env->CallVoidMethod(targetObj, jm_setOperTime, operTime);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_debt_extend_notice_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_debt_extend_notice_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);
}

void Trade::generateCrdFundExtraInfoObj(JNIEnv* env, jobject& targetObj,XTPCrdFundExtraInfo *fund_info, int request_id) {

    //call setMfRsAvlUsed
    jmethodID jm_setMfRsAvlUsed = env->GetMethodID(crd_fund_extra_info_class_, "setMfRsAvlUsed", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMfRsAvlUsed, fund_info->mf_rs_avl_used);

    //call setReserve
    jstring reserve = env->NewStringUTF(fund_info->reserve);
    jmethodID jm_setReserve = env->GetMethodID(crd_fund_extra_info_class_, "setReserve", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setReserve, reserve);

    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_fund_extra_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

}

void Trade::generateCrdPositionExtraObj(JNIEnv* env, jobject& targetObj,XTPCrdPositionExtraInfo *position_extra_info, int request_id, bool is_last) {

    //call setMarketType
    int marketType = (int)position_extra_info->market;
    jmethodID jm_setMarketType = env->GetMethodID(crd_position_extra_info_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, marketType);

    //call setTicker
    jstring jtickerstr = env->NewStringUTF(position_extra_info->ticker);
    jmethodID jm_setTicker = env->GetMethodID(crd_position_extra_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    //call setMfRsAvlUsed
    jmethodID jm_setMfRsAvlUsed = env->GetMethodID(crd_position_extra_info_class_, "setMfRsAvlUsed", "(D)V");
    env->CallVoidMethod(targetObj, jm_setMfRsAvlUsed, position_extra_info->mf_rs_avl_used);

    //call setReserve
    jstring reserve = env->NewStringUTF(position_extra_info->reserve);
    jmethodID jm_setReserve = env->GetMethodID(crd_position_extra_info_class_, "setReserve", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setReserve, reserve);


    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(crd_position_extra_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    //call setLastResp
    jmethodID jm_setLastResp = env->GetMethodID(crd_position_extra_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp,is_last);

}

void  Trade::generateErrorMsgObj(JNIEnv* env, jobject& errorMsgObj,XTPRI *error_info, int request_id) {
    //call setRequestId
    jmethodID jm_setRequestId = env->GetMethodID(xtp_error_msg_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(errorMsgObj, jm_setRequestId, request_id);

    //call setErrorId
    jmethodID jm_setErrorId = env->GetMethodID(xtp_error_msg_class_, "setErrorId", "(I)V");
    assert(jm_setErrorId != NULL);
    env->CallVoidMethod(errorMsgObj, jm_setErrorId, error_info->error_id);

    //call setErrorMsg
    jstring jerrorMsgStr = env->NewStringUTF(error_info->error_msg);
    jmethodID jm_setErrorMsg = env->GetMethodID(xtp_error_msg_class_, "setErrorMsg", "(Ljava/lang/String;)V");
    env->CallVoidMethod(errorMsgObj, jm_setErrorMsg, jerrorMsgStr);
}

//==================For XTP API 1.16===========================
void  Trade::generateFundInfoObj(JNIEnv* env, jobject& targetObj, XTPStructuredFundInfo *sourceObj,int request_id, bool is_last) {
    int xtpChangeType = (int)sourceObj->exchange_id;
    jmethodID jm_setExchangeType = env->GetMethodID(structured_fund_info_class_, "setExchangeType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setExchangeType, xtpChangeType);

    jstring jsftickerstr = env->NewStringUTF(sourceObj->sf_ticker);
    jmethodID jm_setSfTicker = env->GetMethodID(structured_fund_info_class_, "setSfTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setSfTicker, jsftickerstr);

    jstring jsftickernamestr = env->NewStringUTF(sourceObj->sf_ticker_name);
    jmethodID jm_setSfTickerName = env->GetMethodID(structured_fund_info_class_, "setSfTickerName", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setSfTickerName, jsftickernamestr);

    jstring jtickerstr = env->NewStringUTF(sourceObj->ticker);
    jmethodID jm_setTicker = env->GetMethodID(structured_fund_info_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    jstring jtickernamestr = env->NewStringUTF(sourceObj->ticker_name);
    jmethodID jm_setTickerName = env->GetMethodID(structured_fund_info_class_, "setTickerName", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTickerName, jtickernamestr);

    int splitMergeStatus = (int)sourceObj->split_merge_status;
    jmethodID jm_setSplitMergeStatus= env->GetMethodID(structured_fund_info_class_, "setSplitMergeStatus", "(I)V");
    env->CallVoidMethod(targetObj, jm_setSplitMergeStatus, splitMergeStatus);

    jmethodID jm_setRatio = env->GetMethodID(structured_fund_info_class_, "setRatio", "(I)V");
    assert(jm_setRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setRatio, sourceObj->ratio);

    jmethodID jm_setMinSplitQty = env->GetMethodID(structured_fund_info_class_, "setMinSplitQty", "(I)V");
    assert(jm_setRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setMinSplitQty, sourceObj->min_split_qty);

    jmethodID jm_setMinMergeQty = env->GetMethodID(structured_fund_info_class_, "setMinMergeQty", "(I)V");
    assert(jm_setMinMergeQty != NULL);
    env->CallVoidMethod(targetObj, jm_setMinMergeQty, sourceObj->min_merge_qty);

    jmethodID jm_setNetPrice = env->GetMethodID(structured_fund_info_class_, "setNetPrice", "(D)V");
    assert(jm_setNetPrice != NULL);
    double new_net_price = sourceObj->net_price;
    env->CallVoidMethod(targetObj, jm_setNetPrice, new_net_price);

    jmethodID jm_setRequestId = env->GetMethodID(structured_fund_info_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(structured_fund_info_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateFundTransferNoticeObj(JNIEnv* env, jobject& targetObj,XTPFundTransferNotice *sourceObj,int request_id, bool is_last) {
    jmethodID jm_setSerialId = env->GetMethodID(fund_transfer_notice_class_, "setSerialId", "(Ljava/lang/String;)V");
    assert(jm_setSerialId != NULL);
    jstring serialIdStr = env->NewStringUTF((std::to_string(sourceObj->serial_id)).c_str());
    env->CallVoidMethod(targetObj, jm_setSerialId, serialIdStr);

    int xtpTransferType = (int)sourceObj->transfer_type;
    jmethodID jm_setXtpTransferType = env->GetMethodID(fund_transfer_notice_class_, "setFundTransferType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setXtpTransferType, xtpTransferType);

    jmethodID jm_setAmount = env->GetMethodID(fund_transfer_notice_class_, "setAmount", "(D)V");
    assert(jm_setAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setAmount, sourceObj->amount);

    int fundOperStatus = (int)sourceObj->oper_status;
    jmethodID jm_setOperStatus= env->GetMethodID(fund_transfer_notice_class_, "setFundOperStatus", "(I)V");
    env->CallVoidMethod(targetObj, jm_setOperStatus, fundOperStatus);

    jmethodID jm_setTransferTime = env->GetMethodID(fund_transfer_notice_class_, "setTransferTime", "(Ljava/lang/String;)V");
    assert(jm_setTransferTime != NULL);
    jstring transferTimeStr = env->NewStringUTF((std::to_string(sourceObj->transfer_time)).c_str());
    env->CallVoidMethod(targetObj, jm_setTransferTime, transferTimeStr);

    jmethodID jm_setRequestId = env->GetMethodID(fund_transfer_notice_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(fund_transfer_notice_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateXTPFundQueryRspObj(JNIEnv*env, jobject& resultObj, XTPFundQueryRsp *sourceObj, int request_id) {
    jmethodID jm_setAmount = env->GetMethodID(fund_query_rsp_class_, "setAmount", "(D)V");
    assert(jm_setAmount != NULL);
    env->CallVoidMethod(resultObj, jm_setAmount, sourceObj->amount);

    int fundQueryType = (int)sourceObj->query_type;
    jmethodID jm_setFundQueryType= env->GetMethodID(fund_query_rsp_class_, "setXTPFundQueryType", "(I)V");
    env->CallVoidMethod(resultObj, jm_setFundQueryType, fundQueryType);

    jmethodID jm_setRequestId = env->GetMethodID(fund_query_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(resultObj, jm_setRequestId, request_id);
}

void  Trade::generateETFBaseObj(JNIEnv* env, jobject& targetObj, XTPQueryETFBaseRsp *sourceObj,int request_id, bool is_last) {
    int xtpMarketType = (int)sourceObj->market;
    jmethodID jm_setMarketType = env->GetMethodID(query_etf_base_rsp_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, xtpMarketType);

    jstring jetfstr = env->NewStringUTF(sourceObj->etf);
    jmethodID jm_setEtf = env->GetMethodID(query_etf_base_rsp_class_, "setEtf", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setEtf, jetfstr);

    jstring jsrtickerstr = env->NewStringUTF(sourceObj->subscribe_redemption_ticker);
    jmethodID jm_setSRTicker = env->GetMethodID(query_etf_base_rsp_class_, "setSubscribeRedemptionTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setSRTicker, jsrtickerstr);

    jmethodID jm_setUnit = env->GetMethodID(query_etf_base_rsp_class_, "setUnit", "(I)V");
    assert(jm_setUnit != NULL);
    env->CallVoidMethod(targetObj, jm_setUnit, sourceObj->unit);

    jmethodID jm_setSubscribeStatus = env->GetMethodID(query_etf_base_rsp_class_, "setSubscribeStatus", "(I)V");
    assert(jm_setUnit != NULL);
    env->CallVoidMethod(targetObj, jm_setSubscribeStatus, sourceObj->subscribe_status);

    jmethodID jm_setRedemptionStatus = env->GetMethodID(query_etf_base_rsp_class_, "setRedemptionStatus", "(I)V");
    assert(jm_setRedemptionStatus != NULL);
    env->CallVoidMethod(targetObj, jm_setRedemptionStatus, sourceObj->redemption_status);

    jmethodID jm_setMaxCashRatio = env->GetMethodID(query_etf_base_rsp_class_, "setMaxCashRatio", "(D)V");
    assert(jm_setMaxCashRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setMaxCashRatio, sourceObj->max_cash_ratio);

    jmethodID jm_setEstimateAmount = env->GetMethodID(query_etf_base_rsp_class_, "setEstimateAmount", "(D)V");
    assert(jm_setEstimateAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setEstimateAmount, sourceObj->estimate_amount);

    jmethodID jm_setCashComponent = env->GetMethodID(query_etf_base_rsp_class_, "setCashComponent", "(D)V");
    assert(jm_setCashComponent != NULL);
    env->CallVoidMethod(targetObj, jm_setCashComponent, sourceObj->cash_component);

    jmethodID jm_setNetValue = env->GetMethodID(query_etf_base_rsp_class_, "setNetValue", "(D)V");
    assert(jm_setNetValue != NULL);
    env->CallVoidMethod(targetObj, jm_setNetValue, sourceObj->net_value);

    jmethodID jm_setTotalAmount = env->GetMethodID(query_etf_base_rsp_class_, "setTotalAmount", "(D)V");
    assert(jm_setTotalAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setTotalAmount, sourceObj->total_amount);

    jmethodID jm_setRequestId = env->GetMethodID(query_etf_base_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(query_etf_base_rsp_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateETFComponentObj(JNIEnv* env, jobject& targetObj, XTPQueryETFComponentRsp *sourceObj,int request_id, bool is_last) {
    int xtpMarketType = (int)sourceObj->market;
    jmethodID jm_setMarketType = env->GetMethodID(query_etf_component_rsp_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, xtpMarketType);

    jstring jtickerstr = env->NewStringUTF(sourceObj->ticker);
    jmethodID jm_setTicker = env->GetMethodID(query_etf_component_rsp_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    jstring jcomponenttickerstr = env->NewStringUTF(sourceObj->component_ticker);
    jmethodID jm_setComponentTicker = env->GetMethodID(query_etf_component_rsp_class_, "setComponentTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setComponentTicker, jcomponenttickerstr);

    jstring jcomponentnamestr = env->NewStringUTF(sourceObj->component_name);
    jmethodID jm_setComponentName = env->GetMethodID(query_etf_component_rsp_class_, "setComponentName", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setComponentName, jcomponentnamestr);

    jmethodID jm_setQuantity = env->GetMethodID(query_etf_component_rsp_class_, "setQuantity", "(J)V");
    env->CallVoidMethod(targetObj, jm_setQuantity, sourceObj->quantity);

    int xtpComponentMarketType = (int)sourceObj->component_market;
    jmethodID jm_setComponentMarketType = env->GetMethodID(query_etf_component_rsp_class_, "setComponentMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setComponentMarketType, xtpComponentMarketType);

    int xtpReplaceType = (int)sourceObj->replace_type;
    jmethodID jm_setReplaceType = env->GetMethodID(query_etf_component_rsp_class_, "setEtfReplaceType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setReplaceType, xtpReplaceType);

    jmethodID jm_setPremiumRatio = env->GetMethodID(query_etf_component_rsp_class_, "setPremiumRatio", "(D)V");
    assert(jm_setPremiumRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setPremiumRatio, sourceObj->premium_ratio);

    jmethodID jm_setAmount = env->GetMethodID(query_etf_component_rsp_class_, "setAmount", "(D)V");
    assert(jm_setAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setAmount, sourceObj->amount);

    jmethodID jm_setCreationPremiumRatio = env->GetMethodID(query_etf_component_rsp_class_, "setCreationPremiumRatio", "(D)V");
    assert(jm_setCreationPremiumRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setCreationPremiumRatio, sourceObj->creation_premium_ratio);

    jmethodID jm_setRedemptionDiscountRatio = env->GetMethodID(query_etf_component_rsp_class_, "setRedemptionDiscountRatio", "(D)V");
    assert(jm_setRedemptionDiscountRatio != NULL);
    env->CallVoidMethod(targetObj, jm_setRedemptionDiscountRatio, sourceObj->redemption_discount_ratio);

    jmethodID jm_setCreationAmount = env->GetMethodID(query_etf_component_rsp_class_, "setCreationAmount", "(D)V");
    assert(jm_setCreationAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setCreationAmount, sourceObj->creation_amount);

    jmethodID jm_setRedemptionAmount = env->GetMethodID(query_etf_component_rsp_class_, "setRedemptionAmount", "(D)V");
    assert(jm_setRedemptionAmount != NULL);
    env->CallVoidMethod(targetObj, jm_setRedemptionAmount, sourceObj->redemption_amount);

    jmethodID jm_setRequestId = env->GetMethodID(query_etf_component_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(query_etf_component_rsp_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateIPOTickerObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOTickerRsp *sourceObj,int request_id, bool is_last) {
    int xtpMarketType = (int)sourceObj->market;
    jmethodID jm_setMarketType = env->GetMethodID(query_ipo_ticker_rsp_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, xtpMarketType);

    jstring jtickerstr = env->NewStringUTF(sourceObj->ticker);
    jmethodID jm_setTicker = env->GetMethodID(query_ipo_ticker_rsp_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    jstring jtickernamestr = env->NewStringUTF(sourceObj->ticker_name);
    jmethodID jm_setTickerName = env->GetMethodID(query_ipo_ticker_rsp_class_, "setTickerName", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTickerName, jtickernamestr);

    int tickerType = (int)sourceObj->ticker_type;
    jmethodID jm_setTickerType = env->GetMethodID(query_ipo_ticker_rsp_class_, "setTickerType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setTickerType, tickerType);

    jmethodID jm_setPrice = env->GetMethodID(query_ipo_ticker_rsp_class_, "setPrice", "(D)V");
    assert(jm_setPrice != NULL);
    double new_price = sourceObj->price;
    env->CallVoidMethod(targetObj, jm_setPrice, new_price);

    jmethodID jm_setUnit = env->GetMethodID(query_ipo_ticker_rsp_class_, "setUnit", "(I)V");
    assert(jm_setUnit != NULL);
    env->CallVoidMethod(targetObj, jm_setUnit, sourceObj->unit);

    jmethodID jm_setQtyUpperLimit = env->GetMethodID(query_ipo_ticker_rsp_class_, "setQtyUpperLimit", "(I)V");
    assert(jm_setQtyUpperLimit != NULL);
    env->CallVoidMethod(targetObj, jm_setQtyUpperLimit, sourceObj->qty_upper_limit);

    jmethodID jm_setRequestId = env->GetMethodID(query_ipo_ticker_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(query_ipo_ticker_rsp_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateIPOQuotaObj(JNIEnv* env, jobject& targetObj, XTPQueryIPOQuotaRsp *sourceObj,int request_id, bool is_last) {
    int xtpMarketType = (int)sourceObj->market;
    jmethodID jm_setMarketType = env->GetMethodID(query_ipo_quota_rsp_class_, "setMarketType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setMarketType, xtpMarketType);

    jmethodID jm_setQuantity = env->GetMethodID(query_ipo_quota_rsp_class_, "setQuantity", "(I)V");
    env->CallVoidMethod(targetObj, jm_setQuantity, sourceObj->quantity);

    jmethodID jm_setTech_quantity = env->GetMethodID(query_ipo_quota_rsp_class_, "setTech_quantity", "(I)V");
    env->CallVoidMethod(targetObj, jm_setTech_quantity, sourceObj->tech_quantity);

    jmethodID jm_setUnused = env->GetMethodID(query_ipo_quota_rsp_class_, "setUnused", "(I)V");
    env->CallVoidMethod(targetObj, jm_setUnused, sourceObj->unused);

    jmethodID jm_setRequestId = env->GetMethodID(query_ipo_quota_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(query_ipo_quota_rsp_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}

void  Trade::generateOptionAuctionInfoObj(JNIEnv* env, jobject& targetObj, XTPQueryOptionAuctionInfoRsp *sourceObj, int request_id, bool is_last) {
    jstring jtickerstr = env->NewStringUTF(sourceObj->ticker);
    jmethodID jm_setTicker = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setTicker", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setTicker, jtickerstr);

    int securityIdSource = (int)sourceObj->security_id_source;
    jmethodID jm_setSecurityIdSource = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setSecurityIdSource", "(I)V");
    env->CallVoidMethod(targetObj, jm_setSecurityIdSource, securityIdSource);
    jstring jsymbolstr = env->NewStringUTF(sourceObj->symbol);
    jmethodID jm_setSymbol = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setSymbol", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setSymbol, jsymbolstr);

    jstring jcontractIdstr = env->NewStringUTF(sourceObj->contract_id);
    jmethodID jm_setContractId = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setContractId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setContractId, jcontractIdstr);

    jstring junderlyingSecurityIdstr = env->NewStringUTF(sourceObj->underlying_security_id);
    jmethodID jm_setUnderlyingSecurityId = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setUnderlyingSecurityId", "(Ljava/lang/String;)V");
    env->CallVoidMethod(targetObj, jm_setUnderlyingSecurityId, junderlyingSecurityIdstr);

    int underlyingSecurityIdSource = (int)sourceObj->underlying_security_id_source;
    jmethodID jm_setUnderlyingSecurityIdSource = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setUnderlyingSecurityIdSource", "(I)V");
    env->CallVoidMethod(targetObj, jm_setUnderlyingSecurityIdSource, underlyingSecurityIdSource);

    jlong jlistDate = (jlong)sourceObj->list_date;
    jmethodID jm_setListDate = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setListDate", "(J)V");
    env->CallVoidMethod(targetObj, jm_setListDate, jlistDate);

    jlong jlastTradeDate = (jlong)sourceObj->last_trade_date;
    jmethodID jm_setLastTradeDate = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLastTradeDate", "(J)V");
    env->CallVoidMethod(targetObj, jm_setLastTradeDate, jlastTradeDate);

    int tickerType = (int)sourceObj->ticker_type;
    jmethodID jm_setTickerType = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setTickerType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setTickerType, tickerType);

    jmethodID jm_setDayTrading = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setDayTrading", "(I)V");
    env->CallVoidMethod(targetObj, jm_setDayTrading, sourceObj->day_trading);

    int callOrPut = (int)sourceObj->call_or_put;
    jmethodID jm_setCallOrPut = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setCallOrPut", "(I)V");
    env->CallVoidMethod(targetObj, jm_setCallOrPut, callOrPut);

    jlong jdeliveryDay = (jlong)sourceObj->delivery_day;
    jmethodID jm_setDeliveryDay = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setDeliveryDay", "(J)V");
    env->CallVoidMethod(targetObj, jm_setDeliveryDay, jdeliveryDay);

    jlong jdeliveryMonth = (jlong)sourceObj->delivery_month;
    jmethodID jm_setDeliveryMonth = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setDeliveryMonth", "(J)V");
    env->CallVoidMethod(targetObj, jm_setDeliveryMonth, jdeliveryMonth);

    int exerciseType = (int)sourceObj->exercise_type;
    jmethodID jm_setExerciseType = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setExerciseType", "(I)V");
    env->CallVoidMethod(targetObj, jm_setExerciseType, exerciseType);

    jlong jexerciseBeginDate = (jlong)sourceObj->exercise_begin_date;
    jmethodID jm_setExerciseBeginDate = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setExerciseBeginDate", "(J)V");
    env->CallVoidMethod(targetObj, jm_setExerciseBeginDate, jexerciseBeginDate);

    jlong jexerciseEndDate = (jlong)sourceObj->exercise_end_date;
    jmethodID jm_setExerciseEndDate = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setExerciseEndDate", "(J)V");
    env->CallVoidMethod(targetObj, jm_setExerciseEndDate, jexerciseEndDate);

    jmethodID jm_setExercisePrice = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setExercisePrice", "(D)V");
    assert(jm_setExercisePrice != NULL);
    env->CallVoidMethod(targetObj, jm_setExercisePrice, sourceObj->exercise_price);

    jlong jqtyUnit = (jlong)sourceObj->qty_unit;
    jmethodID jm_setQtyUnit = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setQtyUnit", "(J)V");
    env->CallVoidMethod(targetObj, jm_setQtyUnit, jqtyUnit);

    jlong jcontractUnit = (jlong)sourceObj->contract_unit;
    jmethodID jm_setContractUnit = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setContractUnit", "(J)V");
    env->CallVoidMethod(targetObj, jm_setContractUnit, jcontractUnit);

    jlong jcontractPosition = (jlong)sourceObj->contract_position;
    jmethodID jm_setContractPosition = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setContractPosition", "(J)V");
    env->CallVoidMethod(targetObj, jm_setContractPosition, jcontractPosition);

    jmethodID jm_setPrevClosePrice = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setPrevClosePrice", "(D)V");
    assert(jm_setPrevClosePrice != NULL);
    env->CallVoidMethod(targetObj, jm_setPrevClosePrice, sourceObj->prev_close_price);

    jmethodID jm_setPrevClearingPrice = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setPrevClearingPrice", "(D)V");
    assert(jm_setPrevClearingPrice != NULL);
    env->CallVoidMethod(targetObj, jm_setPrevClearingPrice, sourceObj->prev_clearing_price);

    jlong jlmtBuyMaxQty = (jlong)sourceObj->lmt_buy_max_qty;
    jmethodID jm_setLmtBuyMaxQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLmtBuyMaxQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setLmtBuyMaxQty, jlmtBuyMaxQty);

    jlong jlmtBuyMinQty = (jlong)sourceObj->lmt_buy_min_qty;
    jmethodID jm_setLmtBuyMinQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLmtBuyMinQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setLmtBuyMinQty, jlmtBuyMinQty);

    jlong jlmtSellMaxQty = (jlong)sourceObj->lmt_sell_max_qty;
    jmethodID jm_setLmtSellMaxQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLmtSellMaxQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setLmtSellMaxQty, jlmtSellMaxQty);

    jlong jlmtSellMinQty = (jlong)sourceObj->lmt_sell_min_qty;
    jmethodID jm_setLmtSellMinQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLmtSellMinQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setLmtSellMinQty, jlmtSellMinQty);

    jlong jmktBuyMaxQty = (jlong)sourceObj->mkt_buy_max_qty;
    jmethodID jm_setMktBuyMaxQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMktBuyMaxQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMktBuyMaxQty, jmktBuyMaxQty);

    jlong jmktBuyMinQty = (jlong)sourceObj->mkt_buy_min_qty;
    jmethodID jm_setMktBuyMinQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMktBuyMinQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMktBuyMinQty, jmktBuyMinQty);

    jlong jmktSellMaxQty = (jlong)sourceObj->mkt_sell_max_qty;
    jmethodID jm_setMktSellMaxQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMktSellMaxQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMktSellMaxQty, jmktSellMaxQty);

    jlong jmktSellMinQty = (jlong)sourceObj->mkt_sell_min_qty;
    jmethodID jm_setMktSellMinQty = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMktSellMinQty", "(J)V");
    env->CallVoidMethod(targetObj, jm_setMktSellMinQty, jmktSellMinQty);

    jmethodID jm_setPriceTick = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setPriceTick", "(D)V");
    assert(jm_setPriceTick != NULL);
    env->CallVoidMethod(targetObj, jm_setPriceTick, sourceObj->price_tick);

    jmethodID jm_setUpperLimitPrice = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setUpperLimitPrice", "(D)V");
    assert(jm_setUpperLimitPrice != NULL);
    env->CallVoidMethod(targetObj, jm_setUpperLimitPrice, sourceObj->upper_limit_price);

    jmethodID jm_setLowerLimitPrice = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLowerLimitPrice", "(D)V");
    assert(jm_setLowerLimitPrice != NULL);
    env->CallVoidMethod(targetObj, jm_setLowerLimitPrice, sourceObj->lower_limit_price);

    jmethodID jm_setSellMargin = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setSellMargin", "(D)V");
    assert(jm_setSellMargin != NULL);
    env->CallVoidMethod(targetObj, jm_setSellMargin, sourceObj->sell_margin);

    jmethodID jm_setMarginRatioParam1 = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMarginRatioParam1", "(D)V");
    assert(jm_setMarginRatioParam1 != NULL);
    env->CallVoidMethod(targetObj, jm_setMarginRatioParam1, sourceObj->margin_ratio_param1);

    jmethodID jm_setMarginRatioParam2 = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setMarginRatioParam2", "(D)V");
    assert(jm_setMarginRatioParam2 != NULL);
    env->CallVoidMethod(targetObj, jm_setMarginRatioParam2, sourceObj->margin_ratio_param2);

    jmethodID jm_setRequestId = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setRequestId", "(I)V");
    assert(jm_setRequestId != NULL);
    env->CallVoidMethod(targetObj, jm_setRequestId, request_id);

    jmethodID jm_setLastResp = env->GetMethodID(query_option_auctionInfo_rsp_class_, "setLastResp", "(Z)V");
    assert(jm_setLastResp != NULL);
    env->CallVoidMethod(targetObj, jm_setLastResp, is_last);
}