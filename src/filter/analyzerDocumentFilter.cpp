/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Iterator on the structure of a analyzer document created as result of a document analysis
/// \file analyzerDocumentFilter.cpp
#include "analyzerDocumentFilter.hpp"

const char* AnalyzerDocumentFilter::getCurrentTagName()
{
	static const char* ar[] = {"doc","metadata","attributes","searchIndexTerms","forwardIndexTerms",0};
	return ar[ m_state];
}

ValueVariant getCurrentValue()
{
	
}

using namespace strus;
/// \brief Get the sub document type name
const std::string& subDocumentTypeName() const		{return m_subdoctypename;}
/// \brief Get the list of the attributes defined in this document
const std::vector<Attribute>& attributes() const	{return m_attributes;}
/// \brief Get the list of the metadata defined in this document
const std::vector<MetaData>& metadata() const		{return m_metadata;}
/// \brief Get the list of the search index terms defined in this document
const std::vector<Term>& searchIndexTerms() const	{return m_searchIndexTerms;}
/// \brief Get the list of the forward index terms defined in this document
const std::vector<Term>& forwardIndexTerms() const	{return m_forwardIndexTerms;}

BindingFilterInterface::Tag AnalyzerDocumentFilter::getNext( ValueVariant& val)
{
	switch (m_state)
	{
		case StateSubDocType:
			break;
		case StateMetaData:
			break;
		case StateAttributes:
			break;
		case StateSearchIndexTerms:
			break;
		case StateForwardIndexTerms:
			break;
	}
}

void AnalyzerDocumentFilter::skip()
{
	if (m_state == StateSubDocType)
	{
		m_idx = 0;
		m_state = StateMetaData;
		m_tag = BindingFilterInterface::Open;
	}
	else
	{
		std::size_t arsize = 0;
		switch (m_state)
		{
			case StateSubDocType:
				break;
			case StateMetaData:
				arsize = m_impl->metadata().size();
				break;
			case StateAttributes:
				arsize = m_impl->attributes().size();
				break;
			case StateSearchIndexTerms:
				arsize = m_impl->searchIndexTerms().size();
				break;
			case StateForwardIndexTerms:
				arsize = m_impl->forwardIndexTerms().size();
				break;
			case StateEnd:
				return;
		}
		if (m_tag == BindingFilterInterface::Open)
		{
			m_idx = 0;
			m_state = (State)(m_state+1);
		}
		else
		{
			m_tag = BindingFilterInterface::Open;
			if (m_idx+1 >= arsize)
			{
				m_idx = 0;
				m_state = (State)(m_state+1);
			}
			else
			{
				++m_idx;
			}
		}
	}
}

BindingFilterInterface* AnalyzerDocumentFilter::createCopy( BindingFilterMem* mem) const
{
	return new (mem) AnalyzerDocumentFilter( *this);
}


	


