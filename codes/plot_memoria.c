#include <Python.h>
// compilar com gcc main.c -o main -I/usr/include/python3.10 -lpython3.10
void plot_memory(int pages[], int length) {
    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')");

    PyObject *pName = PyUnicode_DecodeFSDefault("plot_memoria");
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        PyObject *pFunc = PyObject_GetAttrString(pModule, "plot_memory");

        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pArgs = PyTuple_New(2);
            PyObject *pList = PyList_New(length);

            for (int i = 0; i < length; i++) {
                PyObject *pValue = PyLong_FromLong(pages[i]);
                if (pValue != NULL) {
                    PyList_SetItem(pList, i, pValue);
                } else {
                    PyErr_Print();
                    break;
                }
            }

            PyTuple_SetItem(pArgs, 0, pList);
            PyTuple_SetItem(pArgs, 1, PyLong_FromLong(length));

            PyObject *pReturnValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pReturnValue != NULL) {
                Py_DECREF(pReturnValue);
            } else {
                PyErr_Print();
            }
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
    }

    Py_Finalize();
}
