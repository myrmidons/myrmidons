package AntBot;
use FileHandle;
use IPC::Open2;
use AntMap;

sub start {
    shift;
    my $bot = shift;
    my $pid = open2(*Reader, *Writer, $bot);

    my $fpid = fork();
    if (not defined $fpid) {
        print "resources not avilable.\n";
    } elsif ($fpid == 0) {
        # Child
        while(<Reader>) {
            AntMap->update($_);
        }
        exit(0);
    }
    return $fpid;
}

sub write {
    shift;
    my $what = shift;
    say Writer $what;
}

1;
