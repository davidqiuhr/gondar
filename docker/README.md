# Dockerfiles for building Gondar and its dependencies

## gondar-win32

Builds Gondar for Win32 using MXE. This is used in `package.py`, or
you can run it manually from the root directory of the repository:

    docker build -f docker/mxe.Dockerfile .

## mxe.Dockerfile

This Dockerfile builds the parent container for Gondar's build
container. Essentially it clones
the [MXE repository](https://github.com/mxe/mxe) and builds Qt.

The resulting image is stored in the Docker cloud so that building the
Gondar container from scratch doesn't require rebuilding Qt.

To build the container:

    docker build -f mxe.Dockerfile .

The MXE container is not expected to change frequently, so updating
the latest image in the Docker cloud is currently a manual process. To
ensure reproducible builds of Gondar the MXE environment should be
tagged with a version. Just use the highest existing version number
plus one. For example, if the current version is v1, push v2 like this:

	docker build -f mxe.Dockerfile -t neverware/gondar-build-mxe:v2 .
	
	docker push neverware/gondar-build-mxe:v2
