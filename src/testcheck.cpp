#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serializer.hpp"
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/bindings/callResult.hpp"
#include "strus/index.hpp"
#include <cstring>

using namespace strus;
using namespace strus::bindings;

class TestObject
{
public:
	TestObject(){}
	~TestObject(){}
};

bool test_function( CallResult& res, std::size_t , const ValueVariant* )
{
	bindings::Serializer::serialize_int( res.serialization, 1);

	res.object = HostObjectReference::createOwnership( new TestObject());
	return true;
}

