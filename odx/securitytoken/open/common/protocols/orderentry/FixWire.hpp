#pragma once

#include <cstddef>
#include <cstdint>
#include <charconv>
#include <cstdio>
#include <cstring>
#include <string_view>
#include "FixTags.hpp"
#include "Logon.hpp"
#include "Heartbeat.hpp"
#include "TestRequest.hpp"
#include "ResendRequest.hpp"
#include "Reject.hpp"
#include "SequenceReset.hpp"
#include "Logout.hpp"
#include "NewOrderSingle.hpp"
#include "OrderCancelRequest.hpp"
#include "OrderCancelReplaceRequest.hpp"
#include "OrderCancelReject.hpp"
#include "ExecutionReport.hpp"
#include "BusinessMessageReject.hpp"
#include "TradingSessionStatus.hpp"

namespace odxodxsecuritytoken::order_entry {

namespace detail {

inline char* tag(char* p, std::uint32_t t) {
    p = std::to_chars(p, p + 10, t).ptr;
    *p++ = '=';
    return p;
}
inline char* put_uint(char* p, std::uint32_t t, std::uint64_t v) {
    p = tag(p, t); p = std::to_chars(p, p + 20, v).ptr; *p++ = '\x01'; return p;
}
inline char* put_int(char* p, std::uint32_t t, std::int64_t v) {
    p = tag(p, t); p = std::to_chars(p, p + 20, v).ptr; *p++ = '\x01'; return p;
}
inline char* put_dec(char* p, std::uint32_t t, double v) {
    p = tag(p, t); p += std::snprintf(p, 32, "%.8g", v); *p++ = '\x01'; return p;
}
inline char* put_str(char* p, std::uint32_t t, const char* s, std::size_t n) {
    p = tag(p, t);
    while (n && (s[n - 1] == '\0' || s[n - 1] == ' ')) --n;
    std::memcpy(p, s, n); p += n; *p++ = '\x01'; return p;
}
inline char* put_chr(char* p, std::uint32_t t, char c) {
    p = tag(p, t); *p++ = c; *p++ = '\x01'; return p;
}

inline std::int64_t  get_int(std::string_view v)  { std::int64_t  x = 0; std::from_chars(v.data(), v.data() + v.size(), x); return x; }
inline std::uint64_t get_uint(std::string_view v) { std::uint64_t x = 0; std::from_chars(v.data(), v.data() + v.size(), x); return x; }
inline double        get_dec(std::string_view v)  { double x = 0; std::from_chars(v.data(), v.data() + v.size(), x); return x; }
inline char          get_chr(std::string_view v)  { return v.empty() ? '\0' : v.front(); }
inline void          get_str(std::string_view v, char* dst, std::size_t n) {
    std::size_t len = v.size() < n ? v.size() : n;
    std::memcpy(dst, v.data(), len);
    if (len < n) std::memset(dst + len, 0, n - len);
}

// FIX checksum: sum of every byte in [begin, end) modulo 256.
inline std::uint32_t checksum(const char* begin, const char* end) {
    std::uint32_t sum = 0;
    for (const char* p = begin; p != end; ++p) sum += static_cast<unsigned char>(*p);
    return sum % 256u;
}

// Wrap an assembled body ("35=<type>\x01 …fields… \x01") with the 8=BeginString /
// 9=BodyLength standard header and the 10=CheckSum trailer. Returns bytes written.
inline std::size_t frame(std::string_view begin_string, const char* body, std::size_t body_len, char* out) {
    char* p = out;
    std::memcpy(p, "8=", 2); p += 2;
    std::memcpy(p, begin_string.data(), begin_string.size()); p += begin_string.size();
    *p++ = '\x01';
    std::memcpy(p, "9=", 2); p += 2;
    p = std::to_chars(p, p + 20, body_len).ptr;
    *p++ = '\x01';
    std::memcpy(p, body, body_len); p += body_len;
    std::uint32_t cs = checksum(out, p);
    std::memcpy(p, "10=", 3); p += 3;
    p += std::snprintf(p, 5, "%03u", cs);
    *p++ = '\x01';
    return static_cast<std::size_t>(p - out);
}

}
// Encode Logon body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const Logon& m, char* out) {
    char* p = out;
    p = detail::put_int(p, FixTag::EncryptMethod, m.encrypt_method);
    p = detail::put_int(p, FixTag::HeartBtInt, m.heart_bt_int);
    p = detail::put_chr(p, FixTag::ResetSeqNumFlag, m.reset_seq_num_flag);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into Logon.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, Logon& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::EncryptMethod: out.encrypt_method = static_cast<decltype(out.encrypt_method)>(detail::get_int(v)); break;
            case FixTag::HeartBtInt: out.heart_bt_int = static_cast<decltype(out.heart_bt_int)>(detail::get_int(v)); break;
            case FixTag::ResetSeqNumFlag: out.reset_seq_num_flag = detail::get_chr(v); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode Heartbeat body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const Heartbeat& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::TestReqID, m.test_req_id, sizeof(m.test_req_id));
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into Heartbeat.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, Heartbeat& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::TestReqID: detail::get_str(v, out.test_req_id, sizeof(out.test_req_id)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode TestRequest body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const TestRequest& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::TestReqID, m.test_req_id, sizeof(m.test_req_id));
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into TestRequest.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, TestRequest& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::TestReqID: detail::get_str(v, out.test_req_id, sizeof(out.test_req_id)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode ResendRequest body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const ResendRequest& m, char* out) {
    char* p = out;
    p = detail::put_int(p, FixTag::BeginSeqNo, m.begin_seq_no);
    p = detail::put_int(p, FixTag::EndSeqNo, m.end_seq_no);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into ResendRequest.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, ResendRequest& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::BeginSeqNo: out.begin_seq_no = static_cast<decltype(out.begin_seq_no)>(detail::get_int(v)); break;
            case FixTag::EndSeqNo: out.end_seq_no = static_cast<decltype(out.end_seq_no)>(detail::get_int(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode Reject body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const Reject& m, char* out) {
    char* p = out;
    p = detail::put_int(p, FixTag::RefSeqNum, m.ref_seq_num);
    p = detail::put_str(p, FixTag::Text, m.text, sizeof(m.text));
    p = detail::put_int(p, FixTag::RefTagID, m.ref_tag_id);
    p = detail::put_str(p, FixTag::RefMsgType, m.ref_msg_type, sizeof(m.ref_msg_type));
    p = detail::put_int(p, FixTag::SessionRejectReason, m.session_reject_reason);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into Reject.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, Reject& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::RefSeqNum: out.ref_seq_num = static_cast<decltype(out.ref_seq_num)>(detail::get_int(v)); break;
            case FixTag::Text: detail::get_str(v, out.text, sizeof(out.text)); break;
            case FixTag::RefTagID: out.ref_tag_id = static_cast<decltype(out.ref_tag_id)>(detail::get_int(v)); break;
            case FixTag::RefMsgType: detail::get_str(v, out.ref_msg_type, sizeof(out.ref_msg_type)); break;
            case FixTag::SessionRejectReason: out.session_reject_reason = static_cast<decltype(out.session_reject_reason)>(detail::get_int(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode SequenceReset body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const SequenceReset& m, char* out) {
    char* p = out;
    p = detail::put_int(p, FixTag::NewSeqNo, m.new_seq_no);
    p = detail::put_chr(p, FixTag::GapFillFlag, m.gap_fill_flag);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into SequenceReset.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, SequenceReset& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::NewSeqNo: out.new_seq_no = static_cast<decltype(out.new_seq_no)>(detail::get_int(v)); break;
            case FixTag::GapFillFlag: out.gap_fill_flag = detail::get_chr(v); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode Logout body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const Logout& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::Text, m.text, sizeof(m.text));
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into Logout.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, Logout& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::Text: detail::get_str(v, out.text, sizeof(out.text)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode NewOrderSingle body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const NewOrderSingle& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::Account, m.account, sizeof(m.account));
    p = detail::put_str(p, FixTag::ClOrdID, m.cl_ord_id, sizeof(m.cl_ord_id));
    p = detail::put_chr(p, FixTag::HandlInst, m.handl_inst);
    p = detail::put_chr(p, FixTag::IDSource, m.idsource);
    p = detail::put_dec(p, FixTag::OrderQty, m.order_qty);
    p = detail::put_chr(p, FixTag::OrdType, m.ord_type);
    p = detail::put_dec(p, FixTag::Price, m.price);
    p = detail::put_chr(p, FixTag::Rule80A, m.rule80_a);
    p = detail::put_str(p, FixTag::SecurityID, m.security_id, sizeof(m.security_id));
    p = detail::put_chr(p, FixTag::Side, m.side);
    p = detail::put_str(p, FixTag::Symbol, m.symbol, sizeof(m.symbol));
    p = detail::put_chr(p, FixTag::TimeInForce, m.time_in_force);
    p = detail::put_uint(p, FixTag::TransactTime, m.transact_time);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into NewOrderSingle.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, NewOrderSingle& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::Account: detail::get_str(v, out.account, sizeof(out.account)); break;
            case FixTag::ClOrdID: detail::get_str(v, out.cl_ord_id, sizeof(out.cl_ord_id)); break;
            case FixTag::HandlInst: out.handl_inst = detail::get_chr(v); break;
            case FixTag::IDSource: out.idsource = detail::get_chr(v); break;
            case FixTag::OrderQty: out.order_qty = detail::get_dec(v); break;
            case FixTag::OrdType: out.ord_type = detail::get_chr(v); break;
            case FixTag::Price: out.price = detail::get_dec(v); break;
            case FixTag::Rule80A: out.rule80_a = detail::get_chr(v); break;
            case FixTag::SecurityID: detail::get_str(v, out.security_id, sizeof(out.security_id)); break;
            case FixTag::Side: out.side = detail::get_chr(v); break;
            case FixTag::Symbol: detail::get_str(v, out.symbol, sizeof(out.symbol)); break;
            case FixTag::TimeInForce: out.time_in_force = detail::get_chr(v); break;
            case FixTag::TransactTime: out.transact_time = static_cast<decltype(out.transact_time)>(detail::get_uint(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode OrderCancelRequest body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const OrderCancelRequest& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::ClOrdID, m.cl_ord_id, sizeof(m.cl_ord_id));
    p = detail::put_dec(p, FixTag::OrderQty, m.order_qty);
    p = detail::put_str(p, FixTag::OrigClOrdID, m.orig_cl_ord_id, sizeof(m.orig_cl_ord_id));
    p = detail::put_chr(p, FixTag::Side, m.side);
    p = detail::put_str(p, FixTag::Symbol, m.symbol, sizeof(m.symbol));
    p = detail::put_uint(p, FixTag::TransactTime, m.transact_time);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into OrderCancelRequest.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, OrderCancelRequest& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::ClOrdID: detail::get_str(v, out.cl_ord_id, sizeof(out.cl_ord_id)); break;
            case FixTag::OrderQty: out.order_qty = detail::get_dec(v); break;
            case FixTag::OrigClOrdID: detail::get_str(v, out.orig_cl_ord_id, sizeof(out.orig_cl_ord_id)); break;
            case FixTag::Side: out.side = detail::get_chr(v); break;
            case FixTag::Symbol: detail::get_str(v, out.symbol, sizeof(out.symbol)); break;
            case FixTag::TransactTime: out.transact_time = static_cast<decltype(out.transact_time)>(detail::get_uint(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode OrderCancelReplaceRequest body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const OrderCancelReplaceRequest& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::ClOrdID, m.cl_ord_id, sizeof(m.cl_ord_id));
    p = detail::put_chr(p, FixTag::HandlInst, m.handl_inst);
    p = detail::put_dec(p, FixTag::OrderQty, m.order_qty);
    p = detail::put_chr(p, FixTag::OrdType, m.ord_type);
    p = detail::put_str(p, FixTag::OrigClOrdID, m.orig_cl_ord_id, sizeof(m.orig_cl_ord_id));
    p = detail::put_dec(p, FixTag::Price, m.price);
    p = detail::put_chr(p, FixTag::Rule80A, m.rule80_a);
    p = detail::put_chr(p, FixTag::Side, m.side);
    p = detail::put_str(p, FixTag::Symbol, m.symbol, sizeof(m.symbol));
    p = detail::put_chr(p, FixTag::TimeInForce, m.time_in_force);
    p = detail::put_uint(p, FixTag::TransactTime, m.transact_time);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into OrderCancelReplaceRequest.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, OrderCancelReplaceRequest& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::ClOrdID: detail::get_str(v, out.cl_ord_id, sizeof(out.cl_ord_id)); break;
            case FixTag::HandlInst: out.handl_inst = detail::get_chr(v); break;
            case FixTag::OrderQty: out.order_qty = detail::get_dec(v); break;
            case FixTag::OrdType: out.ord_type = detail::get_chr(v); break;
            case FixTag::OrigClOrdID: detail::get_str(v, out.orig_cl_ord_id, sizeof(out.orig_cl_ord_id)); break;
            case FixTag::Price: out.price = detail::get_dec(v); break;
            case FixTag::Rule80A: out.rule80_a = detail::get_chr(v); break;
            case FixTag::Side: out.side = detail::get_chr(v); break;
            case FixTag::Symbol: detail::get_str(v, out.symbol, sizeof(out.symbol)); break;
            case FixTag::TimeInForce: out.time_in_force = detail::get_chr(v); break;
            case FixTag::TransactTime: out.transact_time = static_cast<decltype(out.transact_time)>(detail::get_uint(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode OrderCancelReject body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const OrderCancelReject& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::ClOrdID, m.cl_ord_id, sizeof(m.cl_ord_id));
    p = detail::put_str(p, FixTag::OrderID, m.order_id, sizeof(m.order_id));
    p = detail::put_chr(p, FixTag::OrdStatus, m.ord_status);
    p = detail::put_str(p, FixTag::OrigClOrdID, m.orig_cl_ord_id, sizeof(m.orig_cl_ord_id));
    p = detail::put_str(p, FixTag::Text, m.text, sizeof(m.text));
    p = detail::put_int(p, FixTag::CxlRejReason, m.cxl_rej_reason);
    p = detail::put_chr(p, FixTag::CxlRejResponseTo, m.cxl_rej_response_to);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into OrderCancelReject.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, OrderCancelReject& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::ClOrdID: detail::get_str(v, out.cl_ord_id, sizeof(out.cl_ord_id)); break;
            case FixTag::OrderID: detail::get_str(v, out.order_id, sizeof(out.order_id)); break;
            case FixTag::OrdStatus: out.ord_status = detail::get_chr(v); break;
            case FixTag::OrigClOrdID: detail::get_str(v, out.orig_cl_ord_id, sizeof(out.orig_cl_ord_id)); break;
            case FixTag::Text: detail::get_str(v, out.text, sizeof(out.text)); break;
            case FixTag::CxlRejReason: out.cxl_rej_reason = static_cast<decltype(out.cxl_rej_reason)>(detail::get_int(v)); break;
            case FixTag::CxlRejResponseTo: out.cxl_rej_response_to = detail::get_chr(v); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode ExecutionReport body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const ExecutionReport& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::Account, m.account, sizeof(m.account));
    p = detail::put_dec(p, FixTag::AvgPx, m.avg_px);
    p = detail::put_str(p, FixTag::ClOrdID, m.cl_ord_id, sizeof(m.cl_ord_id));
    p = detail::put_dec(p, FixTag::CumQty, m.cum_qty);
    p = detail::put_str(p, FixTag::ExecID, m.exec_id, sizeof(m.exec_id));
    p = detail::put_chr(p, FixTag::ExecTransType, m.exec_trans_type);
    p = detail::put_dec(p, FixTag::LastPx, m.last_px);
    p = detail::put_dec(p, FixTag::LastShares, m.last_shares);
    p = detail::put_str(p, FixTag::OrderID, m.order_id, sizeof(m.order_id));
    p = detail::put_dec(p, FixTag::OrderQty, m.order_qty);
    p = detail::put_chr(p, FixTag::OrdStatus, m.ord_status);
    p = detail::put_chr(p, FixTag::OrdType, m.ord_type);
    p = detail::put_str(p, FixTag::OrigClOrdID, m.orig_cl_ord_id, sizeof(m.orig_cl_ord_id));
    p = detail::put_dec(p, FixTag::Price, m.price);
    p = detail::put_chr(p, FixTag::Rule80A, m.rule80_a);
    p = detail::put_chr(p, FixTag::Side, m.side);
    p = detail::put_str(p, FixTag::Symbol, m.symbol, sizeof(m.symbol));
    p = detail::put_str(p, FixTag::Text, m.text, sizeof(m.text));
    p = detail::put_chr(p, FixTag::TimeInForce, m.time_in_force);
    p = detail::put_uint(p, FixTag::TransactTime, m.transact_time);
    p = detail::put_int(p, FixTag::OrdRejReason, m.ord_rej_reason);
    p = detail::put_chr(p, FixTag::ExecType, m.exec_type);
    p = detail::put_dec(p, FixTag::LeavesQty, m.leaves_qty);
    p = detail::put_int(p, FixTag::ExecRestatementReason, m.exec_restatement_reason);
    p = detail::put_str(p, FixTag::ContraBroker, m.contra_broker, sizeof(m.contra_broker));
    p = detail::put_str(p, FixTag::TrdMatchID, m.trd_match_id, sizeof(m.trd_match_id));
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into ExecutionReport.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, ExecutionReport& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::Account: detail::get_str(v, out.account, sizeof(out.account)); break;
            case FixTag::AvgPx: out.avg_px = detail::get_dec(v); break;
            case FixTag::ClOrdID: detail::get_str(v, out.cl_ord_id, sizeof(out.cl_ord_id)); break;
            case FixTag::CumQty: out.cum_qty = detail::get_dec(v); break;
            case FixTag::ExecID: detail::get_str(v, out.exec_id, sizeof(out.exec_id)); break;
            case FixTag::ExecTransType: out.exec_trans_type = detail::get_chr(v); break;
            case FixTag::LastPx: out.last_px = detail::get_dec(v); break;
            case FixTag::LastShares: out.last_shares = detail::get_dec(v); break;
            case FixTag::OrderID: detail::get_str(v, out.order_id, sizeof(out.order_id)); break;
            case FixTag::OrderQty: out.order_qty = detail::get_dec(v); break;
            case FixTag::OrdStatus: out.ord_status = detail::get_chr(v); break;
            case FixTag::OrdType: out.ord_type = detail::get_chr(v); break;
            case FixTag::OrigClOrdID: detail::get_str(v, out.orig_cl_ord_id, sizeof(out.orig_cl_ord_id)); break;
            case FixTag::Price: out.price = detail::get_dec(v); break;
            case FixTag::Rule80A: out.rule80_a = detail::get_chr(v); break;
            case FixTag::Side: out.side = detail::get_chr(v); break;
            case FixTag::Symbol: detail::get_str(v, out.symbol, sizeof(out.symbol)); break;
            case FixTag::Text: detail::get_str(v, out.text, sizeof(out.text)); break;
            case FixTag::TimeInForce: out.time_in_force = detail::get_chr(v); break;
            case FixTag::TransactTime: out.transact_time = static_cast<decltype(out.transact_time)>(detail::get_uint(v)); break;
            case FixTag::OrdRejReason: out.ord_rej_reason = static_cast<decltype(out.ord_rej_reason)>(detail::get_int(v)); break;
            case FixTag::ExecType: out.exec_type = detail::get_chr(v); break;
            case FixTag::LeavesQty: out.leaves_qty = detail::get_dec(v); break;
            case FixTag::ExecRestatementReason: out.exec_restatement_reason = static_cast<decltype(out.exec_restatement_reason)>(detail::get_int(v)); break;
            case FixTag::ContraBroker: detail::get_str(v, out.contra_broker, sizeof(out.contra_broker)); break;
            case FixTag::TrdMatchID: detail::get_str(v, out.trd_match_id, sizeof(out.trd_match_id)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode BusinessMessageReject body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const BusinessMessageReject& m, char* out) {
    char* p = out;
    p = detail::put_int(p, FixTag::RefSeqNum, m.ref_seq_num);
    p = detail::put_str(p, FixTag::Text, m.text, sizeof(m.text));
    p = detail::put_str(p, FixTag::RefMsgType, m.ref_msg_type, sizeof(m.ref_msg_type));
    p = detail::put_int(p, FixTag::BusinessRejectReason, m.business_reject_reason);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into BusinessMessageReject.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, BusinessMessageReject& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::RefSeqNum: out.ref_seq_num = static_cast<decltype(out.ref_seq_num)>(detail::get_int(v)); break;
            case FixTag::Text: detail::get_str(v, out.text, sizeof(out.text)); break;
            case FixTag::RefMsgType: detail::get_str(v, out.ref_msg_type, sizeof(out.ref_msg_type)); break;
            case FixTag::BusinessRejectReason: out.business_reject_reason = static_cast<decltype(out.business_reject_reason)>(detail::get_int(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

// Encode TradingSessionStatus body as tag=value (SOH-delimited).
// Returns bytes written.
inline std::size_t encode(const TradingSessionStatus& m, char* out) {
    char* p = out;
    p = detail::put_str(p, FixTag::TradingSessionID, m.trading_session_id, sizeof(m.trading_session_id));
    p = detail::put_int(p, FixTag::TradSesMode, m.trad_ses_mode);
    p = detail::put_int(p, FixTag::TradSesStatus, m.trad_ses_status);
    return static_cast<std::size_t>(p - out);
}

// Decode a tag=value body into TradingSessionStatus.
// Returns false on a malformed field.
inline bool decode(std::string_view msg, TradingSessionStatus& out) {
    std::size_t i = 0;
    while (i < msg.size()) {
        std::uint32_t t = 0;
        auto r = std::from_chars(msg.data() + i, msg.data() + msg.size(), t);
        if (r.ec != std::errc{} || r.ptr == msg.data() + msg.size() || *r.ptr != '=') return false;
        std::size_t vs = static_cast<std::size_t>(r.ptr - msg.data()) + 1;
        std::size_t soh = msg.find('\x01', vs);
        if (soh == std::string_view::npos) return false;
        std::string_view v = msg.substr(vs, soh - vs);
        switch (t) {
            case FixTag::TradingSessionID: detail::get_str(v, out.trading_session_id, sizeof(out.trading_session_id)); break;
            case FixTag::TradSesMode: out.trad_ses_mode = static_cast<decltype(out.trad_ses_mode)>(detail::get_int(v)); break;
            case FixTag::TradSesStatus: out.trad_ses_status = static_cast<decltype(out.trad_ses_status)>(detail::get_int(v)); break;
            default: break;
        }
        i = soh + 1;
    }
    return true;
}

}
