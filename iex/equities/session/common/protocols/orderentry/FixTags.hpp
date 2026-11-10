#pragma once

#include <cstdint>

namespace iexiexequities::order_entry {

struct FixTag {
    static constexpr uint32_t Account                            =    1;

    static constexpr uint32_t AvgPrice                           =    6;

    static constexpr uint32_t BeginSeqNo                         =    7;

    static constexpr uint32_t ClOrdID                            =   11;

    static constexpr uint32_t CumQty                             =   14;

    static constexpr uint32_t EndSeqNo                           =   16;

    static constexpr uint32_t ExecID                             =   17;

    static constexpr uint32_t ExecInst                           =   18;

    static constexpr uint32_t ExecRefID                          =   19;

    static constexpr uint32_t ExecTransType                      =   20;

    static constexpr uint32_t HandlInst                          =   21;

    static constexpr uint32_t LastMkt                            =   30;

    static constexpr uint32_t LastPx                             =   31;

    static constexpr uint32_t LastShares                         =   32;

    static constexpr uint32_t NewSeqNo                           =   36;

    static constexpr uint32_t OrderID                            =   37;

    static constexpr uint32_t OrderQty                           =   38;

    static constexpr uint32_t OrdStatus                          =   39;

    static constexpr uint32_t OrdType                            =   40;

    static constexpr uint32_t OrigClOrdID                        =   41;

    static constexpr uint32_t OrigTime                           =   42;

    static constexpr uint32_t Price                              =   44;

    static constexpr uint32_t RefSeqNum                          =   45;

    static constexpr uint32_t OrderCapacity                      =   47;

    static constexpr uint32_t Side                               =   54;

    static constexpr uint32_t Symbol                             =   55;

    static constexpr uint32_t Text                               =   58;

    static constexpr uint32_t TimeInForce                        =   59;

    static constexpr uint32_t TransactTime                       =   60;

    static constexpr uint32_t SymbolSfx                          =   65;

    static constexpr uint32_t CxlRejReason                       =  102;

    static constexpr uint32_t OrdRejReason                       =  103;

    static constexpr uint32_t HeartBtInt                         =  108;

    static constexpr uint32_t ClientID                           =  109;

    static constexpr uint32_t MinQty                             =  110;

    static constexpr uint32_t MaxFloor                           =  111;

    static constexpr uint32_t TestReqID                          =  112;

    static constexpr uint32_t LocateReqd                         =  114;

    static constexpr uint32_t GapFillFlag                        =  123;

    static constexpr uint32_t ExpireTime                         =  126;

    static constexpr uint32_t ExecType                           =  150;

    static constexpr uint32_t LeavesQty                          =  151;

    static constexpr uint32_t SecondaryOrderID                   =  198;

    static constexpr uint32_t PegDifference                      =  211;

    static constexpr uint32_t SessionRejectReason                =  373;

    static constexpr uint32_t ContraBroker                       =  375;

    static constexpr uint32_t ExecRestatementReason              =  378;

    static constexpr uint32_t NoContraBrokers                    =  382;

    static constexpr uint32_t CxlRejResponseTo                   =  434;

    static constexpr uint32_t LastLiquidityInd                   =  851;

    static constexpr uint32_t AntiInternalizationQualifiersAIQID = 7928;

    static constexpr uint32_t DisplayRange                       = 8020;

    static constexpr uint32_t InvestorID                         = 8484;

    static constexpr uint32_t ExtendedExecInst                   = 9416;

    static constexpr uint32_t MinQtyInstruction                  = 9500;

    static constexpr uint32_t EffectiveLimitPrice                = 9690;

    static constexpr uint32_t TradeLiquidityIndicator            = 9730;

    static constexpr uint32_t FeeCode                            = 9882;
};

}  // namespace iexiexequities::order_entry
