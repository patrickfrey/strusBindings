/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_ANALYZER_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_ANALYZER_HPP_INCLUDED
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

typedef papuga_ValueVariant ValueVariant;

/// \class DocumentAnalyzerImpl
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
/// \remark The only way to construct a document analyzer instance is to call Context::createDocumentAnalyzer( doctype)
class DocumentAnalyzerImpl
{
public:
	/// \brief Destructor
	virtual ~DocumentAnalyzerImpl(){}

	/// \brief Define a feature to insert into the inverted index (search index) is selected, tokenized and normalized
	/// \example addSearchIndexFeature( "word", "/doc/elem", "word", ["lc",["stem", "en"]])
	/// \param[in] type type of the features produced (your choice)
	/// \example "word"
	/// \example "stem"
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	/// \param[in] options a list of option strings, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	/// \example "content"
	/// \example "unique"
	/// \example "succ"
	/// \example "pred"
	void addSearchIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& options=ValueVariant());

	/// \brief Define a feature to insert into the forward index (for summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \example "word"
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	/// \param[in] options a list of options, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	/// \example "content"
	/// \example "unique"
	/// \example "succ"
	/// \example "pred"
	void addForwardIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& options=ValueVariant());

	/// \brief Declare an element to be used as lexem by post processing pattern matching but not put into the result of document analysis
	/// \param[in] type term type name of the lexem to be feed to the pattern matching
	/// \example "word"
	/// \param[in] selectexpr an expression that decribes what elements are taken from a document for this feature (tag selection in abbreviated syntax of XPath)
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] tokenizer tokenizer (ownership passed to this) to use for this feature
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizers (element ownership passed to this) to use for this feature
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void addPatternLexem(
			const std::string& type,
			const std::string& selectexpr,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers);
	
	/// \brief Define a feature to insert as meta data is selected, tokenized and normalized
	/// \param[in] fieldname name of the addressed meta data field.
	/// \example "date"
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void defineMetaData(
		const std::string& fieldname,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] fieldname name of the addressed meta data field.
	/// \example "doclen"
	/// \param[in] function defining how and from what the value is aggregated
	/// \example ["count","word"]
	void defineAggregatedMetaData(
		const std::string& fieldname,
		const ValueVariant& function);

	/// \brief Define a feature to insert as document attribute (for summarization) is selected, tokenized and normalized
	/// \param[in] attribname name of the addressed attribute.
	/// \example "docid","title"
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \example "/doc/text//()"
	/// \example "/doc/user@id"
	/// \example "/doc/text[@lang='en']//()"
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \example "split"
	/// \example ["regex", "[0-9]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void defineAttribute(
		const std::string& attribname,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Define a result of pattern matching as feature to insert into the search index, normalized
	/// \param[in] type type name of the feature to produce.
	/// \example "concept"
	/// \param[in] patternTypeName name of the pattern to select
	/// \example "word"
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	/// \param[in] options a list of option strings, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	/// \example "content"
	/// \example "unique"
	/// \example "succ"
	/// \example "pred"
	void addSearchIndexFeatureFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers,
		const ValueVariant& options=ValueVariant());

	/// \brief Define a result of pattern matching as feature to insert into the forward index, normalized
	/// \param[in] type type name of the feature to produce.
	/// \example "concept"
	/// \param[in] patternTypeName name of the pattern to select
	/// \example "word"
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	/// \param[in] options a list of options, elements one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
	/// \example "content"
	/// \example "unique"
	/// \example "succ"
	/// \example "pred"
	void addForwardIndexFeatureFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers,
		const ValueVariant& options=ValueVariant());

	/// \brief Define a result of pattern matching to insert as metadata, normalized
	/// \param[in] fieldname field name of the meta data element to produce.
	/// \example "location"
	/// \param[in] patternTypeName name of the pattern to select
	/// \example "word"
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void defineMetaDataFromPatternMatch(
		const std::string& fieldname,
		const std::string& patternTypeName,
		const ValueVariant& normalizers);

	/// \brief Define a result of pattern matching to insert as document attribute, normalized
	/// \param[in] attribname name of the document attribute to produce.
	/// \example "annotation"
	/// \param[in] patternTypeName name of the pattern to select
	/// \example "word"
	/// \param[in] normalizers list of normalizer function descriptions to use for this feature in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void defineAttributeFromPatternMatch(
		const std::string& attribname,
		const std::string& patternTypeName,
		const ValueVariant& normalizers);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \example "location"
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \example ""
	/// \param[in] lexems list of all lexems generated by the feeder (analyzer)
	/// \example "word"
	/// \example ["word","number"]
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const ValueVariant& lexems,
			const ValueVariant& patterns);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \example "location"
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \example ""
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	/// \example "/srv/strus/patterns.bin"
	void definePatternMatcherPostProcFromFile(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const std::string& serializedPatternFile);

	/// \brief Declare a sub document for the handling of multi part documents in an analyzed content or documents of different types with one configuration
	/// \note Sub documents are defined as the sections selected by the expression plus some data selected not belonging to any sub document.
	/// \param[in] selectexpr an expression that defines the content of the sub document declared
	/// \example "/doc/employee"
	/// \param[in] subDocumentTypeName type name assinged to this sub document
	/// \example "employee"
	void defineSubDocument(
			const std::string& subDocumentTypeName,
			const std::string& selectexpr);

	/// \brief Declare a sub parrt of a document with a different document class, needing a switching of the segmenter
	/// \param[in] selectexpr an expression that defines the area of the sub content
	/// \example "/doc/content"
	/// \param[in] documentClass document class of the content, determines what segmenter to use for this part
	/// \example [mimetype:"application/json" encoding:"UTF-8"]
	void defineSubContent(
			const std::string& selectexpr,
			const ValueVariant& documentClass);

	/// \brief Analye a content and return the analyzed document structure (analyzing single document)
	/// \param[in] content content string (NOT a file name !) of the document to analyze
	/// \example "<?xml version='1.0' encoding='UTF-8' standalone=yes?><doc>...</doc>"
	/// \param[in] documentClass document class of the document to analyze, if not specified the document class is guessed from the content with document class detection
	/// \example [ mimetype:"application/xml" encoding:"UTF-8" scheme:"customer" ]
	/// \example [ mimetype:"application/json" encoding:"UTF-8" ]
	/// \return structure of the document analyzed (sub document type names, search index terms, forward index terms, metadata, attributes)
	analyzer::Document* analyzeSingle(
			const std::string& content,
			const ValueVariant& documentClass=ValueVariant()) const;

	/// \brief Analye a content and return the analyzed document structures as iterator (analyzing multipart document)
	/// \note If you are not sure if to use analyzeSingle or analyzeMultiPart, then take analyzeMultiPart, because it covers analyzeSingle, returning an iterator on a set containing the single document only
	/// \param[in] content content string (NOT a file name !) with the documents to analyze
	/// \example "<?xml version='1.0' encoding='UTF-8' standalone=yes?><doc>...</doc>"
	/// \param[in] documentClass document class of the document set to analyze, if not specified the document class is guessed from the content with document class detection
	/// \example [ mimetype:"application/xml" encoding:"UTF-8" scheme:"customer" ]
	/// \example [ mimetype:"application/json" encoding:"UTF-8" ]
	/// \return iterator on structures of the documents analyzed (sub document type names, search index terms, forward index terms, metadata, attributes)
	Iterator analyzeMultiPart(
			const std::string& content,
			const ValueVariant& documentClass=ValueVariant()) const;

