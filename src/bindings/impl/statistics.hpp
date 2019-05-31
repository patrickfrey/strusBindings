/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/index.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class StatisticsProcessorInterface;

namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \class StatisticsMapImpl
/// \brief Map for global statistics for a distributed index
/// \remark The only way to construct a content statistics object is to call Context::createStatisticsMap()
class StatisticsMapImpl
{
public:
	/// \brief Destructor
	virtual ~StatisticsMapImpl(){}

	/// \brief Propagate a change of the number of document inserted
	/// \param[in] increment positive or negative (decrement) value of the local change of the collection size
	void addNofDocumentsInsertedChange( int increment);

	/// \brief Propagate a change in the df (document frequency)
	/// \param[in] type type of the term
	/// \param[in] term value of the term
	/// \param[in] increment positive or negative (decrement) value of the local change of the document frequency
	void addDfChange( const std::string& type, const std::string& term, int increment);

	/// \brief Propagate a blob with some statistics to encode with a viewer associated with this statistics map
	/// \param[in] blob buffer containing the message blob or a statistics message structure with timestamp
	void processStatisticsMessage( const ValueVariant& blob);

	/// \brief Get the total number of documents stored in the map
	/// \return the number of documents in the collection
	GlobalCounter nofDocuments();

	/// \brief Get the df (document frequency) stored in the map
	/// \param[in] type type of the term
	/// \param[in] value value of the term
	/// \return the document frequency
	GlobalCounter df( const std::string& type, const std::string& value);

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	StatisticsMapImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const std::string& statsprocname);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_statmap_impl;
	const StatisticsProcessorInterface* m_statsproc;
};

}}//namespace
#endif




