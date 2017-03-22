#!/usr/bin/python
#
#  Copyright (c) 2017 Patrick P. Frey
# 
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# 
#  This script is used to generate some source articacts for the strus bindings
#  It is called manually when interface changes occurr and the generated artefacts
#  are checked in. Do not call this script until you know what you are doing!
# 

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
                "fullname": 'NumericVariant',
                "includes": ['"strus/numericVariant.hpp"']
        },
        "QueryOpCode": {
                "variantcast": "analyzer::Query::Instruction::opCodeName( val)",
                "fullname": 'analyzer::Query::Instruction::OpCode',
                "includes": ['"strus/analyzer/query.hpp"']
        },
        "QueryElementType": {
                "variantcast": "analyzer::Query::Element::typeName( val)",
                "fullname": 'analyzer::Query::Element::Type',
                "includes": ['"strus/analyzer/query.hpp"']
        }
}

structtypes = {
        "Term": {
                "fullname": 'analyzer::Term',
                "includes": ['"strus/analyzer/term.hpp"'],
                "elements": [
                       { "name": 'type', "type": "string" },
                       { "name": 'value', "type": "string" },
                       { "name": 'pos', "type": "uint" },
                       { "name": 'len', "type": "uint" }
                ]
        },
        "Attribute": {
                "fullname": 'analyzer::Attribute',
                "includes": ['"strus/analyzer/attribute.hpp"'],
                "elements": [
                       { "name": 'name', "type": "string" },
                       { "name": 'value', "type": "string" }
                ]
        },
        "MetaData": {
                "fullname": 'analyzer::MetaData',
                "includes": ['"strus/analyzer/metaData.hpp"'],
                "elements": [
                       { "name": 'name', "type": "string" },
                       { "name": 'value', "type": "NumericVariant" }
                ]
        },
        "QueryElement": {
                "fullname": 'analyzer::Query::Element',
                "includes": ['"strus/analyzer/query.hpp"'],
                "elements": [
                       { "name": 'type', "type": "QueryElementType" },
                       { "name": 'idx', "type": "uint" },
                       { "name": 'position', "type": "uint" },
                       { "name": 'length', "type": "uint" },
                       { "name": 'fieldNo', "type": "uint" }
                ]
        },
        "QueryInstruction": {
                "fullname": 'analyzer::Query::Instruction',
                "includes": ['"strus/analyzer/query.hpp"'],
                "elements": [
                       { "name": 'opCode', "type": "QueryOpCode" },
                       { "name": 'idx', "type": "uint" },
                       { "name": 'nofOperands', "type": "uint" }
                ]
        },
        "DocumentFrequencyChange": {
                "fullname": 'bindings::DocumentFrequencyChange',
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
                       { "name": 'featidx', "type": "int" },
                       { "name": 'weight', "type": "double" }
                ]
        },
        "SummaryElement": {
                "fullname": 'SummaryElement',
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
       "Document": {
               "fullname": 'analyzer::Document',
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
               "fullname": 'analyzer::Query',
               "includes": ['"strus/analyzer/query.hpp"'],
               "elements": [
                      { "name": 'metadata', "type": "MetaData[]" },
                      { "name": 'searchTerms', "type": "Term[]" },
                      { "name": 'elements', "type": "QueryElement[]" },
                      { "name": 'instructions', "type": "QueryInstruction[]" }
               ]
       },
       "StatisticsMessage": {
               "fullname": 'bindings::StatisticsMessage',
               "includes": ['"strus/bindings/statisticsMessage.hpp"'],
               "elements": [
                      { "name": 'df', "type": "DocumentFrequencyChange[]" },
                      { "name": 'nofdocs', "type": "int" }
               ]
       },
       "SummaryElementList": {
               "fullname": 'std::vector<SummaryElement>',
               "includes": ['"strus/summaryElement.hpp"']
       },
       "DocumentRank": {
               "fullname": 'ResultDocument',
               "includes": ['"strus/resultDocument.hpp"'],
               "elements": [
                      { "name": 'docno', "type": "Index" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'summaryElements', "type": "SummaryElementList" }
               ]
       },
       "DocumentRankList": {
               "fullname": 'std::vector<ResultDocument>',
               "includes": ['"strus/resultDocument.hpp"']
       },
       "QueryResult": {
               "fullname": 'QueryResult',
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
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], atomictypes=atomictypes)
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], atomictypes=atomictypes)

# Main:
mapMainTemplate(
        "src/filter/variantValueTemplate.hpp",
        "scripts/variantValueTemplate.hpp.tpl"
)

mapStructFilterTemplates()

