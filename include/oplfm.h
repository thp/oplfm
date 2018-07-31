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

#include <unistd.h>
#include <stdint.h>

typedef struct OPLFM OPLFM;
typedef struct OPLFMInstrument OPLFMInstrument;

typedef void (*oplfm_monitor_func_t)(uint8_t addr, uint8_t value, void *user_data);

OPLFM *oplfm_new(void);
void oplfm_note(OPLFM *oplfm, OPLFMInstrument *instrument, int note, int velocity);
void oplfm_ao(OPLFM *oplfm, int addr, int value);
void oplfm_set_monitor(OPLFM *oplfm, oplfm_monitor_func_t func, void *user_data);
void oplfm_free(OPLFM *oplfm);

OPLFMInstrument *oplfm_instrument_new(const char *name);
void oplfm_instrument_free(OPLFMInstrument *instrument);

#ifdef __cplusplus
}
#endif
