# CS 433 Unix Shell

*Copyright 2017 Caleb Evans, Jonathan Tapia*

This program is a C++ implementation of a very basic Unix shell for CS 433
(Operating Systems) as CSU San Marcos. Supported features include command
entry/execution, background processes, and command history. The purpose
of this assignment was to properly create and manage subprocesses using the C
`fork()` and `wait()` functions.

## How to run

Run `make` to compile the program, then run `./unix-shell.out` to execute the
compiled program.

## Features

### Command entry/execution

Users can type any valid POSIX command (with arguments), as well as any utility
on the user's PATH. Superfluous whitespace between command arguments is ignored.

```sh
echo hello there
```

```sh
ls -la
```

```sh
sleep 10
```

```sh
git status
```

## Background processes

```
echo hello &
```

## History

### View most recent commands

```sh
history
```

### Execute most recent command

```sh
!!
```

### Execute command by ID in history

```sh
!3
```

```sh
!5
```
