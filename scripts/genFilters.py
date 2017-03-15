#!/usr/bin/python
from jinja2 import Template
from sets import Set

atomictypes = {
        "Index": {
                "variantcast": "bindings::ValueVariant( (bindings::ValueVariant::Int)val)",
                "fullname": 'strus::Index',
                "include": '"strus/index.hpp"'
        },
        "double": {
                "variantcast": "bindings::ValueVariant( val)",
                "fullname": 'double'
        },
        "uint": {
                "variantcast": "bindings::ValueVariant( (bindings::ValueVariant::UInt)val)",
                "fullname": 'unsigned int'
        },
        "int": {
                "variantcast": "bindings::ValueVariant( (bindings::ValueVariant::Int)val)",
                "fullname": 'int'
        },
        "bool": {
                "variantcast": "bindings::ValueVariant( (bindings::ValueVariant::Int)val)",
                "fullname": 'bool'
        },
        "charp": {
                "variantcast": "bindings::ValueVariant( val)",
                "paramname": "const char*",
                "fullname": 'const char*'
        },
        "string": {
                "variantcast": "bindings::ValueVariant( val.c_str(), val.size())",
                "fullname": 'std::string',
                "include": '<string>'
        },
        "NumericVariant": {
                "variantcast": "val",
                "fullname": 'strus::NumericVariant',
                "include": '"strus/numericVariant.hpp"'
        }
}

structtypes = {
       "StrusObjectType": {
               "fullname": 'strus::bindings::StrusObjectType',
               "include": '"strus/bindings/hostReference.hpp"'
       },
       "Term": {
               "fullname": 'strus::analyzer::Term',
               "include": '"strus/analyzer/term.hpp"',
               "elements": [
                      { "name": 'type', "type": "string" },
                      { "name": 'value', "type": "string" },
                      { "name": 'pos', "type": "uint" },
                      { "name": 'len', "type": "uint" }
               ]
       },
       "TermList"      : {
               "fullname": 'std::vector<strus::analyzer::Term>',
               "include": '"strus/analyzer/term.hpp"'
       },
       "MetaData": {
               "fullname": 'strus::analyzer::MetaData',
               "include": '"strus/analyzer/metaData.hpp"',
               "elements": [
                      { "name": 'name', "type": "string" },
                      { "name": 'value', "type": "NumericVariant" }
               ]
       },
       "MetaDataList": {
               "fullname": 'std::vector<strus::analyzer::MetaData>',
               "include": '"strus/analyzer/metaData.hpp"'
       },
       "Attribute": {
               "fullname": 'strus::analyzer::Attribute',
               "include": '"strus/analyzer/attribute.hpp"',
               "elements": [
                      { "name": 'name', "type": "string" },
                      { "name": 'value', "type": "string" }
               ]
       },
       "AttributeList": {
               "fullname": 'std::vector<strus::analyzer::Attribute>',
               "include": '"strus/analyzer/attribute.hpp"'
       },
       "Document": {
               "fullname": 'strus::analyzer::Document',
               "include": '"strus/analyzer/document.hpp"',
               "elements": [
                      { "name": 'subDocumentTypeName', "type": "string" },
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'attributes', "type": "Attribute[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'forwardTerms', "type": "Term[]" }
               ]
       },
       "QueryElement": {
               "fullname": 'strus::analyzer::Query::Element',
               "include": '"strus/analyzer/query.hpp"',
               "elements": [
                      { "name": 'type', "type": "string" },
                      { "name": 'idx', "type": "uint" },
                      { "name": 'position', "type": "uint" },
                      { "name": 'length', "type": "uint" },
                      { "name": 'fieldNo', "type": "uint" }
               ]
       },
       "QueryInstruction": {
               "fullname": 'strus::analyzer::Query::Instruction',
               "include": '"strus/analyzer/query.hpp"',
               "elements": [
                      { "name": 'opCode', "type": "string" },
                      { "name": 'idx', "type": "uint" },
                      { "name": 'nofOperands', "type": "uint" }
               ]
       },
       "Query": {
               "fullname": 'strus::analyzer::Query',
               "include": '"strus/analyzer/query.hpp"',
               "elements": [
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'elements', "type": "QueryElement[]" },
                      { "name": 'instructions', "type": "QueryInstruction[]" }
               ]
       },
       "DocumentFrequencyChange": {
               "fullname": 'strus::bindings::DocumentFrequencyChange',
               "include": '"strus/bindings/statisticsMessage.hpp"',
               "elements": [
                      { "name": 'type', "type": "charp" },
                      { "name": 'value', "type": "charp" },
                      { "name": 'increment', "type": "int" }
               ]
       },
       "StatisticsMessage": {
               "fullname": 'strus::bindings::StatisticsMessage',
               "include": '"strus/bindings/statisticsMessage.hpp"',
               "elements": [
                      { "name": 'df', "type": "DocumentFrequencyChange[]" },
                      { "name": 'nofdocs', "type": "int" }
               ]
       },
       "VectorRank": {
               "fullname": 'VectorStorageSearchInterface::Result',
               "include": '"strus/vectorStorageSearchInterface.hpp"',
               "elements": [
                      { "name": 'featidx', "type": "Index" },
                      { "name": 'weight', "type": "double" }
               ]
       },
       "VectorRankList": {
               "fullname": 'std::vector<VectorStorageSearchInterface::Result>',
               "include": '"strus/vectorStorageSearchInterface.hpp"',
       },
       "SummaryElement": {
               "fullname": 'strus::SummaryElement',
               "include": '"strus/summaryElement.hpp"',
               "elements": [
                      { "name": 'name', "type": "string" },
                      { "name": 'value', "type": "string" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'index', "type": "int" }
               ]
       },
       "SummaryElementList": {
               "fullname": 'std::vector<strus::SummaryElement>',
               "include": '"strus/summaryElement.hpp"'
       },
       "DocumentRank": {
               "fullname": 'strus::ResultDocument',
               "include": '"strus/resultDocument.hpp"',
               "elements": [
                      { "name": 'docno', "type": "Index" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'summaryElements', "type": "SummaryElementList" }
               ]
       },
       "DocumentRankList": {
               "fullname": 'std::vector<strus::ResultDocument>',
               "include": '"strus/resultDocument.hpp"'
       },
       "QueryResult": {
               "fullname": 'strus::QueryResult',
               "include": '"strus/queryResult.hpp"',
               "elements": [
                      { "name": 'evaluationPass', "type": "uint" },
                      { "name": 'nofRanked', "type": "uint" },
                      { "name": 'nofVisited', "type": "uint" },
                      { "name": 'ranks', "type": "DocumentRankList" }
               ]
       }
}

mainTemplate = "scripts/variantValueTemplate.hpp.tpl"
mainOutput = "src/variantValueTemplate.hpp"

def mapMainTemplate():
    tplfile = open( mainTemplate, 'r')
    content = tplfile.read()
    template = Template( content)
    includes = Set()
    for tpkey,tp in atomictypes.items():
        if "include" in tp:
            includes.add( tp['include'])
    output = template.render( atomictypes=atomictypes, includes=includes)
    outfile = open( mainOutput, 'w')
    outfile.write( output)

mapMainTemplate()


