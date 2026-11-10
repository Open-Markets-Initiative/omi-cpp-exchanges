# The nasdaq simulator as an image: built from its own tree, in two stages.
#
# Built here rather than copied in: the image then holds exactly what the generator
# wrote, and a tree that does not compile cannot become an image that looks like it does.

FROM ubuntu:24.04 AS build

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        g++-14 cmake make libboost-program-options-dev ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . ./

RUN cmake -S . -B /build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-14 \
 && cmake --build /build --parallel

FROM ubuntu:24.04

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libboost-program-options1.83.0 tzdata \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /venue
COPY --from=build /build/nasdaq_simulator ./

# One binary per component of the distributed venue; distributed/compose.yaml runs one each.
COPY --from=build /build/nasdaq_distributed_gatewaymakerbasic ./
COPY --from=build /build/nasdaq_distributed_gatewaytakerbasic ./
COPY --from=build /build/nasdaq_distributed_gatewayreplay ./
COPY --from=build /build/nasdaq_distributed_gatewayrequests ./
COPY --from=build /build/nasdaq_distributed_engine ./
COPY --from=build /build/nasdaq_distributed_sequencer ./
COPY --from=build /build/nasdaq_distributed_day ./
COPY --from=build /build/nasdaq_distributed_totalview ./
COPY --from=build /build/nasdaq_distributed_glimpse ./
COPY --from=build /build/nasdaq_distributed_totalviewtcp ./
COPY --from=build /build/nasdaq_distributed_makerbasic ./
COPY --from=build /build/nasdaq_distributed_takerbasic ./
COPY --from=build /build/nasdaq_distributed_replay ./
COPY --from=build /build/nasdaq_distributed_requests ./

# The endpoints it serves on in a container, which is the one thing that differs from
# serving on a desktop: it listens on every interface rather than on loopback.
COPY docker/simulator.ini ./simulator.ini

RUN mkdir -p recordings

# Order entry, and the replay services a client logs in to.
EXPOSE 15101
# Glimpse
EXPOSE 15102
# TotalViewTcp
EXPOSE 15103

# Its feeds are multicast, which a bridged network does not carry: a client on another
# machine takes the feed off a replay service above, or shares this container's network.
# feed TotalView: 233.54.12.112:26478/udp

ENTRYPOINT ["./nasdaq_simulator"]
CMD ["--serve"]
