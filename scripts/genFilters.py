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
        }
}

complextypes = {
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
    def firststate( prefix, etype):
        if etype[-2:] == "[]":
            return prefix + "Index"
        if etype in atomictypes:
            return prefix + "Value"
        elif etype in structtypes or etype in complextypes:
            if etype in structtypes:
                elements = structtypes[etype]['elements']
            else:
                elements = complextypes[etype]['elements']
            if elements:
                return prefix + uc1(elements[0]["name"]) + "Open"
            else:
                return None
        raise Exception("type %s not defined" % etype)
    def statelist( prefix, etype):
        indexState = []
        if etype[-2:] == "[]":
            indexState = [prefix + "Index"]
            etype = etype[:-2]
        if etype in atomictypes:
            return indexState + [prefix + "Value"]
        elif etype in structtypes or etype in complextypes:
            if etype in structtypes:
                elements = structtypes[etype]['elements']
            else:
                elements = complextypes[etype]['elements']

            rt = indexState
            for element in elements:
                subprefix = prefix + uc1(element["name"])
                rt += [subprefix + "Open"] + statelist( subprefix, element["type"]) +  [subprefix + "Close"]
            return rt
    def statestructlist( prefix, etype, nextstate):
        class closurevar:
            tagnameIndex = -1
            arrayIndex = -1
        def statestructlist_( prefix, etype, nextstate, valueIndex):
            rt = []
            if etype[-2:] == "[]":
                startstate = firststate( prefix, etype[:-2])
                if startstate:
                    rt += ["{" + prefix + "Index, _INDEX, " + startstate + ", " + prefix + "Index, _TAG, -1, -1}"]
                    rt += statestructlist_( prefix, etype[:-2], prefix + "Index", valueIndex)
                else:
                    rt += ["{" + prefix + "Index, _INDEX, " + prefix + "Index, " + prefix + "Index, _TAG, -1, -1}"]
            elif etype in atomictypes:
                rt += ["{" + prefix + "Value, _VALUE, " + nextstate + ", " + nextstate + ", _ELEM, -1, %u}" % valueIndex]
            elif etype in structtypes or etype in complextypes:
                if etype in structtypes:
                    elements = structtypes[etype]['elements']
                else:
                    elements = complextypes[etype]['elements']
                for eidx,element in enumerate( elements):
                    closurevar.tagnameIndex += 1
                    if eidx+1 == len(elements):
                        followstate = nextstate
                    else:
                        followstate = prefix + uc1(elements[ eidx+1]["name"] + "Open")
                    subprefix = prefix + uc1(element["name"])
                    startstate = firststate(subprefix, element["type"])
                    if startstate:
                        rt += ["{" + subprefix + "Open, _OPEN, " + startstate + ", " + followstate + ", _TAG, %u, -1}" % closurevar.tagnameIndex]
                        rt += statestructlist_( subprefix, element["type"], subprefix + "Close", eidx)
                        rt += ["{" + subprefix + "Close, _CLOSE, " + followstate + ", " + followstate + ", _NULL, -1, -1}"]
                    else:
                        rt += ["{" + subprefix + "Open, _OPEN, " + subprefix + "Close, " + followstate + ", _TAG, %u, -1}" % closurevar.tagnameIndex]
                        rt += ["{" + subprefix + "Close, _CLOSE, " + followstate + ", " + followstate + ", _NULL, -1, -1}"]
            return rt
        closurevar.tagnameIndex = -1
        closurevar.arrayIndex = -1
        return statestructlist_( prefix, etype, nextstate, 0)
    def valueaccesslist( prefix, etype):
        def valueaccesslist_( prefix, etype, arrayIndex):
            rt = []
            if etype[-2:] == "[]":
                rt += valueaccesslist_( prefix + "[m_index[" +  str(arrayIndex) + "]]", etype[:-2], arrayIndex+1)
            elif etype in atomictypes:
                rt.append( atomictypes[etype]['variantcast'] % prefix)
            elif etype in structtypes:
                rt = []
                elements = structtypes[etype]['elements']
                for eidx,element in enumerate(elements):
                    rt += valueaccesslist_( prefix + "." + element["name"] + "()", element["type"], arrayIndex)
            elif etype in complextypes:
                rt = []
                elements = complextypes[etype]['elements']
                for eidx,element in enumerate(elements):
                    rt += valueaccesslist_( prefix + "." + element["name"] + "()", element["type"], arrayIndex)
            return rt
        return valueaccesslist_( prefix, etype, 0)
    def memberlist( etype):
        rt = []
        if etype[-2:] == "[]":
            rt = memberlist( etype[:-2])
        elif etype in atomictypes:
            rt = []
        elif etype in structtypes or etype in complextypes:
            if etype in structtypes:
                elements = structtypes[etype]['elements']
            else:
                elements = complextypes[etype]['elements']
            for element in elements:
                rt.append( element["name"])
                rt += memberlist( element["type"])
            print "++++ GET MEMBERS: %s {%s}" % (etype,rt)
        else:
            raise Exception("type %s not defined" % etype)
        return rt
    if funcname == "uc1":
        return uc1
    if funcname == "statelist":
        return statelist
    if funcname == "statestructlist":
        return statestructlist
    if funcname == "valueaccesslist":
        return valueaccesslist
    if funcname == "memberlist":
        return memberlist
    return None

def mapStructFilterTemplates():
    for tpkey,tp in structtypes.items():
        print >> sys.stderr, "map struct '%s'" % tpkey
        hpp_templatefile = "scripts/structTypeFilter.hpp.tpl"
        cpp_templatefile = "scripts/structTypeFilter.cpp.tpl"
        hpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.hpp"
        cpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.cpp"
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)

def mapComplexFilterTemplates():
    for tpkey,tp in complextypes.items():
        print >> sys.stderr, "map complex struct '%s'" % tpkey
        hpp_templatefile = "scripts/complexTypeFilter.hpp.tpl"
        cpp_templatefile = "scripts/complexTypeFilter.cpp.tpl"
        hpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.hpp"
        cpp_outputfile = "src/filter/" + tpkey[:1].lower() + tpkey[1:] + "Filter.cpp"
        mapTemplateFile( hpp_outputfile, hpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)
        mapTemplateFile( cpp_outputfile, cpp_templatefile, includes=tp['includes'], structname=tpkey, fullname=tp['fullname'], elements=tp['elements'], func=getFunction)

mapStructFilterTemplates()
mapComplexFilterTemplates()

