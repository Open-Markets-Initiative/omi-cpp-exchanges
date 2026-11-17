#pragma once

#include <ostream>
#include <string_view>


namespace iex::iexequities::basicmatchingengine::ipc::v2026 {

namespace ipc_basicmatchingengine = ::iex::iexequities::basicmatchingengine::ipc::v2026;

inline std::ostream& operator<<(std::ostream& os, const message_header& value) {
    os << "message_length=" << value.message_length.get().value()
       << ",message_type=\"" << value.message_type.get().value() << '"'
       ;
    return os;
}

}
