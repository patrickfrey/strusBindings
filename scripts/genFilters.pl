#!/usr/bin/perl

use strict;
use warnings;
use 5.010;

my $num_args = $#ARGV + 1;
if ($num_args == 0)
{
	print STDERR "Usage: genInterface.pl <inputfile>\n";
	print STDERR "<inputfile>: name of input file to process\n";
	exit;
}
my $inputfile = $ARGV[0];

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
