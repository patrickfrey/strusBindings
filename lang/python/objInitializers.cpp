/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2015 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "objInitializers.hpp"
#include "private/internationalization.hpp"
#include "private/internationalization.hpp"
#include <boost/algorithm/string.hpp>

#undef STRUS_LOWLEVEL_DEBUG

#ifdef STRUS_LOWLEVEL_DEBUG
static std::string getObjectTypeName( PyObject* obj)
{
	PyObject* typeobj = PyObject_Type(obj);
	if (!typeobj) throw std::runtime_error(_TXT("object has no type"));
	PyObject* typenam = PyObject_GetAttrString( typeobj, "__name__");
	if (!typenam) throw std::runtime_error(_TXT("type object has no name"));
	if (!PyString_Check( typenam)) throw std::runtime_error(_TXT("type object name is not a string"));
	return std::string( PyString_AS_STRING( typenam));
}
#endif

static bool caseInsensitiveEquals( const std::string& val1, const std::string& val2)
{
	return boost::algorithm::iequals( val1, val2);
}

class PyObjectReference
{
public:
	///\brief Constructor
	PyObjectReference( PyObject* obj_=0)
		:m_obj(obj_){}

	///\brief Assignment operator
	PyObjectReference& operator=( PyObject* obj_)
	{
		if (m_obj) Py_DECREF( m_obj);
		m_obj = obj_;
		return *this;
	}

	///\brief Destructor
	~PyObjectReference()
	{
		if (m_obj) Py_DECREF( m_obj);
	}

	///\brief Cast operator
	operator PyObject*()
	{
		return m_obj;
	}

	PyObject* ptr()
	{
		return m_obj;
	}

	operator bool()
	{
		return m_obj != 0;
	}

private:
	PyObject* m_obj;
};

void initVariant( Variant& result, PyObject* obj)
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
		/* ... borrowed referene */
		result.assignText( std::string( objval));
	}
	else if (PyUnicode_Check( obj))
	{
		PyObjectReference temp_obj( PyUnicode_AsUTF8String( obj));
		if (!temp_obj) throw strus::runtime_error( _TXT( "error initializing function object: %s"), _TXT("failed to convert python unicode string to UTF-8"));
		initVariant( result, temp_obj);
	}
	else
	{
		throw strus::runtime_error( _TXT("cannot convert value to numeric value or string"));
	}
}

template <class Object>
static void initFunctionObject( Object& result, PyObject* obj)
{
	if (PyString_Check( obj))
	{
		char* name = PyString_AS_STRING( obj);
		result.setName( name);
	}
	else if (PyUnicode_Check( obj))
	{
		PyObjectReference temp_obj( PyUnicode_AsUTF8String( obj));
		if (!temp_obj) throw strus::runtime_error( _TXT("failed to convert python unicode string to UTF-8"));
		initFunctionObject( result, temp_obj);
	}
	else if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("function definition expected as sequence")));
		if (seq)
		{
			Py_ssize_t ii=1,len = PySequence_Size( seq);
			if (len == 0)
			{
				throw strus::runtime_error( _TXT( "function definition is empty (no name defined)"));
			}
			else
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), 0);
				/* DON'T DECREF item here */
				if (PyString_Check( item))
				{
					char* name = PyString_AS_STRING( item);
					result.setName( name);
				}
				else if (PyUnicode_Check( item))
				{
					PyObjectReference temp_obj( PyUnicode_AsUTF8String( item));
					if (!temp_obj) throw strus::runtime_error( _TXT( "error initializing function object: %s"), _TXT("failed to convert python unicode string to UTF-8"));

					char* name = PyString_AS_STRING( temp_obj.ptr());
					result.setName( name);
				}
				else
				{
					throw strus::runtime_error( _TXT( "first element of function definition is not a string (name of function)"));
				}
			}
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
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
					else if (PyUnicode_Check( item))
					{
						PyObjectReference temp_obj( PyUnicode_AsUTF8String( item));
						if (!temp_obj) throw strus::runtime_error( _TXT( "error initializing function object: %s"), _TXT("failed to convert python unicode string to UTF-8"));

						char* itemval = PyString_AS_STRING( temp_obj.ptr());
						result.addArgument( itemval);
					}
					else
					{
						throw strus::runtime_error( _TXT( "error initializing function object: %s"), "function argument is not a string or a numeric type");
					}
				}
				catch (const std::exception& err)
				{
					throw strus::runtime_error( _TXT( "error initializing function object: %s"), err.what());
				}
			}
		}
		else
		{
			throw strus::runtime_error( _TXT( "function is not defined as sequence"));
		}
	}
	else
	{
		throw strus::runtime_error( _TXT( "function is not defined as string or sequence (check)"));
	}
}

