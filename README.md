# Demonstrating IPC Methodology Using socketpair() in C

## Requirements

- Ubuntu 22.04
- Input:
    {
      int,
      float,
      string data (C or C++ formatted your choice)
      enum { type1, type2, type3 }
    }

- Output:
  - Print out Minimum 10 different versions of the structure from Input in the following format:
      - int: {value}, float: {value}, string: {value}, enum {human readable, e.g type1}

## Compilation and Execution

1. **Open a Terminal**: Open a terminal window on the Ubuntu 22.04 system.
2. **Navigate to the Directory**: Change to the directory containing the source file.
3. **Compile the Program**: Use the `gcc` compiler to compile the program.

    ```
    gcc ipc_socketpair.c -o ipc_socketpair
    ```

4. **Run the Program**: Execute the compiled program.

    ```
    ./ipc_socketpair
    ```

## Description

This program demonstrates inter-process communication (IPC) using the `socketpair()` function in C. It passes a custom data structure between a parent and a child process and prints the received data while handling any partial writes and reads.
