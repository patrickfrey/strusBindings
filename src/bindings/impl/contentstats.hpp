/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_CONTENTSTATS_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_CONTENTSTATS_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/textProcessorInterface.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/analyzer/contentStatisticsResult.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/iterator.hpp"
#include "impl/value/struct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class ContentStatisticsCollectorImpl;

/// \class ContentStatisticsImpl
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
/// \remark The only way to construct a content statistics object is to call Context::createContentStatistics()
class ContentStatisticsImpl
{
public:
	/// \brief Destructor
	virtual ~ContentStatisticsImpl(){}

	/// \brief Define an element of the content statistics library
	/// \example addSearchIndexFeature( "word", "/doc/elem", "word", ["lc",["stem", "en"]])
	/// \param[in] type type of the features produced (your choice)
	/// \example "word"
	/// \example "stem"
	/// \param[in] regex expression preselecting the candidates to check
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] priority non negative number specifying the priority given to matches, for multiple matches only the ones with the highest priority are selected
	/// \example 1
	/// \example "1"
	/// \param[in] minLength minimum number of tokens or -1 for no restriction
	/// \example 0
	/// \example 2
	/// \example -1
	/// \example "1"
	/// \param[in] maxLength maximum number of tokens or -1 for no restriction
	/// \example 2
	/// \example -1
	/// \example "1"
	/// \param[in] tokenizer tokenizer function description to use for this element
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for this element in ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void addLibraryElement(
		const std::string& type,
		const std::string& regex,
		const ValueVariant& priority,
		const ValueVariant& minLength,
		const ValueVariant& maxLength,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Create a collector for content statistics analysis
	/// \return the collector object (class StorageTransaction) created
	ContentStatisticsCollectorImpl* createCollector() const;

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	ContentStatisticsImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const TextProcessorInterface* textproc_);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_contentstats_impl;
	const TextProcessorInterface* m_textproc;
};

/// \class ContentStatisticsCollectorImpl
/// \brief Object representing a collector of document content statistics
/// \note The only way to construct a collector of document content statistics is to call ContentStatistics::createCollector()
class ContentStatisticsCollectorImpl
{
public:
	/// \brief Destructor
	virtual ~ContentStatisticsCollectorImpl(){}

	/// \brief Put content to collect statistics
	/// \param[in] docid document identifier as appearing in the result
	/// \param[in] content document content
	/// \param[in] doctype document class (schema ignored) if defined, otherwise guessed
	virtual void putContent(
			const std::string& docid,
			const std::string& content,
			const ValueVariant& doctype=ValueVariant());

	/// \brief Do return the overall statistics of the sample documents inserted till now
	/// \return the structure with the statistics
	analyzer::ContentStatisticsResult* statistics();

	/// \brief Get the number of sample documents
	/// \return the number of sample documents put (also member of the statistics() result)
	int nofDocuments() const;

private:
	/// \brief Constructor used by Context
	friend class ContentStatisticsImpl;
	ContentStatisticsCollectorImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& contentstat, const ObjectRef& errorhnd, const TextProcessorInterface* textproc_);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_contentstats_impl;
	ObjectRef m_context_impl;
	const TextProcessorInterface* m_textproc;
};

}}//namespace
#endif




