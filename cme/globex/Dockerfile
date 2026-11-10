# The cme simulator as an image: built from its own tree, in two stages.
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
COPY --from=build /build/cme_simulator ./

# The endpoints it serves on in a container, which is the one thing that differs from
# serving on a desktop: it listens on every interface rather than on loopback.
COPY docker/simulator.ini ./simulator.ini

RUN mkdir -p recordings

# Order entry, and the replay services a client logs in to.
EXPOSE 11000

ENTRYPOINT ["./cme_simulator"]
CMD ["--serve"]
