#!/usr/bin/env python

"""Build Gondar with mingw in a Docker container.

The output executable is copied to package/release/gondar.exe. On Jenkins the
package directory is inside the workspace.
"""

from __future__ import print_function

import argparse
import os
import subprocess


def run_cmd(*args):
    """Print and run a command."""
    print(' '.join(args))
    subprocess.check_call(args)


def build_image(image_name, release, chromeover):
    """Build the Dockerfile from the current directory."""
    cmd = ('sudo', 'docker', 'build',
            '--file', 'docker/gondar-win32.Dockerfile',
            '--tag', image_name, '.')
    if (release):
      cmd += ('--build-arg', 'RELEASE=true',)
    if (chromeover):
      cmd += ('--build-arg', 'CHROMEOVER=true',)
    run_cmd(*cmd)


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
    for volume in volumes:
        full_cmd += ('--volume', '{}:{}'.format(*volume))

    full_cmd += (image_name,)
    full_cmd += cmd

    # Copy the result back to the host
    run_cmd(*full_cmd)


def parse_args():
    """Parse some args"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--release', action='store_true')
    parser.add_argument('--chromeover', action='store_true')
    return parser.parse_args()


def main():
    """Build gondar in a Docker container and copy to host."""
    args = parse_args()
    image_name = 'gondar-build'
    output_path = get_output_path('package')

    build_image(image_name, args.release, args.chromeover)
    volume = (output_path, '/opt/host')
    run_container(image_name,
                  'cp', '-r', '/opt/gondar/build/', '/opt/host',
                  volumes=[volume])
    # Change ownership of the output from root to the user running
    # this script
    run_cmd('sudo', 'chown', '--recursive',
            '{}:{}'.format(os.getuid(), os.getgid()),
            output_path)


if __name__ == '__main__':
    main()
