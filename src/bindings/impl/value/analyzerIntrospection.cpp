/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of analyzers
#include "analyzerIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/constants.hpp"
#include "strus/base/stdint.h"
#include "strus/base/numstring.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include <cstring>
#include <vector>
#include <set>
#include <utility>
#include <limits>

using namespace strus;
using namespace strus::bindings;

template <class IntrospectionType, typename ObjectType>
struct ViewIntrospectionConstructor
{
	static IntrospectionBase* func( ErrorBufferInterface* errorhnd, const ObjectType& obj)
	{
		return new IntrospectionType( errorhnd, obj);
	}
};

class FunctionViewIntrospection
	:public IntrospectionBase
{
public:
	FunctionViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::FunctionView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~FunctionViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "name")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.name());
		}
		else if (name == "arg" && !m_view.parameter().empty())
		{
			return new IntrospectionKeyValueList<ParameterList>( m_errorhnd, m_view.parameter());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"name","arg",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	typedef std::vector<analyzer::FunctionView::NamedParameter> ParameterList;
	const analyzer::FunctionView m_view;
};

class DocumentClassIntrospection
	:public IntrospectionBase
{
public:
	DocumentClassIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::DocumentClass& doctype_)
		:m_errorhnd(errorhnd_)
		,m_doctype(doctype_)
		{}
	virtual ~DocumentClassIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "encoding")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_doctype.encoding());
		}
		else if (name == "mimetype")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_doctype.mimeType());
		}
		else if (name == "schema")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_doctype.schema());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"encoding","mimetype","schema",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const analyzer::DocumentClass m_doctype;
};

class FeatureViewIntrospection
	:public IntrospectionBase
{
public:
	FeatureViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::FeatureView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~FeatureViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "type")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.type());
		}
		else if (name == "select")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.selectexpr());
		}
		else if (name == "tokenizer")
		{
			return new FunctionViewIntrospection( m_errorhnd, m_view.tokenizer());
		}
		else if (name == "normalizer")
		{
			IntrospectionObjectList<std::vector<analyzer::FunctionView> >::ElementConstructor elementConstructor
				= &ViewIntrospectionConstructor<FunctionViewIntrospection,analyzer::FunctionView>::func;
			return new IntrospectionObjectList<std::vector<analyzer::FunctionView> >( m_errorhnd, m_view.normalizer(), elementConstructor);
		}
		else if (name == "posbind")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.posbindOption());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"type","select","tokenizer","normalizer","posbind",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::FeatureView m_view;
};

class AggregatorViewIntrospection
	:public IntrospectionBase
{
public:
	AggregatorViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::AggregatorView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~AggregatorViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "type")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.type());
		}
		else if (name == "function")
		{
			return new FunctionViewIntrospection( m_errorhnd, m_view.function());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"type","function",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::AggregatorView m_view;
};

class SubDocumentDefinitionViewIntrospection
	:public IntrospectionBase
{
public:
	SubDocumentDefinitionViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::SubDocumentDefinitionView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~SubDocumentDefinitionViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "name")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.subDocumentTypeName());
		}
		else if (name == "selection")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.selection());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"name","selection",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::SubDocumentDefinitionView m_view;
};


class SubContentDefinitionViewIntrospection
	:public IntrospectionBase
{
public:
	SubContentDefinitionViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::SubContentDefinitionView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~SubContentDefinitionViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "class")
		{
			return new DocumentClassIntrospection( m_errorhnd, m_view.documentClass());
		}
		else if (name == "selection")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.selection());
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"class","selection",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::SubContentDefinitionView m_view;
};


void DocumentAnalyzerIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}

static IntrospectionBase* createFeatureViewIntrospectionList( ErrorBufferInterface* errorhnd_, const std::vector<analyzer::FeatureView>& view_)
{
	typedef std::vector<analyzer::FeatureView> FeatureViewList;
	IntrospectionObjectList<FeatureViewList>::ElementConstructor elementConstructor
		= &ViewIntrospectionConstructor<FeatureViewIntrospection,analyzer::FeatureView>::func;
	return new IntrospectionObjectList<FeatureViewList>( errorhnd_, view_, elementConstructor);
}

