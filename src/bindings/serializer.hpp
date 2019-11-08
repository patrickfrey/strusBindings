/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_SERIALIZER_HPP_INCLUDED
#define _STRUS_BINDINGS_SERIALIZER_HPP_INCLUDED
/// \brief Serializers of all data types needed for the language bindings
/// \file serializer.hpp
#include "papuga/serialization.h"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "strus/numericVariant.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/queryTermExpression.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/analyzer/contentStatisticsItem.hpp"
#include "strus/analyzer/contentStatisticsResult.hpp"
#include "strus/vectorQueryResult.hpp"
#include "strus/summaryElement.hpp"
#include "strus/resultDocument.hpp"
#include "strus/queryResult.hpp"
#include "strus/sentenceGuess.hpp"
#include "strus/timeStamp.hpp"
#include "strus/statisticsMessage.hpp"
#include "strus/structView.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/documentTermIteratorInterface.hpp"
#include "strus/debugTraceInterface.hpp"
#include "strus/index.hpp"
#include "impl/value/termExpression.hpp"
#include "impl/value/metadataExpression.hpp"
#include "impl/value/metadataComparison.hpp"
#include "strus/bindingObjects.h"
#include "strus/base/enable_if.hpp"
#include "strus/base/type_traits.hpp"
#include "strus/base/base64.hpp"
#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <stdexcept>

namespace strus {
namespace bindings {

template <typename Struct> class StructIdTemplate { public:static void structid(){}};
template <> class StructIdTemplate<StatisticsViewerInterface> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_StatisticsChange;}};
template <> class StructIdTemplate<TermStatisticsChange> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_TermStatisticsChange;}};
template <> class StructIdTemplate<TimeStamp> {public: static int structid()			{return STRUS_BINDINGS_STRUCTID_TimeStamp;}};
template <> class StructIdTemplate<IndexRange> {public: static int structid()			{return STRUS_BINDINGS_STRUCTID_IndexRange;}};
template <> class StructIdTemplate<analyzer::DocumentStructure::PositionRange> {public: static int structid() {return STRUS_BINDINGS_STRUCTID_IndexRange;}};
template <> class StructIdTemplate<StatisticsMessage> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_StatisticsMessage;}};
template <> class StructIdTemplate<analyzer::QueryTerm> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_QueryTerm;}};
template <> class StructIdTemplate<analyzer::QueryTermExpression> {public: static int structid(){return STRUS_BINDINGS_STRUCTID_QueryTermExpression;}};
template <> class StructIdTemplate<DocumentTermIteratorInterface::Term> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_DocumentInvTerm;}};
template <> class StructIdTemplate<analyzer::DocumentTerm> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_DocumentTerm;}};
template <> class StructIdTemplate<analyzer::Document> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_Document;}};
template <> class StructIdTemplate<analyzer::DocumentStructure> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_DocumentStructure;}};
template <> class StructIdTemplate<analyzer::DocumentClass> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_DocumentClass;}};
template <> class StructIdTemplate<VectorQueryResult> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_VectorQueryResult;}};
template <> class StructIdTemplate<SentenceTerm> {public: static int structid()			{return STRUS_BINDINGS_STRUCTID_SentenceTerm;}};
template <> class StructIdTemplate<SentenceGuess> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_SentenceGuess;}};
template <> class StructIdTemplate<SummaryElement> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_SummaryElement;}};
template <> class StructIdTemplate<MetaDataComparison> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_MetaDataComparison;}};
template <> class StructIdTemplate<ResultDocument> {public: static int structid()		{return STRUS_BINDINGS_STRUCTID_ResultDocument;}};
template <> class StructIdTemplate<QueryResult> {public: static int structid()			{return STRUS_BINDINGS_STRUCTID_QueryResult;}};
template <> class StructIdTemplate<analyzer::ContentStatisticsItem> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_ContentStatisticsItem;}};
template <> class StructIdTemplate<analyzer::ContentStatisticsResult> {public: static int structid()	{return STRUS_BINDINGS_STRUCTID_ContentStatisticsResult;}};


template <typename T>
struct has_structid
{
	template<void (*)()> struct tester_structid_void;
	template<int (*)()> struct tester_structid_int;

	template<typename U>
	static int has_structid_member(tester_structid_int<&U::structid>*);
	template<typename U>
	static short has_structid_member(...);

