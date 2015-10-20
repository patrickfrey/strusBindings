
template<class T>
struct VecToList
{
	static PyObject* convert( const std::vector<T>& ar)
	{
		boost::python::list lst;
		typename std::vector<T>::const_iterator ai = ar.begin(), ae = ar.end();
		for (; ai != ae; ++ai)
		{
			lst.append( *ai);
		}
		PyObject* rt = lst.ptr();
		Py_INCREF( rt);
		return rt;
	}
};
bp::to_python_converter<std::vector<Term>, VecToList<Term> >();
bp::to_python_converter<std::vector<Rank>, VecToList<Rank> >();
bp::to_python_converter<std::vector<RankAttribute>, VecToList<RankAttribute> >();
bp::to_python_converter<std::vector<std::string>, VecToList<std::string> >();
bp::to_python_converter<std::vector<Attribute>, VecToList<Attribute> >();
bp::to_python_converter<std::vector<MetaData>, VecToList<MetaData> >();
