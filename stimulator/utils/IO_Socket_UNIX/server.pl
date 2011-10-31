#!/usr/bin/perl
use warnings;
use strict;
use IO::Socket::UNIX;
use POSIX;
$|++;

unlink "/tmp/mysock";
my $server = IO::Socket::UNIX->new(Local => "/tmp/mysock",
        Type => SOCK_DGRAM,
        Listen => 5 ) or die $@;

while() {
    my $text = "";
    $server->recv($text,128);
    print $text;
}
