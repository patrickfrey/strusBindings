/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "objInitializers.hpp"

int initVariant( Variant& result, PyObject* obj)
{
	try
	{
		if (PyLong_Check( obj))
		{
			long objval = PyInt_AS_LONG( obj);
			if (objval < 0)
			{
				result.assignInt( objval);
			}
			else
			{
				result.assignUint( objval);
			}
		}
		else if (PyInt_Check( obj))
		{
			int objval = PyInt_AS_LONG( obj);
			if (objval < 0)
			{
				result.assignInt( objval);
			}
			else
			{
				result.assignUint( objval);
			}
		}
		else if (PyFloat_Check( obj))
		{
			double objval = PyFloat_AS_DOUBLE( obj);
			result.assignFloat( objval);
		}
		else if (PyString_Check( obj))
		{
			char* objval = PyString_AS_STRING( obj);
			result.assignText( std::string( objval));
		}
		else
		{
			PyErr_SetString( PyExc_Exception, "cannot convert value to numeric value or string");
			return -1;
		}
	}
	catch (...)
	{
		PyErr_SetString( PyExc_Exception, "memory allocation error");
		return -1;
	}
	return 0;
}

template <class Object>
static int initFunctionObject( Object& result, PyObject* obj)
{
	int error = 0;
	if (PyString_Check( obj))
	{
		char* name = PyString_AS_STRING( obj);
		try
		{
			result.setName( name);
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "out of memory exception");
			error = -1;
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObject* seq = PySequence_Fast( obj, "function definition expected as sequence");
		if (seq)
		{
			Py_ssize_t ii=1,len = PySequence_Size( seq);
			if (len == 0)
			{
				PyErr_SetString( PyExc_Exception, "function definition is empty (no name defined)");
				Py_DECREF( seq);
				error = -1;
			}
			else
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, 0);
				/* DON'T DECREF item here */
				if (PyString_Check( item))
				{
					char* name = PyString_AS_STRING( item);
					try
					{
						result.setName( name);
					}
					catch (...)
					{
						PyErr_SetString( PyExc_Exception, "out of memory exception");
						error = -1;
					}
				}
				else
				{
					PyErr_SetString( PyExc_Exception, "first element of function definition is not a string (name of function)");
					error = -1;
				}
			}
			if (!error) for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, ii);
				/* DON'T DECREF item here */
				try
				{
					if (PyLong_Check( item))
					{
						long itemval = PyInt_AS_LONG( item);
						result.addArgumentInt( itemval);
					}
					else if (PyInt_Check( item))
					{
						int itemval = PyInt_AS_LONG( item);
						result.addArgumentInt( itemval);
					}
					else if (PyFloat_Check( item))
					{
						double itemval = PyFloat_AS_DOUBLE( item);
						result.addArgumentFloat( itemval);
					}
					else if (PyString_Check( item))
					{
						char* itemval = PyString_AS_STRING( item);
						result.addArgument( itemval);
					}
					else
					{
						PyErr_SetString( PyExc_Exception, "function argument is not a string or a numeric type");
						error = -1;
						break;
					}
				}
				catch (...)
				{
					PyErr_SetString( PyExc_Exception, "out of memory exception");
					error = -1;
					break;
				}
			}
			Py_DECREF( seq);
		}
		else
		{
			PyErr_SetString( PyExc_Exception, "function is not defined as sequence");
			error = -1;
		}
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "function is not defined as string or sequence (check)");
		error = -1;
	}
	return error;
}

int initTokenizer( Tokenizer& result, PyObject* obj)
{
	return initFunctionObject( result, obj);
}

int initNormalizer( Normalizer& result, PyObject* obj)
{
	return initFunctionObject( result, obj);
}

int initNormalizerList( std::vector<Normalizer>& result, PyObject* obj)
{
	int error = 0;
	if (PyString_Check( obj))
	{
		char* name = PyString_AS_STRING( obj);
		try
		{
			result.push_back( Normalizer( name));
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "out of memory exception");
			return -1;
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObject* seq = PySequence_Fast( obj, "normalizer definition expected as sequence");
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			if (len == 0)
			{
				PyErr_SetString( PyExc_Exception, "normalizer list is empty");
				Py_DECREF( seq);
				return -1;
			}
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, ii);
				/* DON'T DECREF item here */
				try
				{
					result.push_back( Normalizer());
					if (0!=initNormalizer( result.back(), item))
					{
						error = -1;
						break;
					}
				}
				catch (...)
				{
					PyErr_SetString( PyExc_Exception, "memory allocation error");
					error = -1;
					break;
				}
			}
			Py_DECREF( seq);
		}
	}
	return error;
}

