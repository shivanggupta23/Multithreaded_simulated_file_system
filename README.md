# Multithreaded_simulated_file_system

# Multi-Threaded File System Simulator

A simulated file system in C using POSIX threads, mutex locks, and condition variables.

## Features
- File creation, deletion, read, and write
- Fine-grained locking per file
- Condition variables for synchronization
- Interactive menu with colorful output

## How to Compile
```bash
gcc filesystem.c -o filesystem -lpthread
./filesystem

****OUTPUT****
✔ File created: test.txt
🔒 Lock acquired on file: test.txt
📖 Read from test.txt: hello world
🔓 Lock released on file: test.txt
