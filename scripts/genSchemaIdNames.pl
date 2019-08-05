#!/usr/bin/perl

# -----------------------------------------------
# Get a map from enum identifiers to string
# from a comma separated list of enums as input
# -----------------------------------------------

use strict;
use warnings;

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
				push( @namparts, lc($1));
				$nam = $2;
			}
			if ($nam ne "")
			{
				push( @namparts, lc($nam));
			}
			$nam = join(' ', @namparts);
			print( "{$id, \"$nam\"},\n");
		}
	}
}

