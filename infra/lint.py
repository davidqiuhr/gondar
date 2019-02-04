#!/usr/bin/env python

# Copyright 2019 Neverware
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
"""Run cpplint on the C++ source code."""

from __future__ import print_function

import subprocess

import shared


def main():
    """Run cpplint to lint source files."""
    bin_path = shared.get_exe(['cpplint.py', 'cpplint'], 'CPPLINT')
    sources = shared.get_source_paths()
    options = [
        '--filter=-build/include,-whitespace/indent,-readability/casting'
    ]
    if bin_path is not None:
        cmd = [bin_path] + options + sources
        print(' '.join(cmd))
        try:
            subprocess.check_call(cmd)
        except subprocess.CalledProcessError:
            # TODO: eventually raise this once it is integrated into PR builder
            # For now there are quite a few errors
            print('Errors found')


if __name__ == '__main__':
    main()