static IntrospectionBase* createAggregatorViewIntrospectionList( ErrorBufferInterface* errorhnd_, const std::vector<analyzer::AggregatorView>& view_)
{
	typedef std::vector<analyzer::AggregatorView> AggregatorViewList;
	static IntrospectionObjectList<AggregatorViewList>::ElementConstructor elementConstructor
		= &ViewIntrospectionConstructor<AggregatorViewIntrospection,analyzer::AggregatorView>::func;
	return new IntrospectionObjectList<AggregatorViewList>( errorhnd_, view_, elementConstructor);
}

static IntrospectionBase* createSubDocumentDefinitionViewIntrospectionList( ErrorBufferInterface* errorhnd_, const std::vector<analyzer::SubDocumentDefinitionView>& view_)
{
	typedef std::vector<analyzer::SubDocumentDefinitionView> SubDocumentDefinitionViewList;
	static IntrospectionObjectList<SubDocumentDefinitionViewList>::ElementConstructor elementConstructor
		= &ViewIntrospectionConstructor<SubDocumentDefinitionViewIntrospection,analyzer::SubDocumentDefinitionView>::func;
	return new IntrospectionObjectList<SubDocumentDefinitionViewList>( errorhnd_, view_, elementConstructor);
}

static IntrospectionBase* createSubContentDefinitionViewIntrospectionList( ErrorBufferInterface* errorhnd_, const std::vector<analyzer::SubContentDefinitionView>& view_)
{
	typedef std::vector<analyzer::SubContentDefinitionView> SubContentDefinitionViewList;
	static IntrospectionObjectList<SubContentDefinitionViewList>::ElementConstructor elementConstructor
		= &ViewIntrospectionConstructor<SubContentDefinitionViewIntrospection,analyzer::SubContentDefinitionView>::func;
	return new IntrospectionObjectList<SubContentDefinitionViewList>( errorhnd_, view_, elementConstructor);
}

IntrospectionBase* DocumentAnalyzerIntrospection::open( const std::string& name)
{
	if (name == "attributes") return createFeatureViewIntrospectionList( m_errorhnd, m_view.attributes());
	else if (name == "metadata") return createFeatureViewIntrospectionList( m_errorhnd, m_view.metadata());
	else if (name == "searchindex") return createFeatureViewIntrospectionList( m_errorhnd, m_view.searchindex());
	else if (name == "forwardindex") return createFeatureViewIntrospectionList( m_errorhnd, m_view.forwardindex());
	else if (name == "aggregator") return createAggregatorViewIntrospectionList( m_errorhnd, m_view.aggregators());
	else if (name == "subdoc") return createSubDocumentDefinitionViewIntrospectionList( m_errorhnd, m_view.subdocuments());
	else if (name == "subcontent") return createSubContentDefinitionViewIntrospectionList( m_errorhnd, m_view.subcontents());
	else if (name == "patternmatcher") return NULL;
	else if (name == "patternlexer") return NULL;
	else if (name == "segmenter") return new FunctionViewIntrospection( m_errorhnd, m_view.segmenter());
	return NULL;
}

std::vector<IntrospectionLink> DocumentAnalyzerIntrospection::list()
{
	static const char* ar[] = {"segmenter","attributes","metadata","searchindex","forwardindex","aggregator","subdoc","subcontent","patternmatcher","patternlexer",NULL};
	return getList( ar);
}

class QueryElementViewIntrospection
	:public IntrospectionBase
{
public:
	QueryElementViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::QueryElementView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~QueryElementViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "type")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.type());
		}
		else if (name == "field")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.field());
		}
		else if (name == "tokenizer")
		{
			return new FunctionViewIntrospection( m_errorhnd, m_view.tokenizer());
		}
		else if (name == "normalizer")
		{
			IntrospectionObjectList<std::vector<analyzer::FunctionView> >::ElementConstructor elementConstructor
				= &ViewIntrospectionConstructor<FunctionViewIntrospection,analyzer::FunctionView>::func;
			return new IntrospectionObjectList<std::vector<analyzer::FunctionView> >( m_errorhnd, m_view.normalizer(), elementConstructor);
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"type","field","tokenizer","normalizer",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::QueryElementView m_view;
};

