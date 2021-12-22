package com.zts.xtp.common.enums;

/**
 * 证券适当性类型
 */
public enum QualificationType {
    /**公众投资者，合格投资者与机构投资者均可*/
    XTP_QUALIFICATION_PUBLIC(0),
    /**仅合格投资者与公众投资者*/
    XTP_QUALIFICATION_COMMON(1),
    /**仅限机构投资者*/
    XTP_QUALIFICATION_ORGANIZATION(2),
    /**未知，期权等可能为此种类型*/
    XTP_QUALIFICATION_UNKNOWN(3);

    public final int type;

    QualificationType(int type) {
        this.type = type;
    }

    public static QualificationType forType(int type) {
        switch (type) {
            case 0:
                return XTP_QUALIFICATION_PUBLIC;
            case 1:
                return XTP_QUALIFICATION_COMMON;
            case 2:
                return XTP_QUALIFICATION_ORGANIZATION;
            case 3:
                return XTP_QUALIFICATION_UNKNOWN;
            default:
                return XTP_QUALIFICATION_UNKNOWN;
        }
    }

    public int getType(){
        return type;
    }
}
