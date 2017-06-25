#!/usr/bin/perl

use strict;
use warnings;
use 5.010;

my $num_args = $#ARGV + 1;
if ($num_args == 0)
{
        print STDERR "Usage: createMetadataTestDocuments.pl <nofdocs>\n";
        print STDERR "Description: Prints generated sub documents as one document to stdout\n";
        print STDERR "<nofdocs>: number of documents to create\n";
        exit;
}
my $nofdocs = $ARGV[0];

sub getPrimeFactors
{
        my @rt = ();
        my ($docidx) = @_;
        my $rest = $docidx;
        my $nn = sqrt( $docidx)+1;
        my $ii = 2;
        while ($rest > 1 && $ii < $nn)
        {
                while ($rest % $ii == 0)
                {
                        push @rt, $ii;
                        $rest = $rest / $ii;
                }
                $ii += 1;
        }
        if ($rest > 1)
        {
                push @rt, $rest;
        }
        return @rt;
}

sub getCrossSum
{
        my $rt = 0;
        my ($arg) = @_;
        $rt += $_ for split(//, $arg);
        return $rt;
}

sub getDocument
{
        my $rt = "";
        my ($docidx) = @_;
        my @primefactors = getPrimeFactors( $docidx);
        my $crossSum = getCrossSum( $docidx);
        my $smallestPrimeFactor = undef;
        my $largestPrimeFactor = undef;
        my $content = join(' ', @primefactors);
        if (@primefactors)
        {
                $smallestPrimeFactor = $primefactors[0];
                $largestPrimeFactor = $primefactors[ $#primefactors];
        }
        $rt .= "<doc id='$docidx'>";
        $rt .= "<cross>$crossSum</cross>";
        if (@primefactors)
        {
                $rt .= "<factors>$#primefactors</factors>";
        }
        if ($smallestPrimeFactor)
        {
                $rt .= "<lo>$smallestPrimeFactor</lo>";
        }
        if ($largestPrimeFactor)
        {
                $rt .= "<hi>$largestPrimeFactor</hi>";
        }
        $rt .= "<content>$content</content>";
        $rt .= "</doc>";
        return $rt;
}

print "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
print "<list>\n";
my $docidx;
for ($docidx=1; $docidx <= $nofdocs; $docidx += 1)
{
	print( getDocument( $docidx) . "\n");
}
print "</list>\n";
print "<!-- This document was generated with $0 $ARGV[0] -->\n";


