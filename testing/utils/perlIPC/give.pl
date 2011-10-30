#!/usr/bin/perl
use warnings;
use strict;
$|++;
# "give" the semaphore
# run this in the original process and you will see
# that the second process continues
my $IPC_KEY = 1234;
my $id = semget($IPC_KEY, 0, 0);
die unless defined($id);
my $semnum  = 0;
my $semflag = 0;
# Decrement the semaphore count
my $semop = -1;
my $opstring = pack("s!s!s!", $semnum, $semop, $semflag);
semop($id, $opstring)   || die "semop: $!";
