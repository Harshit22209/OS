# OS Project: Shell, Smart Loader, and Simple Scheduler

## Overview
This project consists of three main components: a Unix shell, a smart loader, and a simple scheduler. Each component is implemented in C and operates on a Linux system. 

## Components

### Shell
A custom Unix shell that supports basic commands and includes a custom command prompt.

#### How to Execute
1. Run the command `make compile` in the terminal.
2. Execute the binary file with `./a.out`.

#### Usage
1. The shell will prompt for a username.
   - Enter any username or simply enter `harshit`.
   - If the username does not exist, a directory will be created for the user.
2. The custom command prompt `root@{username}:~$` will appear.

#### Supported Commands
1. `mkdir`
2. `rmdir`
3. `ls`
4. `cd`
5. `wc -c`
6. `wc -l`
7. `echo`
8. `sort`
9. `grep`
10. `uniq`
11. `cat`
12. `ls -l`
13. `ls -R`
14. `history`

* The shell also supports running `.sh` and `.c` files.

#### Limitations
* Some commands like `touch`, `vim`, and other parameters are not included.
* Pipe works with commands such as `cat`, `grep`, `uniq`, `sort`, and `wc`.

#### File Structure
- `SimpleShell.c`: Manages user input, process creation, and command execution.
- `commands.h`: Contains all supported commands.
- `helper.h`: Contains various utility functions for input handling, printing, etc.

#### Implementation Details
- A new process is created using `fork` for commands like executing `.c` and `.sh` files, `sort`, `cat`, `grep`, `uniq`, and `wc`.
- Commands like `cd`, `mkdir`, `rmdir`, and `echo` are executed within the parent process.
- Supports the `&` operator for background execution and pipes for command chaining.
- Pressing `Ctrl+C` displays executed commands with details such as PID, start time, and duration.
- The command prompt reflects the current directory relative to the user root.

### Simple Scheduler
A multi-level feedback queue (MLFQ) scheduler implemented with round-robin scheduling.

#### How to Execute
1. Compile the scheduler with `gcc simple_scheduler.c -o sched`.
2. Run the scheduler with `./sched <NCPU> <TSLICE(in milliseconds)>`.
3. Use the `submit` command to add processes: `submit <fileName> <priority>`.

#### Implementation Details
1. Shared memory is allocated for the ready queue, processes, and nodes.
2. Semaphores are initialized for adding and fetching processes from the ready queue.
3. A shell is implemented and a daemon process is created for the scheduler.
4. Processes are created and added to the ready queue based on priority.
5. The scheduler fetches processes from the ready queue and runs them for `TSLICE` milliseconds.
6. Processes are moved between queues to minimize starvation based on waiting time and priority.

#### Scheduling Algorithm
- Based on round-robin with a multi-level queue.
- Processes are executed in round-robin fashion at each priority level.
- Starvation prevention by promoting processes to higher priority queues if they wait too long.
- The factor for promotion is calculated as:
#### factor = (average number of processes in ready queue) * (TSLICE / 1000) / (number of CPUs)

This factor varies across priority levels.

#### Statistics
- Changing the queue level reduces average waiting time and prevents starvation.

### Smart Loader
A loader that reads ELF files and allocates memory lazily.

#### How to Execute
1. Set the library path with `export LD_LIBRARY_PATH=<dir to bin>:$LD_LIBRARY_PATH`.
2. Run `make compile`.

#### Implementation Details
1. Handles segmentation faults with `sigFaultHandler`.
2. Allocates memory for pages using `mmap` and maps them to virtual memory addresses.
3. Tracks page allocations and internal fragmentation.
4. The `_start` method executes before loading pages, causing a segmentation fault which is handled to load pages dynamically.
5. After execution, prints statistics such as page faults, allocations, and internal fragmentation.

## Statistics
![Scheduler Statistics](scheduler_statistics.pdf)

Due to the changing of queue levels, the average waiting time is reduced, and starvation is prevented.

## Conclusion
This project demonstrates the implementation of a custom Unix shell, a smart loader, and a simple scheduler using C on Linux. Each component addresses different aspects of operating systems, including process management, memory allocation, and scheduling.


