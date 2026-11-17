# Nasdaq Stock Market Session Simulator

Generated exchange simulator for Nasdaq Stock Market Session: an order-entry gateway, a price-time matching engine running the trading day,
market data feed publishers, the replay services a client logs into (Glimpse, TotalViewTcp),
and traders that exercise the exchange end to end. Every link between two components carries
framed Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 messages: the gateway owns the client sockets, the engine owns the
book, and each feed owns its market data.

The components are generated once, into `common/`, and deployed three ways - `process/`, one binary with a thread per component, `system/`, a process per component on this host, `distributed/`, a process per component across machines. Each directory has a ReadMe of its own.

## Layout

```
CMakeLists.txt         every target: the single binary, a binary per component, the tests
Dockerfile             the venue as one image; distributed/compose.yaml runs a component per container
simulator.ini          runtime endpoints, read at process start (docker/ holds the container copy)
common/                everything the deployments share, generated once
  protocols/           the generated structs, messages and sessions of each protocol
  engine/ traders/     the matching engine and the venue's own traders
  run/                 the links: a queue, a ring in shared memory, a socket
process/               one binary, a thread per component - deterministic, the reference
system/                a process per component on one host, rings in shared memory
distributed/           a process per component across machines, Tcp, tiers that fail apart
```

Each of `process/`, `system/`, `distributed/` has a ReadMe saying what it is and what it is not; the separated ones have a `launch.sh`
and a `verify.sh` that runs the venue and checks what its components carried.

## Protocols

| Role | Model |
| --- | --- |
| Order Entry | Nasdaq.NsmEquities.Orders.Ouch.v5.0 |
| Order Entry Session | Nasdaq.SoupBinTcp |
| Market Data (TotalView) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Snapshot of TotalView (Glimpse) | Nasdaq.NsmEquities.TotalView.Glimpse.v5.0.2026 |
| Stream of TotalView (TotalViewTcp) | Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 |
| Trading Day | Nasdaq.NsmEquities.Equities.Session.TradingSession |
| Internal Messaging | Nasdaq.NsmEquities.MatchingEngine.Ipc.v2026 |

## Trading Day

The engine runs `Nasdaq.NsmEquities.Equities.Session.TradingSession`: each phase of the day decides whether an order
trades, waits for the next cross, or is refused. Each change of phase is announced on the
internal messaging, and so is each cross completed, with the price it crossed at and the
shares that traded there. The day the simulator is generated with:

| Change | Before startup run turn | Served (seconds after start) |
| --- | --- | --- |
| PreOpen | 0 | 0 |
| OpeningLockIn | 2 | 25 |
| OpeningLockOut | 3 | 28 |
| OpeningLate | 4 | 29 |
| Open | 4 | 30 |
| Halt | 8 | 120 |
| Quote | 9 | 140 |
| Resume | 10 | 150 |
| PreClose | 14 | 240 |
| ClosingLockOut | 16 | 265 |
| ClosingFreeze | 17 | 268 |
| Close | 18 | 270 |
| End | 20 | 300 |

A turn past the run's last is made after it. The startup run asks for each change between its
turns, so it is the same every run; a served venue makes them on the clock.

While an auction collects, the day asks the engine what it would cross at, and the engine
answers on the internal messaging with the price, the shares that would pair there, and those
left over and on which side. The startup run asks once a turn; a served venue asks on this
cadence, in seconds:

| Auction | First ask after the collection starts | Then every | Then every, over the last part | Last part |
| --- | --- | --- | --- | --- |
| Opening | 0 | 10 | 1 | 40% |
| Intraday | 1 | 1 | - | - |
| Closing | 0 | 10 | 1 | 40% |

The engine keeps one book and one kind of order, so the price it would cross at is the only
one it has: a feed that publishes several prices for an auction is given that one for each,
which is an approximation.

## Matching Rules

Beside price and time the engine applies the rules below, each as its matching engine model states it:
where the venue states the rule, the venue's own words, what the engine does of it, and what of that is
the simulator's judgement, not built, or measured on the venue's own feed.

`Nasdaq.NsmEquities.Equities.MatchingEngine`

**PostOnly** - Nasdaq Equity 4, Rule 4702(b)(4)

- The venue: A Post-Only Order is "an Order Type designed to have its price adjusted as needed to post to the Nasdaq Book"; it "will execute if ... it is priced at $1.00 or more and the value of price improvement associated with executing against an Order on the Nasdaq Book (as measured against the original limit price of the Order) equals or exceeds $0.01 per share".
- The engine: It trades as it arrives only with what betters its limit by a price increment, and what is left posts a minimum price increment inside the best displayed order it would lock or cross - or at its limit where only orders the book does not show are in its way.
- Judgement: Below $1.00 the rule weighs fees and rebates the simulator has none of, and the increment stands in for them.
- Not built: The Participant's choice to have the order cancelled back instead of adjusted (OUCH cancel reasons F and G); a quotation of another market, which the rule also keeps it from locking.

