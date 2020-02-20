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
/// \brief Local change of statistics in a storage (distributed search index)
/// \member dfchange array of document frequency changes (_TermStatisticsChange)
/// \member nofdocs change in number of documents inserted
enum _StatisticsChange {dfchange,nofdocs};

/// \struct TermStatisticsChange
/// \brief Change of term statistics
/// \member type type name of the term
/// \member value value of the term
/// \member increment increment of the df (global document term frequency)
enum _TermStatisticsChange {type,value,increment};

/// \struct TimeStamp
/// \brief Timestamp datatype for specifying snapshots for example for changes in the global term statistics
/// \member unixtime number of seconds since 1/1/1970
/// \member counter counter value to order events within the same second (unixtime)
enum _TimeStamp {unixtime,counter};

/// \struct StatisticsMessage
/// \brief One message blob containing global term statistics and changes of the number of documents
/// \member timestamp specifies the time of the transaction where this message blob was created
/// \member blob blob with the statistics changes encoded as base64
enum _StatisticsMessage {timestamp,blob};

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

/// \struct IndexRange
/// \brief Index range
/// \member start start position
/// \member end end position (first position not belonging to the range)
enum _IndexRange {start,end};

/// \struct DocumentStructure
/// \brief Document structures
/// \member name name of the structure
/// \member source header of the block
/// \member sink content of the block
enum _DocumentStructure {name,source,sink};

/// \struct Document
/// \brief Document as returned from the analyzer and accepted by insertDocument
/// \member doctype sub document type
/// \member attribute document attributes (dictionary)
/// \member metadata document metadata (dictionary)
/// \member forwardindex forward index features (list)
/// \member searchindex search index features (list)
/// \member searchstruct search index structures (list)
/// \member access access control list, user role or list of user roles allowed to read the document, not provided by the analyzer
enum _Document {doctype,attribute,metadata,forwardindex,searchindex,searchstruct,access};

/// \struct DocumentClass
/// \brief Document class structure
/// \member mimetype MIME type
/// \member encoding Character set encoding (default UTF-8)
/// \member schema Name for document structure
enum _DocumentClass {mimetype,encoding,schema};

/// \struct VectorQueryResult
/// \brief Result of a similar vector query
/// \member value feature string
/// \member weight weight given to the feature (0.0 <= .. <= 1.0)
enum _VectorQueryResult {value,weight};

/// \struct SentenceTerm
/// \brief Term in a sentence analyzer result
/// \member type type of the term
/// \member value string value of the term
enum _SentenceTerm {type,value};

/// \struct SentenceGuess
/// \brief Result of sentence analyzer
/// \member terms list of terms of the sentence
/// \member weight given to the sentence, used to decide what recognized pattern is the most probably to for the query sentence
enum _SentenceGuess {terms,weight};

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

/// \struct MetaDataTableCommand
/// \brief Command altering the meta data table structure
/// \member op operation (one of 'add', 'replace', 'remove', 'clear', 'alter')
/// \member name name of the meta data element
/// \member type type name of the meta data element ('UINT32','INT8','FLOAT16',etc.)
/// \member oldname name of the element before the operation
enum _MetaDataTableCommand {op,name,type,oldname};

/// \struct ResultDocument
/// \brief Document result description of a storage query result
/// \member docno internal (local) document number
/// \member weight weight of the result document
/// \member field field (ordinal position range) that was weighted in this result
/// \member summary list of summary elements for this result document
enum _ResultDocument {docno,weight,field,summary};

/// \struct QueryResult
/// \brief Storage query result
/// \member evalpass index of query evaluation pass used for this result (multipass query evaluation)
/// \member nofranked number of documents ranked for this result
/// \member nofvisited number of documents visited for this result
/// \member ranks list of results with a maximum size specified in the query
/// \member summary list of summary elements for this result
enum _QueryResult {evalpass,nofranked,nofvisited,ranks,summary};

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