private:
	analyzer::DocumentClass getDocumentClass( const std::string& content, const ValueVariant& dclass) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	DocumentAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const ValueVariant& doctype, const TextProcessorInterface* textproc_);

	/// \brief Constructor used by Inserter
	friend class InserterImpl;
	friend class InserterTransactionImpl;
	DocumentAnalyzerImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& analyzer_, const TextProcessorInterface* textproc_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_analyzer_impl(analyzer_),m_textproc(textproc_){}

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	const TextProcessorInterface* m_textproc;
};

/// \class QueryAnalyzerImpl
/// \brief Analyzer object representing a set of function for transforming a field,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
/// \remark The only way to construct a document analyzer instance is to call Context::createQueryAnalyzer()
class QueryAnalyzerImpl
{
public:
	/// \brief Destructor
	virtual ~QueryAnalyzerImpl(){}

	/// \brief Defines an element (term, metadata) of query analysis.
	/// \param[in] featureType element feature type created from this field type
	/// \example "stem"
	/// \example "word"
	/// \param[in] fieldType name of the field type defined
	/// \example "text"
	/// \example "word"
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \example "content"
	/// \example "word"
	/// \example ["regex", "[A-Za-z]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for the features of this field type in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \example ["date2int","d","%Y-%m-%d"]
	void addElement(
			const std::string& featureType,
			const std::string& fieldType,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers);