**MinimumQuantity** - Nasdaq Equity 4, Rule 4703(e)

- The engine: An order names the fewest shares it trades. It executes as it arrives only where at least that many rest within its price, posts unseen otherwise, and once posted trades only with an order of at least its minimum; it takes no part in a cross.
- Judgement: Of the rule's two ways of meeting the minimum on entry - against several orders, or against each of one or more - OUCH names none, and the first is taken.
- The venue: OUCH 5.0, Appendix A, tag 3: "MinQty must be a round lot."

**Pegged** - Nasdaq Equity 4, Rule 4703(d)

- The engine: A Midpoint Peg follows the midpoint of the best bid and offer, is never shown, and takes no part in a cross; an order pegged to the best price on its own side or the other follows that, by its offset.
- Judgement: Nasdaq prices a peg from the national best bid and offer; the simulator has no other market, and its own best shown bid and offer stand in.
- Not built: A Market Maker Peg, and OUCH's PriceType m, which the document does not explain: an order carrying either is refused as Invalid Peg Type.

**Hidden** - Nasdaq Equity 4, Rule 4757(a)(1)

- The venue: The price/display/time algorithm: "Better priced Orders will be presented for execution first", "Equally priced Orders with a Display Attribute will be ranked in time priority", and "Orders with a Non-Display Attribute, including the Non-Displayed portion of an Order with Reserve Size, will be ranked in time priority".
- The engine: An order the book does not show rests and trades unseen, behind every shown order at its price and in time order among the unseen; the feed publishes nothing of it but its trades.
- Measured: Nasdaq's TotalView-ITCH feed of 2026-08-20, the whole day (898,523,816 messages): each of the 6,615,759 Trade Messages of an order the feed never showed carries Order Reference Number 0 and Buy/Sell Indicator B.

**Expiring** - Nasdaq Equity 4, Rule 4703(a)

- The venue: An Order may deactivate at "a specific time identified by the Participant; provided, however, that an Order specifying an expire time beyond 8:00 p.m. ET will be cancelled at the conclusion of the Day Sesson at 8:00 p.m. ET".
- The engine: A GTT order expires by its own time, OUCH's ExpireTime option in seconds to live, or at the end of the day where that is sooner; one with no expire time, or one already past, is refused.

**CrossOnly** - Nasdaq Equity 4, Rules 4702(b)(8)-(13), 4752, 4753 and 4754

- The venue: OUCH 5.0, of an order whose cancel waits for its cross (Cancel Pending): "any unexecuted portion of this order will automatically be canceled immediately after the cross completes".
- The engine: An order for one cross alone - the Opening, the Closing or a Halt Cross - waits aside until that cross collects, joins it, and what is left of it once the cross completes is canceled. The book never shows one.
- Judgement: That Nasdaq does not display its on-open and on-close orders is the simulator's knowledge of the venue, not a statement of the documents held here.
- Not built: The supplemental, retail, extended life and after hours close crosses: an order for one is rejected Invalid Cross Order. An imbalance-only order.
- Measured: Nasdaq's TotalView-ITCH feed of 2026-08-20, the whole day (898,523,816 messages): an Opening Cross is printed for every security, 4,753 with shares and 8,421 with none, and a Closing Cross likewise, 4,269 and 8,905, each with a match number; a Halt Cross only where it traded, all 57 with shares. The System Event of the hour comes first and its cross prints after it.

**SelfMatchPrevention** - Nasdaq Equity 4, Rule 4757(a)(4)

- The venue: "Market participants may direct that quotes/orders entered into the System not execute against either quotes/orders entered under the same MPID or quotes/orders entered across MPIDs under Common Ownership", and may "assign to orders entered through a specific order entry port a unique group identification modifier that will prevent quotes/orders with such modifier from executing against each other".
- The engine: Two orders of one owner that would trade as one arrives do not: by the strategy of the order removing liquidity the newer is canceled, the older, both, or each is lowered by what would have traded. Nothing is prevented in a cross.
- Judgement: An owner is kept within the session its orders came in on: the simulator has no record of which sessions are one MPID's.

**TradeNow** - Nasdaq Equity 4, Rule 4703(m)

- The venue: Trade Now "allows: (i) a resting Order that is locked or crossed, as applicable, at its non-displayed price by the posted price of an incoming Displayed Order or a Midpoint Peg Post-Only Order or another Order or Orders (where such locking or crossing Order(s) or the order with Trade Now satisfies a Minimum Quantity condition) to execute against a locking or crossing Order(s) as a liquidity taker automatically when such Orders become marketable", and "Any remaining shares of the resting Order will remain posted on the Nasdaq Book with the same priority".
- The engine: As an order posts, each order the book does not show on the other side that carries the attribute and is locked or crossed by it takes it, best price first and in time order, at the prices of the orders it takes - where it names a minimum, only once that much is there - and what is left of it stays where it rests.
- Judgement: It is looked for only as an entered order posts, not as one is priced anew.
- Not built: The rule's second part, a Midpoint Pegging order executing against a M-ELO+CB: the engine has no such order.

