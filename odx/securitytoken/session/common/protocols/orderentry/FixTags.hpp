#pragma once

#include <cstdint>

namespace odxodxsecuritytoken::order_entry {

struct FixTag {
    static constexpr uint32_t Account               =   1;

    static constexpr uint32_t AvgPx                 =   6;

    static constexpr uint32_t BeginSeqNo            =   7;

    static constexpr uint32_t ClOrdID               =  11;

    static constexpr uint32_t CumQty                =  14;

    static constexpr uint32_t EndSeqNo              =  16;

    static constexpr uint32_t ExecID                =  17;

    static constexpr uint32_t ExecTransType         =  20;

    static constexpr uint32_t HandlInst             =  21;

    static constexpr uint32_t IDSource              =  22;

    static constexpr uint32_t LastPx                =  31;

    static constexpr uint32_t LastShares            =  32;

    static constexpr uint32_t NewSeqNo              =  36;

    static constexpr uint32_t OrderID               =  37;

    static constexpr uint32_t OrderQty              =  38;

    static constexpr uint32_t OrdStatus             =  39;

    static constexpr uint32_t OrdType               =  40;

    static constexpr uint32_t OrigClOrdID           =  41;

    static constexpr uint32_t Price                 =  44;

    static constexpr uint32_t RefSeqNum             =  45;

    static constexpr uint32_t Rule80A               =  47;

    static constexpr uint32_t SecurityID            =  48;

    static constexpr uint32_t Side                  =  54;

    static constexpr uint32_t Symbol                =  55;

    static constexpr uint32_t Text                  =  58;

    static constexpr uint32_t TimeInForce           =  59;

    static constexpr uint32_t TransactTime          =  60;

    static constexpr uint32_t EncryptMethod         =  98;

    static constexpr uint32_t CxlRejReason          = 102;

    static constexpr uint32_t OrdRejReason          = 103;

    static constexpr uint32_t HeartBtInt            = 108;

    static constexpr uint32_t TestReqID             = 112;

    static constexpr uint32_t GapFillFlag           = 123;

    static constexpr uint32_t ResetSeqNumFlag       = 141;

    static constexpr uint32_t ExecType              = 150;

    static constexpr uint32_t LeavesQty             = 151;

    static constexpr uint32_t TradingSessionID      = 336;

    static constexpr uint32_t TradSesMode           = 339;

    static constexpr uint32_t TradSesStatus         = 340;

    static constexpr uint32_t RefTagID              = 371;

    static constexpr uint32_t RefMsgType            = 372;

    static constexpr uint32_t SessionRejectReason   = 373;

    static constexpr uint32_t ContraBroker          = 375;

    static constexpr uint32_t ExecRestatementReason = 378;

    static constexpr uint32_t BusinessRejectReason  = 380;

    static constexpr uint32_t CxlRejResponseTo      = 434;

    static constexpr uint32_t TrdMatchID            = 880;
};

}  // namespace odxodxsecuritytoken::order_entry
