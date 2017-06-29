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

#include "playback.h"

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "woody/woody.h"
#include "instrument.h"

struct midi_notes_t {
    // Mapping of midi notes to frequencies, f-numbers and block numbers
    struct {
        int frequency;
        int f_num;
        int block;
    } notes[128];
};

struct midi_notes_t *midi_notes_new();
void midi_notes_get(struct midi_notes_t *midi_notes, uint8_t note, int *f_num, int *block);
void midi_notes_free(struct midi_notes_t *midi_notes);


struct midi_notes_t *midi_notes_new()
{
    struct midi_notes_t *midi_notes = calloc(1, sizeof(struct midi_notes_t));

    int note;
    for (note=0; note<128; note++) {
        double frequency = pow(2., (note - 69.) / 12.) * 440.;

        midi_notes->notes[note].frequency = frequency;

        double freq_scaled = frequency / 49716. * pow(2., 20.);
        int block = -1;
        int f_num = 0;
        do {
            block++;
            f_num = freq_scaled / pow(2., block);
        } while (f_num >= 1024 && block < 7);

        if (f_num < 1024 && block <= 7) {
            midi_notes->notes[note].f_num = f_num;
            midi_notes->notes[note].block = block;
        }
    }

    return midi_notes;
}

void midi_notes_get(struct midi_notes_t *midi_notes, uint8_t note, int *f_num, int *block)
{
    *f_num = midi_notes->notes[note].f_num;
    *block = midi_notes->notes[note].block;
}

void midi_notes_free(struct midi_notes_t *midi_notes)
{
    free(midi_notes);
}



// Offset mapping for [channel][operator] (channel 0..8, operator 0..1)
static const uint8_t channel_operator[9][2] = {
    { 0x03, 0x00, }, // channel 1
    { 0x04, 0x01, }, // channel 2
    { 0x05, 0x02, }, // channel 3

    { 0x0b, 0x08, }, // channel 4
    { 0x0c, 0x09, }, // channel 5
    { 0x0d, 0x0a, }, // channel 6

    { 0x13, 0x10, }, // channel 7
    { 0x14, 0x11, }, // channel 8
    { 0x15, 0x12, }, // channel 9
};




static void playback_ao(struct playback_t *playback, uint8_t addr, uint8_t value);

static void playback_adsr(struct playback_t *playback, uint8_t channel, uint8_t operator,
        uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release);

static void playback_mode(struct playback_t *playback, uint8_t channel, uint8_t operator,
        uint8_t ampmod, uint8_t vib, uint8_t sustained,
        uint8_t ksr, uint8_t mfm, uint8_t scaling, uint8_t level,
        uint8_t waveform);

static void playback_cnote(struct playback_t *playback, uint8_t channel, uint8_t on, uint8_t note);

static int playback_get_channel(struct playback_t *playback, int note, uint8_t velocity, struct instrument_t *instrument);
static int playback_put_channel(struct playback_t *playback, int note, struct instrument_t *instrument);


void playback_ao(struct playback_t *playback, uint8_t addr, uint8_t value)
{
    //printf(" %02x:  %03d\n", addr, value);
    adlib_write(playback->adlib, addr, value);
}

void playback_adsr(struct playback_t *playback, uint8_t channel, uint8_t operator,
        uint8_t attack, uint8_t decay, uint8_t sustain, uint8_t release)
{
    uint8_t ad = ((attack & 0xf) << 4) | (decay & 0xf);
    uint8_t sr = ((sustain & 0xf) << 4) | (release & 0xf);

    uint8_t offset = channel_operator[channel][operator];
    playback_ao(playback, 0x60 + offset, ad);
    playback_ao(playback, 0x80 + offset, sr);
}

void playback_mode(struct playback_t *playback, uint8_t channel, uint8_t operator,
        uint8_t ampmod, uint8_t vib, uint8_t sustained,
        uint8_t ksr, uint8_t mfm, uint8_t scaling, uint8_t level,
        uint8_t waveform)
{
    uint8_t offset = channel_operator[channel][operator];
    uint8_t low = ((ampmod & 0x1) << 7) |
                  ((vib & 0x1) << 6) |
                  ((sustained & 0x1) << 5) |
                  ((ksr & 0x1) << 4) |
                  (mfm & 0xf);
    uint8_t high = ((scaling & 0x3) << 6) |
                   (level & 0x3f);

    playback_ao(playback, 0x20 + offset, low);
    playback_ao(playback, 0x40 + offset, high);
    playback_ao(playback, 0xe0 + offset, waveform);
}

void playback_cnote(struct playback_t *playback, uint8_t channel, uint8_t on, uint8_t note)
{
    int freq = 0;
    int block = 0;
    midi_notes_get(playback->midi_notes, note, &freq, &block);

    uint8_t low = freq & 0xff;
    uint8_t high = ((freq & 0x300) >> 8) | ((on & 0x1) << 5) | ((block & 0x7) << 2);

    playback_ao(playback, 0xa0 + channel, low);
    playback_ao(playback, 0xb0 + channel, high);
}

int playback_get_channel(struct playback_t *playback, int note, uint8_t velocity, struct instrument_t *instrument)
{
    int i;
    for (i=0; i<9; i++) {
        if (playback->channel_note_map[i] == -1) {
            playback->channel_note_map[i] = note;
            playback->channel_velocity_map[i] = velocity;
            playback->channel_instrument_map[i] = instrument;
            playback_upload_instrument(playback, playback->channel_instrument_map[i], i);
            return i;
        }
    }

    return -1;
}

