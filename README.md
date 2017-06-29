# oplfm: OPL2/OPL3 Instrument Toolkit for C and Python

This is a C library and Python 2/3 module for using the DOSBox
OPL2/OPL3 emulation codebase ("woodyopl") on a high level, by
exposing instruments (from AdLib Visual Composer) that can play
notes using a single API function.

## License

    oplfm: OPL2/OPL3 Instrument Toolkit for C and Python
    Copyright (C) 2016, 2017 Thomas Perl <m@thp.io>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

## Dependencies

  * [PortAudio](http://www.portaudio.com) (for audio output)

## Example Usage

    import oplfm
    import time
    
    print('Available instrument names:', oplfm.names)
    
    moog = oplfm.Instrument('MOOGSYNT')
    organ = oplfm.Instrument('PIANO1')
    
    for i in range(64, 90):
        moog.note(64, 127)  # play note, full velocity (127)
        time.sleep(0.2)
        organ.note(i, 64)  # play note, half velocity (64)
        time.sleep(0.5)
        moog.note(64, 0)  # release note
        time.sleep(0.2)
        organ.note(i, 0)  # release note
        time.sleep(0.2)
        i += 1

## Installation Instructions

  1. Static library for use in C projects: `make`
  2. Python 2 module: `python setup.py install`
  3. Python 3 module: `python3 setup.py install`

## Related Libraries

`oplfm` only provides a high-level interface to OPL2/OPL3 FM
synthesis, if you need more low-level access, try one of these:

  * [PyOPL](https://github.com/Malvineous/pyopl): Full register access, raw PCM data output
  * [oply](https://github.com/TexZK/oply): Pure python OPL emulation; supports IMF chunks
