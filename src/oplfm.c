/**
 * oplfm: OPL2/OPL3 Instrument Toolkit for C and Python
 * Copyright (C) 2016, 2017 Thomas Perl <m@thp.io>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/

#include <unistd.h>
#include <stdlib.h>

#include "oplfm.h"

#include "instrument.h"
#include "playback.h"
#include "stream.h"


struct OPLFM {
    struct adlib_stream_t *stream;
    struct playback_t *playback;
};

struct OPLFMInstrument {
    struct instrument_t *instrument;
};


OPLFM *oplfm_new()
{
    OPLFM *oplfm = calloc(1, sizeof(OPLFM));
    oplfm->stream = adlib_stream_new();
    oplfm->playback = playback_new(oplfm->stream->adlib);
    return oplfm;
}

void oplfm_note(OPLFM *oplfm, OPLFMInstrument *instrument, int note, int velocity)
{
    playback_midi_note(oplfm->playback, note, instrument->instrument, velocity);
}

void oplfm_free(OPLFM *oplfm)
{
    playback_free(oplfm->playback);
    adlib_stream_free(oplfm->stream);
    free(oplfm);
}

OPLFMInstrument *oplfm_instrument_new(const char *name)
{
    OPLFMInstrument *instrument = calloc(1, sizeof(OPLFMInstrument));
    instrument->instrument = instrument_new();
    if (name) {
        instrument_load_from_bank(instrument->instrument, name);
    }
    return instrument;
}

void oplfm_instrument_free(OPLFMInstrument *instrument)
{
    instrument_free(instrument->instrument);
    free(instrument);
}
