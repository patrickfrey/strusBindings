/*
 * Copyright (c) 2021 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STATISTICS_STORAGE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STATISTICS_STORAGE_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/timeStamp.hpp"
#include "strus/numericVariant.hpp"
#include "strus/constants.hpp"
#include "strus/storage/index.hpp"
#include "strus/storage/statisticsMessage.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/iterator.hpp"
#include "impl/value/struct.hpp"
#include "impl/value/introspectionBase.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \class StatisticsStorageClientImpl
/// \brief Object representing a client connection to a statistics storage
/// \note The only way to construct a storage client instance is to call Context::createStatisticsStorageClient( config)
class StatisticsStorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~StatisticsStorageClientImpl();

	/// \brief Reload the configuration (PUT)
	/// \param[in] config_ new configuration (string or structure with named elements) of the storage client
	void reload( const ValueVariant& config_);

	/// \brief Get the number of documents in the collection
	/// \return the total number of documents
	/// \example 132112324
	/// \example 529873247
	GlobalCounter nofDocuments() const;

	/// \brief Get the number of documents where a specific feature occurrs in
	/// \param[in] type the term type of the feature queried
	/// \example "WORD"
	/// \example "stem"
	/// \example "Name"
	/// \param[in] term the term value of the feature queried
	/// \example "John"
	/// \example "21314"
	/// \example "Z0 ssd-qx"
	/// \return the number of documents where the argument feature occurrs in
	/// \example 12321
	/// \example 98
	GlobalCounter documentFrequency( const std::string& type, const std::string& term) const;

	/// \brief Get the latest timestamp associated with the storage name (putStatisticsMessage)
	/// \param[in] storageid id of the storage
	/// \return timestamp timestamp associated with the storage name
	TimeStamp storageTimeStamp( const std::string& storageid) const;

	/// \brief Store the contents of a statistics message, related to a storage transaction, into the statistics storage
	/// \param[in] msg statistics message
	/// \param[in] storageid id of the storage, these statistics came from. Stores the timestamp of the message associated with the storage name if it is higher than the previous.
	void putStatisticsMessage( const ValueVariant& msg, const std::string& storageid);

	/// \brief Get the configuration of this storage
	/// \return the configuration as structure
	/// \example [ path: "/srv/searchengine/storage" metadata:"doclen UINT32, date UINT32, docweight FLOAT" ]
	Struct config() const;

	/// \brief Get the configuration of this storage as string
	/// \return the configuration as string
	/// \example "path=/srv/searchengine/storage; metadata=doclen UINT32, date UINT32, docweight FLOAT"
	std::string configstring() const;

	/// \brief Close of the storage client
	void close();

	/// \brief Call compaction method of the database if provided
	void compaction();

	/// \brief Introspect the storage structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \example ["doc"]
	/// \example ["attribute"]
	/// \example ["metadata"]
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	StatisticsStorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
};

}}//namespace
#endif




