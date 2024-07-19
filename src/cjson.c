#include "cjson.h"
#include <string.h>
#include <regex.h>


#define BUFFER_SIZE 2048

static PyMethodDef cjson_methods[] = {
        {"loads", cjson_loads, METH_VARARGS, "Parse JSON string"},
        {"dumps", cjson_dumps, METH_VARARGS, "Serialize dictionay to JSON string"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef cjson_module = {
        PyModuleDef_HEAD_INIT,
        "cjson",
        "JSON parsing and serialization module",
        -1,
        cjson_methods
};

PyMODINIT_FUNC PyInit_cjson() {
    return PyModule_Create(&cjson_module);
}


static PyObject* unmarshall(const char* json_str) {
    return parse_value(&json_str);
}

void skip_whitespaces(const char** json_ptr) {
    regex_t regex;
    regmatch_t match[1];
    const char* pattern = "^[ \t\n]+";

    regcomp(&regex, pattern, REG_EXTENDED);

    while (regexec(&regex, *json_ptr, 1, match, 0) == 0) {
        *json_ptr += match[0].rm_eo;
    }

    regfree(&regex);
}


void start_of_the_key(const char** json_ptr) {
    regex_t regex;
    regmatch_t match[1];
    const char* pattern = "^[ \t\n,]";

    regcomp(&regex, pattern, REG_EXTENDED);

    while (regexec(&regex, *json_ptr, 1, match, 0) == 0) {
        *json_ptr += match[0].rm_eo;
    }

    regfree(&regex);
}

void start_of_the_value(const char** json_ptr) {
    regex_t regex;
    regmatch_t match[1];
    const char* pattern = "^[ \t\n:]";

    regcomp(&regex, pattern, REG_EXTENDED);

    while (regexec(&regex, *json_ptr, 1, match, 0) == 0) {
        *json_ptr += match[0].rm_eo;
    }

    regfree(&regex);
}


static PyObject* parse_value(const char** json_str) {
    skip_whitespaces(json_str);

    if (**json_str == '"') {
        ++(*json_str);
        const char* start = *json_str;
        
        while (**json_str != '"') {
            ++(*json_str);
        }

        PyObject* result = PyUnicode_FromStringAndSize(start, *json_str - start);
        ++(*json_str);
        return result;
    } else if (**json_str == '{') {
        ++(*json_str);
        return parse_object(json_str);
    } else if (**json_str == '[') {
        ++(*json_str);
        return parse_list(json_str);
    } else if (**json_str == 't' && strncmp(*json_str, "true", 4) == 0) {
        *json_str += 4;
        Py_RETURN_TRUE;
    } else if (**json_str == 'f' && strncmp(*json_str, "false", 5) == 0) {
        *json_str += 5;
        Py_RETURN_FALSE;
    } else if (**json_str == 'n' && strncmp(*json_str, "null", 4) == 0) {
        *json_str += 4;
        Py_RETURN_NONE;
    } else if (**json_str == '-' || (**json_str >= '0' && **json_str <= '9')) {
        const char* start = *json_str;
        while (**json_str == '-' || (**json_str >= '0' && **json_str <= '9')) {
            ++(*json_str);
        }

        PyObject* result = PyFloat_FromString(PyBytes_FromStringAndSize(start, *json_str - start));
        return result;
    }

    PyErr_SetString(PyExc_ValueError, "Invalid JSON value");
    return NULL;
}

static PyObject* parse_object(const char** json_str) {
    PyObject* dict = PyDict_New();
    
    if (!dict) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create dictionary");
        return NULL;
    }

    while (**json_str && **json_str != '}') {
        
        start_of_the_key(json_str);

        PyObject* key = parse_value(json_str);
        if (!key) {
            Py_XDECREF(key);
            Py_XDECREF(dict);
            return NULL;
        }

        start_of_the_value(json_str);

        PyObject* value = parse_value(json_str);
        if (!value) {
            Py_XDECREF(key);
            Py_XDECREF(value);
            Py_XDECREF(dict);
            return NULL;
        }

        if (PyDict_SetItem(dict, key, value) < 0) {
            Py_XDECREF(key);
            Py_XDECREF(value);
            Py_XDECREF(dict);
            return NULL;
        }

        Py_XDECREF(key);
        Py_XDECREF(value);
    }

    if (**json_str != '}') {
        PyErr_SetString(PyExc_RuntimeError, "Invalid JSON string");
        return NULL;
    }

    ++(*json_str);
    return dict;
}

static PyObject* parse_list(const char** json_str) {
    PyObject* list = PyList_New(0);
    if (!list) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create a list");
        return NULL;
    }

    while (**json_str && **json_str != ']') { 
        PyErr_SetString(PyExc_RuntimeError, "Invalid JSON string");
        return NULL;
    }

        start_of_the_key(json_str);

        PyObject* value = parse_value(json_str);
        if (!value) {
            Py_XDECREF(list);
            Py_XDECREF(value);
            return NULL;
        }

        if (PyList_Append(list, value) < 0) {
            Py_XDECREF(list);
            Py_XDECREF(value);
            return NULL;
        }

        Py_XDECREF(value);
    }


    if (**json_str != ']') {
        PyErr_SetString(PyExc_RuntimeError, "Invalid JSON string");
        return NULL;
    }

    ++(*json_str);
    return list;
}

