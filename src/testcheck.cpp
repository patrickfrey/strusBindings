#include "filter/variantValueTemplate.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/index.hpp"

using namespace strus;

void bla()
{
	strus::Index val_index;
	bindings::ValueVariant vv;
	filter::VariantValueTemplate<int>::init( vv, val_index);
}
