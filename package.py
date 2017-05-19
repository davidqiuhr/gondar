#!/usr/bin/env python

"""Build Gondar with mingw in a Docker container.

The output executable is copied to package/gondar.exe. On Jenkins the
package directory is inside the workspace.
"""

from __future__ import print_function

import os
import subprocess


def run_cmd(*args):
    """Print and run a command."""
    print(' '.join(args))
    subprocess.check_call(args)


def build_image(image_name):
    """Build the Dockerfile from the current directory."""
    run_cmd('sudo', 'docker', 'build', '--tag', image_name, '.')


def get_output_path(subdir):
    """Get the host path where the package will be placed."""
    workspace = os.environ.get('WORKSPACE', os.getcwd())
    rel_output_path = os.path.join(workspace, subdir)
    return os.path.abspath(rel_output_path)


def run_container(image_name, *cmd, **kwargs):
    """Run a command in a new Docker container.

    optional kwarg: volumes=[(host, guest), ...]
    """
    volumes = kwargs.get('volumes', [])

    full_cmd = ('sudo', 'docker', 'run', '--rm=true')
    #full_cmd = ('sudo', 'docker', 'run')
    for volume in volumes:
        full_cmd += ('--volume', '{}:{}'.format(*volume))

    full_cmd += (image_name,)
    full_cmd += cmd

    # Copy the result back to the host
    run_cmd(*full_cmd)


def main():
    """Build gondar in a Docker container and copy to host."""
    image_name = 'gondar-build'
    output_path = get_output_path('package')

    build_image(image_name)
    volume = (output_path, '/opt/host')
    run_container(image_name,
                  'cp', '-r', '/opt/gondar/release/', '/opt/host',
                  volumes=[volume])


if __name__ == '__main__':
    main()
