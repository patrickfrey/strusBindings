#!/usr/bin/python
from jinja2 import Template
from sets import Set
import sys

atomictypes = {
        "double": {
                "variantcast": "val",
                "fullname": 'double'
        },
        "uint": {
                "variantcast": "(bindings::ValueVariant::UIntType)val",
                "fullname": 'unsigned int'
        },
        "int": {
                "variantcast": "(bindings::ValueVariant::IntType)val",
                "fullname": 'int'
        },
        "bool": {
                "variantcast": "(bindings::ValueVariant::IntType)val",
                "fullname": 'bool'
        },
        "charp": {
                "variantcast": "val",
                "paramname": "const char*",
                "fullname": 'const char*'
        },
        "string": {
                "variantcast": "val.c_str(), val.size()",
                "fullname": 'std::string',
                "includes": ['<string>']
        },
        "NumericVariant": {
                "variantcast": "val",
                "fullname": 'strus::NumericVariant',
                "includes": ['"strus/numericVariant.hpp"']
        }
}

structtypes = {
        "Term": {
                "fullname": 'strus::analyzer::Term',
                "includes": ['"strus/analyzer/term.hpp"'],
                "elements": [
                       { "name": 'type', "type": "string" },
                       { "name": 'value', "type": "string" },
                       { "name": 'pos', "type": "uint" },
                       { "name": 'len', "type": "uint" }
                ]
        },
        "Attribute": {
                "fullname": 'strus::analyzer::Attribute',
                "includes": ['"strus/analyzer/attribute.hpp"'],
                "elements": [
                       { "name": 'name', "type": "string" },
                       { "name": 'value', "type": "string" }
                ]
        },
        "MetaData": {
                "fullname": 'strus::analyzer::MetaData',
                "includes": ['"strus/analyzer/metaData.hpp"'],
                "elements": [
                       { "name": 'name', "type": "string" },
                       { "name": 'value', "type": "NumericVariant" }
                ]
        },
        "QueryElement": {
                "fullname": 'strus::analyzer::Query::Element',
                "includes": ['"strus/analyzer/query.hpp"'],
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
                "includes": ['"strus/analyzer/query.hpp"'],
                "elements": [
                       { "name": 'opCode', "type": "string" },
                       { "name": 'idx', "type": "uint" },
                       { "name": 'nofOperands', "type": "uint" }
                ]
        },
        "DocumentFrequencyChange": {
                "fullname": 'strus::bindings::DocumentFrequencyChange',
                "includes": ['"strus/bindings/statisticsMessage.hpp"'],
                "elements": [
                       { "name": 'type', "type": "charp" },
                       { "name": 'value', "type": "charp" },
                       { "name": 'increment', "type": "int" }
                ]
        },
        "VectorRank": {
                "fullname": 'VectorStorageSearchInterface::Result',
                "includes": ['"strus/vectorStorageSearchInterface.hpp"'],
                "elements": [
                       { "name": 'featidx', "type": "Index" },
                       { "name": 'weight', "type": "double" }
                ]
        },
        "SummaryElement": {
                "fullname": 'strus::SummaryElement',
                "includes": ['"strus/summaryElement.hpp"'],
                "elements": [
                       { "name": 'name', "type": "string" },
                       { "name": 'value', "type": "string" },
                       { "name": 'weight', "type": "double" },
                       { "name": 'index', "type": "int" }
                ]
        }
}

complextypes = {
        "StrusObjectType": {
               "fullname": 'strus::bindings::StrusObjectType',
               "includes": ['"strus/bindings/hostReference.hpp"']
       },
       "TermList"      : {
               "fullname": 'std::vector<strus::analyzer::Term>',
               "includes": ['"strus/analyzer/term.hpp"']
       },
       "MetaDataList": {
               "fullname": 'std::vector<strus::analyzer::MetaData>',
               "includes": ['"strus/analyzer/metaData.hpp"']
       },
       "AttributeList": {
               "fullname": 'std::vector<strus::analyzer::Attribute>',
               "includes": ['"strus/analyzer/attribute.hpp"']
       },
       "Document": {
               "fullname": 'strus::analyzer::Document',
               "includes": ['"strus/analyzer/document.hpp"'],
               "elements": [
                      { "name": 'subDocumentTypeName', "type": "string" },
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'attributes', "type": "Attribute[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'forwardTerms', "type": "Term[]" }
               ]
       },
       "Query": {
               "fullname": 'strus::analyzer::Query',
               "includes": ['"strus/analyzer/query.hpp"'],
               "elements": [
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'elements', "type": "QueryElement[]" },
                      { "name": 'instructions', "type": "QueryInstruction[]" }
               ]
       },
       "StatisticsMessage": {
               "fullname": 'strus::bindings::StatisticsMessage',
               "includes": ['"strus/bindings/statisticsMessage.hpp"'],
               "elements": [
                      { "name": 'df', "type": "DocumentFrequencyChange[]" },
                      { "name": 'nofdocs', "type": "int" }
               ]
       },
       "VectorRankList": {
               "fullname": 'std::vector<VectorStorageSearchInterface::Result>',
               "includes": ['"strus/vectorStorageSearchInterface.hpp"'],
       },
       "SummaryElementList": {
               "fullname": 'std::vector<strus::SummaryElement>',
               "includes": ['"strus/summaryElement.hpp"']
       },
       "DocumentRank": {
               "fullname": 'strus::ResultDocument',
               "includes": ['"strus/resultDocument.hpp"'],
               "elements": [
                      { "name": 'docno', "type": "Index" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'summaryElements', "type": "SummaryElementList" }
               ]
       },
       "DocumentRankList": {
               "fullname": 'std::vector<strus::ResultDocument>',
               "includes": ['"strus/resultDocument.hpp"']
       },
       "QueryResult": {
               "fullname": 'strus::QueryResult',
               "includes": ['"strus/queryResult.hpp"'],
               "elements": [
                      { "name": 'evaluationPass', "type": "uint" },
                      { "name": 'nofRanked', "type": "uint" },
                      { "name": 'nofVisited', "type": "uint" },
                      { "name": 'ranks', "type": "DocumentRankList" }
               ]
       }
}

def mapTemplateFile( outFilename, tplFilename, **kwargs):
    tplfile = open( tplFilename, 'r')
    content = tplfile.read()
    template = Template( content)
    output = template.render( **kwargs)
    outfile = open( outFilename, 'w')
    outfile.write( output)

def mapMainTemplate( mainOutput, mainTemplate):
    includes = Set()
    for tpkey,tp in atomictypes.items():
        if "includes" in tp:
            for incfile in tp['includes']:
                includes.add( incfile)
    mapTemplateFile( mainOutput, mainTemplate, atomictypes=atomictypes, includes=includes)

def mapStructFilterTemplates():
    for tpkey,tp in structtypes.items():
        print >> sys.stderr, "map struct '%s'" % tpkey
        hpp_templatefile = "scripts/structTypeFilter.hpp.tpl"
        cpp_templatefile = "scripts/structTypeFilter.cpp.tpl"
        hpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.hpp"
        cpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.cpp"
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'])
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'])

# Main:
mapMainTemplate(
        "src/variantValueTemplate.hpp",
        "scripts/variantValueTemplate.hpp.tpl"
)

mapStructFilterTemplates()

