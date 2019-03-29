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
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_tradeEventResult = env->GetMethodID(tradePluginClass_, "onTradeEvent",
    "(Lcom/zts/xtp/trade/model/response/TradeResponse;Ljava/lang/String;)V");

    //fetch the default construct
    jmethodID defaultConstr = env->GetMethodID(trade_report_class_, "<init>","()V");
    if (defaultConstr == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    //create the object
    jobject tradeReportObj = env->NewObject(trade_report_class_, defaultConstr);
    if (tradeReportObj == NULL) {
        jvm_->DetachCurrentThread();
        return;
    }

    generateTradeReportObj(env, tradeReportObj,trade_info, 0, true);

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_tradeEventResult, tradeReportObj, strSessionId);
    jvm_->DetachCurrentThread();
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

void Trade::OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryStkPositionResult = env->GetMethodID(tradePluginClass_, "onQueryPosition",
    "(Lcom/zts/xtp/trade/model/response/StockPositionResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    jobject queryStkPositionRespObj=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_stkposition_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        queryStkPositionRespObj = env->NewObject(query_stkposition_rsp_class_, defaultConstr);
        if (queryStkPositionRespObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //call setTicker
        jstring jtickerstr = env->NewStringUTF(position->ticker);
        jmethodID jm_setTicker = env->GetMethodID(query_stkposition_rsp_class_, "setTicker", "(Ljava/lang/String;)V");
        env->CallVoidMethod(queryStkPositionRespObj, jm_setTicker, jtickerstr);

        //call setTickerName
        jstring jtickernamestr = env->NewStringUTF(position->ticker_name);
        jmethodID jm_setTickerName = env->GetMethodID(query_stkposition_rsp_class_, "setTickerName", "(Ljava/lang/String;)V");
        env->CallVoidMethod(queryStkPositionRespObj, jm_setTickerName, jtickernamestr);

        //call setMarketType
        int xtpMarketType = (int)position->market;
        jmethodID jm_setMarketType = env->GetMethodID(query_stkposition_rsp_class_, "setMarketType", "(I)V");
        env->CallVoidMethod(queryStkPositionRespObj, jm_setMarketType, xtpMarketType);

        //call setTotalQty
        jmethodID jm_setTotalQty = env->GetMethodID(query_stkposition_rsp_class_, "setTotalQty", "(J)V");
        assert(jm_setTotalQty != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setTotalQty, position->total_qty);

        //call setSellableQty
        jmethodID jm_setSellableQty = env->GetMethodID(query_stkposition_rsp_class_, "setSellableQty", "(J)V");
        assert(jm_setSellableQty != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setSellableQty, position->sellable_qty);

        //call setAvgPrice
        jmethodID jm_setAvgPrice = env->GetMethodID(query_stkposition_rsp_class_, "setAvgPrice", "(D)V");
        assert(jm_setAvgPrice != NULL);
        double new_avg_price = position->avg_price;
        env->CallVoidMethod(queryStkPositionRespObj, jm_setAvgPrice, new_avg_price);

        //call setUnrealizedPnl
        jmethodID jm_setUnrealizedPnl = env->GetMethodID(query_stkposition_rsp_class_, "setUnrealizedPnl", "(D)V");
        assert(jm_setUnrealizedPnl != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setUnrealizedPnl, position->unrealized_pnl);

        //call setYesterdayPosition
        jmethodID jm_setYesterdayPosition = env->GetMethodID(query_stkposition_rsp_class_, "setYesterdayPosition", "(J)V");
        assert(jm_setYesterdayPosition != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setYesterdayPosition, position->yesterday_position);

        //call setPurchaseRedeemableQty
        jmethodID jm_setPurchaseRedeemableQty = env->GetMethodID(query_stkposition_rsp_class_, "setPurchaseRedeemableQty", "(J)V");
        assert(jm_setPurchaseRedeemableQty != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setPurchaseRedeemableQty, position->purchase_redeemable_qty);

        //call setPositionDirection
        jmethodID jm_setPositionDirection = env->GetMethodID(query_stkposition_rsp_class_, "setPositionDirectionType", "(I)V");
        assert(jm_setPositionDirection != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setPositionDirection, position->position_direction);

        //call setReserved1
        jmethodID jm_setReserved1 = env->GetMethodID(query_stkposition_rsp_class_, "setReserved1", "(I)V");
        assert(jm_setReserved1 != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setReserved1, position->reserved1);

        //call setExecutableOption
        jmethodID jm_setExecutableOption = env->GetMethodID(query_stkposition_rsp_class_, "setExecutableOption", "(J)V");
        assert(jm_setExecutableOption != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setExecutableOption, position->executable_option);

         //call setLockablePosition
        jmethodID jm_setLockablePosition = env->GetMethodID(query_stkposition_rsp_class_, "setLockablePosition", "(J)V");
        assert(jm_setLockablePosition != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setLockablePosition, position->lockable_position);

        //call setExecutableUnderlying
        jmethodID jm_setExecutableUnderlying = env->GetMethodID(query_stkposition_rsp_class_, "setExecutableUnderlying", "(J)V");
        assert(jm_setExecutableUnderlying != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setExecutableUnderlying, position->executable_underlying);

        //call setLockedPosition
        jmethodID jm_setLockedPosition = env->GetMethodID(query_stkposition_rsp_class_, "setLockedPosition", "(J)V");
        assert(jm_setLockedPosition != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setLockedPosition, position->locked_position);

         //call setUsableLockedPosition
        jmethodID jm_setUsableLockedPosition = env->GetMethodID(query_stkposition_rsp_class_, "setUsableLockedPosition", "(J)V");
        assert(jm_setUsableLockedPosition != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setUsableLockedPosition, position->usable_locked_position);

        //call setRequestId
        jmethodID jm_setRequestId = env->GetMethodID(query_stkposition_rsp_class_, "setRequestId", "(I)V");
        assert(jm_setRequestId != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setRequestId, request_id);

        //call setLastResp
        jmethodID jm_setLastResp = env->GetMethodID(query_stkposition_rsp_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(queryStkPositionRespObj, jm_setLastResp,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryStkPositionResult, queryStkPositionRespObj,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_queryAssetResult = env->GetMethodID(tradePluginClass_, "onQueryAsset",
    "(Lcom/zts/xtp/trade/model/response/AssetResponse;Lcom/zts/xtp/common/model/ErrorMessage;Ljava/lang/String;)V");

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

    jobject queryAssetRespObj=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0) {
        //fetch the default construct
        jmethodID defaultConstr = env->GetMethodID(query_asset_rsp_class_, "<init>","()V");
        if (defaultConstr == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //create the object
        queryAssetRespObj = env->NewObject(query_asset_rsp_class_, defaultConstr);
        if (queryAssetRespObj == NULL) {
           jvm_->DetachCurrentThread();
           return;
        }

        //call setTotalAsset
        jmethodID jm_setTotalAsset = env->GetMethodID(query_asset_rsp_class_, "setTotalAsset", "(D)V");
        assert(jm_setTotalAsset != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setTotalAsset, asset->total_asset);

        //call setBuyingPower
        jmethodID jm_setBuyingPower = env->GetMethodID(query_asset_rsp_class_, "setBuyingPower", "(D)V");
        assert(jm_setBuyingPower != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setBuyingPower, asset->buying_power);

        //call setSecurityAsset
        jmethodID jm_setSecurityAsset = env->GetMethodID(query_asset_rsp_class_, "setSecurityAsset", "(D)V");
        assert(jm_setSecurityAsset != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setSecurityAsset, asset->security_asset);

        //call setFundBuyAmount
        jmethodID jm_setFundBuyAmount = env->GetMethodID(query_asset_rsp_class_, "setFundBuyAmount", "(D)V");
        assert(jm_setFundBuyAmount != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFundBuyAmount, asset->fund_buy_amount);

        //call setFundBuyFee
        jmethodID jm_setFundBuyFee = env->GetMethodID(query_asset_rsp_class_, "setFundBuyFee", "(D)V");
        assert(jm_setFundBuyFee != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFundBuyFee, asset->fund_buy_fee);

        //call setFundSellAmount
        jmethodID jm_setFundSellAmount = env->GetMethodID(query_asset_rsp_class_, "setFundSellAmount", "(D)V");
        assert(jm_setFundSellAmount != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFundSellAmount, asset->fund_sell_amount);

        //call setFundSellFee
        jmethodID jm_setFundSellFee = env->GetMethodID(query_asset_rsp_class_, "setFundSellFee", "(D)V");
        assert(jm_setFundSellFee != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFundSellFee, asset->fund_sell_fee);

        //call setWithholdingAmount
        jmethodID jm_setWithholdingAmount = env->GetMethodID(query_asset_rsp_class_, "setWithholdingAmount", "(D)V");
        assert(jm_setWithholdingAmount != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setWithholdingAmount, asset->withholding_amount);

        //call setAccountType
        jmethodID jm_setAccountType = env->GetMethodID(query_asset_rsp_class_, "setAccountType", "(I)V");
        assert(jm_setAccountType != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setAccountType, asset->account_type);

        //call setFrozenMargin
        jmethodID jm_setFrozenMargin = env->GetMethodID(query_asset_rsp_class_, "setFrozenMargin", "(D)V");
        assert(jm_setFrozenMargin != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFrozenMargin, asset->frozen_margin);

       //call setFrozenExecCash
        jmethodID jm_setFrozenExecCash = env->GetMethodID(query_asset_rsp_class_, "setFrozenExecCash", "(D)V");
        assert(jm_setFrozenExecCash != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFrozenExecCash, asset->frozen_exec_cash);

        //call setFrozenExecFee
        jmethodID jm_setFrozenExecFee = env->GetMethodID(query_asset_rsp_class_, "setFrozenExecFee", "(D)V");
        assert(jm_setFrozenExecFee != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setFrozenExecFee, asset->frozen_exec_fee);

         //call setPayLater
        jmethodID jm_setPayLater = env->GetMethodID(query_asset_rsp_class_, "setPayLater", "(D)V");
        assert(jm_setPayLater != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setPayLater, asset->pay_later);

        //call setPreadvaPay
        jmethodID jm_setPreadvaPay = env->GetMethodID(query_asset_rsp_class_, "setPreadvaPay", "(D)V");
        assert(jm_setPreadvaPay != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setPreadvaPay, asset->preadva_pay);

        //call setOrigBanlance
        jmethodID jm_setOrigBanlance = env->GetMethodID(query_asset_rsp_class_, "setOrigBanlance", "(D)V");
        assert(jm_setOrigBanlance != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setOrigBanlance, asset->orig_banlance);

         //call setBanlance
        jmethodID jm_setBanlance = env->GetMethodID(query_asset_rsp_class_, "setBanlance", "(D)V");
        assert(jm_setBanlance != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setBanlance, asset->banlance);

        //call setDepositWithdraw
        jmethodID jm_setDepositWithdraw = env->GetMethodID(query_asset_rsp_class_, "setDepositWithdraw", "(D)V");
        assert(jm_setDepositWithdraw != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setDepositWithdraw, asset->deposit_withdraw);

        //call setTradeNetting
        jmethodID jm_setTradeNetting = env->GetMethodID(query_asset_rsp_class_, "setTradeNetting", "(D)V");
        assert(jm_setTradeNetting != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setTradeNetting, asset->trade_netting);

        //call setCaptialAsset
        jmethodID jm_setCaptialAsset = env->GetMethodID(query_asset_rsp_class_, "setCaptialAsset", "(D)V");
        assert(jm_setCaptialAsset != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setCaptialAsset, asset->captial_asset);

         //call setForceFreezeAmount
        jmethodID jm_setForceFreezeAmount = env->GetMethodID(query_asset_rsp_class_, "setForceFreezeAmount", "(D)V");
        assert(jm_setForceFreezeAmount != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setForceFreezeAmount, asset->force_freeze_amount);

        //call setPreferredAmount
        jmethodID jm_setPreferredAmount = env->GetMethodID(query_asset_rsp_class_, "setPreferredAmount", "(D)V");
        assert(jm_setPreferredAmount != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setPreferredAmount, asset->preferred_amount);

        //call setRequestId
        jmethodID jm_setRequestId = env->GetMethodID(query_asset_rsp_class_, "setRequestId", "(I)V");
        assert(jm_setRequestId != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setRequestId,request_id );

        //call setLastResp
        jmethodID jm_setLastResp = env->GetMethodID(query_asset_rsp_class_, "setLastResp", "(Z)V");
        assert(jm_setLastResp != NULL);
        env->CallVoidMethod(queryAssetRespObj, jm_setLastResp,is_last);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_queryAssetResult, queryAssetRespObj,errorMsgObj, strSessionId);
    jvm_->DetachCurrentThread();
}

void Trade::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id) {
    LOG(INFO) << __PRETTY_FUNCTION__ ;

    if(error_info->error_id!=0){
    	LOG(ERROR) << __PRETTY_FUNCTION__ << " error_info:" << error_info->error_msg;
		}

    JNIEnv* env;
    // prepare the invocation
    env = preInvoke();
    jclass tradePluginClass_ = env->GetObjectClass(trade_plugin_obj_);
    assert(tradePluginClass_ != NULL);
    jmethodID jm_onOrderEvent = env->GetMethodID(tradePluginClass_, "onOrderEvent",
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

    generateErrorMsgObj(env, errorMsgObj,error_info,0);

    jobject tradeOrderInfoObj=NULL;
    int errorCode=error_info->error_id;
    //error_id =0 means successful
    if (errorCode == 0 || NULL != order_info) {
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

        generateOrderInfoObj(env, tradeOrderInfoObj,order_info, 0, true);
    }

    jstring strSessionId = env->NewStringUTF((std::to_string(session_id)).c_str());
    env->CallVoidMethod(trade_plugin_obj_, jm_onOrderEvent, tradeOrderInfoObj,errorMsgObj, strSessionId);
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
    if (errorCode == 0) {
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
    }

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
    if (errorCode == 0) {
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
    }

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
    jint res = jvm_->AttachCurrentThread((void**) &env, &att_args_);

    if (res !=0) {
        return NULL;
    }

    return env;
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
    jmethodID jm_setOrderCancelXtpId = env->GetMethodID(trade_order_info_class_, "setOrderCancelXtpId", "(I)V");
    env->CallVoidMethod(tradeOrderInfoObj, jm_setOrderCancelXtpId, order_info->order_cancel_xtp_id);

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
    int positionEffect = XTP_POSITION_EFFECT_UNKNOWN;
    if (order_info->business_type == XTP_BUSINESS_TYPE::XTP_BUSINESS_TYPE_OPTION)
    {
        positionEffect = (int)order_info->position_effect;
    }
    LOG(INFO)<< "generateOrderInfoObj: position_effect" << order_info->position_effect << "; positionEffect: " << positionEffect;
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
    int positionEffect = XTP_POSITION_EFFECT_UNKNOWN;
    if (trade_info->business_type == XTP_BUSINESS_TYPE::XTP_BUSINESS_TYPE_OPTION)
    {
        positionEffect = (int)trade_info->position_effect;
    }
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