void initTokenizer( Tokenizer& result, PyObject* obj)
{
	initFunctionObject( result, obj);
}

void initNormalizer( Normalizer& result, PyObject* obj)
{
	initFunctionObject( result, obj);
}

void initNormalizerList( std::vector<Normalizer>& result, PyObject* obj)
{
	if (PyString_Check( obj))
	{
		char* name = PyString_AS_STRING( obj);
		try
		{
			result.push_back( Normalizer( name));
		}
		catch (const std::exception& err)
		{
			throw strus::runtime_error( _TXT( "error initializing normalizer list: %s"), err.what());
		}
	}
	else if (PyUnicode_Check( obj))
	{
		PyObjectReference temp_obj( PyUnicode_AsUTF8String( obj));
		if (!temp_obj) throw strus::runtime_error( _TXT( "error initializing normalizer list: %s"), _TXT("failed to convert python unicode string to UTF-8"));
		initNormalizerList( result, temp_obj);
	}
	else if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("normalizer definition expected as sequence")));
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			if (len == 0)
			{
				throw strus::runtime_error( _TXT( "normalizer list is empty"));
			}
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
				/* DON'T DECREF item here */
				try
				{
					result.push_back( Normalizer());
					initNormalizer( result.back(), item);
				}
				catch (const std::exception& err)
				{
					throw strus::runtime_error( _TXT( "error initializing normalizer list: %s"), err.what());
				}
			}
		}
	}
	else
	{
		throw strus::runtime_error( _TXT( "error initializing normalizer list: %s"), _TXT("cannot convert from this type"));
	}
}

void initAggregator( Aggregator& result, PyObject* obj)
{
	initFunctionObject( result, obj);
}

template <class Object>
static void defineQueryEvaluationFunctionParameter( Object& result, PyObject* keyitem, PyObject* valueitem)
{
	char* key;
	PyObjectReference key_obj;
	Variant value;

	if (PyString_Check( keyitem))
	{
		key = PyString_AS_STRING( keyitem);
	}
	else if (PyUnicode_Check( keyitem))
	{
		key_obj = PyUnicode_AsUTF8String( keyitem);
		if (!key_obj) throw strus::runtime_error( _TXT( "cannot define query evaluation function parameter: %s"), _TXT("failed to convert python unicode string to UTF-8"));
		key = PyString_AS_STRING( key_obj.ptr());
	}
	else
	{
		throw strus::runtime_error( _TXT("cannot define query evaluation function parameter: %s"), _TXT( "string expected as query evaluation function parameter name"));
	}
	initVariant( value, valueitem);
	if (key[0] == '.')
	{
		if (value.type() != Variant_TEXT)
		{
			throw strus::runtime_error( _TXT("string expected as query evaluation function feature parameter value"));
		}
		else try
		{
			const char* valuestr = value.getText();
			result.defineFeature( std::string( key+1), valuestr);
		}
		catch (const std::exception& err)
		{
			throw strus::runtime_error( _TXT("cannot define query evaluation function parameter: %s"), err.what());
		}
	}
	else
	{
		try
		{
			result.defineParameter( std::string( key), value);
		}
		catch (const std::exception& err)
		{
			throw strus::runtime_error( _TXT("cannot define query evaluation function parameter: %s"), err.what());
		}
	}
}

