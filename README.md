# Telemetry

## About

Telemetry is a comprehensive library designed for the collection and processing of telemetry data. It offers data structures for representing directories, files, and their contents, along with methods for reading and writing telemetry data. The library enables efficient management of telemetry data in various applications.

AppFs is a complementary library that integrates with Telemetry to expose telemetry data as a FUSE (Filesystem in Userspace) filesystem. This integration allows telemetry data to be accessed and manipulated through standard filesystem operations, providing a familiar interface for telemetry data management.

## How to build

Install build dependencies of the Telemetry and AppFs libraries.

**RHEL/CentOS:**
```
yum install gcc-c++ make cmake3 fuse3-devel
# Optionally: rpm-build
```

Finally, build and install the Telemtry and AppFs libraries:
```
$ git clone https://github.com/CESNET/telemetry.git
$ cd telemetry
# make install
```

Optionally, build RPM package and install:
```
$ git clone https://github.com/CESNET/telemetry.git
$ cd telemetry
# make rpm
# yum install build/pkg/rpm/rpmbuild/RPMS/x86_64/telemetry-*
```

## How to start

Repository comes with example file in the example directory. Start from there and adapt the code!
