# OS Lab 1

In this lab, we're going to gather OS information using `sys/utsname.h` and the `proc` pseudo-filesystem

## Compilation
To Compile the lab
```
make clean all
```
To run after compilation
```
./lab1
```

### Information Gathered
| Information  | Source             |
|--------------|--------------------|
| System Name  | utsname            |
| Node Name    | utsname            |
| Release      | utsname            |
| Version      | utsname            |
| Machine      | utsname            |
| Boot time    | `/proc/stat` btime |
| Uptime       | `/proc/uptime`     |
| Idle-time    | `/proc/uptime`     |
| User-mode    | `/proc/stat` cpu   |
| System-mode  | `/proc/stat` cpu   |
| MemTotal     | `/proc/meminfo`    |
| MemAvailable | `/proc/meminfo`    |

