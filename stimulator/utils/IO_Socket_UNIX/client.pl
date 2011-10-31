#!/usr/bin/perl
use warnings;
use strict;
use IO::Socket::UNIX;
$|++;

my $client = IO::Socket::UNIX->new(Peer => "/tmp/mysock",
        Type => SOCK_DGRAM,
        Timeout => 10 ) or die $@;

$client->send("foo");
