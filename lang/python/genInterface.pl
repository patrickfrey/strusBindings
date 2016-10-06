#!/usr/bin/perl

use strict;
use warnings;
use 5.014;

my $num_args = $#ARGV + 1;
if ($num_args == 0)
{
	print STDERR "Usage: genInterface.pl {<inputfile>}\n";
	print STDERR "<inputfile>: name of input file to process\n";
	exit;
}

sub getTokens
{
	my @rt = ();
	my ($src) = @_;
	my $tok = "";
	my $ch;
	foreach $ch( split('', $src))
	{
		if ($ch =~ m/[ \t]/)
		{
			if (length( $tok) > 0)
			{
				push @rt, $tok;
				$tok = "";
			}
		}
		elsif ($ch =~ m/[a-zA-Z0-9_:.]/)
		{
			$tok .= $ch;
		}
		else
		{
			if (length( $tok) > 0)
			{
				push @rt, $tok;
				$tok = "";
			}
			push @rt, $ch;
		}
	}
	return @rt;
}

my @tokens = {};
my $ti = 0;
my $te = 0;

sub initSource
{
	my ($src) = @_;
	@tokens = getTokens( $src);
	$ti = 0;
	$te = $#tokens + 1;
}

sub nextToken()
{
	if ($ti+1 >= $te)
	{
		return "";
	}
	return $tokens[ ++$ti];
}

sub prevToken()
{
	if ($ti > 0)
	{
		--$ti;
	}
}

sub currToken()
{
	if ($ti >= $te)
	{
		return "";
	}
	return $tokens[ $ti];
}

sub hasToken()
{
	if ($ti+1 >= $te)
	{
		return 0;
	}
	return 1;
}

sub skipBrackets
{
	my ($sb,$eb) = @_;
	my $bcnt = 1;
	while ($bcnt && hasToken())
	{
		my $tok = nextToken();
		if ($tok eq $eb)
		{
			--$bcnt;
		}
		elsif ($tok eq $sb)
		{
			++$bcnt;
		}
	}
	hasToken() or die "unexpected end of file while scanning for end bracket";
}

sub joinIdent
{
	my ($orig, $suffix) = @_;
	if ($orig eq "")
	{
		$orig = $suffix;
	}
	else
	{
		$orig = $orig . " " . $suffix;
	}
}

my %ignoreClassMap = ();
$ignoreClassMap{"Reference"} = 1;
my %ignoreMethodMap = ();
$ignoreMethodMap{"setMetaData"} = 1;
$ignoreMethodMap{"setAttribute"} = 1;
$ignoreMethodMap{"analyze"} = 1;
$ignoreMethodMap{"analyzePhrase"} = 1;
$ignoreMethodMap{"defineParameter"} = 1;
$ignoreMethodMap{"addMetaDataRestrictionCondition"} = 1;
$ignoreMethodMap{"addForwardIndexFeature"} = 1;
$ignoreMethodMap{"addSearchIndexFeature"} = 1;
$ignoreMethodMap{"pushExpression"} = 1;
$ignoreMethodMap{"pushTerm"} = 1;
$ignoreMethodMap{"push"} = 1;
$ignoreMethodMap{"attachVariable"} = 1;
$ignoreMethodMap{"defineFeature"} = 1;
$ignoreMethodMap{"defineTermStatistics"} = 1;
$ignoreMethodMap{"defineGlobalStatistics"} = 1;
$ignoreMethodMap{"addDocumentEvaluationSet"} = 1;
$ignoreMethodMap{"createDocumentAnalyzer"} = 1;
$ignoreMethodMap{"createStorageClient"} = 1;
$ignoreMethodMap{"createStorage"} = 1;
$ignoreMethodMap{"destroyStorage"} = 1;
$ignoreMethodMap{"detectDocumentClass"} = 1;
$ignoreMethodMap{"deleteDocument"} = 1;
$ignoreMethodMap{"deleteUserAccessRights"} = 1;
$ignoreMethodMap{"addResourcePath"} = 1;
$ignoreMethodMap{"addModulePath"} = 1;
$ignoreMethodMap{"addUserName"} = 1;
$ignoreMethodMap{"decode"} = 1;