**Reserve** - Nasdaq Equity 4, Rule 4703(h)

- The venue: "the full size of the Order will be presented for potential execution"; "if the remaining total size is less than the display size stipulated by the Participant, the Displayed Order will post without Reserve Size"; when an execution causes the displayed size "to decrease below a normal unit of trading, another Displayed Order will be entered at the limit price and size stipulated by the Participant", and "The new Displayed Order will receive a new timestamp".
- The engine: An order posts showing its floor where it has more than that. A fill takes only what it shows; once that is spent it is shown again from what is behind it, at the back of the shown orders at its price and under a new id, which its feed publishes as a new order and its owner is told as Order Restated. Lowering it comes off what is behind its floor first.
- Judgement: The rule refreshes as the displayed size falls below a normal unit of trading, what is left of it staying beside the new order; here it is refreshed once it is spent.
- Judgement: The rule ranks the non-displayed part as a non-displayed order; here it trades only through what it refreshes, so behind every shown order at its price and ahead of the unseen ones.
- Judgement: In a cross it trades its whole size where it stands, what was not shown told as not shown.
- Not built: A displayed size chosen at random (OUCH RandomReserves); the liquidity flag u.
- Measured: Nasdaq's TotalView-ITCH feed of 2026-08-20, the whole day (898,523,816 messages): 206,231 of the 10,094,128 orders executed away are followed, as their security's next message, by an add on the same side at the same price - 123,063 of them a round lot, 202,410 within a millisecond, none attributed. Consistent with a refresh being a new Add under a new reference; the feed does not say which of them are reserve orders.

**ShortSaleTest** - Nasdaq Equity 4, Rule 4763; Rule 201 of Regulation SHO

- The venue: The System "shall not execute or display a short sale order with respect to a covered security at a price that is less than or equal to the current national best bid"; "short sale orders that are limited to the national best bid or lower and short sale market orders will be re-priced by the System one minimum allowable price increment above the current national best bid"; and IOC orders "will be executed to the extent possible at a Permitted Price and higher and then cancelled, and will not be re-priced".
- The engine: While its security is under the test a short sale priced at or below the best bid the book shows, or unpriced, is priced an increment above it before it is taken; an IOC one trades only above the bid and is canceled. A short sale exempt, OUCH's Side E, is held to nothing.
- Judgement: The national best bid is the book's own best shown bid: the simulator has no other market.
- Not built: Repricing again as the bid falls or rises; the test's own trigger, a fall of 10% from the prior close, in whose place the declaration says whether a day starts under it; a short sale entered for a cross.
- Measured: Nasdaq's TotalView-ITCH feed of 2026-08-20, the whole day (898,523,816 messages): the Reg SHO Restriction of the start of day spin is 0 for 12,721 securities and 2 for 432, and 399 more Reg SHO Restriction messages of action 1, the test put in place, follow in the day.

## Build

Requirements: C++20, CMake 3.28+, Boost, and a standard library with the C++20 time zone database - GCC 14 or newer (g++-14, `-DCMAKE_CXX_COMPILER=g++-14`; Ubuntu 24.04's default g++ is 13) - with tzdata installed

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/nasdaq_simulator                # endpoints from simulator.ini
./build/nasdaq_simulator --config my.ini
./build/simulator_tests              # Boost.Test suite
```

Runtime endpoints are read from simulator.ini at process start:

| Connection | Default |
| --- | --- |
| Order entry (Tcp) | 127.0.0.1:15101 |
| TotalView feed (Udp multicast) | 233.54.12.112:26478 |
| Glimpse snapshot (Tcp) | 127.0.0.1:15102 |
| TotalViewTcp stream (Tcp) | 127.0.0.1:15103 |

Recording output (order-entry log, market data captures) lands under `recordings/`.

## Book check

`nasdaq_book` holds the book the venue's TotalView feed builds against the engine's own.
The book is generated from the book model Nasdaq.NsmEquities.Equities (`common/book/`), and each feed message
the model binds to an operation is applied to it. Run the venue with its book written as it goes -
`[book] dump = book.csv` and `every = phase`, or a number of milliseconds - then:

```bash
./build/nasdaq_book recordings/marketdata/TotalView.dat book.csv
```

Each time the engine was asked for its book, the feed's recorder marked where in the feed the
answer belongs (`TotalView.dat.books`). At each mark the two books are compared: the same orders,
each on the same side at the same price for what the engine's book shows of it, and at each price
in the order the engine would fill them. What the engine holds and does not show - a hidden
order, the rest of a reserve order - is no part of a book built from the feed, and is not compared.

## Latency

What this venue does under load is measured by the latency lab, which reads a capture of it
running in containers through the venue's own declaration: `omi-latency-lab`, generated from
the same specifications, with a worked example per venue. Nothing of the lab is in this tree.