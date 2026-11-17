// Generated Nasdaq Stock Market Session book check: the book Nasdaq.NsmEquities.Equities states, built from a recording
// of the venue's TotalView feed, held against the engine's own book.
//
// The book is generated from the book model (Types.hpp, State.hpp, Book.hpp). Each message of
// Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 the model's Protocol binds to an operation is read through the feed's own
// structs and applied. Each time the engine was asked for its book it wrote it to the file its
// settings' [book] dump names, and the feed's recorder marked, beside the recording, where in the
// feed that answer belongs; at each mark the two books are compared: the same orders, each on the
// same side at the same price for what the engine's book shows of it, and at each price in the
// order the engine would fill them.

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "Book.hpp"
#include "../protocols/marketdata/totalview/structs/PacketHeader.hpp"
#include "../protocols/marketdata/totalview/structs/MessageHeader.hpp"
#include "../protocols/marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../protocols/marketdata/totalview/messages/AddOrderWithMpidAttributionMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderExecutedMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderExecutedWithPriceMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderCancelMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderReplaceMessage.hpp"

namespace {

namespace feed = ::nasdaq::nsmequities::totalview::itch::v5_0_2026;

// The key the feed names each of the venue's orderbooks by, in the order the venue declares them,
// which is how the engine numbers its books.
constexpr std::array<book::Key, 1> listed{ { 1 } };

// The engine's prices are integers of 4 decimals; the book's are its own.
constexpr book::Price engine_scale = book::price_scale(4);

// One order as the engine said it shows it: where it holds it, and how much of it its book shows.
struct Said {
    char where;
    book::Key key;
    book::Side side;
    book::Price price;
    std::uint64_t order;
    std::uint64_t showing;
};

// The feed's recording, driving the book: each packet's messages, walked as
// 'Packet' lays them out.
class Built {
public:
    book::Equities built;
    std::uint64_t applied = 0;   // the messages that applied an operation
    std::uint64_t unknown = 0;   // those of them for an order the book does not hold

    // Every packet of the recording from where the last call stopped up to `upto`; false where
    // `upto` does not fall on a packet's end.
    bool walk(const std::vector<char>& recording, std::size_t upto) {
        if (upto > recording.size()) return false;
        const auto* data = reinterpret_cast<const std::byte*>(recording.data());
        while (at_ + sizeof(feed::packet_header) <= upto) {
            const std::size_t messages = feed::packet_header::parse(data + at_)->message_count.get().value();
            at_ += sizeof(feed::packet_header);
            if (messages == 65535) continue;   // the session's end, said by the count: no message follows
            for (std::size_t i = 0; i < messages; ++i) {
                if (at_ + sizeof(feed::message_header) > upto) return false;
                const auto* head = feed::message_header::parse(data + at_);
                const std::size_t block = 0 + sizeof(head->message_length) + head->message_length.get().value();
                if (at_ + block > upto) return false;
                apply(static_cast<char>(head->message_type.get().value()), data + at_);
                at_ += block;
            }
        }
        return at_ == upto;
    }

private:
    // One message: the operation the book model binds it to, or nothing where it binds none.
    void apply(char kind, const std::byte* bytes) {
        switch (kind) {
            case 'A': {   // Add Order No Mpid Attribution Message: OrderAdd
                const auto& m = *feed::add_order_no_mpid_attribution_message::parse(bytes);
                built.order_add(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Key>(m.fields.stock_locate.get().value()), static_cast<char>(m.fields.buy_sell_indicator.get().value()) == 'B' ? book::Side::Bid : book::Side::Ask, static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value()));
                ++applied;
                break;
            }
            case 'F': {   // Add Order With Mpid Attribution Message: OrderAdd
                const auto& m = *feed::add_order_with_mpid_attribution_message::parse(bytes);
                built.order_add(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Key>(m.fields.stock_locate.get().value()), static_cast<char>(m.fields.buy_sell_indicator.get().value()) == 'B' ? book::Side::Bid : book::Side::Ask, static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value()));
                ++applied;
                break;
            }
            case 'E': {   // Order Executed Message: OrderExecute
                const auto& m = *feed::order_executed_message::parse(bytes);
                if (!built.order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.executed_shares.get().value())).reported) ++unknown;   // for an order the book does not hold
                ++applied;
                break;
            }
            case 'C': {   // Order Executed With Price Message: OrderExecute
                const auto& m = *feed::order_executed_with_price_message::parse(bytes);
                if (!built.order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.executed_shares.get().value())).reported) ++unknown;   // for an order the book does not hold
                ++applied;
                break;
            }
            case 'X': {   // Order Cancel Message: OrderExecute
                const auto& m = *feed::order_cancel_message::parse(bytes);
                if (!built.order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.canceled_shares.get().value())).reported) ++unknown;   // for an order the book does not hold
                ++applied;
                break;
            }
            case 'D': {   // Order Delete Message: OrderDelete
                const auto& m = *feed::order_delete_message::parse(bytes);
                if (!built.order_delete(static_cast<book::OrderId>(m.fields.order_reference_number.get().value())).reported) ++unknown;   // for an order the book does not hold
                ++applied;
                break;
            }
            case 'U': {   // Order Replace Message: OrderReplace
                const auto& m = *feed::order_replace_message::parse(bytes);
                if (!built.order_replace(static_cast<book::OrderId>(m.fields.original_order_reference_number.get().value()), static_cast<book::OrderId>(m.fields.new_order_reference_number.get().value()), static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value())).reported) ++unknown;   // for an order the book does not hold
                ++applied;
                break;
            }
            default: break;   // a message the book model binds to no operation
        }
    }

    std::size_t at_ = 0;   // how far into the recording the book has been driven
};

