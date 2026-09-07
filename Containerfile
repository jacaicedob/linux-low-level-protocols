FROM debian:trixie
RUN apt-get update && apt-get install -y \
    crossbuild-essential-arm64 \
    cmake \
    git \
    gdb-multiarch \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*
RUN dpkg --add-architecture arm64 && apt-get update && apt-get install -y \
    libgpiod-dev:arm64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

