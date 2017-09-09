#!/usr/bin/perl

use strict;
use warnings;
use 5.010;

my $num_args = $#ARGV + 1;
if ($num_args == 0)
{
	print STDERR "Usage: splitLuaDocFile.pl <inputfile>\n";
	print STDERR "<inputfile>: name of input file to process\n";
	exit;
}


my $inputfile = $ARGV[0];
my $extension = "";
if ($inputfile =~ m/([\.][^\.]+)$/)
{
	$extension = $1;
}
print STDERR "process file $inputfile\n";
open( INPUT, "<$inputfile") or die "Couldn't open file $inputfile, $!";

sub writeFile
{
	my ($filename, $content) = @_;
	open( OUTFILE, ">$filename") or die "Couldn't open file $filename, $!";
	print OUTFILE $content;
	close OUTFILE;
}

my $header = "";
my $content = "";
my $outputfile = undef;
my $line;
my $src;
while (<INPUT>)
{
	chomp;
	$line = $_;
	if (/^[\-][\-]\s*[@]file\s+(\S.*)$/)
	{
		if ($outputfile)
		{
			writeFile( $outputfile . $extension, $content);
		}
		$content = $header;
		$outputfile = $1;
	}
	elsif ($outputfile)
	{
		$content .= $line . "\n";
	}
	else
	{
		$header .= $line . "\n";
	}
}

if ($outputfile)
{
	writeFile( $outputfile . $extension, $content);
}
else
{
	die "no output file tag found";
}

