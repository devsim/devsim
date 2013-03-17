#!/usr/bin/perl -w
use strict;

# ARGUMENTS
# testexe with full path
# goldenresults directory with full path
# name of the test output file to create and to compare with the goldenresults

my $testexe = $ARGV[0];
my $testout = $ARGV[2];
my $goldenresult = $ARGV[1]."/${testout}";
my $testrundir = $ARGV[3];
my $testoutdir = $ARGV[4];
chdir $testrundir;
my $command = "$testexe > $testoutdir/$testout 2>&1";
#print "\n$command\n";
unlink($testout);
my $retval = (system($command)==0) or die "$testexe failed to run";

my $diffcmd = "/usr/bin/diff -w -q $testoutdir/$testout $goldenresult";
$retval = (system($diffcmd)==0) or die "$testout differs from $goldenresult";
