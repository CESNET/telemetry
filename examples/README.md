# Telemetry Example

## Overview

This example demonstrates two different telemetry applications: a simple example and an advanced example.
The simple example showcases the basic functionality of the telemetry system, providing essential information
such as process ID, start time, uptime, and version. The advanced example, on the other hand, offers a more
comprehensive telemetry data structure that organizes and stores metrics from multiple servers located in
different data centers, utilizing symbolic links for easier access by location and ID.

## Simple Example

The simple example allows users to quickly access fundamental telemetry information.
The resulting directory structure after running the simple example is as follows:

```bash
$ tree /tmp/telemetry/
tmp/telemetry/
├── parameters
├── pid
├── start_time
├── uptime
└── version
```

Example output from the files in the telemetry directory:

```bash
$ cat /tmp/telemetry/*
./simple-example /tmp/telemetry/    # parameters
1528349                             # pid
2024-10-07 15:30:22                 # start_time
154 (s)                             # uptime
1.0.0                               # version
```
## Advanced Example

The advanced example demonstrates a telemetry data structure that organizes and stores metrics from multiple
servers located in different data centers. This structure allows for efficient retrieval and monitoring of key
telemetry metrics such as CPU usage, memory usage, latency, and disk usage.

The resulting directory structure after running the advanced example is as follows:

```bash
$ tree /tmp/telemetry/
tmp/telemetry/
└── data_centers
    ├── 0-prague
    │   ├── server_count
    │   ├── servers
    │   │   ├── server_0
    │   │   │   └── stats
    │   │   ├── server_1
    │   │   │   └── stats
    │   │   └── server_2
    │   │       └── stats
    │   └── summary
    │       └── summary_stats
    ├── 1-new_york
    │   ├── server_count
    │   ├── servers
    │   │   ├── server_0
    │   │   │   └── stats
    │   │   ├── server_1
    │   │   │   └── stats
    │   │   └── server_2
    │   │       └── stats
    │   └── summary
    │       └── summary_stats
    ├── 2-tokyo
    │   ├── server_count
    │   ├── servers
    │   │   ├── server_0
    │   │   │   └── stats
    │   │   ├── server_1
    │   │   │   └── stats
    │   │   └── server_2
    │   │       └── stats
    │   └── summary
    │       └── summary_stats
    ├── by-id
    │   ├── 0 -> ../0-prague
    │   ├── 1 -> ../1-new_york
    │   └── 2 -> ../2-tokyo
    └── by-location
        ├── new_york -> ../1-new_york
        ├── prague -> ../0-prague
        └── tokyo -> ../2-tokyo
```

### Example of Telemetry Output

Here is an example of the contents of the `stats` file for a specific server:

```bash
$ cat /tmp/telemetry/data_centers/0-prague/servers/server_0/stats
cpu_usage:    74.28 (%)
disk_usage:   13.48 (%)
latency:      170.20 (ms)
memory_usage: 31.17 (%)
timestamp:    2024-10-03 15:18:41
```

Example of the contents of the `summary_stats` file for a data center:

```bash
$ cat /tmp/telemetry/data_centers/by-location/prague/summary/summary_stats
cpu_usage [avg]:    44.20 (%)
disk_usage [avg]:   35.78 (%)
latency [avg]:      121.82 (ms)
latency [max]:      193.88 (ms)
latency [min]:      7.04 (ms)
memory_usage [avg]: 54.20 (%)
```

## How to build & run

### Build
To build the examples, simply run the following command in the root of the project:

```bash
make example
```

### Run

Once the build is complete, you can run the examples by specifying the desired mount point:

```bash
./simple-example <mount_point>
# or
./advanced-example <mount_point>
```
For example:

```bash
./simple-example /tmp/telemetry
```
