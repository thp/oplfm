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
import math

instruments = [oplfm.Instrument(name) for name in ('BASS1', 'BASS2', 'CLARINET', 'ELPIANO2', 'OBOE1',
                                                  'ORGAN1', 'STRINGS1', 'VIBRA2', 'XYLO1',)]

note = 30
delta = 7
idx = 0
while True:
    instr = instruments[idx % len(instruments)]
    note += delta
    if note > 90 or note < 30:
        delta = (-5 if delta > 0 else 7)
        note = max(30, min(90, note))
        idx += 1

    for velocity in range(20, 127, 30):
        instr.note(note, 0)
        time.sleep(0.01)
        instr.note(note, velocity)

    time.sleep(0.2 * abs(math.sin(note * 0.01)))

    instr.note(note, 0)
