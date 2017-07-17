BUILD_DIR ?= build

# qmake is called different things in different distributions
ifneq (, $(shell which qmake-qt5))
    QMAKE=qmake-qt5
else
    QMAKE=qmake
endif


# Default target: build gondar and run the tests
all: build-gondar test


# Build libminizip.a
build-minizip: update-submodules
	mkdir -p "${BUILD_DIR}/minizip" && \
		cd ${BUILD_DIR}/minizip && \
		cmake -DUSE_AES=OFF ../../minizip && \
		cmake --build .


# Build gondar executable
build-gondar: build-minizip
	cd ${BUILD_DIR} && \
		${QMAKE} .. && \
		make -j


clean:
	rm -rf ${BUILD_DIR}


format:
	clang-format -i src/*.{h,c,cc} test/*.{h,cc}


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
		test \
		update-submodules
