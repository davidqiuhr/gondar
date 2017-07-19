BUILD_DIR ?= build
CMAKE ?= cmake

# Some distros use different names for qmake and clang-format

ifneq (, $(shell which clang-format))
    CLANG_FORMAT ?= clang-format
else
    CLANG_FORMAT ?= clang-format-4.0
endif

ifneq (, $(shell which qmake-qt5))
    QMAKE ?= qmake-qt5
else
    QMAKE ?= qmake
endif


# Default target: build gondar and run the tests
all: build-gondar test


# Build libminizip.a
build-minizip: print-config update-submodules
	mkdir -p "${BUILD_DIR}/minizip" && \
		cd ${BUILD_DIR}/minizip && \
		${CMAKE} -DUSE_AES=OFF ../../minizip && \
		${CMAKE} --build .


# Build gondar executable
build-gondar: build-minizip
	cd ${BUILD_DIR} && \
		${QMAKE} .. && \
		make -j


clean:
	rm -rf ${BUILD_DIR}


format:
	${CLANG_FORMAT} -i src/*.{h,c,cc} test/*.{h,cc}


help:
	@echo "usage: make [build-gondar|clean|format|jenkins|test]"


# The Jenkins entry point. The Jenkins job should be configured to run
# "make jenkins".
jenkins: jenkins-linux jenkins-win32


jenkins-linux:
	sudo docker build -f docker/gondar-linux.Dockerfile .

jenkins-win32:
ifdef RELEASE
	python package.py --release
else
	python package.py
endif


print-config:
	@echo "Build config {"
	@echo "  BUILD_DIR: ${BUILD_DIR}"
	@echo "  CMAKE: ${CMAKE}"
	@echo "  QMAKE: ${QMAKE}"
	@echo "}"


# Run tests in headless mode
test: build-gondar
	cd ${BUILD_DIR} && TESTARGS="-platform offscreen" make check


update-submodules:
	git submodule update --init


# The targets under "build-" are phony targets because those targets
# handle the real dependency tracking internally
.PHONY: all \
		build-gondar \
		build-minizip \
		clean \
		format \
		jenkins \
		jenkins-linux \
		jenkins-win32 \
		print-config \
		test \
		update-submodules