int playback_put_channel(struct playback_t *playback, int note, struct instrument_t *instrument)
{
    int i;
    for (i=0; i<9; i++) {
        if (playback->channel_note_map[i] == note && playback->channel_instrument_map[i] == instrument) {
            playback->channel_note_map[i] = -1;
            playback->channel_velocity_map[i] = 0;
            playback->channel_instrument_map[i] = NULL;
            return i;
        }
    }

    return -1;
}






struct playback_t *playback_new(struct adlib_t *adlib)
{
    struct playback_t *playback = calloc(1, sizeof(struct playback_t));

    playback->adlib = adlib;
    playback->midi_notes = midi_notes_new();
    playback->touch_sensitivity = true;

    int i;
    for (i=0; i<9; i++) {
        playback->channel_note_map[i] = -1;
    }

    return playback;

}


void playback_upload_instrument(struct playback_t *playback, struct instrument_t *instrument, uint8_t channel)
{
    uint8_t velocity = playback->channel_velocity_map[channel];

    // load feedback
    uint8_t fm = ((instrument->feedback.feedback & 0x7) << 1) | (instrument->feedback.modulation & 0x1);
    playback_ao(playback, 0xc0 + channel, fm);

    playback_adsr(playback, channel, 0, instrument->adsr.cAttack, instrument->adsr.cDecay, instrument->adsr.cSustain, instrument->adsr.cRelease);
    playback_adsr(playback, channel, 1, instrument->adsr.mAttack, instrument->adsr.mDecay, instrument->adsr.mSustain, instrument->adsr.mRelease);

    uint16_t scLevel = instrument->mode.cLevel;
    uint16_t smLevel = instrument->mode.mLevel;

    if (playback->touch_sensitivity) {
        float vLevel = (float)velocity / (float)0x7f;
        float cLevel = 1.f - (float)instrument->mode.cLevel / (float)0x3f;
        scLevel = (1.0 - cLevel * pow(vLevel, 1. / 2.)) * 0x3f;
        if (instrument->feedback.modulation == 0) {
            float mLevel = 1.f - (float)instrument->mode.mLevel / (float)0x3f;
            smLevel = (1.0 - mLevel * pow(vLevel, 1. / 2.)) * 0x3f;
        }
    }

    playback_mode(playback, channel, 0, instrument->mode.cAmpmod, instrument->mode.cVib, instrument->mode.cSustained,
            instrument->mode.cKsr, instrument->mode.cMfm, instrument->mode.cScaling, scLevel,
            instrument->carrier.wave);
    playback_mode(playback, channel, 1, instrument->mode.mAmpmod, instrument->mode.mVib, instrument->mode.mSustained,
            instrument->mode.mKsr, instrument->mode.mMfm, instrument->mode.mScaling, instrument->mode.mLevel,
            instrument->modulator.wave);
}



void playback_all_notes_off(struct playback_t *playback)
{
    int i;
    for (i=0; i<9; i++) {
        playback_cnote(playback, 0, 0, 0);
        playback->channel_note_map[i] = -1;
        playback->channel_velocity_map[i] = 0;
    }
}

void playback_update_instruments(struct playback_t *playback)
{
#if 0
    if (!playback->rhythm_mode) {
        // load rhythm section
        uint8_t bd = ((app->instrument->misc.amDepth & 1) << 7) |
                     ((app->instrument->misc.vibDepth & 1) << 6);
        ao(0xbd, bd);
    }
#endif

    int i;
    for (i=0; i<9; i++) {
        if (playback->channel_instrument_map[i]) {
            playback_upload_instrument(playback, playback->channel_instrument_map[i], i);
        }
    }
}

void playback_midi_note(struct playback_t *playback, uint16_t note, struct instrument_t *instrument, uint8_t velocity)
{
    if (playback->rhythm_mode && instrument->misc.rhythmOn) {
        uint8_t rhythmBit = (instrument->misc.rhythmType >= 6 &&
                             instrument->misc.rhythmType <= 10) ? (1 << (10 - instrument->misc.rhythmType)) : 0;

        if (velocity > 0) {
            playback->channel_instrument_map[6] = instrument;
            playback->channel_velocity_map[6] = velocity;
            playback_upload_instrument(playback, playback->channel_instrument_map[6], 6);
            playback_cnote(playback, 6, 0, note);

            playback->channel_instrument_map[7] = instrument;
            playback->channel_velocity_map[7] = velocity;
            playback_upload_instrument(playback, playback->channel_instrument_map[7], 7);
            playback_cnote(playback, 7, 0, note);

            playback->channel_instrument_map[8] = instrument;
            playback->channel_velocity_map[8] = velocity;
            playback_upload_instrument(playback, playback->channel_instrument_map[8], 8);
            playback_cnote(playback, 8, 0, note);
        } else {
            playback_cnote(playback, 6, 0, 0);
            playback_cnote(playback, 7, 0, 0);
            playback_cnote(playback, 8, 0, 0);
        }

        uint8_t bd = ((instrument->misc.amDepth & 1) << 7) |
                     ((instrument->misc.vibDepth & 1) << 6) |
                     ((instrument->misc.rhythmOn & 1) << 5) |
                     ((velocity > 0) ? rhythmBit : 0);

        playback_ao(playback, 0xbd, bd);

        return;
    }

    int channel = (velocity > 0) ? playback_get_channel(playback, note, velocity, instrument) : playback_put_channel(playback, note, instrument);

    playback_cnote(playback, channel, (velocity > 0) ? instrument->carrier.enabled : 0, note);
}

void playback_free(struct playback_t *playback)
{
    midi_notes_free(playback->midi_notes);
    free(playback);
}