my %renameMethodMap = ();
$renameMethodMap{"setMetaData_double"} = "setMetaData";
$renameMethodMap{"setMetaData_int"} = "setMetaData";
$renameMethodMap{"setMetaData_uint"} = "setMetaData";
$renameMethodMap{"setAttribute_obj"} = "setAttribute";
$renameMethodMap{"analyze_obj_1"} = "analyze";
$renameMethodMap{"analyze_obj_2"} = "analyze";
$renameMethodMap{"analyzePhrase_obj"} = "analyzePhrase";
$renameMethodMap{"push_obj"} = "push";
$renameMethodMap{"push_obj_1"} = "push";
$renameMethodMap{"push_obj_2"} = "push";
$renameMethodMap{"addMetaDataRestrictionCondition_double"} = "addMetaDataRestrictionCondition";
$renameMethodMap{"addMetaDataRestrictionCondition_int"} = "addMetaDataRestrictionCondition";
$renameMethodMap{"addMetaDataRestrictionCondition_uint"} = "addMetaDataRestrictionCondition";
$renameMethodMap{"addForwardIndexFeature_4"} = "addForwardIndexFeature";
$renameMethodMap{"addForwardIndexFeature_5"} = "addForwardIndexFeature";
$renameMethodMap{"addSearchIndexFeature_4"} = "addSearchIndexFeature";
$renameMethodMap{"addSearchIndexFeature_5"} = "addSearchIndexFeature";
$renameMethodMap{"pushTerm_obj"} = "pushTerm";
$renameMethodMap{"pushExpression_2"} = "pushExpression";
$renameMethodMap{"pushExpression_3"} = "pushExpression";
$renameMethodMap{"pushExpression_4"} = "pushExpression";
$renameMethodMap{"attachVariable_obj"} = "attachVariable";
$renameMethodMap{"defineFeature_2"} = "defineFeature";
$renameMethodMap{"defineFeature_3"} = "defineFeature";
$renameMethodMap{"defineFeature_expr_2"} = "defineFeature";
$renameMethodMap{"defineFeature_expr_3"} = "defineFeature";
$renameMethodMap{"defineTermStatistics_obj_struct"} = "defineTermStatistics";
$renameMethodMap{"defineGlobalStatistics_struct"} = "defineGlobalStatistics";
$renameMethodMap{"addDocumentEvaluationSet_struct"} = "addDocumentEvaluationSet";
$renameMethodMap{"createStorageClient_0"} = "createStorageClient";
$renameMethodMap{"createStorageClient_obj"} = "createStorageClient";
$renameMethodMap{"createStorage_obj"} = "createStorage";
$renameMethodMap{"destroyStorage_obj"} = "destroyStorage";
$renameMethodMap{"detectDocumentClass_obj"} = "detectDocumentClass";
$renameMethodMap{"createDocumentAnalyzer_obj"} = "createDocumentAnalyzer";
$renameMethodMap{"createDocumentAnalyzer_0"} = "createDocumentAnalyzer";
$renameMethodMap{"defineMetaData_obj"} = "defineMetaData";
$renameMethodMap{"defineAggregatedMetaData_obj"} = "defineAggregatedMetaData";
$renameMethodMap{"defineAttribute_obj"} = "defineAttribute";
$renameMethodMap{"definePhraseType_obj"} = "definePhraseType";
$renameMethodMap{"addSummarizer_obj"} = "addSummarizer";
$renameMethodMap{"addWeightingFunction_obj"} = "addWeightingFunction";
$renameMethodMap{"addWeightingFormula_obj"} = "addWeightingFormula";
$renameMethodMap{"setWeightingVariables_obj"} = "setWeightingVariables";
$renameMethodMap{"deleteDocument_obj"} = "deleteDocument";
$renameMethodMap{"deleteUserAccessRights_obj"} = "deleteUserAccessRights";
$renameMethodMap{"addResourcePath_obj"} = "addResourcePath";
$renameMethodMap{"addModulePath_obj"} = "addModulePath";
$renameMethodMap{"addUserName_obj"} = "addUserName";
$renameMethodMap{"decode_datablob"} = "decode";

sub parseType
{
	my $prefix = "";
	my $type = nextToken();
	my $typeprefix = "";
	while ($type eq "const")
	{
		$prefix = joinIdent( $prefix, $type);
		$type = nextToken();
	}
	while ($type eq "unsigned")
	{
		$typeprefix = $type . '_';
		$type = nextToken();
	}
	my $bcnt = 0;
	my $typepostfix = "";
	if ($type eq "std::vector")
	{
		$typepostfix = "[]";
		$type = "";
	}
	my $tok = nextToken();
	if ($tok eq "<")
	{
		$bcnt = 1;
		while (hasToken() && $bcnt > 0)
		{
			$tok = nextToken();
			if ($tok eq "unsigned" || $tok eq "const")
			{
				$typeprefix = $typeprefix . $tok . '_';
			}
			elsif ($tok eq "*")
			{
				$type .= $tok;
			}
			elsif ($tok eq "<")
			{
				++$bcnt;
			}
			elsif ($tok eq ">")
			{
				--$bcnt;
			}
			elsif ($tok =~ m/[;=\{\}\(\)]/)
			{
				die "expected '>' closing template";
			}
			else
			{
				$type = joinIdent( $type, $typeprefix . $tok);
				$typeprefix = "";
			}
		}
		if (!hasToken())
		{
			die "unexpected end of file in std::vector template argument";
		}
		$tok = nextToken();
	}
	while ($tok eq "*")
	{
		$prefix = $prefix . '^';
		$tok = nextToken();
	}
	if ($tok eq "&")
	{
		$prefix = $prefix . '&';
	}
	else
	{
		prevToken();
	}
	return joinIdent( $prefix, $typeprefix . $type . $typepostfix);
}