// The marks beside the recording: each request for the engine's book, and how many bytes of the
// recording had been published when the feed heard the end of its answer.
std::vector<std::pair<std::uint64_t, std::size_t>> marked(const std::string& path) {
    std::vector<std::pair<std::uint64_t, std::size_t>> marks;
    std::ifstream in(path);
    std::string line;
    std::getline(in, line);
    while (std::getline(in, line)) {
        std::uint64_t request = 0, segments = 0;
        std::size_t bytes = 0;
        char comma = 0;
        std::istringstream row(line);
        if (row >> request >> comma >> segments >> comma >> bytes) marks.push_back({ request, bytes });
    }
    return marks;
}

// The engine's own book at each request, as it wrote it: sequence,book,where,side,price,quantity,
// order,shown,showing, an answer ending "# sequence,orders". Only what its book shows is kept, in
// the order it said it. False where a line is none of those.
bool said(const std::string& path, std::map<std::uint64_t, std::vector<Said>>& engine) {
    std::ifstream in(path);
    std::string line;
    std::getline(in, line);
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') { engine[std::stoull(line.substr(1))]; continue; }   // an answer, were it of no orders
        std::vector<std::string> cell;
        std::string part;
        std::istringstream row(line);
        while (std::getline(row, part, ',')) cell.push_back(part);
        if (cell.size() < 9 || cell[2].empty() || cell[3].empty()) { std::fprintf(stderr, "not a line of the engine's book: %s\n", line.c_str()); return false; }
        const std::size_t book = std::stoull(cell[1]);
        if (book >= listed.size()) { std::fprintf(stderr, "book %zu is none the venue lists: %s\n", book, line.c_str()); return false; }
        const std::uint64_t showing = std::stoull(cell[8]);
        if (showing == 0) continue;   // an order its book does not show, which no feed tells of
        engine[std::stoull(cell[0])].push_back(Said{ cell[2][0], listed[book],
            cell[3][0] == 'B' ? book::Side::Bid : book::Side::Ask,
            static_cast<book::Price>(std::stoll(cell[4])) * engine_scale, std::stoull(cell[6]), showing });
    }
    return true;
}

}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s <the TotalView recording> <the engine's book, as its [book] dump wrote it>\n", argv[0]);
        return 2;
    }
    const std::string recorded = argv[1];
    std::ifstream file(recorded, std::ios::binary);
    if (!file) { std::fprintf(stderr, "cannot read %s\n", argv[1]); return 2; }
    const std::vector<char> recording((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    const auto marks = marked(recorded + ".books");
    if (marks.empty()) { std::fprintf(stderr, "%s.books marks no answer of the engine's: was its book asked for ([book] every)?\n", argv[1]); return 2; }
    std::map<std::uint64_t, std::vector<Said>> engine;
    if (!said(argv[2], engine)) return 2;

    Built feed_book;
    std::size_t checks = 0, failed = 0, compared = 0;
    for (const auto& [request, upto] : marks) {
        ++checks;
        if (!feed_book.walk(recording, upto)) {
            ++failed;
            std::printf("FAIL request %llu: its mark, %zu bytes, falls on no packet's end in the recording\n", static_cast<unsigned long long>(request), upto);
            break;
        }
        const auto answered = engine.find(request);
        if (answered == engine.end()) {
            ++failed;
            std::printf("FAIL request %llu: the engine wrote no answer to it\n", static_cast<unsigned long long>(request));
            continue;
        }
        const auto& shows = answered->second;
        const auto& routed = feed_book.built.routing();

        // the same orders, each on the same side at the same price for the same quantity
        std::size_t differing = routed.size() == shows.size() ? 0 : 1;
        for (const auto& shown : shows) {
            const auto held = routed.find(shown.order);
            if (held != routed.end() && held->second.order() != nullptr && held->second.key() == shown.key && held->second.side() == shown.side
                && held->second.price() == shown.price && held->second.order()->size() == shown.showing) continue;
            if (differing++ < 4)
                std::printf("   request %llu: order %llu, which the engine shows %llu of, the book built from the feed %s\n", static_cast<unsigned long long>(request),
                            static_cast<unsigned long long>(shown.order), static_cast<unsigned long long>(shown.showing),
                            held == routed.end() ? "does not hold" : "holds otherwise");
        }

        // and at each price in the same order: of two orders the engine says one after the other
        // where it holds them alike, the second rests somewhere behind the first
        std::size_t misplaced = 0;
        std::map<std::tuple<char, book::Key, book::Side, book::Price>, const book::Order*> last;
        for (const auto& shown : shows) {
            const auto held = routed.find(shown.order);
            if (held == routed.end() || held->second.order() == nullptr) continue;
            const book::Order* order = held->second.order();
            const auto [before, first] = last.try_emplace({ shown.where, shown.key, shown.side, shown.price }, order);
            if (first) continue;
            const book::Order* behind = before->second->behind();
            while (behind != nullptr && behind != order) behind = behind->behind();
            if (behind == nullptr && misplaced++ < 4)
                std::printf("   request %llu: order %llu is not behind the order the engine says before it at its price\n", static_cast<unsigned long long>(request),
                            static_cast<unsigned long long>(shown.order));
            before->second = order;
        }

        compared += shows.size();
        if (differing == 0 && misplaced == 0)
            std::printf("ok   request %llu: %zu orders shown, the same in the engine and in the book built from the feed, in the same order at every price\n",
                        static_cast<unsigned long long>(request), shows.size());
        else {
            ++failed;
            std::printf("FAIL request %llu: the engine shows %zu orders and the book built from the feed holds %zu; %zu differ, %zu out of order\n",
                        static_cast<unsigned long long>(request), shows.size(), routed.size(), differing, misplaced);
        }
    }
    std::printf("%llu feed messages applied, %llu of them for an order the book does not hold\n",
                static_cast<unsigned long long>(feed_book.applied), static_cast<unsigned long long>(feed_book.unknown));
    std::printf("%zu of %zu books match (%zu orders compared)\n", checks - failed, marks.size(), compared);
    return failed == 0 ? 0 : 1;
}