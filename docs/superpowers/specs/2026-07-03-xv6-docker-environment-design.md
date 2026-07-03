# xv6 Ubuntu 20.04 Docker Environment

## Goal

Provide a reproducible Ubuntu 20.04 development container for the existing
`xv6-labs-2023` repository. The repository remains on the host and is mounted
into the container so source changes persist independently of the container.

## Components

- `Dockerfile` uses `ubuntu:20.04` and installs:
  - `git`
  - `build-essential`
  - `gdb-multiarch`
  - `qemu-system-misc`
  - `gcc-riscv64-linux-gnu`
  - `binutils-riscv64-linux-gnu`
- `compose.yaml` defines one service named `xv6`.
- The service bind-mounts the repository root at
  `/workspace/xv6-labs-2023`.
- The service uses that mounted directory as its working directory.
- An interactive terminal and a long-running idle command keep the container
  available for repeated shell access.

## Usage

Build and start the environment from the repository root:

```sh
docker compose up -d --build
```

Open a shell in the running container:

```sh
docker compose exec xv6 bash
```

After Docker or the container has stopped, restore access with:

```sh
docker compose up -d
docker compose exec xv6 bash
```

Source files and build outputs are stored in the bind-mounted host repository.
Rebuilding or removing the container does not remove those files.

## Error Handling

- Package installation runs non-interactively and removes downloaded package
  indexes after installation.
- The image build fails immediately if any requested package cannot be
  installed.
- Compose recreates the service from the declared configuration when needed.

## Verification

The completed environment must pass these checks:

1. `docker compose config` accepts the Compose configuration.
2. `docker compose build` completes successfully.
3. The service starts and remains running.
4. Each requested command or package is present in the container.
5. `make` successfully builds xv6 with the RISC-V Linux GNU toolchain.
6. A second `docker compose exec xv6 bash` invocation can access the same
   mounted repository.
