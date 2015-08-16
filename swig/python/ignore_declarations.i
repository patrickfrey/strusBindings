%ignore Reference;
#if defined(SWIGOCAML)
%ignore Variant::Variant( unsigned int v);
%ignore Variant::Variant( double v);
%ignore Variant::assign( unsigned int v);
%ignore Variant::assign( double v);
#endif
%ignore Variant::m_value;
%ignore Reference::Reference( Reference::Deleter deleter_);
%ignore Reference::Reference( const Reference& o);
%ignore Reference::~Reference();
%ignore Reference::reset( void* obj_=0);
%ignore Reference::operator=(const Reference& o);
%ignore Reference::get() const;
%ignore Reference::get();
%warnfilter(451) VariantValue::TEXT;
