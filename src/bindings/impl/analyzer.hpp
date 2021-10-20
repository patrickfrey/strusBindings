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
#include "strus/textProcessorInterface.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/analyzer/document.hpp"
#include "impl/value/termExpression.hpp"
#include "impl/value/sentenceTermExpression.hpp"
#include "impl/value/metadataExpression.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/iterator.hpp"
#include "impl/value/struct.hpp"
#include "queryAnalyzerStruct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;
///\brief Forward declaration
class SentenceLexerImpl;


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
	/// \param[in] priority priority element priority analyzer element with lower or undefined priority are ousted if they are completely covered by elements with higher priority
	/// \example 3
	/// \example 1
	/// \example 0
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
		const ValueVariant& priority=ValueVariant(),
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
	/// \param[in] priority priority element priority analyzer element with lower or undefined priority are ousted if they are completely covered by elements with higher priority
	/// \example 3
	/// \example 1
	/// \example 0
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
		const ValueVariant& priority=ValueVariant(),
		const ValueVariant& options=ValueVariant());

	/// \brief Define field in a document as coherent areas selected by a selection expression
	/// \note Fields are building blocks of structures int the search index defined as directional binary relations of fields
	/// \param[in] name name of this field type
	/// \example "title","text"
	/// \param[in] scopeexpr expression selecting the scope of the field, the other two expression parameters 'selectexpr' and 'keyexpr' are defined relative to this expression
	/// \remark The scope of two fields related in a structure has to be the same
	/// \example "/doc"
	/// \example "/doc/text"
	/// \param[in] selectexpr expression selecting the coherent area defining what is in the field
	/// \param[in] keyexpr (optional) expression selecting the key of the field that has to be the same for fields related in a structure
	/// \remark selectexpr has to be a prefix of keyexpr
	void addSearchIndexField(
			const std::string& name,
			const std::string& scopeexpr,
			const std::string& selectexpr,
			const ValueVariant& keyexpr=ValueVariant());

	/// \brief Define a structure as directional relation of two field types
	/// \note The field defining the source of the directional relation is called 'header'
	/// \note The field defining the sink of the directional relation is called 'content'
	/// \remark The header field areas of a structures of the same structure declaration must not overlap
	/// \remark The content field areas of a structure of the same structure declaration must not overlap
	/// \param[in] name name of this structure type
	/// \example "chapter"
	/// \param[in] headerFieldName name of the field type that defines the header part of this structure
	/// \example "title"
	/// \example "h1"
	/// \param[in] contentFieldName name of the field type that defines the content part of this structure
	/// \example "text"
	/// \example "p"
	/// \param[in] structureType one of "cover","label","header","footer" defining the building of this structure type in the document analysis
	/// \note Structure types are unified and treated uniformly after analysis
	/// \note "cover" is used in case of the header covering the content
	/// \note "label" is used in case of the header beeing embedded in the content structure
	/// \note "header" is used in case of the header preceeding the content structure
	/// \note "footer" is used in case of the header preceeding the content structure
	void addSearchIndexStructure(
			const std::string& name,
			const std::string& headerFieldName,
			const std::string& contentFieldName,
			const ValueVariant& structureType);

	/// \brief Define a feature to insert as meta data is selected, tokenized and normalized
	/// \param[in] name name of the addressed meta data element.
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
		const std::string& name,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] name name of the addressed meta data element.
	/// \example "doclen"
	/// \param[in] function defining how and from what the value is aggregated
	/// \example ["count","word"]
	void defineAggregatedMetaData(
		const std::string& name,
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

	/// \brief Analyze a content and return the analyzed document structure (analyzing single document)
	/// \param[in] content content string (NOT a file name !) of the document to analyze
	/// \example "<?xml version='1.0' encoding='UTF-8' standalone=yes?><doc>...</doc>"
	/// \param[in] documentClass document class of the document to analyze, if not specified the document class is guessed from the content with document class detection
	/// \example [ mimetype:"application/xml" encoding:"UTF-8" scheme:"customer" ]
	/// \example [ mimetype:"application/json" encoding:"UTF-8" ]
	/// \return structure of the document analyzed (sub document type names, search index terms, forward index terms, metadata, attributes)
	analyzer::Document* analyzeSingle(
			const std::string& content,
			const ValueVariant& documentClass=ValueVariant()) const;

	/// \brief Analyze a content and return the analyzed document structures as iterator (analyzing multipart document)
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

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

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
	/// \param[in] priority priority element priority analyzer element with lower or undefined priority are ousted if they are completely covered by elements with higher priority
	/// \example 3
	/// \example 1
	/// \example 0
	void addElement(
			const std::string& featureType,
			const std::string& fieldType,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers,
			const ValueVariant& priority=ValueVariant());

	/// \brief Define a field containing sentence, meaning that the content of the field is a text string whose content requires syntactical and semantical analysis to determine its class and to decide what to do with it.
	/// \param[in] fieldType field type name
	/// \example "querystr"
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type before normalizing it
	/// \example "content"
	/// \example "word"
	/// \param[in] normalizers list of normalizer function descriptions to use for the features of this field type (in the ascending order of appearance) before passing it to analysis.
	/// \example "uc"
	/// \example ["lc",["convdia", "en"]]
	/// \param[in] expansion the declarations how to expand detectect type with similar values
	/// \example [[type: "V" similarity: "0.85"],[type: "N" similarity: "0.9"]]
	/// \param[in] lexer the sentence lexer to use
	void addSentenceType(
			const std::string& fieldType,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers,
			const ValueVariant& expansion,
			SentenceLexerImpl* lexer);

	/// \brief Define a field with contents to collect and return the closest features (vector similarity) of a specific type
	/// \param[in] fieldType field type name
	/// \example "feature"
	/// \param[in] prefixSeparator separator used to separate word type info from word value, the second part is normalized
	/// \example "#"
	/// \param[in] normalizers list of normalizer function descriptions to use for the features of this field type (in the ascending order of appearance) before passing it to analysis.
	/// \example "uc"
	/// \example ["lc","entityid"]
	/// \param[in] lexer the sentence lexer to use
	void addCollectorType(
			const std::string& fieldType,
			const std::string& prefixSeparator,
			const ValueVariant& normalizers,
			SentenceLexerImpl* lexer);

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

	/// \brief Analyze a term expression
	/// \param[in] expression query term expression tree
	/// \example  [ "within" 5 ["word" "Worlds"]  ["word" "powers"]]
	/// \example  [ "word" "PUBLIC" ]
	/// \return structure analyzed
	/// \example  [ "within" 5 ["word" "world"]  ["word" "power"]]
	/// \example  [ "word" "public" ]
	TermExpression* analyzeTermExpression( const ValueVariant& expression) const;

	/// \brief Analyze a unique term expression resulting in a single and unique result
	/// \remark issues an error if the result does not exist or is not unique
	/// \param[in] expression query term expression tree
	/// \example  [ "within" 5 ["word" "Worlds"]  ["word" "powers"]]
	/// \example  [ "word" "PUBLIC" ]
	/// \return structure analyzed
	/// \example  [ "within" 5 ["word" "world"]  ["word" "power"]]
	/// \example  [ "word" "public" ]
	TermExpression* analyzeSingleTermExpression( const ValueVariant& expression) const;
	
	/// \brief Analyze a metadata expression
	/// \param[in] expression query metadata expression tree
	/// \example  ["<" "year" "26.9.2017"]
	/// \return structure analyzed
	/// \example  ["<" "year" "17071"]
	MetaDataExpression* analyzeMetaDataExpression( const ValueVariant& expression) const;

	/// \brief Analyze a metadata expression having union,condition typeing tags in the output, following a stricter schema more suitable for web requests
	/// \param[in] expression query metadata expression tree
	/// \example  [ condition: ["<" "year" "26.9.2017"] ]
	/// \return structure analyzed
	/// \example  [ condition: ["<" "year" "17071"] ]
	MetaDataExpression* analyzeSchemaMetaDataExpression( const ValueVariant& expression) const;

	/// \brief Analyzes a field as sentence, meaning that the content of the field is tokenized,normalized,concatenated by spaces ' ' and split into lexems by a sentence analyzer that finds the most probable patterns matching the field.
	/// \param[in] fieldType field type name
	/// \example "querystr"
 	/// \param[in] fieldContent source to analyze
	/// \example "best football manager in the world"
	/// \param[in] maxNofResults maximum number of results to return by the sentence analyzer
	/// \example 4
	/// \param[in] minWeight minimum weight to accept of a result of the sentence analyzer, best result has weight 1.0 and the others are normalized to be smaller or equal to 1.0
	/// \example 0.99
	/// \example 0.8
	/// \return list of named sentence guesses with weight (same as SentenceLexer::call(..))
	/// \example [[type: "M", value: "best"],[type: "N", value: "football_manager"],[type: "T", value: "in"],[type: "T", value: "the"],[type: "N", value: "world"]]
	SentenceTermExpression* analyzeSentence(
			const std::string& fieldType,
			const std::string& fieldContent,
			int maxNofResults,
			double minWeight);

	/// \brief Find closest neighbours of a list of words typed by a prefix
	/// \param[in] fieldType field type name that selects the method defined with addCollectorType
	/// \example "collected"
 	/// \param[in] fields list of elements collected
	/// \example ["V#run", "N#manager", "N#football"]
	/// \param[in] searchType word type to seek for
	/// \example "G"
	/// \param[in] maxNofResults maximum number of results to return
	/// \example 4
	/// \param[in] minSimilarity minimum similarity
	/// \example 0.99
	/// \example 0.8
	/// \param[in] minNormalizedWeight minimum normalized weight for cut off
	/// \example 0.75
	/// \example 0.8
	/// \return list of results
	/// \example [[type: "G", value: "2384798", weight: "0.92121324"],[type: "G", value: "870349244", weight: "0.912923"],[type: "G", value: "131235", weight: "0.813423"],[type: "G", value: "214546546", weight: "0.787834"]]
	Struct getCollectedNeighbours(
			const std::string& fieldType,
			const ValueVariant& fields,
			const std::string& searchType,
			int maxNofResults,
			double minSimilarity,
			double minNormalizedWeight);

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	TermExpression* analyzeTermExpression_( const ValueVariant& expression, bool unique) const;
	MetaDataExpression* analyzeMetaDataExpression_( const ValueVariant& expression, bool schemaTypedOutput) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	friend class InserterImpl;

	QueryAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const TextProcessorInterface* textproc_);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	ObjectRef m_sentence_lexer_map_impl;
	ObjectRef m_neighbour_collector_map_impl;
	QueryAnalyzerStruct m_queryAnalyzerStruct;
	const TextProcessorInterface* m_textproc;
};

}}//namespace
#endif

