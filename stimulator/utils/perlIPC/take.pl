#!/usr/bin/perl
use warnings;
use strict;
#use IPC::SysV qw(IPC_CREAT);
$|++;
# create a semaphore
my $IPC_KEY = 1234;
my $id = semget($IPC_KEY, 0, 0);
defined($id)                    || die "shmget: $!";
my $semnum  = 0;
my $semflag = 0;
# "take" semaphore
# wait for semaphore to be zero
my $semop = 0;
my $opstring1 = pack("s!s!s!", $semnum, $semop, $semflag);
# Increment the semaphore count
$semop = 1;
my $opstring2 = pack("s!s!s!", $semnum, $semop,  $semflag);
my $opstring  = $opstring1 . $opstring2;
semop($id, $opstring)   || die "semop: $!";
