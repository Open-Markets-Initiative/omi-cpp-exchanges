# The iex simulator as an image: built from its own tree, in two stages.
#
# Built here rather than copied in: the image then holds exactly what the generator
# wrote, and a tree that does not compile cannot become an image that looks like it does.

FROM ubuntu:24.04 AS build

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        g++ cmake make libboost-program-options-dev ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . ./

RUN cmake -S . -B /build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build /build --parallel

FROM ubuntu:24.04

RUN apt-get update \
 && apt-get install -y --no-install-recommends libboost-program-options1.83.0 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /venue
COPY --from=build /build/iex_simulator ./

# One binary per component of the distributed venue; distributed/compose.yaml runs one each.
COPY --from=build /build/iex_distributed_gatewaymakerbasic ./
COPY --from=build /build/iex_distributed_gatewaytakerbasic ./
COPY --from=build /build/iex_distributed_engine ./
COPY --from=build /build/iex_distributed_sequencer ./
COPY --from=build /build/iex_distributed_tops ./
COPY --from=build /build/iex_distributed_deep ./
COPY --from=build /build/iex_distributed_deepplus ./
COPY --from=build /build/iex_distributed_makerbasic ./
COPY --from=build /build/iex_distributed_takerbasic ./

# The endpoints it serves on in a container, which is the one thing that differs from
# serving on a desktop: it listens on every interface rather than on loopback.
COPY docker/simulator.ini ./simulator.ini

RUN mkdir -p recordings

# Order entry, and the replay services a client logs in to.
EXPOSE 9001
# Deep snapshot
EXPOSE 11379
# Tops snapshot
EXPOSE 11380
# DeepPlus snapshot
EXPOSE 11381

# Its feeds are multicast, which a bridged network does not carry: a client on another
# machine takes the feed off a replay service above, or shares this container's network.
# feed Tops: 233.54.12.1:10001/udp
# feed Deep: 233.54.12.2:10002/udp
# feed DeepPlus: 233.54.12.3:10003/udp

ENTRYPOINT ["./iex_simulator"]
CMD ["--serve"]
