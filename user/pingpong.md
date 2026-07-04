Q: Why do `read()` and `write()` not need a `while` loop?

A: This program sends exactly one byte in each direction. A one-byte `write()` either writes that byte or returns an error, and the matching one-byte `read()` either receives it or returns an error. There is no second message to process, so each operation should run once and its return value should be checked:

```c
if (write(fd, &byte, 1) != 1) {
    // handle error
}

if (read(fd, &byte, 1) != 1) {
    // handle error
}
```

A `while (read(...) > 0)` loop means "keep reading until end-of-file." After receiving the one byte, it would call `read()` again and could block waiting for data that will never arrive, especially while another process still has the pipe's write end open. Loops are useful when transferring an unknown amount of data or processing multiple messages, but not for this single-byte exchange.

Q: Why does `pingpong` need two pipes?

A: A pipe is one shared byte stream. After `fork()`, both the parent and child inherit its read and write file descriptors. If the same pipe is used for both directions, either process can consume a byte from that stream. The parent might read back its own `"ping"`, or the child might read back its own `"pong"`, depending on process scheduling.

Two pipes give each direction a separate stream:

```text
parent -- parent_to_child --> child
parent <-- child_to_parent -- child
```

The parent writes `"ping"` only to `parent_to_child` and reads `"pong"` only from `child_to_parent`. The child does the opposite. Each process should also close the pipe ends it does not use. This makes the communication direction unambiguous and allows end-of-file to work correctly after all writers for a pipe are closed.
