#!/usr/bin/perl

# -----------------------------------------------
# Get a map from enum identifiers to string
# from a comma separated list of enums as input
# -----------------------------------------------

use strict;
use warnings;

sub mapNamePart {
	my ($arg) = @_;
	if ($arg eq "dist") {
		return "distributed";
	}
	elsif ($arg eq "eval") {
		return "evaluation";
	}
	elsif ($arg eq "config") {
		return "configuration";
	}
	elsif ($arg eq "dir") {
		return "directory";
	}
	elsif ($arg eq "nof") {
		return "number of";
	}
	elsif ($arg eq "arg") {
		return "argument";
	}
	elsif ($arg eq "char") {
		return "character";
	}
	elsif ($arg eq "stats") {
		return "statistics";
	}
	elsif ($arg eq "docs") {
		return "documents";
	}
	elsif ($arg eq "doc") {
		return "document";
	}
	elsif ($arg eq "docno") {
		return "internal document number";
	}
	elsif ($arg eq "id") {
		return "identifier";
	}
	elsif ($arg eq "def") {
		return "definition";
	}
	elsif ($arg eq "len") {
		return "length";
	}
	elsif ($arg eq "pos") {
		return "position";
	}
	elsif ($arg eq "dim") {
		return "dimension";
	}
	elsif ($arg eq "op") {
		return "operator";
	}
	elsif ($arg eq "subst") {
		return "substitution";
	}
	elsif ($arg eq "min") {
		return "minumum";
	}
	elsif ($arg eq "max") {
		return "maximum";
	}
	elsif ($arg eq "res") {
		return "result";
	}
	else
	{
		return $arg;
	}
}

while (<>)
{
	my @names = split/,/;
	foreach my $nam( @names) {
		$nam =~ s/^\s*//;
		$nam =~ s/\s*$//;
		my $id = $nam;
		if ($nam ne "") {
			my @namparts = ();
			while ($nam =~ m/^([A-Z][a-z0-9]*)([A-Z].*)$/)
			{
				push( @namparts, mapNamePart( lc($1)));
				$nam = $2;
			}
			if ($nam ne "")
			{
				push( @namparts, mapNamePart( lc($nam)));
			}
			$nam = join(' ', @namparts);
			print( "{$id, \"$nam\"},\n");
		}
	}
}

