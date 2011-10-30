#!/usr/bin/perl
use warnings;
use strict;
use IPC::SysV qw(IPC_CREAT);
$|++;

my $IPC_KEY = 1234;
my $id = semget($IPC_KEY, 10, 0666 | IPC_CREAT);
defined($id)                    || die "shmget: $!";
print "shm key $id\n";
