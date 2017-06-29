#
# oplfm: OPL2/OPL3 Instrument Toolkit for C and Python
# Copyright (C) 2016, 2017 Thomas Perl <m@thp.io>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

import oplfm
import time
import sys

i = {
    'piano':   oplfm.Instrument('PIANO1'),
    'bdrum': oplfm.Instrument('BDRUM1'),
    'hihat': oplfm.Instrument('NOISE1'),
}

notes = {
    'bdrum': 50,
    'hihat': 60,
    'piano': 20,
}

rhythm = [
    {'bdrum': 'x...x...x...x.x.x...x...x...x.x.x...x...x...x.x.x.x.x.x.x.x.x.x.',
     'hihat': '..................x...x...........x...x.........................',
     'piano': '................x...............x...........x.......x.......x...',},

    {'bdrum': 'x...x...x...x.x.x...x...x...x.x.x...x...x...x...x...x...x...x...',
     'hihat': '..................x...x...........x...x.....x.x.x.x.x.xxxxxxxxxx',
     'piano': '................x...............................................',},

    {'bdrum': 'x...x...x...x.x.x...x...x...x.x.x...x.......x.......x.......x...',
     'hihat': '.......................................xx......xx......xx.......',
     'piano': '.x...x...x.x.x...x...x...x.x.x..................................',},
]

delay = 0.05

while True:
    for row in rhythm:
        for index in range(max(len(pattern) for pattern in row.values())):
            if index % 4 == 0:
                print()

            for instrument in sorted(row.keys()):
                inst = i[instrument]
                c = row[instrument][index]
                sys.stdout.write(instrument[0] if c == 'x' else '-')
                if c == 'x':
                    inst.note(notes[instrument], 127)

            time.sleep(delay)

            for instrument in sorted(row.keys()):
                inst = i[instrument]
                c = row[instrument][index]
                if c == 'x':
                    inst.note(notes[instrument], 0)

            time.sleep(delay)
            sys.stdout.write(' ')
            sys.stdout.flush()
