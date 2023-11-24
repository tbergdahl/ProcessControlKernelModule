#### Kernel Process Management Module


_This simple module is designed to track processes inside of the kernel space. It is written in C. It takes a C executable that creates a process to run in the background and uses the /proc filesystem to register these processes within the kernel. Registered processes are stored within a kernel linked list using spinlocks and workqueues. It then updates the CPU time of the processes using a kernel timer. When a user reads from /proc the contents of the list are displayed including process id and remaining CPU time._
