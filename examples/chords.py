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

piano = oplfm.Instrument('PIANO2')
bass = oplfm.Instrument('HARP1')

C1, D1, E1, F1, G1, A1, B1 = 24, 26, 28, 27, 31, 33, 35

OCTAVE = 12

OFF3 = E1 - C1
OFF5 = G1 - C1

CHORDS = [C1, F1, G1, C1, G1, F1, C1, F1]

MELODY = [
    [   0, OFF3, OFF5, OFF3, OFF5,    0, OFF5,    0],
    [OFF3, OFF3, OFF3, OFF3, OFF5, OFF3, OFF3, OFF3],
    [OFF3, OFF5, None, None, OFF3, OFF5, None, None],
    [   0,    1,    2,    3,    4,    5,    6,    7],
    [   7,    6,    5,    4,    3,    2,    1,    0],
]

def play_chord(instrument, base, velocity):
    base += 2 * OCTAVE
    instrument.note(base, velocity)
    instrument.note(base + OFF3, velocity)
    instrument.note(base + OFF5, velocity)


def play_melody(instrument, note, offset, velocity, delay_duration, press_duration, release_duration):
    time.sleep(delay_duration)
    if note is not None:
        instrument.note(note + offset, velocity)
    time.sleep(press_duration)
    if note is not None:
        instrument.note(note + offset, 0)
    time.sleep(release_duration)

while True:
    for i, chord in enumerate(CHORDS):
        print('='*chord)
        play_chord(bass, chord, 127)
        for j, note in enumerate(MELODY[i%len(MELODY)]):
            print('-'*(note or 0))
            play_melody(piano, note, chord + 3 * OCTAVE, 100, 0.01, 0.1, 0.02)
        play_chord(bass, chord, 0)
