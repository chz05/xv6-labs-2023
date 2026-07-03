# xv6 Ubuntu 20.04 Docker Environment Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a reproducible, persistent Ubuntu 20.04 Docker development environment for building and running `xv6-labs-2023`.

**Architecture:** A `Dockerfile` builds an image containing the requested host and RISC-V development tools. A Compose service keeps a named development container running and bind-mounts the repository at `/workspace/xv6-labs-2023`, allowing repeated shell access while keeping source and build outputs on the host.

**Tech Stack:** Docker, Docker Compose, Ubuntu 20.04, GNU Make, RISC-V GNU cross-toolchain, QEMU

---

## File Structure

- `Dockerfile`: Defines the Ubuntu 20.04 image, installs all requested
  packages, selects the xv6 working directory, and provides the idle default
  command.
- `compose.yaml`: Defines the persistent `xv6` development service, terminal
  settings, bind mount, and working directory.

### Task 1: Development Image

**Files:**
- Create: `Dockerfile`

- [ ] **Step 1: Verify that no project Dockerfile exists**

Run:

```sh
test ! -e Dockerfile
```

Expected: exit status 0.

- [ ] **Step 2: Add the Ubuntu 20.04 image definition**

Create `Dockerfile` with exactly:

```dockerfile
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
```

- [ ] **Step 3: Build the development image**

Run:

```sh
docker build --tag xv6-labs-2023:dev .
```

Expected: exit status 0 and a final successful image build message.

- [ ] **Step 4: Verify every requested tool in the image**

Run:

```sh
docker run --rm xv6-labs-2023:dev sh -lc '
  command -v git &&
  command -v make &&
  command -v gcc &&
  command -v gdb-multiarch &&
  command -v qemu-system-riscv64 &&
  command -v riscv64-linux-gnu-gcc &&
  command -v riscv64-linux-gnu-ld
'
```

Expected: exit status 0 and seven absolute executable paths.

- [ ] **Step 5: Commit the image definition**

```sh
git add Dockerfile
git commit -m "build: add xv6 Ubuntu development image"
```

Expected: one commit containing only `Dockerfile`.

### Task 2: Persistent Compose Service

**Files:**
- Create: `compose.yaml`

- [ ] **Step 1: Verify that no Compose configuration exists**

Run:

```sh
test ! -e compose.yaml
```

Expected: exit status 0.

- [ ] **Step 2: Add the persistent development service**

Create `compose.yaml` with exactly:

```yaml
services:
  xv6:
    build:
      context: .
    image: xv6-labs-2023:dev
    container_name: xv6-labs-2023
    stdin_open: true
    tty: true
    working_dir: /workspace/xv6-labs-2023
    volumes:
      - .:/workspace/xv6-labs-2023
```

- [ ] **Step 3: Validate the Compose configuration**

Run:

```sh
docker compose config --quiet
```

Expected: exit status 0 with no validation errors.

- [ ] **Step 4: Build and start the service**

Run:

```sh
docker compose up -d --build
```

Expected: exit status 0 and service `xv6` is running.

- [ ] **Step 5: Verify the bind mount and working directory**

Run:

```sh
docker compose exec xv6 sh -lc '
  test "$(pwd)" = /workspace/xv6-labs-2023 &&
  test -f Makefile &&
  test -d kernel &&
  test -d user
'
```

Expected: exit status 0.

- [ ] **Step 6: Verify repeated access after a restart**

Run:

```sh
docker compose stop
docker compose start
docker compose exec xv6 sh -lc 'test -f Makefile'
```

Expected: all three commands exit with status 0.

- [ ] **Step 7: Commit the Compose service**

```sh
git add compose.yaml
git commit -m "build: add persistent xv6 Compose service"
```

Expected: one commit containing only `compose.yaml`.

### Task 3: End-to-End xv6 Verification

**Files:**
- Test only; no file changes.

- [ ] **Step 1: Confirm the service is running**

Run:

```sh
docker compose ps --status running
```

Expected: the `xv6` service appears with state `running`.

- [ ] **Step 2: Build xv6 with the mounted repository**

Run:

```sh
docker compose exec xv6 make clean
docker compose exec xv6 make
```

Expected: both commands exit with status 0; the output uses
`riscv64-linux-gnu-gcc`, and `kernel/kernel` is created.

- [ ] **Step 3: Confirm QEMU supports the required RISC-V system emulator**

Run:

```sh
docker compose exec xv6 qemu-system-riscv64 --version
```

Expected: exit status 0 and a QEMU version string.

- [ ] **Step 4: Confirm the repository remains accessible in a new shell**

Run:

```sh
docker compose exec xv6 bash -lc '
  test -f /workspace/xv6-labs-2023/kernel/kernel &&
  git status --short
'
```

Expected: exit status 0. Git reports no unexpected tracked-file changes.

- [ ] **Step 5: Check final repository state**

Run:

```sh
git status --short
git log -3 --oneline
```

Expected: the worktree has no unexpected tracked changes and the image and
Compose commits are present.
