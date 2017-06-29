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

moog = oplfm.Instrument('MOOGSYNT')
organ = oplfm.Instrument('PIANO1')
bass = oplfm.Instrument('BASS2')

pattern0 = [ 60, 70, 60, 68, 60, 66, 60, 64 ]
pattern1 = [ 60, 60, 70, 70, 65, 65, 55, 55 ]
pattern2 = [ 60, 70, 60, 70, 60, 70, 66, 62 ]
pattern3 = [ 60, 50, 55, 45, 50, 40, 45, 35 ]

sequence = [
    pattern0, pattern0, pattern0, pattern1,
    pattern0, pattern2, pattern0, pattern3,
    pattern0, pattern1, pattern2, pattern3,
    pattern3, pattern3, pattern3, pattern1,
]

while True:
    for pattern in sequence:
        for note in pattern:
            moog.note(note, 127)
            bass.note(20+note/2, 120)
            time.sleep(.01)

            organ.note(note+5, 100)
            time.sleep(.4)

            bass.note(20+note/2, 0)
            organ.note(note+5, 0)
            moog.note(note, 0)
            time.sleep(.01)
