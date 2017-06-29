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

#include <stdint.h>

struct instrument_t {
    char name[64];

    struct {
        uint8_t enabled;
        uint8_t wave;
    } carrier;

    struct {
        uint8_t enabled;
        uint8_t wave;
        uint8_t note;
    } modulator;

    struct {
        uint8_t cAttack, cDecay, cSustain, cRelease;
        uint8_t mAttack, mDecay, mSustain, mRelease;
    } adsr;

    struct {
        uint8_t cAmpmod, cVib, cSustained, cKsr, cMfm, cScaling, cLevel;
        uint8_t mAmpmod, mVib, mSustained, mKsr, mMfm, mScaling, mLevel;
    } mode;

    struct {
        uint8_t feedback;
        uint8_t modulation;
    } feedback;

    struct {
        uint8_t amDepth;
        uint8_t vibDepth;
        uint8_t rhythmOn;
        uint8_t rhythmType;
    } misc;
};

struct instrument_t *instrument_new();
void instrument_load_from_bank(struct instrument_t *instrument, const char *name);
void instrument_list_bank(void (*fn)(const char *, void *), void *user_data);
void instrument_free(struct instrument_t *instrument);

#ifdef __cplusplus
}
#endif
