/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Structures for return values
#ifndef _STRUS_BINDING_IMPL_STRUCTS_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STRUCTS_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/numericVariant.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/reference.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/analyzer/document.hpp"
#include "impl/value/termExpression.hpp"
#include "impl/value/metadataExpression.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/iterator.hpp"
#include "queryAnalyzerStruct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \struct StatisticsChange
/// \brief local change of statistics in a storage (distributed search index)
/// \member dfchange array of document frequency changes (_TermStatisticsChange)
/// \member nofdocs change in number of documents inserted
enum _StatisticsChange {dfchange,nofdocs};

/// \struct TermStatisticsChange
/// \brief Document class structure
/// \member type type name of the term
/// \member value value of the term
/// \member increment increment of the df (global document term frequency)
enum _TermStatisticsChange {type,value,increment};

/// \struct QueryTerm
/// \brief Query term structure
/// \member type type name of the term
/// \member value value of the term
/// \member length (optional) span of the term in number of ordinal position steps covered (>=1)
/// \member variable (optional) name of the variable attached to this term
enum _QueryTerm {type,value,length,variable};

/// \struct QueryTermExpression
/// \brief Query term structure
/// \member op query term expression operator name
/// \member range maximal ordinal position span allowed for a match or 0 if no limits defined
/// \member cardinality minimum number of elements a selection operator must select or 0 if unspecified)
/// \member arg list of arguments of the expression
/// \member variable (optional) name of the variable attached to this term
enum _QueryTermExpression {op,range,cardinality,arg,variable};

/// \struct DocumentInvTerm
/// \brief Term structure provided by document term iterator
/// \member tf term frequency in the document
/// \member firstpos first occurrence ordinal term position in the document
/// \member value term value string
enum _DocumentInvTerm {tf,firstpos,value};

/// \struct DocumentTerm
/// \brief Document term
/// \member type type name of the term
/// \member value value of the term
/// \member pos ordinal position of the term (>=1)
enum _DocumentTerm {type,value,pos};

/// \struct Document
/// \brief Document structure as returned from the analyzer and accepted by insertDocument
/// \member doctype sub document type
/// \member attribute document attributes (dictionary)
/// \member metadata document metadata (dictionary)
/// \member forwardindex forward index features (list)
/// \member searchindex search index features (list)
/// \member access access control list, user role or list of user roles allowed to read the document, not provided by the analyzer
enum _Document {doctype,attribute,metadata,forwardindex,searchindex,access};

/// \struct DocumentClass
/// \brief Document class structure
/// \member mimetype MIME type
/// \member encoding Character set encoding (default UTF-8)
/// \member scheme Document type id
enum _DocumentClass {mimetype,encoding,scheme};

/// \struct VectorQueryResult
/// \brief Result of a similar vector query
/// \member featidx index of the feature
/// \member weight weight of the result (0.0 <= .. <= 1.0)
enum _VectorQueryResult {featidx,weight};

/// \struct SummaryElement
/// \brief Summary element in a query result 
/// \member name name of the summary element
/// \member value value of the summary element
/// \member weight weight of the summary element
/// \member index index of the summary element in cast of a list
enum _SummaryElement {name,value,weight,index};

/// \struct MetaDataComparison
/// \brief Meta data comparison in a meta data query expression
/// \member op comparison operator
/// \member name name of the meta data element
/// \member value query value for the meta data element comparison
enum _MetaDataComparison {op,name,value};

/// \struct ResultDocument
/// \brief Document result description of a storage query result
/// \member docno internal (local) document number
/// \member weight weight of the result document
/// \member summary list of summary elements for this result document
enum _ResultDocument {docno,weight,summary};

/// \struct QueryResult
/// \brief Storage query result
/// \member evalpass index of query evaluation pass used for this result (multipass query evaluation)
/// \member nofranked number of documents ranked for this result
/// \member nofvisited number of documents visited for this result
/// \member ranks list of results with a maximum size specified in the query
enum _QueryResult {evalpass,nofranked,nofvisited,ranks};

/// \struct ContentStatisticsItem
/// \brief One item of content statistics
/// \member select select expression for the item
/// \member type type name from the content statistics library assigned to it
/// \member example first example found for this categorization (select expression + type)
/// \member df document frequency
/// \member tf total number of occurrencies
enum _ContentStatisticsItem {select,type,example,df,tf};

/// \struct ContentStatisticsResult
/// \brief Overall content statistics structure
/// \member nofdocs number of sample documents
/// \member items list of item classes found
enum _ContentStatisticsResult {nofdocs,items};

}}//namespace
#endif



