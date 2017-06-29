#pragma once

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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "instrument.h"


struct playback_t {
    struct adlib_t *adlib;
    struct midi_notes_t *midi_notes;

    int channel_note_map[9];
    struct instrument_t *channel_instrument_map[9];
    uint8_t channel_velocity_map[9];

    bool touch_sensitivity;
    bool rhythm_mode;
};


struct playback_t *playback_new(struct adlib_t *adlib);
void playback_upload_instrument(struct playback_t *playback, struct instrument_t *instrument, uint8_t channel);
void playback_update_instruments(struct playback_t *playback);
void playback_midi_note(struct playback_t *playback, uint16_t note, struct instrument_t *instrument, uint8_t velocity);
void playback_all_notes_off(struct playback_t *playback);
void playback_free(struct playback_t *playback);

#ifdef __cplusplus
}
#endif
