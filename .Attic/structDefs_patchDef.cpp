
PatchDef::Op PatchDef::getOpFromString( const char* val)
{
	int idx = opNameMap().index(val);
	if (idx == StructureNameMap::Undefined) throw strus::runtime_error(_TXT("unknown configuration patch operator '%s' (case sensitive)"), val);
	return (PatchDef::Op)idx;
}

static PatchDef::Op getPatchDefOp( papuga_SerializationIter& seriter)
{
	char buf[ 32];
	const char* patchDefOpStr = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);
	if (!patchDefOpStr)
	{
		buf[ sizeof(buf)-1] = 0;
		throw strus::runtime_error(_TXT("unknown configuration patch operator '%s'"), buf);
	}
	PatchDef::Op rt = PatchDef::getOpFromString( patchDefOpStr);
	papuga_SerializationIter_skip( &seriter);
	return rt;
}

static std::vector<std::string> getJsonPathAsArray( const std::string& pathstr)
{
	std::vector<std::string> rt;
	if (pathstr.empty()) return std::vector<std::string>();

	char const* si = pathstr.c_str();
	char const* sn = std::strchr( pathstr.c_str(), '/');
	for (; sn; si=sn+1,sn=std::strchr( pathstr.c_str(), '/'))
	{
		if (sn != si) rt.push_back( std::string( si, sn-si));
	}
	if (*si) rt.push_back( si);
	return rt;
}

PatchDef::PatchDef( papuga_SerializationIter& seriter)
		:op(OpAdd),path(),value(),from()
{
	static const char* context = _TXT("patch definition");
	static const StructureNameMap namemap( "op,path,value,from", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		op = getPatchDefOp( seriter);
		path = getJsonPathAsArray( Deserializer::getString( seriter));
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			value = Deserializer::getString( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			from = getJsonPathAsArray( Deserializer::getString( seriter));
		}
	}
	else
	{
		unsigned char defined[4] = {0,0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "op", context);
					op = getPatchDefOp( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "path", context);
					path = getJsonPathAsArray( Deserializer::getString( seriter));
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
					value = Deserializer::getString( seriter);
					break;
				case 3:	if (defined[3]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "from", context);
					from = getJsonPathAsArray( Deserializer::getString( seriter));
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'op' or 'name' or 'value' expected"), context);
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "op", context);
		if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "path", context);
	}
}

std::vector<PatchDef> PatchDef::parseList( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("patch definitions");
	std::vector<PatchDef> rt;
	while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		rt.push_back( PatchDef( seriter));
		Deserializer::consumeClose( seriter);
	}
	if (!papuga_SerializationIter_eof( &seriter))
	{
		throw strus::runtime_error( _TXT("list of structures expected as %s"), context);
	}
	return rt;
}

std::vector<PatchDef> PatchDef::parseList( const papuga_ValueVariant& value)
{
	static const char* context = _TXT("patch definitions");
	std::vector<PatchDef> rt;
	if (!papuga_ValueVariant_defined( &value)) return rt;
	if (value.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error( _TXT("type serialization expected as %s"), context);
	}
	papuga_SerializationIter seriter;
	papuga_init_SerializationIter( &seriter, value.value.serialization);
	return parseList( seriter);
}

static std::string joinPath( const std::vector<std::string>& pt)
{
	std::string rt;
	std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		if (!rt.empty()) rt.push_back('/');
		rt.append( *pi);
	}
	return rt;
}

std::string PatchDef::pathAsString() const
{
	return joinPath( path);
}

std::string PatchDef::fromAsString() const
{
	return joinPath( from);
}

