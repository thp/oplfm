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

#ifndef WOODYGLUE_H
#define WOODYGLUE_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct adlib_t *adlib_new(uint16_t samplerate);
void adlib_write(struct adlib_t *adlib, uint8_t addr, uint8_t data);
void adlib_getsample(struct adlib_t *adlib, short *buf, size_t samples);
void adlib_free(struct adlib_t *adlib);

#ifdef __cplusplus
};
#endif

#endif /* WOODYGLUE_H */