template <class Object>
static void initQueryEvalFunctionConfig( Object& result, PyObject* obj)
{
	if (PyDict_Check( obj))
	{
		PyObject *keyitem, *valueitem;
		Py_ssize_t pos = 0;
		
		while (PyDict_Next( obj, &pos, &keyitem, &valueitem))
		{
			defineQueryEvaluationFunctionParameter( result, keyitem, valueitem);
		}
	}
	else if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("query evaluation function object definition expected as sequence of pairs or as dictionary")));
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq), elemlen = 0;
			for (; ii<len; ++ii)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
				/* DON'T DECREF item here */
				if (PySequence_Check( item) && 2==(elemlen=PySequence_Size( item)))
				{
					PyObject* keyitem = PySequence_Fast_GET_ITEM( item, 0);
					PyObject* valueitem = PySequence_Fast_GET_ITEM( item, 1);

					defineQueryEvaluationFunctionParameter( result, keyitem, valueitem);
				}
				else
				{
					throw strus::runtime_error( _TXT("query evaluation function object definition sequence elements must be pairs"));
				}
			}
		}
		else
		{
			throw strus::runtime_error( _TXT("query evaluation function object definition expected a sequence of pairs or as dictionary"));
		}
	}
	else
	{
		throw strus::runtime_error( _TXT("dictionary or list of pairs expected as query evaluation function object definition"));
	}
}

void initSummarizerConfig( SummarizerConfig& result, PyObject* obj)
{
	initQueryEvalFunctionConfig( result, obj);
}

void initWeightingConfig( WeightingConfig& result, PyObject* obj)
{
	initQueryEvalFunctionConfig( result, obj);
}

static char* getString( PyObject* item, PyObjectReference& temp_obj)
{
	if (PyString_Check( item))
	{
		return PyString_AS_STRING( item);
	}
	else if (PyUnicode_Check( item))
	{
		temp_obj = PyUnicode_AsUTF8String( item);
		if (!temp_obj) throw strus::runtime_error( _TXT("failed to convert python unicode string to UTF-8"));

		return PyString_AS_STRING( temp_obj.ptr());
	}
	else
	{
		throw strus::runtime_error( "not a string type");
	}
}

void initQueryExpression( QueryExpression& result, PyObject* obj)
{
	if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("query evaluation function object definition expected as sequence of pairs or as dictionary")));
		if (seq)
		{
			enum {
				Init,
				Funcname,
				Range,
				Cardinality,
				Arguments
			} state = Init;
			char* varname = 0;
			char* funcname = 0;
			char* termtype = 0;
			char* termval = 0;
			int range = 0;
			unsigned int cardinality = 0;
			unsigned int nof_arguments = 0;
			int argcnt = 0;
			char* itemstr;
			PyObjectReference temp_obj_varname;
			PyObjectReference temp_obj_funcname;
			PyObjectReference temp_obj_termval;

			Py_ssize_t ii=0,len = PySequence_Size( seq);
			for (; ii<len; ++ii)
			{
				PyObject* item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
				/* DON'T DECREF item here */
				switch (state)
				{
					case Init:
						itemstr = getString( item, temp_obj_varname);
						if (itemstr[0] == '=')
						{
							varname = itemstr+1;
							state = Funcname;
						}
						else
						{
							if (ii+1 == len)
							{
								result.pushTerm( itemstr, "");
							}
							else
							{
								funcname = itemstr;
								state = Range;
							}
						}
						break;
					case Funcname:
						try
						{
							try
							{
								funcname = getString( item, temp_obj_funcname);
							}
							catch (const std::runtime_error& err)
							{
								throw strus::runtime_error( _TXT("error fetching element after variable assignment in expression tuple: %s"), err.what());
							}
							if (ii+1 == len)
							{
								result.pushTerm( funcname, "");
								if (varname)
								{
									result.attachVariable( varname);
									varname = 0;
								}
								funcname = 0;
								break;
							}
						}
						catch (const std::runtime_error& err)
						{
							throw strus::runtime_error( _TXT("error fetching element after variable assignment in expression tuple: %s"), err.what());
						}
						catch (const std::exception& err)
						{
							throw strus::runtime_error( _TXT("error fetching element after variable assignment in expression tuple: %s"), err.what());
						}
						catch (...)
						{
							throw strus::runtime_error( _TXT("error fetching element after variable assignment in expression tuple: %s"), "unknown exception");
						}
						state = Range;
						break;
					case Range:
					case Cardinality:
						if (PyLong_Check( item) || PyInt_Check( item))
						{
							long itemval = PyInt_AS_LONG( item);
							if (state == Range)
							{
								if (itemval <= std::numeric_limits<int>::min() || itemval >= std::numeric_limits<int>::max())
								{
									throw strus::runtime_error( _TXT("range value exceeds size allowed"));
								}
								range = (int)itemval;
								state = Cardinality;
							}
							else
							{
								// ... state == Cardinality
								if (itemval <= 0 || itemval >= std::numeric_limits<int>::max())
								{
									throw strus::runtime_error( _TXT("cardinality value exceeds size allowed"));
								}
								cardinality = (unsigned int)itemval;
								state = Arguments;
							}
							break;
						}
						state = Arguments;
						/*no break here!*/
					case Arguments:
						if (argcnt++ == 0)
						{
							if (PyString_Check( item))
							{
								// [A] handle special case of 1 or 2-tuple of strings, that defines a term:
								try
								{
									termtype = funcname;
									termval = getString( item, temp_obj_termval);
									++ii;
									if (ii == len)
									{
										result.pushTerm( termtype, termval);
										if (varname)
										{
											result.attachVariable( varname);
											varname = 0;
										}
										funcname = 0;
										break;
									}
									else
									{
										throw strus::runtime_error( _TXT("term definition has too many arguments"));
									}
								}
								catch (const std::exception& err)
								{
									throw strus::runtime_error( _TXT("error fetching term definition: %s"), err.what());
								}
							}
						}
						initQueryExpression( result, item);
						++nof_arguments;
						break;
				}
			}
			if (funcname)
			{
				result.pushExpression( funcname, nof_arguments, range, cardinality);
				if (varname)
				{
					result.attachVariable( varname);
				}
			}
			else if (varname)
			{
				throw strus::runtime_error( _TXT("single variable reference without expression or term definition is ignored"));
			}
		}
		else
		{
			throw strus::runtime_error( _TXT("query evaluation function object definition expected a sequence of pairs or as dictionary"));
		}
	}
	else
	{
		throw strus::runtime_error( _TXT("query expression object definition expected as sequence of tuples"));
	}
}

