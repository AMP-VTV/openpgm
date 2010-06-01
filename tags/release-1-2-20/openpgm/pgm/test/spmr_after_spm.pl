#!/usr/bin/perl
# spmr.pl
# 13.3.1. SPM Requests

use strict;
use Time::HiRes qw( gettimeofday tv_interval );
use PGM::Test;

BEGIN { require "test.conf.pl"; }

$| = 1;

my $mon = PGM::Test->new(tag => 'mon', host => $config{mon}{host}, cmd => $config{mon}{cmd});
my $sim = PGM::Test->new(tag => 'sim', host => $config{sim}{host}, cmd => $config{sim}{cmd});
my $app = PGM::Test->new(tag => 'app', host => $config{app}{host}, cmd => $config{app}{cmd});

$mon->connect;
$sim->connect;
$app->connect;

sub close_ssh {
	$mon = $sim = $app = undef;
	print "finished.\n";
}

$SIG{'INT'} = sub { print "interrupt caught.\n"; close_ssh(); };

$mon->say ("filter $config{app}{ip}");
print "mon: ready.\n";

$app->say ("create ao");
$app->say ("bind ao");
$app->say ("listen ao");

## capture GSI of test spp
$app->say ("send ao nashi");
print "mon: wait for odata ...\n";
my $odata = $mon->wait_for_odata;
print "mon: odata received.\n";
my $t0 = [gettimeofday];
my $elapsed;

## spm hearbeats are going to clear out the data, lets wait for some quiet
print "mon: wait for SPM interval > 5 seconds ...\n";
do {
	$mon->wait_for_spm;
	$elapsed = tv_interval ( $t0, [gettimeofday] );
	print "mon: received SPM after $elapsed seconds.\n";
} while ($elapsed < 5);

$sim->say ("create fake ao");
$sim->say ("bind ao");
print "sim: ready.\n";

## app needs to send packet for sim to learn of local NLA
$app->say ("send ao budo");
print "sim: wait for odata ...\n";
$odata = $sim->wait_for_odata;
print "sim: odata received.\n";

print "sim: request SPM via SPMR.\n";
$sim->say ("net send spmr ao $odata->{PGM}->{gsi}.$odata->{PGM}->{sourcePort}");
$t0 = [gettimeofday];

print "sim: wait for SPM ...\n";
$sim->wait_for_spm;
$elapsed = tv_interval ( $t0, [gettimeofday] );
print "sim: SPM received after $elapsed seconds.\n";
die "SPM interval too large, indicates heartbeat not SPMR induced.\n" unless ($elapsed < 5.0);

print "test completed successfully.\n";

$mon->disconnect (1);
$sim->disconnect;
$app->disconnect;
close_ssh;

# eof
