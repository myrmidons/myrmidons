#!/usr/bin/perl
use warnings;
use strict;
use feature ':5.10';
use FileHandle;
use IPC::Open2;
use AntMap;
use AntBot;
$|++;

my $mapfile = shift;
my $bot = shift;

AntMap->readfile($mapfile);
my $pid = AntBot->start($bot);

#AntMap->print_map();

init();

AntBot->write("turn 1");

for (my $i=0; $i<AntMap->nrows(); $i++) {
    for (my $j=0; $j<AntMap->ncols(); $j++) {
        my $var = AntMap->getpos($j, $i);
        next if ($var =~ /\./);
        AntBot->write("h $i $j $var") if ($var =~ /\d/);
        AntBot->write("w $i $j") if ($var =~ /\%/);
        AntBot->write("a $i $j ".(ord($var)-97)) if ($var =~ /[a-j]/);
        if ($var =~ /[A-J]/) {
            AntBot->write("a $i $j ".(ord($var)-97));
            AntBot->write("h $i $j $var");
        }
    }
}

AntBot->write("go");

waitpid($pid,0);

sub init {
    AntBot->write("turn 0");
    AntBot->write("loadtime 3000");
    AntBot->write("turntime 1000");
    AntBot->write("rows ".AntMap->nrows());
    AntBot->write("cols ".AntMap->ncols());
    AntBot->write("turns 2");
    AntBot->write("viewradius2 55");
    AntBot->write("attackradius2 5");
    AntBot->write("spawnradius2 1");
    AntBot->write("player_seed 42");
    AntBot->write("ready");
    AntBot->write("go");
}

