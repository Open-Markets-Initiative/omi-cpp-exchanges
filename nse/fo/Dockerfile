# The nse simulator as an image: built from its own tree, in two stages.
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
COPY --from=build /build/nse_simulator ./

# One binary per component of the distributed venue; cluster/compose.yaml runs one each.
COPY --from=build /build/nse_cluster_gateway ./
COPY --from=build /build/nse_cluster_engine ./
COPY --from=build /build/nse_cluster_mtbt ./
COPY --from=build /build/nse_cluster_maker ./
COPY --from=build /build/nse_cluster_taker ./

# The endpoints it serves on in a container, which is the one thing that differs from
# serving on a desktop: it listens on every interface rather than on loopback.
COPY docker/simulator.ini ./simulator.ini

RUN mkdir -p recordings

# Order entry, and the replay services a client logs in to.
EXPOSE 10900

# Its feeds are multicast, which a bridged network does not carry: a client on another
# machine takes the feed off a replay service above, or shares this container's network.
# feed Mtbt: 239.70.70.61:17761/udp

ENTRYPOINT ["./nse_simulator"]
CMD ["--serve"]
