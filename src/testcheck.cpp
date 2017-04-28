#include "papuga/valueVariant.hpp"
#include "papuga/hostObjectReference.hpp"
#include "papuga/callResult.hpp"
#include "strus/index.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include <cstring>

using namespace strus;
using namespace strus::bindings;

class TestObject
{
public:
	TestObject(){}
	~TestObject(){}
};

bool test_function( papuga::CallResult& res, std::size_t , const papuga::ValueVariant* )
{
	bindings::Serializer::serialize_int( res.serialization, 1);

	res.object = papuga::HostObjectReference::createOwnership( new TestObject());
	return true;
}