static PyObject* serialize_value(PyObject* value) {
    if (PyDict_Check(value)) {
        return serialize_object(value);
    } else if (PyList_Check(value)) {
        return serialize_list(value);
    } else if (PyUnicode_Check(value)) {
        const char* str = PyUnicode_AsUTF8(value);
        return PyUnicode_FromFormat("\"%s\"", str);
    } else if (PyLong_Check(value)) {
        long num = PyLong_AsLong(value);
        return PyUnicode_FromFormat("%ld", num);
    } else if (PyFloat_Check(value)) {
        double num = PyFloat_AsDouble(value);
        return PyUnicode_FromFormat("%f", num);
    } else if (value == Py_True) {
        return PyUnicode_FromString("true");
    } else if (value == Py_False) {
        return PyUnicode_FromString("false");
    } else if (value == Py_None) {
        return PyUnicode_FromString("null");
    }

    PyErr_SetString(PyExc_TypeError, "Unsupported type");
    return NULL;
}

static PyObject* serialize_object(PyObject* dict) {
    if (!PyDict_Check(dict)) {
        PyErr_SetString(PyExc_TypeError, "Expected a dictionary");
        return NULL;
    }

    PyObject* items = PyDict_Items(dict);
    if (!items) {
        PyErr_SetString(PyExc_RuntimeError, "Unable to extract items");
        return NULL;
    }

    Py_ssize_t size = PyList_Size(items);
    char* json_str = malloc(BUFFER_SIZE);

    json_str[0] = '\0';

    strcat(json_str, "{");

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* key = PyList_GetItem(items, i);
        PyObject* value = PyDict_GetItem(dict, key);
        PyObject* key_str = PyObject_Str(key);
        PyObject* value_str = serialize_value(value);


        if (!key_str || !value_str) {
            Py_XDECREF(key_str);
            Py_XDECREF(value_str);
            free(json_str);
            Py_XDECREF(items);
            return NULL;
        }

        char* key_cstr = PyUnicode_AsUTF8(key_str);
        char* value_cstr = PyUnicode_AsUTF8(value_str);

        if (i > 0) {
            strcat(json_str, ",");
        }

        strcat(json_str, "\"");
        strcat(json_str, key_cstr);
        strcat(json_str, "\":");
        strcat(json_str, value_cstr);

        Py_DECREF(key_str);
        Py_DECREF(value_str);
    }

    strcat(json_str, "}");

    Py_DECREF(items);
    PyObject* result = PyUnicode_FromString(json_str);
    free(json_str);
    return result;
}

static PyObject* serialize_list(PyObject* list) {

}


static PyObject* cjson_loads(PyObject* self, PyObject* args) {
    const char* json_str;

    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid input");
        return NULL;
    }

    PyObject* result = unmarshall(json_str);

    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid JSON string");
        return NULL;
    }

    return result;
}


static PyObject* cjson_dumps(PyObject* self, PyObject* args) {
    
}