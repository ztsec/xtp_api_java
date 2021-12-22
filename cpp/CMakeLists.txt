cmake_minimum_required(VERSION 2.8)
project(cpp)

# 设置是否debug模式编译
#set(CMAKE_BUILD_TYPE Debug)

# 设置JAVA_HOME路径
#set(JAVA_HOME /Library/Java/JavaVirtualMachines/jdk1.8.0_101.jdk/Contents/Home)
set(JAVA_HOME $ENV{JAVA_HOME})

set(CMAKE_CXX_STANDARD 11)
include_directories(${JAVA_HOME}/include)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")
    include_directories(${JAVA_HOME}/include/win32)
	IF (CMAKE_CL_64)#win64, modify by jiangch
		LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/win64/dll)
	ELSE ()
		LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/win32/dll)
	ENDIF ()
ELSEIF (APPLE)
    MESSAGE(STATUS "Now is Apple systens.")
    include_directories(${JAVA_HOME}/include/darwin)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/macosx/)
ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")
    include_directories(${JAVA_HOME}/include/linux)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/linux/)
ENDIF ()


include_directories(src/include)
include_directories(src/include/gflags)
include_directories(src/include/glog)
include_directories(src/include/xtp)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


add_library(tradeplugin SHARED
        src/include/gflags/gflags.h
        src/include/gflags/gflags_completions.h
        src/include/gflags/gflags_declare.h
        src/include/glog/log_severity.h
        src/include/glog/logging.h
        src/include/glog/raw_logging.h
        src/include/glog/stl_logging.h
        src/include/glog/vlog_is_on.h
        src/include/xtp/xoms_api_fund_struct.h
        src/include/xtp/xoms_api_struct.h
        src/include/xtp/xquote_api_struct.h
        src/include/xtp/xtp_api_data_type.h
        src/include/xtp/xtp_api_struct.h
        src/include/xtp/xtp_api_struct_common.h
        src/include/xtp/xtp_quote_api.h
        src/include/xtp/xtp_trader_api.h
        src/include/com_zts_xtp_quote_api_QuoteApi.h
        src/include/com_zts_xtp_trade_api_TradeApi.h
        src/include/handle.h
        src/include/InitGlog.h
        src/include/RingBuffer.hpp
        src/include/XtpTradeApi.h
        src/InitGlog.cpp
        src/XtpTradeAdapter.cpp
        src/XtpTradeApi.cpp
        )
add_library(quoteplugin SHARED
        src/include/gflags/gflags.h
        src/include/gflags/gflags_completions.h
        src/include/gflags/gflags_declare.h
        src/include/glog/log_severity.h
        src/include/glog/logging.h
        src/include/glog/raw_logging.h
        src/include/glog/stl_logging.h
        src/include/glog/vlog_is_on.h
        src/include/xtp/xoms_api_fund_struct.h
        src/include/xtp/xoms_api_struct.h
        src/include/xtp/xquote_api_struct.h
        src/include/xtp/xtp_api_data_type.h
        src/include/xtp/xtp_api_struct.h
        src/include/xtp/xtp_api_struct_common.h
        src/include/xtp/xtp_quote_api.h
        src/include/xtp/xtp_trader_api.h
        src/include/com_zts_xtp_quote_api_QuoteApi.h
        src/include/com_zts_xtp_trade_api_TradeApi.h
        src/include/handle.h
        src/include/InitGlog.h
        src/include/XtpQuoteApi.h
        src/InitGlog.cpp
        src/XtpQuoteAdapter.cpp
        src/XtpQuoteApi.cpp
        )
IF (WIN32)
    #copy tradeplugin quoteplugin to os lib path
#    TARGET_LINK_LIBRARIES(tradeplugin xtptraderapi.dll glog.dll )
    TARGET_LINK_LIBRARIES(tradeplugin xtptraderapi_xalgo.dll glog.dll )
    TARGET_LINK_LIBRARIES(quoteplugin xtpquoteapi.dll glog.dll )
    install(TARGETS tradeplugin DESTINATION C:/Windows/System32)
    install(TARGETS quoteplugin DESTINATION C:/Windows/System32)
	
    #copy xtp lib and glog lib to os lib path
	IF (CMAKE_CL_64)# win64,add by jiangch
	#win64
	install(FILES lib/win64/dll/glog.dll DESTINATION C:/Windows/System32)
    install(FILES lib/win64/dll/xtpquoteapi.dll DESTINATION C:/Windows/System32)
