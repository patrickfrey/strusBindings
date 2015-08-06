namespace std {
	%template(RankVector) vector<Rank>;
}

%typemap(out) std::vector<Rank>
{
	array_init( return_value);
	std::vector<Rank>::const_iterator ri = $input.begin(), re = $input.end();
	for (; ri != re; ++ri)
	{
		zval* rank;
		MAKE_STD_ZVAL( rank);
		object_init( rank);
		add_property_long( rank, "docno", ri->docno());
		add_property_double( rank, "weight", ri->weight());
		std::vector<RankAttribute>::const_iterator
			ai = ri->attributes().begin(), ae = ri->attributes().end();
		std::multimap<std::string,const char*> attr;
		for (; ai != ae; ++ai)
		{
			attr.insert( std::pair<std::string,const char*>( ai->name(), ai->value().c_str()));
		}
		std::multimap<std::string,const char*>::const_iterator
			mi = attr.begin(), me = attr.end();
		for (; mi != me; ++mi)
		{
			std::multimap<std::string,const char*>::const_iterator next_mi = mi;
			++next_mi;
			if (next_mi == me || next_mi->first != mi->first)
			{
				// ... single attribute
				add_property_string( rank, const_cast<char*>(mi->first.c_str()), mi->second, 1);
			}
			else
			{
				// ... multi element attribute
				zval* multiattr;
				MAKE_STD_ZVAL( multiattr);
				array_init( multiattr);
				while (next_mi != me && next_mi->first == mi->first)
				{
					add_next_index_string( multiattr, mi->second, 1);
					++next_mi;
					++mi;
				}
				add_next_index_string( multiattr, mi->second, 1);
				add_property_zval( rank, mi->first.c_str(), multiattr);
			}
		}
		add_next_index_zval( return_value, rank);
	}
}
