#include "filewrapper.h"
#include "py2py3.h"

int FileWrapper_GetFd(PyObject *self)
{
  return ((FileWrapper*)self)->fd;
}

void FileWrapper_Done(PyObject *self)
{
  PyFile_DecUseCount((PyFileObject*)((FileWrapper*)self)->file);
  Py_DECREF(((FileWrapper*)self)->file);
}

static PyObject*
FileWrapper_New(PyTypeObject* cls, PyObject* args, PyObject* kwargs)
{
  PyObject* file;
  int fd;
  unsigned int ignored_blocksize;

  if(!PyArg_ParseTuple(args, "O|I:FileWrapper", &file, &ignored_blocksize))
    return NULL;

  fd = PyObject_AsFileDescriptor(file);
  if (fd == -1) {
    return NULL;
  }

  Py_INCREF(file);
  PyFile_IncUseCount((PyFileObject*)file);

  FileWrapper* wrapper = PyObject_NEW(FileWrapper, cls);
  wrapper->file = file;
  wrapper->fd = fd;

  return (PyObject*)wrapper;
}

static PyObject*
FileWrapper_GetAttrO(PyObject* self, PyObject* name)
{
  return PyObject_GetAttr(((FileWrapper*)self)->file, name);
}

static PyObject*
FileWrapper_Iter(PyObject* self)
{
  return PyObject_GetIter(((FileWrapper*)self)->file);
}

PyTypeObject FileWrapper_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "FileWrapper",                    /* tp_name (__name__)                     */
  sizeof(FileWrapper)               /* tp_basicsize                           */
};

void _init_filewrapper(void)
{
  FileWrapper_Type.tp_new = FileWrapper_New;
  FileWrapper_Type.tp_iter = FileWrapper_Iter;
  FileWrapper_Type.tp_getattro = FileWrapper_GetAttrO;
  FileWrapper_Type.tp_flags |= Py_TPFLAGS_DEFAULT;
}
