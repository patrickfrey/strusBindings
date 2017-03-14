#!/usr/bin/python
from jinja2 import Template

atomictypes = {
        "Index": {
                "basictype": "int",
                "fullname": 'strus::Index',
                "include": '"strus/index.hpp"'
        },
        "double": {
                "basictype": "double",
                "fullname": 'double'
        },
        "uint": {
                "basictype": "unsigned int",
                "fullname": 'unsigned int'
        },
        "int": {
                "basictype": "int",
                "fullname": 'int'
        },
        "bool": {
                "basictype": "int",
                "fullname": 'bool'
        },
        "charp": {
                "basictype": "char*",
                "fullname": 'char*'
        },
        "string": {
                "basictype": "string",
                "fullname": 'std::string',
                "include": '<string>'
        },
        "NumericVariant": {
                "basictype": "NumericVariant",
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
               "include": '"strus/analyzer/term.hpp"'
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
               "include": '"strus/analyzer/metaData.hpp"'
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
               "include": '"strus/analyzer/attribute.hpp"'
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
               "include": '"strus/analyzer/document.hpp"'
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
               "include": '"strus/analyzer/query.hpp"'
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
               "include": '"strus/analyzer/query.hpp"'
               "elements": [
                      { "name": 'opCode', "type": "string" },
                      { "name": 'idx', "type": "uint" },
                      { "name": 'nofOperands', "type": "uint" }
               ]
       },
       "Query": {
               "fullname": 'strus::analyzer::Query',
               "include": '"strus/analyzer/query.hpp"'
               "elements": [
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'elements', "type": "QueryElement[]" }
                      { "name": 'instructions', "type": "QueryInstruction[]" }
               ]
       },
       "DocumentFrequencyChange": {
               "fullname": 'strus::bindings::DocumentFrequencyChange',
               "include": '"strus/bindings/statisticsMessage.hpp"'
               "elements": [
                      { "name": 'type', "type": "charp" },
                      { "name": 'value', "type": "charp" },
                      { "name": 'increment', "type": "int" }
               ]
       },
       "StatisticsMessage": {
               "fullname": 'strus::bindings::StatisticsMessage',
               "include": '"strus/bindings/statisticsMessage.hpp"'
               "elements": [
                      { "name": 'df', "type": "DocumentFrequencyChange[]" },
                      { "name": 'nofdocs', "type": "int" }
               ]
       },
       "VectorRank": {
               "fullname": 'VectorStorageSearchInterface::Result'
               "include": '"strus/vectorStorageSearchInterface.hpp"'
               "elements": [
                      { "name": 'featidx', "type": "Index" },
                      { "name": 'weight', "type": "double" }
               ]
       }
       "VectorRankList": {
               "fullname": 'std::vector<VectorStorageSearchInterface::Result>',
               "include": '"strus/vectorStorageSearchInterface.hpp"'
       },
       "SummaryElement": {
               "fullname": 'strus::SummaryElement'
               "include": '"strus/summaryElement.hpp"'
               "elements": [
                      { "name": 'name', "type": "string" },
                      { "name": 'value', "type": "string" }
                      { "name": 'weight', "type": "double" }
                      { "name": 'index', "type": "int" }
               ]
       }
       "SummaryElementList": {
               "fullname": 'std::vector<strus::SummaryElement>',
               "include": '"strus/summaryElement.hpp"'
       },
       "DocumentRank": {
               "fullname": 'strus::ResultDocument'
               "include": '"strus/resultDocument.hpp"'
               "elements": [
                      { "name": 'docno', "type": "Index" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'summaryElements', "type": "SummaryElementList" }
               ]
       }
       "DocumentRankList": {
               "fullname": 'std::vector<strus::ResultDocument>',
               "include": '"strus/resultDocument.hpp"'
       },
       "QueryResult": {
               "fullname": 'strus::QueryResult'
               "include": '"strus/queryResult.hpp"'
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
    content = tmpfile.read()
    template = Template( content)
    values = ()
    for key, val in filtertable.items():
        if (val.basictype):
            values.append( val)
    output = template.render( atomictypes=atomictypes)
    outfile = open( mainOutput, 'w')
    outfile.write( output)

mapMainTemplate()