int initAggregator( Aggregator& result, PyObject* obj)
{
	return initFunctionObject( result, obj);
}

template <class Object>
static int defineQueryEvaluationFunctionParameter( Object& result, PyObject* keyitem, PyObject* valueitem)
{
	int error = 0;
	char* key;
	Variant value;

	if (PyString_Check( keyitem))
	{
		key = PyString_AS_STRING( keyitem);
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "string expected as query evaluation function parameter name");
		return -1;
	}
	if (0!=initVariant( value, valueitem))
	{
		error = -1;
	}
	else if (key[0] == '.')
	{
		if (value.type() != Variant_TEXT)
		{
			PyErr_SetString( PyExc_Exception, "string expected as query evaluation function feature parameter value");
			error = -1;
		}
		else try
		{
			const char* valuestr = value.getText();
			result.defineFeature( std::string( key+1), valuestr);
		}
		catch (const std::runtime_error& err)
		{
			PyErr_SetString( PyExc_Exception, err.what());
			error = -1;
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "memory allocation error");
			error = -1;
		}
	}
	else
	{
		try
		{
			result.defineParameter( std::string( key), value);
		}
		catch (const std::runtime_error& err)
		{
			PyErr_SetString( PyExc_Exception, err.what());
			error = -1;
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "memory allocation error");
			error = -1;
		}
	}
	return error;
}

template <class Object>
static int initQueryEvalFunctionConfig( Object& result, PyObject* obj)
{
	int error = 0;
	if (PyDict_Check( obj))
	{
		PyObject *keyitem, *valueitem;
		Py_ssize_t pos = 0;
		
		while (PyDict_Next( obj, &pos, &keyitem, &valueitem))
		{
			if (0!=defineQueryEvaluationFunctionParameter( result, keyitem, valueitem))
			{
				error = -1;
				break;
			}
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObject* seq = PySequence_Fast( obj, "query evaluation function object definition expected as sequence of pairs or as dictionary");
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq), elemlen = 0;
			for (; ii<len; ++ii)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, ii);
				/* DON'T DECREF item here */
				if (PySequence_Check( item) && 2==(elemlen=PySequence_Size( item)))
				{
					PyObject* keyitem = PySequence_Fast_GET_ITEM( item, 0);
					PyObject* valueitem = PySequence_Fast_GET_ITEM( item, 1);

					if (0!=defineQueryEvaluationFunctionParameter( result, keyitem, valueitem))
					{
						error = -1;
						break;
					}
				}
				else
				{
					PyErr_SetString( PyExc_Exception, "query evaluation function object definition sequence elements must be pairs");
					error = -1;
					break;
				}
			}
		}
		else
		{
			PyErr_SetString( PyExc_Exception, "query evaluation function object definition expected a sequence of pairs or as dictionary");
			error = -1;
		}
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "dictionary or list of pairs expected as query evaluation function object definition");
		error = -1;
	}
	return error;
}

int initSummarizerConfig( SummarizerConfig& result, PyObject* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
}

int initWeightingConfig( WeightingConfig& result, PyObject* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
}

int initStringVector( std::vector<std::string>& result, PyObject* obj)
{
	int error = 0;
	if (PyString_Check( obj))
	{
		char* item = PyString_AS_STRING( obj);
		try
		{
			result.push_back( item);
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "out of memory exception");
			error = -1;
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObject* seq = PySequence_Fast( obj, "string list expected as sequence");
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, ii);
				/* DON'T DECREF item here */
				try
				{
					if (PyString_Check( item))
					{
						char* itemval = PyString_AS_STRING( item);
						result.push_back( itemval);
					}
					else
					{
						PyErr_SetString( PyExc_Exception, "string expected as element of string list");
						error = -1;
						break;
					}
				}
				catch (...)
				{
					PyErr_SetString( PyExc_Exception, "out of memory exception");
					error = -1;
					break;
				}
			}
			Py_DECREF( seq);
		}
		else
		{
			PyErr_SetString( PyExc_Exception, "list of strings expected");
			error = -1;
		}
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "list of strings or single string expected (check)");
		error = -1;
	}
	return error;
}

