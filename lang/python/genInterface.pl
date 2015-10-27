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
$ignoreMethodMap{"analyze"} = 1;
$ignoreMethodMap{"defineParameter"} = 1;
$ignoreMethodMap{"defineMetaDataRestriction"} = 1;
$ignoreMethodMap{"addForwardIndexFeature"} = 1;
$ignoreMethodMap{"addSearchIndexFeature"} = 1;
$ignoreMethodMap{"pushExpression"} = 1;
$ignoreMethodMap{"defineFeature"} = 1;
$ignoreMethodMap{"createDocumentAnalyzer"} = 1;
my %renameMethodMap = ();
$renameMethodMap{"setMetaData_double"} = "setMetaData";
$renameMethodMap{"setMetaData_int"} = "setMetaData";
$renameMethodMap{"setMetaData_uint"} = "setMetaData";
$renameMethodMap{"setAttribute_unicode"} = "setAttribute";
$renameMethodMap{"analyze_unicode_1"} = "analyze";
$renameMethodMap{"analyze_unicode_2"} = "analyze";
$renameMethodMap{"analyze_1"} = "analyze";
$renameMethodMap{"analyze_2"} = "analyze";
$renameMethodMap{"analyzePhrase_unicode"} = "analyzePhrase";
$renameMethodMap{"analyzePhrase"} = "analyzePhrase";
$renameMethodMap{"push_unicode"} = "push";
$renameMethodMap{"defineParameter_string"} = "defineParameter";
$renameMethodMap{"defineParameter_charp"} = "defineParameter";
$renameMethodMap{"defineParameter_int"} = "defineParameter";
$renameMethodMap{"defineParameter_uint"} = "defineParameter";
$renameMethodMap{"defineParameter_double"} = "defineParameter";
$renameMethodMap{"defineParameter_unicode"} = "defineParameter";
$renameMethodMap{"defineMetaDataRestriction_double_3"} = "defineMetaDataRestriction";
$renameMethodMap{"defineMetaDataRestriction_double_4"} = "defineMetaDataRestriction";
$renameMethodMap{"defineMetaDataRestriction_int_3"} = "defineMetaDataRestriction";
$renameMethodMap{"defineMetaDataRestriction_int_4"} = "defineMetaDataRestriction";
$renameMethodMap{"defineMetaDataRestriction_uint_3"} = "defineMetaDataRestriction";
$renameMethodMap{"defineMetaDataRestriction_uint_4"} = "defineMetaDataRestriction";
$renameMethodMap{"addForwardIndexFeature_4"} = "addForwardIndexFeature";
$renameMethodMap{"addForwardIndexFeature_5"} = "addForwardIndexFeature";
$renameMethodMap{"addSearchIndexFeature_4"} = "addSearchIndexFeature";
$renameMethodMap{"addSearchIndexFeature_5"} = "addSearchIndexFeature";
$renameMethodMap{"pushExpression2"} = "pushExpression";
$renameMethodMap{"pushExpression3"} = "pushExpression";
$renameMethodMap{"pushExpression4"} = "pushExpression";
$renameMethodMap{"defineFeature1"} = "defineFeature";
$renameMethodMap{"defineFeature2"} = "defineFeature";
$renameMethodMap{"createStorageClient_unicode"} = "createStorageClient";
$renameMethodMap{"createStorage_unicode"} = "createStorage";
$renameMethodMap{"destroyStorage_unicode"} = "destroyStorage";
$renameMethodMap{"detectDocumentClass_unicode"} = "detectDocumentClass";
$renameMethodMap{"createDocumentAnalyzer_unicode"} = "createDocumentAnalyzer";
$renameMethodMap{"createDocumentAnalyzer_0"} = "createDocumentAnalyzer";
$renameMethodMap{"createDocumentAnalyzer_1"} = "createDocumentAnalyzer";
$renameMethodMap{"defineMetaData_obj"} = "defineMetaData";
$renameMethodMap{"defineAggregatedMetaData_obj"} = "defineAggregatedMetaData";
$renameMethodMap{"defineAttribute_obj"} = "defineAttribute";
$renameMethodMap{"definePhraseType_obj"} = "definePhraseType";
$renameMethodMap{"addSummarizer_obj"} = "addSummarizer";
$renameMethodMap{"addWeightingFunction_obj"} = "addWeightingFunction";
$renameMethodMap{"deleteDocument_unicode"} = "deleteDocument";
$renameMethodMap{"deleteUserAccessRights_unicode"} = "deleteUserAccessRights";
$renameMethodMap{"pushTerm_unicode"} = "pushTerm";
$renameMethodMap{"addResourcePath_unicode"} = "addResourcePath";
$renameMethodMap{"addModulePath_unicode"} = "addModulePath";
$renameMethodMap{"addUserName_unicode"} = "addUserName";

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
				if ($public != 0 && $operator == 0)
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
/*

! THIS IS A GENERATED FILE. DO NOT MODYFY IT. CALL genInterface.sh TO RECREATE IT !

---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include <string>

namespace bp = boost::python;

BOOST_PYTHON_MODULE(strus)
{
bp::class_<TermVector>("TermVector") .def( bp::vector_indexing_suite<TermVector>());
bp::class_<RankVector>("RankVector") .def( bp::vector_indexing_suite<RankVector>());
bp::class_<RankAttributeVector>("RankAttributeVector") .def( bp::vector_indexing_suite<RankAttributeVector>());
bp::class_<StringVector>("StringVector") .def( bp::vector_indexing_suite<StringVector>());
bp::class_<AttributeVector>("AttributeVector") .def( bp::vector_indexing_suite<AttributeVector>());
bp::class_<MetaDataVector>("MetaDataVector") .def( bp::vector_indexing_suite<MetaDataVector>());
EOF

my $classdef;
foreach $classdef( @classlist)
{
	my @itemlist = split( '%', $classdef);
	my $classname = shift( @itemlist);

	if (!$ignoreClassMap{$classname})
	{
		print OUTFILE "bp" . "::class_<$classname>(\"$classname\")\n";
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
};
EOF
close OUTFILE;

