
/// \brief Patch according to https://tools.ietf.org/html/rfc6902
/// \note See also https://williamdurand.fr/2014/02/14/please-do-not-patch-like-an-idiot
/** \example for storageClient::patch
{
"patch": [
    {"op":"rename", "path":"metadata/code", "from":"cd"},
    {"op":"replace", "path":"metadata/id", "value":"INT32", "from":"id"},
    {"op":"add", "path":"metadata/doclen", "value":"INT32"},
    {"op":"replace", "path":"buffer_size", "value":"4K"}
]}
**/
struct PatchDef
{
	enum Op
	{
		OpAdd,
		OpRemove,
		OpReplace,
		OpMove,
		OpCopy,
		OpTest
	};
	Op op;
	std::vector<std::string> path;
	std::string value;
	std::vector<std::string> from;

	static Op getOpFromString( const char* val);
	static const char* opName( Op op)
	{
		static const char* ar[] = {"add","remove","replace","move","copy","test"};
		return ar[ (int)op];
	}
	static const StructureNameMap& opNameMap()
	{
		static const StructureNameMap namemap( "add,remove,replace,move,copy,test", ',');
		return namemap;
	}
	static std::vector<PatchDef> parseList( papuga_SerializationIter& seriter);
	static std::vector<PatchDef> parseList( const papuga_ValueVariant& value);

	std::string pathAsString() const;
	std::string fromAsString() const;

	PatchDef()
		:op(OpAdd),path(),value(),from(){}
	PatchDef( papuga_SerializationIter& seriter);
	PatchDef( const PatchDef& o)
		:op(o.op),path(o.path),value(o.value),from(o.from){}
};


