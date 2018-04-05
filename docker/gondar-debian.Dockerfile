FROM debian:sid

RUN apt-get update -y
RUN apt-get install -y \
	aptitude \
	git \
  vim \
  qt5-default \
  make \
  cmake \
  build-essential \
  zlib1g-dev

ENV TREAT_WARNINGS_AS_ERRORS=true

ADD CMakeLists.txt Makefile /opt/gondar/
ADD infra /opt/gondar/infra
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD resources /opt/gondar/resources
ADD src /opt/gondar/src
ADD test /opt/gondar/test

# needed by slowtest
RUN mkdir /root/Downloads
# Build gondar and run tests
RUN make -C /opt/gondar build-gondar test

#ADD gondar /opt/nevereware/gondar
