/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.16.4 on Fri Dec  6 15:49:47 2019
 */

#include "sipAPICPyConverter.h"

#line 534 "CPyConverter.sip"
#include <vector>
#line 12 "./sipCPyConverterstdvector2100.cpp"



extern "C" {static void assign_std_vector_2100(void *, SIP_SSIZE_T, const void *);}
static void assign_std_vector_2100(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<std::vector<unsigned long> *>(sipDst)[sipDstIdx] = *reinterpret_cast<const std::vector<unsigned long> *>(sipSrc);
}


extern "C" {static void *array_std_vector_2100(SIP_SSIZE_T);}
static void *array_std_vector_2100(SIP_SSIZE_T sipNrElem)
{
    return new std::vector<unsigned long>[sipNrElem];
}


extern "C" {static void *copy_std_vector_2100(const void *, SIP_SSIZE_T);}
static void *copy_std_vector_2100(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new std::vector<unsigned long>(reinterpret_cast<const std::vector<unsigned long> *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_std_vector_2100(void *, int);}
static void release_std_vector_2100(void *ptr, int)
{
    delete reinterpret_cast<std::vector<unsigned long> *>(ptr);
}



extern "C" {static int convertTo_std_vector_2100(PyObject *, void **, int *, PyObject *);}
static int convertTo_std_vector_2100(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *sipTransferObj)
{
    std::vector<unsigned long> **sipCppPtr = reinterpret_cast<std::vector<unsigned long> **>(sipCppPtrV);

#line 558 "CPyConverter.sip"
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (SIP_SSIZE_T i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      #if PY_MAJOR_VERSION >= 3
      	  if (!PyLong_Check(item)) {
	  #else
          if (!PyLong_Check(item) && !PyInt_Check(item)) {
	  #endif
          //printErrorMessage("TypeError : object in list of type " + std::string(item->ob_type->tp_name) + " can not be converted to unsigned long");
          return 0;
        }
      }
      return 1;
    }

    // Convert Python list of integers to a std::vector<int>
    std::vector<unsigned long> *v = new std::vector<unsigned long>();
    for (SIP_SSIZE_T i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
     #if PY_MAJOR_VERSION >= 3
      	 v->push_back(PyLong_AsUnsignedLong(PyList_GET_ITEM(sipPy, i)));
     #else
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (PyLong_Check(item)) {
        v->push_back(PyLong_AsUnsignedLong(item));
      } else {
        v->push_back(PyInt_AsUnsignedLongMask(item));
      }
     #endif
    }

    *sipCppPtr = v;
    return sipGetState(sipTransferObj);
#line 88 "./sipCPyConverterstdvector2100.cpp"
}


extern "C" {static PyObject *convertFrom_std_vector_2100(void *, PyObject *);}
static PyObject *convertFrom_std_vector_2100(void *sipCppV, PyObject *)
{
   std::vector<unsigned long> *sipCpp = reinterpret_cast<std::vector<unsigned long> *>(sipCppV);

#line 538 "CPyConverter.sip"
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (std::vector<unsigned long>::iterator iter = sipCpp->begin(); iter != sipCpp->end(); ++iter) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyLong_FromUnsignedLong(*iter));
  }

  // Return the Python list.
  return l;
#line 115 "./sipCPyConverterstdvector2100.cpp"
}


sipMappedTypeDef sipTypeDef_CPyConverter_std_vector_2100 = {
    {
        -1,
        0,
        0,
        SIP_TYPE_MAPPED,
        sipNameNr_0,
        {0}
    },
    {
        -1,
        {0, 0, 1},
        0, 0,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    assign_std_vector_2100,
    array_std_vector_2100,
    copy_std_vector_2100,
    release_std_vector_2100,
    convertTo_std_vector_2100,
    convertFrom_std_vector_2100
};
