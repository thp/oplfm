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

#include "woody.h"
#include "woodyopl.h"


struct adlib_t *adlib_new(uint16_t samplerate)
{
    OPLChipClass *opl = new OPLChipClass(0);
    opl->adlib_init(samplerate, true);
    opl->adlib_write(0x01, 1 << 5, 0);
    return (struct adlib_t *)(opl);
}

void adlib_write(struct adlib_t *adlib, uint8_t addr, uint8_t data)
{
    OPLChipClass *opl = (OPLChipClass *)(adlib);
    opl->adlib_write(addr, data, 0);
}

void adlib_getsample(struct adlib_t *adlib, short *buf, size_t samples)
{
    OPLChipClass *opl = (OPLChipClass *)(adlib);
    opl->adlib_getsample(buf, samples);
}

void adlib_free(struct adlib_t *adlib)
{
    OPLChipClass *opl = (OPLChipClass *)(adlib);
    delete opl;
}
