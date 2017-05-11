/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/numericVariant.hpp"
#include "impl/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
///\brief Forward declaration
class StatisticsViewerInterface;
///\brief Forward declaration
class StatisticsProcessorInterface;

namespace bindings {

///\brief Forward declaration
class Struct;

typedef papuga_ValueVariant ValueVariant;

/// \brief Iterator on messages with storage statistics
class StatisticsIteratorImpl
{
public:
	/// \brief Destructor
	virtual ~StatisticsIteratorImpl(){}

	/// \brief Fetches the next statistics message
	/// \return message blob or empty string if there is no message left
	std::string* getNext();

private:
	friend class StorageClientImpl;
	StatisticsIteratorImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd_, const ObjectRef& storage_, const ObjectRef& iter_);

private:
	ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_iter_impl;
};


/// \brief Translation to update a storage with statistics messages
class StatisticsProcessorImpl
{
public:
	/// \brief Destructor
	virtual ~StatisticsProcessorImpl(){}

	/// \brief Decode a statistics message blob for introspection
	/// \param[in] blob statistics message blob
	/// \return the statistics message structure
	StatisticsViewerInterface* decode( const std::string& blob) const;

	/// \brief Create binary blob from statistics message
	/// \param[in] msg statistics message structure
	/// \return the statistics message blob
	std::string* encode( const ValueVariant& msg) const;

private:
	friend class ContextImpl;
	StatisticsProcessorImpl( const ObjectRef& objbuilder_, const ObjectRef& trace_, const std::string& name_, const ObjectRef& errorhnd_);

private:
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	const StatisticsProcessorInterface* m_statsproc;
};

}}//namespace
#endif

