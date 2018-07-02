# For this example, you need to install "mido" (pip3 install mido)
# and obtain a copy of "canyon.mid" from Windows 3.x or 9x.
# It ignores most instruments and MIDI commands, but just plays
# the voice and bass on the OPL with basic instruments.

import mido
import oplfm

organ = oplfm.Instrument('PIANO1')
bass = oplfm.Instrument('BASS2')

for msg in mido.MidiFile('canyon.mid').play():
    print(msg)
    if msg.type in ('note_on', 'note_off'):
        if msg.channel == 1:
            organ.note(msg.note, msg.velocity)
        elif msg.channel == 2:
            bass.note(msg.note, msg.velocity)
