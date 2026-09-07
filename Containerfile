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

# --- Neovim + clangd for editing/LSP inside the container ---
RUN apt-get update && apt-get install -y \
    curl \
    unzip \
    ripgrep \
    clangd \
    && rm -rf /var/lib/apt/lists/*

RUN curl -LO https://github.com/neovim/neovim/releases/latest/download/nvim-linux-x86_64.tar.gz \
    && tar xzf nvim-linux-x86_64.tar.gz \
    && mv nvim-linux-x86_64 /opt/nvim \
    && ln -sf /opt/nvim/bin/nvim /usr/local/bin/nvim \
    && rm nvim-linux-x86_64.tar.gz

RUN mkdir -p /root/.config/nvim
COPY container_config/init.lua /root/.config/nvim/init.lua

# Pre-install plugins at build time so no network access is needed at runtime
RUN nvim --headless "+Lazy! sync" +qa

WORKDIR /workspace

