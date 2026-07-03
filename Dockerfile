FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        binutils-riscv64-linux-gnu \
        build-essential \
        gcc-riscv64-linux-gnu \
        gdb-multiarch \
        git \
        qemu-system-misc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/xv6-labs-2023

CMD ["sleep", "infinity"]
