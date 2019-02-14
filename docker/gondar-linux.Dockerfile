FROM fedora:25

RUN dnf install -y \
    cmake \
    gcc \
    gcc-c++ \
    git \
    libmicrohttpd-devel \
    make \
    python \
    qt5-qtbase-devel \
    which \
    zlib-devel

ENV TREAT_WARNINGS_AS_ERRORS=true
ENV PATH="$PATH:/opt/gondar/styleguide/cpplint"

ADD CMakeLists.txt Makefile /opt/gondar/
ADD infra /opt/gondar/infra
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD resources /opt/gondar/resources
ADD src /opt/gondar/src
ADD test /opt/gondar/test
ADD styleguide /opt/gondar/styleguide

# needed by slowtest
RUN mkdir /root/Downloads
# Build gondar and run tests
RUN make -C /opt/gondar build-gondar test lint