void initString( std::string& result, PyObject* obj)
{
	if (PyString_Check( obj))
	{
		char* item = PyString_AS_STRING( obj);
		result = item;
	}
	else if (PyUnicode_Check( obj))
	{
		PyObjectReference temp_obj( PyUnicode_AsUTF8String( obj));
		if (!temp_obj) throw strus::runtime_error( _TXT("failed to convert python unicode string to UTF-8"));
		initString( result, temp_obj.ptr());
	}
	else
	{
		throw strus::runtime_error( _TXT("string expected as argument"));
	}
}

void initStringVector( std::vector<std::string>& result, PyObject* obj)
{
	if (PyString_Check( obj))
	{
		char* item = PyString_AS_STRING( obj);
		result.push_back( item);
	}
	else if (PyUnicode_Check( obj))
	{
		PyObjectReference temp_obj( PyUnicode_AsUTF8String( obj));
		if (!temp_obj) throw strus::runtime_error( _TXT("failed to convert python unicode string to UTF-8"));
		initStringVector( result, temp_obj);
	}
	else if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("string list expected as sequence")));
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			result.reserve( result.size() + len); //... avoid bad_alloc exceptions in following push_back's
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
				/* DON'T DECREF item here */
				try
				{
					if (PyString_Check( item))
					{
						char* itemval = PyString_AS_STRING( item);
						result.push_back( itemval);
					}
					else if (PyUnicode_Check( item))
					{
						PyObjectReference temp_obj( PyUnicode_AsUTF8String( item));
						if (!temp_obj) throw strus::runtime_error( _TXT("failed to convert python unicode string to UTF-8"));

						char* itemval = PyString_AS_STRING( temp_obj.ptr());
						result.push_back( itemval); //... does never throw because we have reserved the memory before this loop
					}
					else
					{
						throw strus::runtime_error( _TXT("string expected as element of string list"));
					}
				}
				catch (const std::exception& err)
				{
					throw strus::runtime_error( _TXT("error initializing string vector: %s"), err.what());
				}
			}
		}
		else
		{
			throw strus::runtime_error( _TXT("list of strings expected"));
		}
	}
	else
	{
		throw strus::runtime_error( _TXT("list of strings or single string expected (check)"));
	}
}