	static const bool value=sizeof(has_structid_member<StructIdTemplate<T> >(0))==sizeof(int);
};

class Serializer
{
public:
	template<typename SERVAL>
	static typename strus::enable_if<has_structid<SERVAL>::value,void>::type serialize( papuga_Serialization* result, SERVAL& param, bool deep)
	{
		papuga_ErrorCode errcode = papuga_NoMemError;
		if (papuga_Serialization_empty( result))
		{
			papuga_Serialization_set_structid( result, StructIdTemplate<SERVAL>::structid());
		}
		if (!serialize_nothrow( result, param, errcode, deep)) throw std::runtime_error(papuga_ErrorCode_tostring(errcode));
	}
	template<typename SERVAL>
	static typename strus::enable_if<has_structid<SERVAL>::value,void>::type serialize( papuga_Serialization* result, const SERVAL& param, bool deep)
	{
		papuga_ErrorCode errcode = papuga_NoMemError;
		if (papuga_Serialization_empty( result))
		{
			papuga_Serialization_set_structid( result, StructIdTemplate<SERVAL>::structid());
		}
		if (!serialize_nothrow( result, param, errcode, deep)) throw std::runtime_error(papuga_ErrorCode_tostring(errcode));
	}
	template<typename SERVAL>
	static typename strus::enable_if<!has_structid<SERVAL>::value,void>::type serialize( papuga_Serialization* result, const SERVAL& param, bool deep)
	{
		papuga_ErrorCode errcode = papuga_NoMemError;
		if (!serialize_nothrow( result, param, errcode, deep)) throw std::runtime_error(papuga_ErrorCode_tostring(errcode));
	}

