ifeq ($(CMAKE),)
CMAKE := $(shell which cmake3)
endif

ifeq ($(CMAKE),)
CMAKE := cmake
endif

ifeq ($(CLANG_FORMAT),)
CLANG_FORMAT := clang-format
endif

ifeq ($(RUN_CLANG_TIDY),)
RUN_CLANG_TIDY := run-clang-tidy
endif

ifeq ($(CPPCHECK),)
CPPCHECK := cppcheck
endif

SRC_DIR = "$(shell pwd)/src"
INC_DIR = "$(shell pwd)/include"

HEADE_FILTER = "$(SRC_DIR)|$(INC_DIR)"
SOURCE_DIR = "$(SRC_DIR)" "$(INC_DIR)"
SOURCE_REGEX = '.*\.\(cpp\|hpp\)'

CPPCHECK_FLAGS = \
	--std=c++17 \
	--error-exitcode=1 \
	--inline-suppr \
	--suppress=missingIncludeSystem \
	--library=googletest
CPPCHECK_CHECKS = warning,performance,portability,style,information

.PHONY: all
all: build/Makefile
	@$(MAKE) --no-print-directory -C build

%: build/Makefile
	@$(MAKE) --no-print-directory -C build $@

build/Makefile: | build
	@cd build && $(CMAKE) $(CMAKE_ARGS) ..

build:
	@mkdir -p $@

.PHONY: format
format:
	@find $(SOURCE_DIR) -type f -regex $(SOURCE_REGEX) -print0 | xargs -0 $(CLANG_FORMAT) --dry-run

.PHONY: format-fix
format-fix:
	@find $(SOURCE_DIR) -type f -regex $(SOURCE_REGEX) -print0 | xargs -0 $(CLANG_FORMAT) -i

.PHONY: tidy
tidy: all
	$(RUN_CLANG_TIDY) -p build -quiet -j $(shell nproc) -header-filter=$(HEADE_FILTER) $(SOURCE_DIR)

.PHONY: tidy-fix
tidy-fix: all
	$(RUN_CLANG_TIDY) -p build -quiet -fix -j $(shell nproc) -header-filter=$(HEADE_FILTER) $(SOURCE_DIR)

.PHONY: cppcheck
cppcheck: build/Makefile
	@$(CPPCHECK) $(CPPCHECK_FLAGS) --enable=$(CPPCHECK_CHECKS) $(SOURCE_DIR)

.PHONY: test
test: build
	@cd build && $(CMAKE) $(CMAKE_ARGS) -DTELEMETRY_ENABLE_TESTS=ON ..
	@$(MAKE) --no-print-directory -C build
	@$(MAKE) test --no-print-directory -C build

doxygen: build
	@cd build && $(CMAKE) $(CMAKE_ARGS) -DTELEMETRY_ENABLE_DOC_DOXYGEN=ON ..
	@$(MAKE) --no-print-directory -C build $@