void initIntVector( std::vector<int>& result, PyObject* obj)
{
	if (PyInt_Check( obj))
	{
		int item = PyInt_AS_LONG( obj);
		result.push_back( item);
	}
	else if (PySequence_Check( obj))
	{
		PyObjectReference seq( PySequence_Fast( obj, _TXT("integer list expected as sequence")));
		if (seq)
		{
			Py_ssize_t ii=0,len = PySequence_Size( seq);
			for (; ii < len; ii++)
			{
				PyObject *item = PySequence_Fast_GET_ITEM( seq.ptr(), ii);
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
						throw strus::runtime_error( _TXT("integer expected as element of integer list"));
					}
				}
				catch (const std::exception& err)
				{
					throw strus::runtime_error( _TXT("failed to build vector of integers: %s"), err.what());
				}
			}
		}
		else
		{
			throw strus::runtime_error( _TXT("list of integers expected"));
		}
	}
	else
	{
		throw strus::runtime_error( _TXT("list of integers or single integer expected (check)"));
	}
}

class TermStatisticsBuilder
{
public:
	explicit TermStatisticsBuilder( TermStatistics* result_)
		:m_result(result_){}

	void set( const char* name, GlobalCounter value)
	{
		if (caseInsensitiveEquals( name, "df"))
		{
			m_result->set_df( value);
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown term statistics identifier: %s (only 'df' known)"), name);
		}
	}

private:
	TermStatistics* m_result;
};

class GlobalStatisticsBuilder
{
public:
	explicit GlobalStatisticsBuilder( GlobalStatistics* result_)
		:m_result(result_){}

	void set( const char* name, GlobalCounter value)
	{
		if (caseInsensitiveEquals( name, "nofdocs"))
		{
			m_result->set_nofdocs( value);
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown global statistics identifier: %s (only 'nofdocs' known)"), name);
		}
	}

private:
	GlobalStatistics* m_result;
};

template <class Builder, class Result>
void initStatistics( Result& result, PyObject* obj)
{
	Builder builder( &result);
	if (PyDict_Check( obj))
	{
		PyObjectReference key_obj;
		PyObject *keyitem, *valueitem;
		Py_ssize_t pos = 0;
		
		while (PyDict_Next( obj, &pos, &keyitem, &valueitem))
		{
			const char* key = 0;
			GlobalCounter value;
			if (PyString_Check( keyitem))
			{
				key = PyString_AS_STRING( keyitem);
			}
			else if (PyUnicode_Check( keyitem))
			{
				key_obj = PyUnicode_AsUTF8String( keyitem);
				if (!key_obj) throw strus::runtime_error( _TXT( "statistics element with non string key"));
				key = PyString_AS_STRING( key_obj.ptr());
			}
			else
			{
				throw strus::runtime_error( _TXT("illegal key for statistics structure"), key);
			}
			if (PyLong_Check( valueitem))
			{
				value = PyInt_AS_LONG( valueitem);
			}
			else if (PyInt_Check( valueitem))
			{
				value = PyInt_AS_LONG( valueitem);
			}
			else
			{
				throw strus::runtime_error( _TXT("expected int or long for statistics value of %s"), key);
			}
			builder.set( key, value);
		}
	}
	else
	{
		throw strus::runtime_error( _TXT("expected dictionary for statistics"));
	}
}

void initTermStatistics( TermStatistics& result, PyObject* obj)
{
	initStatistics<TermStatisticsBuilder,TermStatistics>( result, obj);
}

void initGlobalStatistics( GlobalStatistics& result, PyObject* obj)
{
	initStatistics<GlobalStatisticsBuilder,GlobalStatistics>( result, obj);
}

void initDataBlob( std::string& result, PyObject* obj)
{
	if (PyString_Check( obj))
	{
		Py_ssize_t size = PyString_GET_SIZE( obj);
		char* ptr = PyString_AS_STRING( obj);
		result.clear();
		result.append( ptr, size);
	}
	else if (PyBytes_Check( obj))
	{
		Py_ssize_t size = PyBytes_GET_SIZE(obj);
		char* ptr = PyBytes_AS_STRING( obj);
		result.clear();
		result.append( ptr, size);
	}
	else if (PyByteArray_Check( obj))
	{
		Py_ssize_t size = PyByteArray_GET_SIZE( obj);
		char* ptr = PyByteArray_AS_STRING( obj);
		result.clear();
		result.append( ptr, size);
	}
	else
	{
		throw strus::runtime_error( _TXT("expected byte array or bytes as DataBlob"));
	}
}