int initIntVector( std::vector<int>& result, PyObject* obj)
{
	int error = 0;
	if (PyInt_Check( obj))
	{
		int item = PyInt_AS_LONG( obj);
		try
		{
			result.push_back( item);
		}
		catch (...)
		{
			PyErr_SetString( PyExc_Exception, "out of memory exception");
			error = -1;
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObject* seq = PySequence_Fast( obj, "integer list expected as sequence");
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq, ii);
				/* DON'T DECREF item here */
				try
				{
					if (PyInt_Check( item))
					{
						int itemval = PyInt_AS_LONG( item);
						result.push_back( itemval);
					}
					else
					{
						PyErr_SetString( PyExc_Exception, "integer expected as element of integer list");
						error = -1;
						break;
					}
				}
				catch (...)
				{
					PyErr_SetString( PyExc_Exception, "out of memory exception");
					error = -1;
					break;
				}
			}
			Py_DECREF( seq);
		}
		else
		{
			PyErr_SetString( PyExc_Exception, "list of integers expected");
			error = -1;
		}
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "list of integers or single integer expected (check)");
		error = -1;
	}
	return error;
}

PyObject* getTermVector( const std::vector<Term>& ar)
{
	PyObject* rt = PyList_New( ar.size());
	PyObject* elem = 0;
	PyObject* type = 0;
	PyObject* value = 0;
	PyObject* position = 0;

	std::vector<Term>::const_iterator ai = ar.begin(), ae = ar.end();
	for (Py_ssize_t aidx=0; ai != ae; ++ai,++aidx)
	{
		elem = PyDict_New(); if (!elem) goto ERROR;

		type = PyString_FromString( ai->type().c_str()); if (!type) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "type", type)) goto ERROR;
		Py_DECREF( type); type = 0;

		value = PyString_FromString( ai->value().c_str()); if (!value) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "value", value)) goto ERROR;
		Py_DECREF( value); value = 0;

		position = PyInt_FromLong( ai->position()); if (!position) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "position", position)) goto ERROR;
		Py_DECREF( position); position = 0;

		PyList_SET_ITEM( rt, aidx, elem);
		Py_DECREF( elem); elem = 0;
	}
	return rt;
ERROR:
	if (rt) Py_DECREF( rt);
	if (type) Py_DECREF( type);
	if (value) Py_DECREF( value);
	if (position) Py_DECREF( position);
	if (elem) Py_DECREF( elem);
	return 0;
}

static PyObject* getRankAttributeVector( const std::vector<RankAttribute>& ar)
{
	PyObject* rt = PyList_New( ar.size());
	PyObject* name = 0;
	PyObject* value = 0;
	PyObject* elem = 0;

	std::vector<RankAttribute>::const_iterator ai = ar.begin(), ae = ar.end();
	for (Py_ssize_t aidx=0; ai != ae; ++ai,++aidx)
	{
		name = PyString_FromString( ai->name().c_str()); if (!name) goto ERROR;
		value = PyString_FromString( ai->value().c_str()); if (!value) goto ERROR;
		elem = PyTuple_New( 2); if (!elem) goto ERROR;

		if (0!=PyTuple_SetItem( elem, 0, name)) goto ERROR;
//		Py_DECREF( name);
		name = 0;

		if (0!=PyTuple_SetItem( elem, 1, value)) goto ERROR;
//		Py_DECREF( value);
		value= 0;

		PyList_SET_ITEM( rt, aidx, elem);
//		Py_DECREF( elem);
		elem= 0;
	}
	return rt;
ERROR:
	if (rt) Py_DECREF( rt);
	if (name) Py_DECREF( name);
	if (value) Py_DECREF( value);
	if (elem) Py_DECREF( elem);
	return 0;
}

PyObject* getRankVector( const std::vector<Rank>& ar)
{
	PyObject* rt = PyList_New( ar.size());
	if (!rt) return 0;
	PyObject* docno = 0;
	PyObject* weight = 0;
	PyObject* attributes = 0;
	PyObject* elem = 0;

	std::vector<Rank>::const_iterator ai = ar.begin(), ae = ar.end();
	for (Py_ssize_t aidx=0; ai != ae; ++ai,++aidx)
	{
		elem = PyDict_New();

		docno = PyInt_FromLong( ai->docno()); if (!docno) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "docno", docno)) goto ERROR;
//		Py_DECREF( docno);
		docno = 0;

		weight = PyFloat_FromDouble( ai->weight()); if (!weight) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "weight", weight)) goto ERROR;
//		Py_DECREF( weight);
		weight = 0;

		attributes = getRankAttributeVector( ai->attributes()); if (!attributes) goto ERROR;
		if (0!=PyDict_SetItemString( elem, "attributes", attributes)) goto ERROR;
//		Py_DECREF( attributes);
		attributes = 0;

		PyList_SET_ITEM( rt, aidx, elem);
//		Py_DECREF( elem);
		elem = 0;
	}
	return rt;
ERROR:
	if (rt) Py_DECREF( rt);
	if (docno) Py_DECREF( docno);
	if (weight) Py_DECREF( weight);
	if (attributes) Py_DECREF( attributes);
	if (elem) Py_DECREF( elem);
	return 0;
}



