# Preemptive User-Level Thread Library in C

A lightweight **preemptive user-level threading library** implemented in C using `ucontext`. The library provides timer-driven context switching, Round Robin scheduling, synchronization primitives, and thread lifecycle management without relying on kernel-level threading APIs for scheduling.

---

***Feel free to look at the comments I added throughout the project for  better understanding of the workflow but keep in mind that some definitions I added in those comments for the functions were for my understanding and can be vague or slightly misleading to the actual description of the function you might find on the web!***

## Features

- Preemptive user-level threading
- Round Robin scheduler
- Timer-driven preemption using `setitimer()` and `SIGALRM`
- Context switching with `ucontext`
- Thread creation and termination
- Thread joining
- Thread yielding
- Thread synchronization using mutexes
- Thread-safe ready queue implementation
- Scheduler performance metrics
  - Response Time
  - Turnaround Time
  - Execution Time
- Benchmark applications for validation

---

## Project Structure

```
ThreadLibrary/
│
├── include/
│   ├── uthreads.h
│   ├── queue.h
│   ├── list.h
│   └── stack.h
│
├── src/
│   ├── uthreads.c
│   └── STL/
│       ├── queue.c
│       ├── list.c
│       └── stack.c
│
├── Metrics/
│   ├── vector_multiply.c
│   ├── test.c
│   └── README.md
│
└── README.md
```

---

## Threading APIs

The library currently supports:

| Function | Description |
|----------|-------------|
| `thread_create()` | Creates a new thread |
| `thread_join()` | Waits for a thread to finish |
| `thread_yield()` | Voluntarily yields CPU |
| `thread_exit()` | Terminates current thread |
| `mutex_init()` | Initializes a mutex |
| `mutex_lock()` | Locks a mutex |
| `mutex_unlock()` | Unlocks a mutex |
| `thread_self()` | Returns current thread ID |

---

## Scheduler

The scheduler uses a **Round Robin** policy.

Each thread receives a configurable time slice. When the timer expires:

1. A `SIGALRM` interrupt is generated.
2. The scheduler saves the running thread's context.
3. The next READY thread is selected.
4. Execution switches to the selected thread.

This enables **preemptive multitasking** entirely in user space.

---

## Context Switching

Context switching is implemented using the POSIX `ucontext` API.

The scheduler preserves:

- CPU registers
- Stack pointer
- Program counter
- Execution stack

Each thread maintains its own:

- Thread Control Block (TCB)
- Stack
- Context
- Scheduling statistics

---

## Synchronization

The library includes a lightweight mutex implementation for protecting shared resources.

Mutexes support:

- Lock
- Unlock
- Ownership tracking
- Waiting thread management

This prevents race conditions during concurrent execution.

---

## Performance Metrics

The scheduler records runtime statistics for each thread:

- Creation Time
- First Run Time
- Completion Time
- Response Time
- Turnaround Time
- Execution Time

These metrics are useful for evaluating scheduling performance.

---

## Benchmark

A benchmark application is included that performs **parallel vector multiplication** using multiple user-level threads.

The benchmark validates:

- Correct scheduling
- Context switching
- Synchronization
- Mutex correctness
- Result correctness

Example output:

```
Round robin scheduler is running!

Thread with the ID 2 exiting!
Thread with the ID 3 exiting!
Thread with the ID 4 exiting!

Parallel Result using Threads: 333328333350000

Verified sum by manual calculation: 333328333350000

Running Time: 231 microseconds

Mutex and thread library worked correctly!
```

---

## Compilation

Compile the benchmark:

```bash
gcc -I include \
Metrics/vector_multiply.c \
src/STL/queue.c \
src/uthreads.c \
-o vector_benchmark
```

Run:

```bash
./vector_benchmark
```

---

## Technologies Used

- C
- POSIX `ucontext`
- POSIX Signals
- `setitimer()`
- Linux
- GCC

---

## Learning Objectives

This project demonstrates practical understanding of:

- Operating System scheduling
- User-level threading
- Context switching
- Interrupt handling
- Synchronization
- Mutex implementation
- Thread lifecycle management
- Performance measurement
- Concurrent programming

---

## Future Improvements

- Priority scheduling
- Multi-Level Feedback Queue (MLFQ)
- Sleeping threads
- Condition variables
- Semaphores
- Read-write locks
- Deadlock detection
- Work-stealing scheduler
- Thread pools
- Multiple scheduling algorithms selectable at runtime

---

## Author

**Mukhyansh Bhateja**
