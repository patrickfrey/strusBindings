/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STATISTICS_HPP_INCLUDED
#include "papuga/hostObjectReference.h"
#include "papuga/valueVariant.h"
#include "strus/numericVariant.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "impl/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
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
	StatisticsIteratorImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_, const HostObjectReference& iter_);

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_iter_impl;
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
	Struct* decode( const std::string& blob) const;

	/// \brief Create binary blob from statistics message
	/// \param[in] msg statistics message structure
	/// \return the statistics message blob
	std::string* encode( const ValueVariant& msg) const;

private:
	friend class ContextImpl;
	StatisticsProcessorImpl( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const std::string& name_, const HostObjectReference& errorhnd_);

private:
	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	const StatisticsProcessorInterface* m_statsproc;
};

}}//namespace
#endif

