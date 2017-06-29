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
#include <string.h>

#include "woody/woody.h"
#include "stream.h"


static int adlib_stream_callback(const void *input, void *output, unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    struct adlib_stream_t *stream = userData;

    adlib_getsample(stream->adlib, output, frameCount);

    return 0;
}


struct adlib_stream_t *adlib_stream_new()
{
    struct adlib_stream_t *stream = calloc(1, sizeof(struct adlib_stream_t));

    int samplerate = 44100;
    stream->adlib = adlib_new(samplerate);

    Pa_Initialize();

    Pa_OpenDefaultStream(&stream->stream, 0, 1, paInt16, samplerate,
            paFramesPerBufferUnspecified, adlib_stream_callback, stream);

    Pa_StartStream(stream->stream);

    return stream;
}

void adlib_stream_free(struct adlib_stream_t *stream)
{
    Pa_StopStream(stream->stream);
    Pa_Terminate();

    adlib_free(stream->adlib);
    free(stream);
}
