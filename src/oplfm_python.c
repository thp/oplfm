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

#include "Python.h"
#include "structmember.h"

#include "oplfm.h"
#include "instrument.h"

static OPLFM *
g_oplfm = 0;

static int
g_oplfm_refcnt = 0;

typedef struct {
    PyObject_HEAD

    OPLFMInstrument *handle;
} InstrumentObject;

#ifndef Py_TYPE
    #define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

static void
Instrument_dealloc(InstrumentObject *self)
{
    if (self->handle) {
        oplfm_instrument_free(self->handle);
        if (--g_oplfm_refcnt == 0) {
            oplfm_free(g_oplfm);
            g_oplfm = 0;
        }
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
Instrument_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    InstrumentObject *self = (InstrumentObject *)type->tp_alloc(type, 0);
    self->handle = 0;
    return (PyObject *)self;
}

static int
Instrument_init(InstrumentObject *self, PyObject *args, PyObject *kwargs)
{
    const char *filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return -1;
    }

    if (!g_oplfm) {
        g_oplfm = oplfm_new();
    }
    g_oplfm_refcnt++;

    self->handle = oplfm_instrument_new(filename);

    return 0;
}

static PyObject *
Instrument_note(InstrumentObject *self, PyObject *args, PyObject *kwargs)
{
    int note, velocity;
    if (!PyArg_ParseTuple(args, "ii", &note, &velocity)) {
        return NULL;
    }

    oplfm_note(g_oplfm, self->handle, note, velocity);

    Py_RETURN_NONE;
}

static PyMemberDef
Instrument_members[] = {
    {NULL}
};

static PyMethodDef
Instrument_methods[] = {
    {"note", (PyCFunction)Instrument_note, METH_VARARGS, "Play a note at a given velocity"},
    {NULL}
};

static PyTypeObject
InstrumentType = {
    PyObject_HEAD_INIT(NULL)

    .tp_name = "oplfm.Instrument",
    .tp_basicsize = sizeof(InstrumentObject),
    .tp_dealloc = (destructor)Instrument_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "Instrument for playing notes",
    .tp_methods = Instrument_methods,
    .tp_members = Instrument_members,
    .tp_init = (initproc)Instrument_init,
    .tp_new = Instrument_new,
};

static void
pylist_append_string(const char *filename, void *user_data)
{
    PyObject *list = user_data;
    PyObject *str = PyUnicode_FromString(filename);
    PyList_Append(list, str);
    Py_DECREF(str);
}

#if PY_MAJOR_VERSION >= 3
static PyObject *
pyoplfm_ao(PyObject *self, PyObject *args)
{
    int addr, value;

    if (!PyArg_ParseTuple(args, "ii", &addr, &value)) {
        return NULL;
    }

    {
        // FIXME: Never freed
        if (!g_oplfm) {
            g_oplfm = oplfm_new();
        }
        g_oplfm_refcnt++;
    }

    oplfm_ao(g_oplfm, addr, value);

    Py_RETURN_NONE;
}

static PyObject *
_pyoplfm_monitor_obj = NULL;

static void
_pyoplfm_monitor_func(uint8_t addr, uint8_t value, void *user_data)
{
    PyObject *args = Py_BuildValue("(ii)", (int)addr, (int)value);
    PyObject *result = PyObject_CallObject(_pyoplfm_monitor_obj, args);
    Py_XDECREF(result);
    Py_XDECREF(args);
}

static PyObject *
pyoplfm_set_monitor(PyObject *self, PyObject *obj)
{
    Py_INCREF(obj);
    Py_XDECREF(_pyoplfm_monitor_obj);
    _pyoplfm_monitor_obj = obj;

    {
        // FIXME: Never freed
        if (!g_oplfm) {
            g_oplfm = oplfm_new();
        }
        g_oplfm_refcnt++;
    }

    if (obj == Py_None) {
        oplfm_set_monitor(g_oplfm, NULL, NULL);
        Py_XDECREF(obj);
        _pyoplfm_monitor_obj = NULL;
    } else {
        oplfm_set_monitor(g_oplfm, _pyoplfm_monitor_func, NULL);
    }

    Py_RETURN_NONE;
}

static PyObject *
pyoplfm_reset(PyObject *self, PyObject *obj)
{
    if (g_oplfm) {
        oplfm_free(g_oplfm);
        g_oplfm = oplfm_new();
    }

    Py_RETURN_NONE;
}

static PyMethodDef
OplFMMethods[] = {
    {"ao", pyoplfm_ao, METH_VARARGS, "Direct audio output."},
    {"set_monitor", pyoplfm_set_monitor, METH_O, "Set port write monitor."},
    {"reset", pyoplfm_reset, METH_NOARGS, "Reset the internal state (forget all instruments)."},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "oplfm",
    NULL,
    -1,
    OplFMMethods,
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_oplfm()
#else
PyMODINIT_FUNC
initoplfm()
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *m = PyModule_Create(&moduledef);
#else
    PyObject *m = Py_InitModule("oplfm", NULL);
#endif

    InstrumentType.tp_new = PyType_GenericNew;
    PyType_Ready(&InstrumentType);
    Py_INCREF(&InstrumentType);
    PyModule_AddObject(m, "Instrument", (PyObject *)&InstrumentType);

    PyObject *names = PyList_New(0);
    instrument_list_bank(pylist_append_string, names);
    PyModule_AddObject(m, "names", names);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
