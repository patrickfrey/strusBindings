%ignore Reference;
%ignore Variant::defined() const;
%ignore Variant::type() const;
%ignore Variant::getUInt() const;
%ignore Variant::getInt() const;
%ignore Variant::getFloat() const;
%ignore Variant::getText() const;
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
%ignore Reference::get() const;
%ignore Reference::get();
%ignore MetaData::name() const;
%ignore MetaData::value() const;
%ignore Attribute::name() const;
%ignore Attribute::value() const;
%ignore FunctionDef::name() const;
%ignore FunctionDef::arguments() const;
%warnfilter(451) VariantValue::TEXT;
