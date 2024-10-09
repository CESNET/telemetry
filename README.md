# Telemetry

## About

**Telemetry** is a comprehensive library designed for the collection and processing of telemetry data.
It offers data structures for representing directories, files, and their contents, along with methods
for reading and writing telemetry data. The library enables efficient management of telemetry data
in various applications.

**AppFs** is a complementary library that integrates with Telemetry to expose telemetry data through
a FUSE (Filesystem in Userspace) interface. This integration allows telemetry data to be accessed and
manipulated as if it were part of the local filesystem, enabling standard filesystem operations such
as reading and writing files. This makes the telemetry data easy to manage and monitor using familiar
file-based tools and utilities.

## Key Features:

- Efficient telemetry data collection and management.
- Data structures to represent telemetry files, directories, and metrics.
- AppFs integration, providing a FUSE-based interface for filesystem-style telemetry data access.
- Flexibility for real-time monitoring and manipulation of telemetry data.

## How to Build

### Build Dependencies

To build the Telemetry and AppFs libraries, ensure that you have the following build dependencies installed.

#### RHEL/CentOS:

```bash
yum install gcc-c++ make cmake3 fuse3-devel
# Optionally: rpm-build for RPM packaging
```

#### Clone and Build the Libraries
First, clone the repository and build the Telemetry and AppFs libraries:

```bash
$ git clone https://github.com/CESNET/telemetry.git
$ cd telemetry
$ make install
```

This will install the necessary libraries, including both Telemetry and AppFs.

#### Optional: Build RPM Package
If you prefer to create an RPM package for installation:

```bash
$ git clone https://github.com/CESNET/telemetry.git
$ cd telemetry
$ make rpm
```

This will package the Telemetry library as an RPM, making installation easier across systems using package managers.

## How to start

The repository includes two example files: a simple example and an advanced example in the `examples` directory.
You can use these to get started with the library and adapt the example code to fit your needs.

## Running the Examples

To run the examples, first build them:

```bash
$ make examples
```

Then, execute the simple example with the desired mount point:

```bash
$ ./simple-example /tmp/telemetry
```

This will create a directory structure representing telemetry data in the specified mount point (e.g., /tmp/telemetry),
where data can be accessed and manipulated using standard filesystem operations.

## Simple Example

Once the simple example is running, it will create a directory structure similar to the following:

```bash
/tmp/telemetry/
├── parameters
├── pid
├── start_time
├── uptime
└── version
```

Example output from the files in the simple example:

```bash
$ cat /tmp/telemetry/*
./simple-example /tmp/telemetry/    # parameters
1528349                             # pid
2024-10-07 15:30:22                 # start_time
154 (s)                             # uptime
1.0.0                               # version
```

## Advanced Example

The advanced example demonstrates a telemetry data structure that organizes and stores metrics
from multiple servers located in different data centers, utilizing symbolic links for easier
access by location and ID.

### Advanced Example Directory Structure
After running the advanced example, the resulting directory structure is as follows:

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

### Example Telemetry Output
Telemetry metrics are stored in files such as stats, which contain real-time data.
Below is an example of the contents of a stats file for a specific server:

```bash
$ cat /tmp/telemetry/data_centers/0-prague/servers/server_0/stats
cpu_usage:    74.28 (%)
disk_usage:   13.48 (%)
latency:      170.20 (ms)
memory_usage: 31.17 (%)
timestamp:    2024-10-03 15:18:41
```

You can also view summary statistics for a data center by reading the summary_stats file:

```bash
$ cat /tmp/telemetry/data_centers/by-location/prague/summary/summary_stats
cpu_usage [avg]:    44.20 (%)
disk_usage [avg]:   35.78 (%)
latency [avg]:      121.82 (ms)
latency [max]:      193.88 (ms)
latency [min]:      7.04 (ms)
memory_usage [avg]: 54.20 (%)
```

## Troubleshooting

### **Problem: FUSE Error – `fusermount3: option allow_other only allowed if 'user_allow_other' is set in /etc/fuse.conf`**

When running the application, you may encounter the following error:

```
fusermount3: option allow_other only allowed if 'user_allow_other' is set in /etc/fuse.conf
fuse_mount() has failed.
```

This error occurs because the FUSE configuration does not allow non-root users to use the allow_other option,
which permits other users to access the mounted filesystem. By default, this option is disabled for security reasons.

### **Solution:**

To resolve this, follow these steps:

1. Open the `/etc/fuse.conf` file using a text editor:
    ```bash
    sudo nano /etc/fuse.conf
    ```

2. Add the following line if it’s missing:
    ```bash
    user_allow_other
    ```

3. Save the file and restart the application:
    ```bash
    ./example /tmp/telemetry
    ```
This will resolve the issue, allowing non-root users to access the FUSE filesystem.