#    install(FILES lib/win64/dll/xtptraderapi.dll DESTINATION C:/Windows/System32)
    install(FILES lib/win64/dll/xtptraderapi_xalgo.dll DESTINATION C:/Windows/System32)
	 #copy xtp lib and glog lib to os lib path
    install(FILES C:/Windows/System32/tradeplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/win64)
    install(FILES C:/Windows/System32/quoteplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/win64)
    #copy all lib to os javaproject/build/
    install(FILES C:/Windows/System32/glog.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
#    install(FILES C:/Windows/System32/xtptraderapi.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
    install(FILES C:/Windows/System32/xtptraderapi_xalgo.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
    install(FILES C:/Windows/System32/xtpquoteapi.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
    install(FILES C:/Windows/System32/tradeplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
    install(FILES C:/Windows/System32/quoteplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win64/Windows/System32)
	ELSE ()
	#win32
    install(FILES lib/win32/dll/glog.dll DESTINATION C:/Windows/System32)
    install(FILES lib/win32/dll/xtpquoteapi.dll DESTINATION C:/Windows/System32)
#    install(FILES lib/win32/dll/xtptraderapi.dll DESTINATION C:/Windows/System32)
    install(FILES lib/win32/dll/xtptraderapi_xalgo.dll DESTINATION C:/Windows/System32)
	install(FILES C:/Windows/System32/tradeplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/win32)
    install(FILES C:/Windows/System32/quoteplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/win32)
    #copy all lib to os javaproject/build/
    install(FILES C:/Windows/System32/glog.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
#    install(FILES C:/Windows/System32/xtptraderapi.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
    install(FILES C:/Windows/System32/xtptraderapi_xalgo.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
    install(FILES C:/Windows/System32/xtpquoteapi.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
    install(FILES C:/Windows/System32/tradeplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
    install(FILES C:/Windows/System32/quoteplugin.dll DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win32/Windows/System32)
	ENDIF()
	
   

    #copy all lib to os javaproject/build/
	#begin remove，removed by jiangch below
    #install(FILES C:/Windows/System32/glog.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/win/C:/Windows/System32)
    #install(FILES C:/Windows/System32/xtptraderapi.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/win/C:/Windows/System32)
    #install(FILES C:/Windows/System32/xtpquoteapi.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/win/C:/Windows/System32)
	#end remove above
ELSEIF (APPLE)
    #copy tradeplugin quoteplugin to os lib path
    TARGET_LINK_LIBRARIES(tradeplugin xtptraderapi.dylib glog.dylib )
    TARGET_LINK_LIBRARIES(quoteplugin xtpquoteapi.dylib glog.dylib )
    install(TARGETS tradeplugin DESTINATION /usr/local/lib)
    install(TARGETS quoteplugin DESTINATION /usr/local/lib)
    #copy xtp lib and glog lib to os lib path
    install(FILES lib/macosx/libglog.dylib DESTINATION /usr/local/lib)
    install(FILES lib/macosx/libxtptraderapi.dylib DESTINATION /usr/local/lib)
    install(FILES lib/macosx/libxtpquoteapi.dylib DESTINATION /usr/local/lib)
    #copy tradeplugin quoteplugin lib to os buildcpp path
    install(FILES /usr/local/lib/libtradeplugin.dylib DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/macosx)
    install(FILES /usr/local/lib/libquoteplugin.dylib DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/macosx)
    #copy all lib to os javaproject/build/
    install(FILES /usr/local/lib/libglog.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/macosx/usr/local/lib)
    install(FILES /usr/local/lib/libxtptraderapi.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/macosx/usr/local/lib)
    install(FILES /usr/local/lib/libxtpquoteapi.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/macosx/usr/local/lib)
    install(FILES /usr/local/lib/libtradeplugin.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/macosx/usr/local/lib)
    install(FILES /usr/local/lib/libquoteplugin.dylib DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/macosx/usr/local/lib)
ELSEIF (UNIX)
    #copy tradeplugin quoteplugin to os lib path
    TARGET_LINK_LIBRARIES(tradeplugin xtptraderapi.so glog.so )
    TARGET_LINK_LIBRARIES(quoteplugin xtpquoteapi.so glog.so )
    install(TARGETS tradeplugin DESTINATION /usr/local/lib)
    install(TARGETS quoteplugin DESTINATION /usr/local/lib)
    #copy xtp lib and glog lib to os lib path
    install(FILES lib/linux/libglog.so DESTINATION /usr/local/lib)
    install(FILES lib/linux/libxtptraderapi.so DESTINATION /usr/local/lib)
    install(FILES lib/linux/libxtpquoteapi.so DESTINATION /usr/local/lib)
    #copy tradeplugin quoteplugin lib to os buildcpp path
    install(FILES /usr/local/lib/libtradeplugin.so DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/linux)
    install(FILES /usr/local/lib/libquoteplugin.so DESTINATION ${PROJECT_SOURCE_DIR}/buildcpp/linux)
    #copy all lib to os javaproject/build/
    install(FILES /usr/local/lib/libglog.so DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/linux/usr/local/lib)
    install(FILES /usr/local/lib/libxtptraderapi.so DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/linux/usr/local/lib)
    install(FILES /usr/local/lib/libxtpquoteapi.so DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/linux/usr/local/lib)
    install(FILES /usr/local/lib/libtradeplugin.so DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/linux/usr/local/lib)
    install(FILES /usr/local/lib/libquoteplugin.so DESTINATION ${PROJECT_SOURCE_DIR}/../build/libs/linux/usr/local/lib)
ENDIF ()