my @classlist = ();

sub parseClass
{
	my ($classname) = @_;
	my @methodlist = ();
	my $bcnt = 1;
	my $public = 0;
	my $operator = 0;
	nextToken();
	while (hasToken() && $bcnt > 0)
	{
		my $tok = currToken();
		if ($tok eq 'public:')
		{
			$public = 1;
			nextToken();
		}
		elsif ($tok eq 'private:')
		{
			$public = 0;
			nextToken();
		}
		elsif ($tok eq 'protected:')
		{
			$public = 0;
			nextToken();
		}
		elsif ($tok eq '}')
		{
			--$bcnt;
			nextToken();
		}
		elsif ($public == 0)
		{
			nextToken();
			if ($tok eq '{')
			{
				++$bcnt;
			}
			next;
		}
		elsif ($tok eq '{')
		{
			die "cannot handle nested structures";
		}
		elsif ($tok eq 'typedef')
		{
			while (nextToken() ne ';'){}
			nextToken();
		}
		elsif ($tok eq 'friend')
		{
			while (nextToken() ne ';'){}
			nextToken();
		}
		elsif ($tok eq 'explicit')
		{
			nextToken();
		}
		elsif ($bcnt == 1)
		{
			if ($tok eq '~')
			{
				nextToken();
				next;
			}
			prevToken();
			my $returntype = parseType();
			my $methodname;
			if ($returntype eq $classname)
			{
				$methodname = $returntype;
				$returntype = "";
			}
			else
			{
				$methodname = nextToken();
			}
			if ($methodname eq "(")
			{
				my $mm;
				while (($mm = nextToken()) ne ')')
				{
					$methodname .= $mm;
				}
			}
			if ($methodname eq "operator")
			{
				$operator = 1;
				$methodname = nextToken();
				if ($methodname eq '!' || $methodname eq '=')
				{
					if (nextToken() eq '=')
					{
						$methodname .= '=';
					}
					else
					{
						prevToken();
					}
				}
			}
			else
			{
				$operator = 0;
			}
			my @paramlist = ();

			$tok = nextToken();
			if ($tok eq ';')
			{
				$tok = nextToken();
				next;
			}
			if ($tok ne "(")
			{
				die "syntax error parsing method: start of argument list expected";
			}
			while (nextToken() ne ")")
			{
				prevToken();
				$tok = parseType();
				push @paramlist, $tok;
				$tok = nextToken();

				if ($tok =~ m/^[a-zA-Z_][a-zA-Z0-9_]*$/)
				{
					$tok = nextToken();
					if ($tok eq '=')
					{
						$tok = nextToken(); #... skip argument of assign
						if ($tok eq '"')
						{
							while (nextToken() ne '"'){}
							$tok = nextToken();
						}
						elsif ($tok eq "'")
						{
							while (nextToken() ne "'"){}
							$tok = nextToken();
						}
						else
						{
							$tok = nextToken();
						}
						if ($tok eq '(')
						{
							skipBrackets( "(",")");
							$tok = nextToken();
						}
					}
				}
				if ($tok eq ")")
				{
					prevToken();
				}
				elsif ($tok ne ",")
				{
					die "expected ',' as separator or ')' as end of parameter list";
				}
			}
			$tok = nextToken();
			if ($tok eq "const")
			{
				$tok = nextToken();
			}
			if ($tok =~ m/^[:].*/ || $tok eq '{')
			{
				while (nextToken() ne '}'){}
				$tok = nextToken();
			}
			if ($tok eq ';')
			{
				$tok = nextToken();
			}
			if ($methodname ne $classname)
			{
				if ($operator == 0)
				{
					my $return_value_policy = "";
					if ($returntype =~ m/const[&]/)
					{
						$return_value_policy = "copy_const_reference";
					}
					push( @methodlist, $methodname . "/" . $return_value_policy);
				}
			}
		}
		else
		{
			nextToken();
		}
	}
	push( @classlist, $classname . "%" . join( "%", @methodlist));
	hasToken() or die "unexpected end of source parsing class";
}


