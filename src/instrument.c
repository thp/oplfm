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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "woody/woody.h"
#include "instrument.h"

#include "standard_bnk.h"


struct instrument_t *
instrument_new()
{
    struct instrument_t *i = malloc(sizeof(struct instrument_t));
    memset(i, 0, sizeof(struct instrument_t));

    strncpy(i->name, "default", sizeof(i->name));

    i->carrier.enabled = 1;
    i->carrier.wave = 0;

    i->modulator.enabled = 1;
    i->modulator.wave = 0;

    i->adsr.cAttack = 0xf;
    i->adsr.cDecay = 0x0;
    i->adsr.cSustain = 0x1;
    i->adsr.cRelease = 0xf;

    i->adsr.mAttack = 0xf;
    i->adsr.mDecay = 0x0;
    i->adsr.mSustain = 0x3;
    i->adsr.mRelease = 0xf;

    i->mode.cAmpmod = 0;
    i->mode.cVib = 0;
    i->mode.cSustained = 1;
    i->mode.cKsr = 0;
    i->mode.cMfm = 1;
    i->mode.cScaling = 0;
    i->mode.cLevel = 0;

    i->mode.mAmpmod = 0;
    i->mode.mVib = 0;
    i->mode.mSustained = 1;
    i->mode.mKsr = 0;
    i->mode.mMfm = 1;
    i->mode.mScaling = 0;
    i->mode.mLevel = 0;

    i->feedback.feedback = 0;
    i->feedback.modulation = 0;

    i->misc.amDepth = 0;
    i->misc.vibDepth = 0;
    i->misc.rhythmOn = 0;
    i->misc.rhythmType = 0;

    return i;
}

struct bnkOplRegs {
    uint8_t ksl;
    uint8_t multiple;
    uint8_t feedback;
    uint8_t attack;
    uint8_t sustain;
    uint8_t eg;
    uint8_t decay;
    uint8_t releaseRate;
    uint8_t totalLevel;
    uint8_t am;
    uint8_t vib;
    uint8_t ksr;
    uint8_t con;
};

struct bnkInstrument {
    uint8_t iPercussive;
    uint8_t iVoiceNum;
    struct bnkOplRegs oplModulator;
    struct bnkOplRegs oplCarrier;
    uint8_t iModWaveSel;
    uint8_t iCarWaveSel;
};

void
instrument_load_from_bank(struct instrument_t *instrument, const char *name)
{
    uint16_t numUsed = (standard_bnk[9] << 8) | standard_bnk[8];
    uint32_t offsetName = (standard_bnk[15] << 24) | (standard_bnk[14] << 16) | (standard_bnk[13] << 8) | standard_bnk[12];
    uint32_t offsetData = (standard_bnk[19] << 24) | (standard_bnk[18] << 16) | (standard_bnk[17] << 8) | standard_bnk[16];

    //printf("Would load %s from bank; numUsed=%d, offsetName=%d, offsetData=%d)\n",
    //       name, numUsed, offsetName, offsetData);

    int i;
    uint8_t *names = &(standard_bnk[offsetName]);
    for (i=0; i<numUsed; i++) {
        uint16_t index = (names[1] << 8) | names[0];
        uint8_t flags = names[2];

        // currently unused
        (void)flags;

        char tmp[10];
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, names + 3, 9);
        //printf("Got instrument index=%d, flags=%d, name='%s'\n", index, flags, tmp);
        names += 2 + 1 + 9;

        if (strcmp(tmp, name) == 0) {
            struct bnkInstrument def;
            memcpy(&def, standard_bnk + offsetData + sizeof(def) * index, sizeof(def));

            // set instrument name
            memset(instrument->name, 0, 64);
            strncpy(instrument->name, tmp, 64);

            instrument->carrier.enabled = 1;
            instrument->carrier.wave = def.iCarWaveSel;
            instrument->modulator.enabled = 1;
            instrument->modulator.wave = def.iModWaveSel;

            instrument->adsr.cAttack = def.oplCarrier.attack;
            instrument->adsr.cDecay = def.oplCarrier.decay;
            instrument->adsr.cSustain = def.oplCarrier.sustain;
            instrument->adsr.cRelease = def.oplCarrier.releaseRate;

            instrument->adsr.mAttack = def.oplModulator.attack;
            instrument->adsr.mDecay = def.oplModulator.decay;
            instrument->adsr.mSustain = def.oplModulator.sustain;
            instrument->adsr.mRelease = def.oplModulator.releaseRate;

            instrument->feedback.feedback = def.oplModulator.feedback;
            instrument->feedback.modulation = 1-def.oplModulator.con;

            instrument->mode.cAmpmod = def.oplCarrier.am;
            instrument->mode.cVib = def.oplCarrier.vib;
            instrument->mode.cSustained = def.oplCarrier.eg;
            instrument->mode.cKsr = def.oplCarrier.ksr;
            instrument->mode.cMfm = def.oplCarrier.multiple;
            instrument->mode.cScaling = def.oplCarrier.ksl;
            instrument->mode.cLevel = def.oplCarrier.totalLevel;

            instrument->mode.mAmpmod = def.oplModulator.am;
            instrument->mode.mVib = def.oplModulator.vib;
            instrument->mode.mSustained = def.oplModulator.eg;
            instrument->mode.mKsr = def.oplModulator.ksr;
            instrument->mode.mMfm = def.oplModulator.multiple;
            instrument->mode.mScaling = def.oplModulator.ksl;
            instrument->mode.mLevel = def.oplModulator.totalLevel;

            // TODO instrument->misc.amDepth
            // TODO instrument->misc.vibDepth

            instrument->misc.rhythmOn = def.iPercussive;
            instrument->misc.rhythmType = def.iVoiceNum;

            break;
        }
    }
}

void
instrument_list_bank(void (*fn)(const char *, void *), void *user_data)
{
    uint16_t numUsed = (standard_bnk[9] << 8) | standard_bnk[8];
    uint32_t offsetName = (standard_bnk[15] << 24) | (standard_bnk[14] << 16) | (standard_bnk[13] << 8) | standard_bnk[12];

    uint8_t *names = &(standard_bnk[offsetName]);
    int i;
    for (i=0; i<numUsed; i++) {
        fn((char *)(names + 3), user_data);
        names += 12;
    }
}

void
instrument_free(struct instrument_t *instrument)
{
    free(instrument);
}
