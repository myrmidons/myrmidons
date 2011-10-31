#!/usr/bin/perl
use warnings;
use strict;
use feature ':5.10';
use AntMap;
use AntBot;
use IO::Socket::UNIX;

$|++;

my $mapfile = shift;
my $bot = shift;

AntMap->readfile($mapfile);
my $pid = AntBot->start($bot);

#AntMap->print_map();

init();

foreach(1..3) {
    AntBot->turn();

    for (my $i=0; $i<AntMap->nrows(); $i++) {
        for (my $j=0; $j<AntMap->ncols(); $j++) {
            my $var = AntMap->getpos($i, $j);
            next if ($var =~ /\./);
            AntBot->write("h $i $j $var") if ($var =~ /\d/);
            AntBot->write("w $i $j") if ($var =~ /\%/);
            AntBot->write("f $i $j") if ($var =~ /\*/);
            AntBot->write("a $i $j ".(ord($var)-97)) if ($var =~ /[a-j]/);
            if ($var =~ /[A-J]/) {
                AntBot->write("a $i $j ".(ord($var)-65));
                AntBot->write("h $i $j ".(ord($var)-65));
            }
        }
    }

    unlink "/tmp/antsock";
    my $server = IO::Socket::UNIX->new(Local => "/tmp/antsock",
            Type => SOCK_DGRAM,
            Listen => 5 ) or die $@;

    AntBot->go();

    while() {
        my $text = "";
        $server->recv($text, 128);
        if ($text =~ /o (\d+) (\d+) (.)/) {
            AntMap->makemove($1, $2, $3)
        }

        # Bot is finished with turn
        if ($text =~ /go/) {
            AntMap->print_map();
            last;
        }
    }
}

waitpid($pid, 0);

sub init {
    AntBot->turn();
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
}

