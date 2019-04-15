#include "include/InitGlog.h"
#include "include/glog/logging.h"
#include <iostream>

using namespace std;

string &replace_all(string &str, const string &old_value, const string &new_value);

void init_glog(string log_folder,string log_subFolder) {
    string logName = log_subFolder;//string("smart_proxy_center");
    try {
        google::InitGoogleLogging(logName.c_str());

    } catch (std::exception &e) {

    }
    string str_smart_log_folder = log_folder + string("/") + log_subFolder;//string("/smartcpplog");
    FLAGS_log_dir = str_smart_log_folder.c_str();
    google::SetStderrLogging(google::GLOG_INFO);//设置级别高于 google::INFO 的日志同时输出到屏幕
    FLAGS_colorlogtostderr = true; //设置输出到屏幕的日志显示相应颜色
    FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出
    //设置日志路径及文件名
    string logPathName = str_smart_log_folder + "/" + logName + "_fatal_";
    google::SetLogDestination(google::GLOG_FATAL, logPathName.c_str()); // 设置 google::FATAL 级别的日志存储路径和文件名前缀
    logPathName = str_smart_log_folder + "/" + logName + "_error_";
    google::SetLogDestination(google::GLOG_ERROR, logPathName.c_str()); //设置 google::ERROR 级别的日志存储路径和文件名前缀
    logPathName = str_smart_log_folder + "/" + logName + "_warning_";
    google::SetLogDestination(google::GLOG_WARNING, logPathName.c_str()); //设置 google::WARNING 级别的日志存储路径和文件名前缀
    logPathName = str_smart_log_folder + "/" + logName + "_info_";
    google::SetLogDestination(google::GLOG_INFO, logPathName.c_str()); //设置 google::INFO 级别的日志存储路径和文件名前缀
    //        FLAGS_max_log_size = 1; //单个日志文件最大，单位M 默认1.8G
    google::SetDeleteLogBeyondHowLongTimeBySeconds(3600 * 24 * 15); //log最长保留15天
    LOG(INFO) << "XTP JAVA API Log start in folder: " << str_smart_log_folder;
    //        for(int i=0;i<100000;i++){
    //            LOG(ERROR) << "====================================================================================="<<i;
    //        }
}

void shutdown_glog() {
    LOG(INFO) << "Shutdown Glog";
    google::ShutdownGoogleLogging();
}