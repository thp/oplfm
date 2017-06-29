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
#include <stdbool.h>

#include <portaudio.h>


struct adlib_stream_t {
    struct adlib_t *adlib;
    PaStream *stream;
};

struct adlib_stream_t *adlib_stream_new();
void adlib_stream_free(struct adlib_stream_t *stream);

#ifdef __cplusplus
}
#endif
