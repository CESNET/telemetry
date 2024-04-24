# Project dependencies
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(fuse REQUIRED IMPORTED_TARGET fuse3>=3.2.1)
