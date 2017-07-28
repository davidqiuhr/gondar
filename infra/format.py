#!/usr/bin/env python

# Copyright 2017 Neverware
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Run clang-format on the C++ source code."""

from __future__ import print_function

import glob
import os
import subprocess


def is_exe(fpath):
    """True if |fpath| is an executable file, False otherwise."""
    return os.path.isfile(fpath) and os.access(fpath, os.X_OK)


def which(program):
    """Get path of |program| if it exists in the $PATH.

    # Source: https://stackoverflow.com/a/377028/1007956
    """
    fpath, _ = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None


def get_source_paths():
    """Get a list of all source paths that should be formatted."""
    script_dir = os.path.dirname(os.path.realpath(__file__))

    paths = []
    for subdir in ('src', 'test'):
        for extension in ('*.h', '*.cc'):
            path = os.path.join(script_dir, os.pardir, subdir, extension)
            path = os.path.relpath(path, os.getcwd())
            paths += glob.glob(path)

    return paths


def get_clang_format_exe():
    """Get the appropriate clang-format executable.

    First the environment variable CLANG_FORMAT is checked. If not set
    or empty, "clang-format-4.0" and then "clang-format" are tried.
    """
    env_var = os.environ.get('CLANG_FORMAT', '')
    if env_var:
        path = which(env_var)
        if path is None:
            raise Exception('executable not found: ' + env_var)
        else:
            return path

    for name in ('clang-format-4.0', 'clang-format'):
        path = which(name)
        if path is not None:
            return path

    raise Exception('clang-format not found')


def main():
    """Run clang-format to do in-place cleanups on source files."""
    bin_path = get_clang_format_exe()
    sources = get_source_paths()
    if bin_path is not None:
        cmd = [bin_path, '-i'] + sources
        print(' '.join(cmd))
        subprocess.check_call(cmd)


if __name__ == '__main__':
    main()
