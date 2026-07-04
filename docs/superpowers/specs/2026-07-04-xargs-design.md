# xv6 xargs Design

## Goal

Implement `user/xargs.c` so it reads standard input one line at a time and
runs the requested command once for every line. Arguments parsed from each
line are appended to the fixed arguments passed to `xargs`.

For example:

```sh
printf "hello too\none two\n" | xargs echo bye
```

must run the equivalent of:

```sh
echo bye hello too
echo bye one two
```

An empty input line must still execute the fixed command.

## Input handling

A separate `readline` function will read standard input one byte at a time
until it encounters `\n` or EOF. Its status will distinguish:

- a line, including an empty line;
- EOF before another line starts;
- a read error or a line that exceeds the fixed buffer.

The newline will not be included in the returned line.

## Argument handling

For each line, the program will:

1. Copy the fixed command and arguments from `argv[1]` onward.
2. Split the input line on spaces and tabs.
3. Append the resulting words.
4. Add the null pointer required by `exec`.

The argument array must remain within xv6's `MAXARG` limit. Too many
arguments, an overlong line, or a read failure will produce an error and
terminate `xargs`.

## Process handling

The program will fork once for each input line. The child will call `exec`
using the first constructed argument as the command. If `exec` fails, the
child will print an error and exit unsuccessfully. The parent will call
`wait(0)` before reading and executing the next line.

## Verification

Verification will cover:

- one input line;
- multiple input lines;
- an empty input line;
- fixed arguments plus line arguments;
- the existing `xargstest.sh` pipeline;
- successful compilation with the xv6 build.
