FROM neverware/gondar-build-mxe:v3

ENV TREAT_WARNINGS_AS_ERRORS=true

ADD CMakeLists.txt Makefile /opt/gondar/
ADD infra /opt/gondar/infra
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD gdisk /opt/gondar/gdisk
ADD resources /opt/gondar/resources
ADD src /opt/gondar/src
ADD test /opt/gondar/test

ARG RELEASE=false
ARG CHROMEOVER=false
ARG METRICS_API_KEY=""
RUN make -C /opt/gondar build-gondar
