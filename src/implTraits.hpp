/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_IMPL_TRAITS_HPP_INCLUDED
#define _STRUS_BINDINGS_IMPL_TRAITS_HPP_INCLUDED
/// \file implTraits.hpp
#include "strus/bindingClassId.hpp"
#include "papuga/valueVariant.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "serializer.hpp"
#include "internationalization.hpp"

namespace strus {
namespace bindings {

struct ClassIdMap
{
	static int get( const ContextImpl& )			{return ClassContext;}
	static int get( const DocumentAnalyzerImpl& )		{return ClassDocumentAnalyzer;}
	static int get( const QueryAnalyzerImpl&)		{return ClassQueryAnalyzer;}
	static int get( const QueryEvalImpl&)			{return ClassQueryEval;}
	static int get( const QueryImpl&)			{return ClassQuery;}
	static int get( const StorageClientImpl&)		{return ClassStorageClient;}
	static int get( const StorageTransactionImpl&)		{return ClassStorageTransaction;}
	static int get( const StatisticsProcessorImpl&)		{return ClassStatisticsProcessor;}
	static int get( const StatisticsIteratorImpl&)		{return ClassStatisticsIterator;}
	static int get( const DocumentBrowserImpl&)		{return ClassDocumentBrowser;}
	static int get( const VectorStorageSearcherImpl&)	{return ClassVectorStorageSearcher;}
	static int get( const VectorStorageClientImpl&)		{return ClassVectorStorageClient;}
	static int get( const VectorStorageTransactionImpl&)	{return ClassVectorStorageTransaction;}
};

template <class IMPL>
struct ImplTraits {
	static int classid()		{throw strus::runtime_error(_TXT("intenal: unknown class"));}
	static const char* name()	{throw strus::runtime_error(_TXT("intenal: unknown class"));}
};

template <> struct ImplTraits<ContextImpl>
{
	static int classid()		{return ClassContext;}
	static const char* name()	{return "Context";}
};
template <> struct ImplTraits<DocumentAnalyzerImpl>
{
	static int classid()		{return ClassDocumentAnalyzer;}
	static const char* name()	{return "DocumentAnalyzer";}
};
template <> struct ImplTraits<QueryAnalyzerImpl>
{
	static int classid()		{return ClassQueryAnalyzer;}
	static const char* name()	{return "QueryAnalyzer";}
};
template <> struct ImplTraits<QueryEvalImpl>
{
	static int classid()		{return ClassQueryEval;}
	static const char* name()	{return "QueryEval";}
};
template <> struct ImplTraits<QueryImpl>
{
	static int classid()		{return ClassQuery;}
	static const char* name()	{return "Query";}
};
template <> struct ImplTraits<StorageClientImpl>
{
	static int classid()		{return ClassStorageClient;}
	static const char* name()	{return "StorageClient";}
};
template <> struct ImplTraits<StorageTransactionImpl>
{
	static int classid()		{return ClassStorageTransaction;}
	static const char* name()	{return "StorageTransaction";}
};
template <> struct ImplTraits<StatisticsProcessorImpl>
{
	static int classid()		{return ClassStatisticsProcessor;}
	static const char* name()	{return "StatisticsProcessor";}
};
template <> struct ImplTraits<StatisticsIteratorImpl>
{
	static int classid()		{return ClassStatisticsIterator;}
	static const char* name()	{return "StatisticsIterator";}
};
template <> struct ImplTraits<DocumentBrowserImpl>
{
	static int classid()		{return ClassDocumentBrowser;}
	static const char* name()	{return "DocumentBrowser";}
};
template <> struct ImplTraits<VectorStorageSearcherImpl>
{
	static int classid()		{return ClassVectorStorageSearcher;}
	static const char* name()	{return "VectorStorageSearcher";}
};
template <> struct ImplTraits<VectorStorageClientImpl>
{
	static int classid()		{return ClassVectorStorageClient;}
	static const char* name()	{return "VectorStorageClient";}
};
template <> struct ImplTraits<VectorStorageTransactionImpl>
{
	static int classid()		{return ClassVectorStorageTransaction;}
	static const char* name()	{return "VectorStorageTransaction";}
};

template <class ClassImpl>
static ClassImpl* implObjectCast( const papuga::ValueVariant& val)
{
	if (val.type != papuga::ValueVariant::HostObject || val.classid() != ImplTraits<ClassImpl>::classid())
	{
		throw strus::runtime_error(_TXT("expected class '%s'"), ImplTraits<ClassImpl>::name());
	}
	return (ClassImpl*)const_cast<void*>( val.value.hostObject);
}

}}// namespace
#endif


