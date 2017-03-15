#include "variantValueTemplate.hpp"
#include "strus/bindings/valueVariant.hpp"

using namespace strus;

template <typename TYPE>
static VariantValue getValue( const TYPE& val)
{
	return bindings::VariantValueTemplate<TYPE>::get( val);
}

void bla()
{
	strus::Index val_index;
	ValueVariant v1( getValue( val_index));
}