class ContentStatisticsElementViewIntrospection
	:public IntrospectionBase
{
public:
	ContentStatisticsElementViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const analyzer::ContentStatisticsElementView& view_)
		:m_errorhnd(errorhnd_)
		,m_view(view_)
		{}
	virtual ~ContentStatisticsElementViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "type")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.type());
		}
		else if (name == "regex")
		{
			return new IntrospectionAtomic<std::string>( m_errorhnd, m_view.regex());
		}
		else if (name == "priority")
		{
			return new IntrospectionAtomic<int>( m_errorhnd, m_view.priority());
		}
		else if (name == "minlen")
		{
			return new IntrospectionAtomic<int>( m_errorhnd, m_view.minLen());
		}
		else if (name == "maxlen")
		{
			return new IntrospectionAtomic<int>( m_errorhnd, m_view.maxLen());
		}
		else if (name == "tokenizer")
		{
			return new FunctionViewIntrospection( m_errorhnd, m_view.tokenizer());
		}
		else if (name == "normalizer")
		{
			IntrospectionObjectList<std::vector<analyzer::FunctionView> >::ElementConstructor elementConstructor
				= &ViewIntrospectionConstructor<FunctionViewIntrospection,analyzer::FunctionView>::func;
			return new IntrospectionObjectList<std::vector<analyzer::FunctionView> >( m_errorhnd, m_view.normalizer(), elementConstructor);
		}
		else
		{
			return NULL;
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"type","regex","priority","minlen","maxlen","tokenizer","normalizer",NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	analyzer::ContentStatisticsElementView m_view;
};

void QueryAnalyzerIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}

IntrospectionBase* QueryAnalyzerIntrospection::open( const std::string& name)
{
	if (name == "elements")
	{
		IntrospectionObjectList<std::vector<analyzer::QueryElementView> >::ElementConstructor elementConstructor
			= &ViewIntrospectionConstructor<QueryElementViewIntrospection,analyzer::QueryElementView>::func;
		return new IntrospectionObjectList<std::vector<analyzer::QueryElementView> >( m_errorhnd, m_view.elements(), elementConstructor);
	}
	else if (name == "lexems")
	{
		IntrospectionObjectList<std::vector<analyzer::QueryElementView> >::ElementConstructor elementConstructor
			= &ViewIntrospectionConstructor<QueryElementViewIntrospection,analyzer::QueryElementView>::func;
		return new IntrospectionObjectList<std::vector<analyzer::QueryElementView> >( m_errorhnd, m_view.patternLexems(), elementConstructor);
	}
	return NULL;
}

std::vector<IntrospectionLink> QueryAnalyzerIntrospection::list()
{
	static const char* ar[] = {"elements","lexems","priorities",NULL};
	return getList( ar);
}


void ContentStatisticsIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}

IntrospectionBase* ContentStatisticsIntrospection::open( const std::string& name)
{
	if (name == "elements")
	{
		IntrospectionObjectList<std::vector<analyzer::ContentStatisticsElementView> >::ElementConstructor elementConstructor
			= &ViewIntrospectionConstructor<ContentStatisticsElementViewIntrospection,analyzer::ContentStatisticsElementView>::func;
		return new IntrospectionObjectList<std::vector<analyzer::ContentStatisticsElementView> >( m_errorhnd, m_view.elements(), elementConstructor);
	}
	else if (name == "attributes")
	{
		return new IntrospectionValueList<std::vector<std::string> >( m_errorhnd, m_view.attributes());
	}
	else if (name == "expressions")
	{
		return new IntrospectionValueList<std::vector<std::string> >( m_errorhnd, m_view.expressions());
	}
	return NULL;
}

std::vector<IntrospectionLink> ContentStatisticsIntrospection::list()
{
	static const char* ar[] = {"elements","attributes","expressions",NULL};
	return getList( ar);
}

