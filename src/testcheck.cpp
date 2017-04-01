#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serializationTemplates.hpp"
#include "strus/index.hpp"

using namespace strus;

void bla()
{
	bindings::Serialization res;
	bindings::SerializationTemplate<int>::serialize( res, 1);
}

