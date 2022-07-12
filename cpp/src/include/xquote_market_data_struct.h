//
// Created by root on 7/7/22.
//

#ifndef CPP_XQUOTE_MARKET_DATA_STRUCT_H
#define CPP_XQUOTE_MARKET_DATA_STRUCT_H

#endif //CPP_XQUOTE_MARKET_DATA_STRUCT_H

#include <stdint.h>
#include "xquote_api_struct.h"

///ÐÐÇé
typedef struct XTPFullMarketDataStruct
{

    XTPMD market_data;
    int64_t bid1_qty[50];
    int32_t bid1_count;
    int32_t max_bid1_count;
    int64_t ask1_qty[50];
    int32_t ask1_count;
    int32_t max_ask1_count;

} XTPFMD;