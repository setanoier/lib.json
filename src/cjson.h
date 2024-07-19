#ifndef CJSON_H
#define CJSON_H

#include <Python.h>

static PyObject* cjson_loads(PyObject* self, PyObject* args);
static PyObject* cjson_dumps(PyObject* self, PyObject* args);

static PyMethodDef cjson_methods[];

static struct PyModuleDef cjson_module;

static PyObject* unmarshall(const char* json_str);
static PyObject* parse_value(const char** json_str);
static PyObject* parse_object(const char** json_str);
static PyObject* parse_list(const char** json_str);

static PyObject* serialize_value(PyObject* value);
static PyObject* serialize_object(PyObject* object);
static PyObject* serialize_list(PyObject* list);


#endif