	/// \brief Defines an element from a pattern matching result.
	/// \param[in] type element type created from this pattern match result type
	/// \example "name"
	/// \param[in] patternTypeName name of the pattern match result item
	/// \example "entity"
	/// \param[in] normalizers list of normalizer functions
	/// \example ["lc",["stem", "en"]]
	void addElementFromPatternMatch(
			const std::string& type,
			const std::string& patternTypeName,
			const ValueVariant& normalizers);

	/// \brief Declare an element to be used as lexem by post processing pattern matching but not put into the result of query analysis
	/// \param[in] termtype term type name of the lexem to be feed to the pattern matching
	/// \example "titlestem"
	/// \example "titleword"
	/// \param[in] fieldtype type of the field of this element in the query
	/// \example "text"
	/// \example "word"
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \example "content"
	/// \example "word"
	/// \example ["regex", "[A-Za-z]+"]
	/// \param[in] normalizers list of normalizer function descriptions to use for the features of this field type in the ascending order of appearance
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	void addPatternLexem(
			const std::string& termtype,
			const std::string& fieldtype,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \example "entity"
	/// \example "match"
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] lexems list of all lexems generated by the feeder (analyzer)
	/// \example ["word" "number" "name"]
	/// \example "word"
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const ValueVariant& lexems,
			const ValueVariant& patterns);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	void definePatternMatcherPostProcFromFile(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const std::string& serializedPatternFile);

	/// \brief Declare an implicit grouping operation for a query field type. The implicit group operation is always applied when more than one term are resulting from analysis of this field to ensure that you get only one node in the query from it.
	/// \param[in] fieldtype name of the field type where this grouping operation applies
	/// \example "text"
	/// \example "word"
	/// \param[in] opname query operator name generated as node for grouping
	/// \example "join"
	/// \example "within"
	/// \param[in] range positional range attribute for the node used for grouping (0 for no range)
	/// \example 0
	/// \example 2
	/// \param[in] cardinality cardinality attribute for the node used for grouping (0 for all)
	/// \example 0
	/// \example 3
	/// \param[in] groupBy kind of selection of the arguments grouped ("position": elements with same position get their own group, "all" (or "" default): all elements of the field get into one group
	/// \example "position"
	/// \example "all"
	/// \example ""
	void defineImplicitGroupBy( const std::string& fieldtype, const std::string& groupBy, const std::string& opname, int range=0, unsigned int cardinality=0);

	/// \brief Analye a term expression
	/// \param[in] expression query term expression tree
	/// \example  [ "within" 5 ["word" "Worlds"]  ["word" "powers"]]
	/// \example  [ "word" "PUBLIC" ]
	/// \return structure analyzed
	/// \example  [ "within" 5 ["word" "world"]  ["word" "power"]]
	/// \example  [ "word" "public" ]
	TermExpression* analyzeTermExpression( const ValueVariant& expression) const;

	/// \brief Analye a unique term expression resulting in a single and unique result
	/// \remark issues an error if the result does not exist or is not unique
	/// \param[in] expression query term expression tree
	/// \example  [ "within" 5 ["word" "Worlds"]  ["word" "powers"]]
	/// \example  [ "word" "PUBLIC" ]
	/// \return structure analyzed
	/// \example  [ "within" 5 ["word" "world"]  ["word" "power"]]
	/// \example  [ "word" "public" ]
	TermExpression* analyzeSingleTermExpression( const ValueVariant& expression) const;

	/// \brief Analye a metadata expression
	/// \param[in] expression query metadata expression tree
	/// \example  ["<" "year" "26.9.2017"]
	/// \return structure analyzed
	/// \example  ["<" "year" "17071"]
	MetaDataExpression* analyzeMetaDataExpression( const ValueVariant& expression) const;

private:
	TermExpression* analyzeTermExpression_( const ValueVariant& expression, bool unique) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	friend class InserterImpl;

	QueryAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	QueryAnalyzerStruct m_queryAnalyzerStruct;
};

}}//namespace
#endif