	template <typename TYPE>
	static void serializeWithConstName( papuga_Serialization* result, const char* tagname, const TYPE& val, bool deep)
	{
		papuga_ErrorCode errcode = papuga_NoMemError;
		if (!serializeStructMember( result, tagname, val, errcode, deep)) throw std::runtime_error(papuga_ErrorCode_tostring(errcode));
	}
	template <typename TYPE>
	static void serializeWithName( papuga_Serialization* result, const char* tagname, const TYPE& val, bool deep)
	{
		papuga_ErrorCode errcode = papuga_NoMemError;
		if (!serializeStructMemberConstName( result, tagname, val, errcode, deep)) throw std::runtime_error(papuga_ErrorCode_tostring(errcode));
	}

private:
	static inline bool serialize_nothrow( papuga_Serialization* result, const double& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_double( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_Int& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_int( papuga_Serialization* result, const papuga_Int& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static typename strus::enable_if<!strus::is_same<int,papuga_Int>::value,bool>::type 
		serialize_nothrow( papuga_Serialization* result, const int& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const bool& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_bool( papuga_Serialization* result, const bool& val, papuga_ErrorCode& errcode, bool deep)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const std::string& val, papuga_ErrorCode& errcode, bool deep)
	{
		const char* valstr;
		if (val.empty()) return papuga_Serialization_pushValue_string( result, "", 0);
		if (deep)
		{
			valstr = papuga_Allocator_copy_string( result->allocator, val.c_str(), val.size());
			if (!valstr) return false;
		}
		else
		{
			valstr = val.c_str();
		}
		return papuga_Serialization_pushValue_string( result, valstr, val.size());
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const char* val, papuga_ErrorCode& errcode, bool deep)
	{
		if (deep)
		{
			val = papuga_Allocator_copy_charp( result->allocator, val);
			if (!val) return false;
		}
		return papuga_Serialization_pushValue_charp( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const char* val, std::size_t valsize, papuga_ErrorCode& errcode, bool deep)
	{
		if (deep)
		{
			val = papuga_Allocator_copy_string( result->allocator, val, valsize);
			if (!val) return false;
		}
		return papuga_Serialization_pushValue_string( result, val, valsize);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_ValueVariant& val, papuga_ErrorCode& errcode, bool deep)
	{
		if (deep)
		{
			errcode = papuga_NotImplemented;
			return false;
		}
		return papuga_Serialization_pushValue( result, &val);
	}
	static bool serialize_nothrow( papuga_Serialization* result, const NumericVariant& val, papuga_ErrorCode& errcode, bool deep);

	typedef std::vector<std::pair<std::string,std::string> > ConfigurationItemList;
	static bool serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val, papuga_ErrorCode& errcode, bool deep);

	static bool serialize_nothrow( papuga_Serialization* result, const TermStatisticsChange& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const TimeStamp& timestamp, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const IndexRange& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentStructure::PositionRange& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const StatisticsMessage& msg, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, DocumentTermIteratorInterface* dtitr, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentStructure& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentStructure>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::ContentStatisticsItem& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::ContentStatisticsItem>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::ContentStatisticsResult& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const VectorQueryResult& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const SentenceTerm& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const SentenceGuess& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const SummaryElement& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const TermExpression& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const MetaDataComparison& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const ResultDocument& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const QueryResult& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<VectorQueryResult>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<SentenceTerm>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<SentenceGuess>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<float>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const strus::DebugTraceMessage& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<strus::DebugTraceMessage>& val, papuga_ErrorCode& errcode, bool deep);
	static bool serialize_nothrow( papuga_Serialization* result, const strus::StructView& val, papuga_ErrorCode& errcode, bool deep);

	struct atomictype_ {};
	struct structtype_ {};
	struct maptype_ {};

	template <typename T>
	static typename strus::enable_if<
		strus::is_arithmetic<T>::value 
		|| strus::is_same<std::string,T>::value
		|| strus::is_same<char*,T>::value
		|| strus::is_same<const char*,T>::value
		|| strus::is_same<NumericVariant,T>::value
		|| strus::is_same<papuga_ValueVariant,T>::value
		,const atomictype_&>::type getCategory( const T&) { static atomictype_ rt; return rt;}

	template <typename T>
	static typename strus::enable_if<
		has_structid<T>::value
		,const structtype_&>::type getCategory( const T&) { static structtype_ rt; return rt;}

	template <typename T>
	static typename strus::enable_if<
		!(strus::is_arithmetic<T>::value 
		|| strus::is_same<std::string,T>::value
		|| strus::is_same<char*,T>::value
		|| strus::is_same<const char*,T>::value
		|| strus::is_same<NumericVariant,T>::value
		|| strus::is_same<papuga_ValueVariant,T>::value
		|| has_structid<T>::value)
		,const maptype_&>::type getCategory( const T&) { static maptype_ rt; return rt;}

	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const atomictype_& category, papuga_ErrorCode& errcode, bool deep)
	{
		return serialize_nothrow( result, val, errcode, deep);
	}
	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const structtype_& category, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen_struct( result, StructIdTemplate<SERVAL>::structid());
		rt &= serialize_nothrow( result, val, errcode, deep);
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const maptype_& category, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen( result);
		rt &= serialize_nothrow( result, val, errcode, deep);
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}

	template <typename TYPE>
	static bool serializeStructMemberConstName( papuga_Serialization* result, const char* tagname, const TYPE& val, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushName_charp( result, tagname);
		rt &= serializeStructMemberValue( result, val, getCategory( val), errcode, deep);
		return rt;
	}
	template <typename TYPE>
	static bool serializeStructMember( papuga_Serialization* result, const char* tagname, const TYPE& val, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		const char* tagnamestr;
		if (deep)
		{
			tagnamestr = papuga_Allocator_copy_charp( result->allocator, tagname);
			if (!tagnamestr)
			{
				errcode = papuga_NoMemError;
				return false;
			}
		}
		else
		{
			tagnamestr = tagname;
		}
		rt &= papuga_Serialization_pushName_charp( result, tagnamestr);
		rt &= serializeStructMemberValue( result, val, getCategory( val), errcode, deep);
		return rt;
	}
	template <typename TYPE>
	static bool serializeArrayElement( papuga_Serialization* result, const TYPE& val, papuga_ErrorCode& errcode, bool deep)
	{
		return serializeStructMemberValue( result, val, getCategory( val), errcode, deep);
	}

	template <typename TYPE>
	static bool serializeArray( papuga_Serialization* result, const std::vector<TYPE>& val, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		typename std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serializeArrayElement( result, *vi, errcode, deep);
		}
		return rt;
	}
	template <typename TYPE>
	static bool serializeIntArray( papuga_Serialization* result, const std::vector<TYPE>& val, papuga_ErrorCode& errcode, bool deep)
	{
		bool rt = true;
		typename std::vector<int>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serializeArrayElement( result, (papuga_Int)*vi, errcode, deep);
		}
		return rt;
	}
};

}}//namespace
#endif