my @inputfiles = @ARGV;
my $inputfile;
foreach $inputfile( @inputfiles)
{
	print STDERR "process file $inputfile\n";
	open( INPUT, "<$inputfile") or die "Couldn't open file $inputfile, $!";

	my $line;
	my $src;
	while (<INPUT>)
	{
		chomp;
		if (/^[\#]/)
		{
			$line = '';
		}
		elsif (/(^.*)[\/][\/][\/]/)
		{
			$line = $1;
		}
		elsif (/(^.*)[\/][\/]/)
		{
			$line = $1;
		}
		else
		{
			$line = $_;
		}
		$line =~ s/\t+/ /g;
		$src .= $line . ' ';
	}
	$src =~ s@/\*[^\*]*\*/@@g;

	initSource( $src);

	while (hasToken())
	{
		my $tok = currToken();
		if ($tok eq "class")
		{
			my $interfacename = nextToken();
			if (nextToken() eq "{")
			{
				if (nextToken() ne "}")
				{
					prevToken();
					parseClass( $interfacename);
				}
			}
		}
		++$ti;
	}
}


my $outputfile = "strusPythonModule.cpp";
open( OUTFILE, ">$outputfile") or die "Couldn't open file $outputfile, $!";

print OUTFILE <<EOF;
// ! THIS IS A GENERATED FILE. DO NOT MODYFY IT. CALL genInterface.sh TO RECREATE IT !
/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "strus/bindingObjects.hpp"
#include <string>
#include <stdexcept>

namespace bp = boost::python;

namespace {
void translate_runtime_error( std::runtime_error const& err)
{
	PyErr_SetString( PyExc_Exception, err.what());
}
void translate_bad_alloc( std::bad_alloc const& err)
{
	PyErr_SetString( PyExc_MemoryError, "out of memory");
}
void translate_logic_error( std::logic_error const& err)
{
	PyErr_SetString( PyExc_AssertionError, err.what());
}
void translate_exception( std::exception const& err)
{
	PyErr_SetString( PyExc_Exception, err.what());
}
}

BOOST_PYTHON_MODULE(strus)
{
bp::register_exception_translator<std::runtime_error>( translate_runtime_error);
bp::register_exception_translator<std::bad_alloc>( translate_bad_alloc);
bp::register_exception_translator<std::logic_error>( translate_logic_error);
bp::register_exception_translator<std::exception>( translate_exception);

bp::class_<TermVector>("TermVector") .def( bp::vector_indexing_suite<TermVector>());
bp::class_<RankVector>("RankVector") .def( bp::vector_indexing_suite<RankVector>());
bp::class_<SummaryElementVector>("SummaryElementVector") .def( bp::vector_indexing_suite<SummaryElementVector>());
bp::class_<StringVector>("StringVector") .def( bp::vector_indexing_suite<StringVector>());
bp::class_<AttributeVector>("AttributeVector") .def( bp::vector_indexing_suite<AttributeVector>());
bp::class_<MetaDataVector>("MetaDataVector") .def( bp::vector_indexing_suite<MetaDataVector>());
bp::class_<DocumentFrequencyChangeVector>("DocumentFrequencyChangeVector") .def( bp::vector_indexing_suite<DocumentFrequencyChangeVector>());
EOF

my $classdef;
foreach $classdef( @classlist)
{
	my @itemlist = split( '%', $classdef);
	my $classname = shift( @itemlist);

	if (!$ignoreClassMap{$classname})
	{
		if ($classname eq "Context")
		{
			print OUTFILE "bp" . '::class_<Context>("Context",bp::init<>())' . "\n";
			print OUTFILE "\t" . '.def(bp::init<const unsigned int>())' . "\n";
			print OUTFILE "\t" . '.def(bp::init<const unsigned int, const std::string&>())' . "\n";
			print OUTFILE "\t" . '.def(bp::init<const std::string&>())' . "\n";
			print OUTFILE "\t" . '.def(bp::init<const std::string&, unsigned int>())' . "\n";
		}
		else
		{
			print OUTFILE "bp" . "::class_<$classname>(\"$classname\")\n";
		}
		my $method;
		foreach $method( @itemlist)
		{
			my ($methodname, $return_value_policy) = split( '/', $method);
			if (!$ignoreMethodMap{$methodname})
			{
				my $methodid = $methodname;
				if ($renameMethodMap{$methodname})
				{
					$methodid = $renameMethodMap{$methodname};
				}
				if ($return_value_policy ne "")
				{
					print OUTFILE "\t.def(\"$methodid\", &" . "$classname" . "::" . "$methodname, bp::return_value_policy<bp::" . "$return_value_policy>())\n";
				}
				else
				{
					print OUTFILE "\t.def(\"$methodid\", &" . "$classname" . "::" . "$methodname)\n";
				}
			}
		}
		print OUTFILE ";\n";
	}
}
print OUTFILE <<EOF;
}
EOF
close OUTFILE;

