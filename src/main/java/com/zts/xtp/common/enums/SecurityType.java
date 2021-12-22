package com.zts.xtp.common.enums;

/**
 * 证券详细分类
 */
public enum SecurityType {
    /** 主板股票*/
    XTP_SECURITY_MAIN_BOARD(0),
    /** 中小板股票*/
    XTP_SECURITY_SECOND_BOARD(1),
    /** 创业板股票*/
    XTP_SECURITY_STARTUP_BOARD(2),
    /** 指数*/
    XTP_SECURITY_INDEX(3),
    /** 科创板股票(上海)*/
    XTP_SECURITY_TECH_BOARD(4),
    /** 国债*/
    XTP_SECURITY_STATE_BOND(5),
    /** 企业债*/
    XTP_SECURITY_ENTERPRICE_BOND(6),
    /** 公司债*/
    XTP_SECURITY_COMPANEY_BOND(7),
    /** 转换债券*/
    XTP_SECURITY_CONVERTABLE_BOND(8),
    /** 国债逆回购*/
    XTP_SECURITY_NATIONAL_BOND_REVERSE_REPO(12),
    /** 本市场股票 ETF*/
    XTP_SECURITY_ETF_SINGLE_MARKET_STOCK(14),
    /** 跨市场股票 ETF*/
    XTP_SECURITY_ETF_INTER_MARKET_STOCK(15),
    /**跨境股票 ETF*/
    XTP_SECURITY_ETF_CROSS_BORDER_STOCK(16),
    /** 本市场实物债券 ETF*/
    XTP_SECURITY_ETF_SINGLE_MARKET_BOND(17),
    /** 黄金 ETF*/
    XTP_SECURITY_ETF_GOLD(19),
    /** 分级基金子基金*/
    XTP_SECURITY_STRUCTURED_FUND_CHILD(24),
    /** 深交所仅申赎基金*/
    XTP_SECURITY_SZSE_RECREATION_FUND(26),
    /** 个股期权*/
    XTP_SECURITY_STOCK_OPTION(29),
    /** ETF期权*/
    XTP_SECURITY_ETF_OPTION(30),
    /** 配股*/
    XTP_SECURITY_ALLOTMENT(100),
    /** 上交所申赎型货币基金*/
    XTP_SECURITY_MONETARY_FUND_SHCR(110),
    /** 上交所交易型货币基金*/
    XTP_SECURITY_MONETARY_FUND_SHTR(111),
    /** 深交所货币基金*/
    XTP_SECURITY_MONETARY_FUND_SZ(112),
    /** 其他*/
    XTP_SECURITY_OTHERS(255);

    public final int type;

    SecurityType(int type) {
        this.type = type;
    }

    public static SecurityType forType(int type) {
        switch (type) {
            case 0:
                return XTP_SECURITY_MAIN_BOARD;
            case 1:
                return XTP_SECURITY_SECOND_BOARD;
            case 2:
                return XTP_SECURITY_STARTUP_BOARD;
            case 3:
                return XTP_SECURITY_INDEX;
            case 4:
                return XTP_SECURITY_TECH_BOARD;
            case 5:
                return XTP_SECURITY_STATE_BOND;
            case 6:
                return XTP_SECURITY_ENTERPRICE_BOND;
            case 7:
                return XTP_SECURITY_COMPANEY_BOND;
            case 8:
                return XTP_SECURITY_CONVERTABLE_BOND;
            case 12:
                return XTP_SECURITY_NATIONAL_BOND_REVERSE_REPO;
            case 14:
                return XTP_SECURITY_ETF_SINGLE_MARKET_STOCK;
            case 15:
                return XTP_SECURITY_ETF_INTER_MARKET_STOCK;
            case 16:
                return XTP_SECURITY_ETF_CROSS_BORDER_STOCK;
            case 17:
                return XTP_SECURITY_ETF_SINGLE_MARKET_BOND;
            case 19:
                return XTP_SECURITY_ETF_GOLD;
            case 24:
                return XTP_SECURITY_STRUCTURED_FUND_CHILD;
            case 26:
                return XTP_SECURITY_SZSE_RECREATION_FUND;
            case 29:
                return XTP_SECURITY_STOCK_OPTION;
            case 30:
                return XTP_SECURITY_ETF_OPTION;
            case 100:
                return XTP_SECURITY_ALLOTMENT;
            case 110:
                return XTP_SECURITY_MONETARY_FUND_SHCR;
            case 111:
                return XTP_SECURITY_MONETARY_FUND_SHTR;
            case 112:
                return XTP_SECURITY_MONETARY_FUND_SZ;
            case 255:
                return XTP_SECURITY_OTHERS;
            default:
                return XTP_SECURITY_OTHERS;
        }
    }

    public int getType(){
        return type;
    }
}
