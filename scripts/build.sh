#!/usr/bin/env bash

podman run --rm -v "$(pwd):/workspace:Z" linux-low-level-protocols bash -c "
  mkdir -p build-arm64 && cd build-arm64 &&
  cmake -DCMAKE_TOOLCHAIN_FILE=/workspace/container_config/toolchain-aarch64.cmake .. &&
  make
"
