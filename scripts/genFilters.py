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
                "variantcast": "%s",
                "fullname": 'double'
        },
        "uint": {
                "variantcast": "(bindings::ValueVariant::UIntType)%s",
                "fullname": 'unsigned int'
        },
        "int": {
                "variantcast": "(bindings::ValueVariant::IntType)%s",
                "fullname": 'int'
        },
        "bool": {
                "variantcast": "(bindings::ValueVariant::IntType)%s",
                "fullname": 'bool'
        },
        "charp": {
                "variantcast": "%s",
                "paramname": "const char*",
                "fullname": 'const char*'
        },
        "string": {
                "variantcast": "%s",
                "fullname": 'std::string',
                "includes": ['<string>']
        },
        "NumericVariant": {
                "variantcast": "%s",
                "fullname": 'NumericVariant',
                "includes": ['"strus/numericVariant.hpp"']
        },
        "QueryOpCode": {
                "variantcast": "analyzer::Query::Instruction::opCodeName( %s)",
                "fullname": 'analyzer::Query::Instruction::OpCode',
                "includes": ['"strus/analyzer/query.hpp"']
        },
        "QueryElementType": {
                "variantcast": "analyzer::Query::Element::typeName( %s)",
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
        },
        "Document": {
                "fullname": 'analyzer::Document',
                "includes": ['"strus/analyzer/document.hpp"',
                             '"strus/analyzer/metaData.hpp"',
                             '"strus/analyzer/attribute.hpp"',
                             '"strus/analyzer/term.hpp"'],
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
                "includes": ['"strus/analyzer/query.hpp"',
                             '"strus/analyzer/metaData.hpp"',
                             '"strus/analyzer/term.hpp"'],
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
        "ResultDocument": {
                "fullname": 'ResultDocument',
                "includes": ['"strus/resultDocument.hpp"'],
                "elements": [
                      { "name": 'docno', "type": "int" },
                      { "name": 'weight', "type": "double" },
                      { "name": 'summaryElements', "type": "SummaryElement[]" }
                ]
        },
        "QueryResult": {
                "fullname": 'QueryResult',
                "includes": ['"strus/queryResult.hpp"'],
                "elements": [
                      { "name": 'evaluationPass', "type": "uint" },
                      { "name": 'nofRanked', "type": "uint" },
                      { "name": 'nofVisited', "type": "uint" },
                      { "name": 'ranks', "type": "ResultDocument[]" }
                ]
        }
}

arraytypes = { "VectorRank", "Term", "string", "int", "double" }

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

def getFunction( funcname):
    def uc1( str):
        return str[:1].upper() + str[1:]

    def c_iteratortype( etype):
        if etype in atomictypes:
            return "std::vector<" + atomictypes[ etype] + ">::const_iterator"
        elif etype in structtypes:
            return "std::vector<" + structtypes[etype]['fullname'] + ">::const_iterator"
        else:
            raise Exception("type %s not defined" % etype)

    def serialization( resultname, srcelem, etype):
        class closurevar:
            varIndex = 0
            resultname = "res"
        def serialization_( srcelem, etype):
            rt = []
            if etype[-2:] == "[]":
                subtype = etype[:-2]
                itr = c_iteratortype( subtype)
                closurevar.varIndex += 1
                vidx = closurevar.varIndex
                rt += ["%s ii%u = %s.begin(), ie%u = %s.end(); " % (itr,vidx,srcelem,vidx,srcelem) ]
                rt += ["for (; ii%u != ie%u; ++ii%u)" % (vidx,vidx,vidx),"{",">>","%s.pushIndex();" % closurevar.resultname ]
                rt += serialization_( "(*%u)" % vidx, subtype)
                rt += ["<<","}"]
            elif etype in atomictypes:
                rt += ["%s.pushValue( %s);" % (closurevar.resultname,srcelem) ]
            elif etype in structtypes:
                elements = structtypes[etype]['elements']
                for element in elements:
                    subsrcelem = srcelem + "." + element["name"]
                    subtype = element["type"]
                    rt += [ "%s.pushOpen(\"%s\");" % (closurevar.resultname, element["name"]) ]
                    rt += serialization_( subsrcelem, subtype)
                    rt += [ "%s.pushClose();" % (closurevar.resultname) ]
            else:
                raise Exception("type %s not defined" % etype)
            return rt
        closurevar.varIndex = 0
        closurevar.resultname = resultname
        return serialization_( srcelem, etype)
    if funcname == "uc1":
        return uc1
    if funcname == "serialization":
        return serialization
    return None

def mapStructFilterTemplates():
    for tpkey,tp in structtypes.items():
        print >> sys.stderr, "map struct '%s'" % tpkey
        hpp_templatefile = "scripts/complexTypeFilter.hpp.tpl"
        cpp_templatefile = "scripts/complexTypeFilter.cpp.tpl"
        hpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.hpp"
        cpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.cpp"
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=tp['includes'], structname=tpkey, typename=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=tp['includes'], structname=tpkey, typename=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)

def mapArrayFilterTemplates():
    for tpkey in arraytypes:
        if tpkey in atomictypes:
            tp = atomictypes[tpkey]
        else:
            tp = structtypes[tpkey]
        print >> sys.stderr, "map array '%s'" % tpkey
        hpp_templatefile = "scripts/complexTypeFilter.hpp.tpl"
        cpp_templatefile = "scripts/complexTypeFilter.cpp.tpl"
        hpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "ArrayFilter.hpp"
        cpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "ArrayFilter.cpp"
        includes = []
        if 'includes' in tp:
            includes = tp['includes']
        elements = []
        if 'elements' in tp:
            elements = tp['elements']
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=includes, structname=tpkey+"Array", typename=tpkey+"[]", fullname='std::vector<' + tp['fullname'] + '>', elements=elements, func=getFunction)
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=includes, structname=tpkey+"Array", typename=tpkey+"[]", fullname='std::vector<' + tp['fullname'] + '>', elements=elements, func=getFunction)

mapStructFilterTemplates()
mapArrayFilterTemplates()

