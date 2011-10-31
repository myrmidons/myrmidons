package AntBot;
use FileHandle;
use IO::Socket::UNIX;
use IPC::Open2;

use vars qw($turn);
$turn = 0;

sub start {
    my $self = shift;
    my $bot = shift;
    my $pid = open2(*Reader, *Writer, $bot);

    $self->createsem();

    my $fpid = fork();
    if (not defined $fpid) {
        print "resources not avilable.\n";
    } elsif ($fpid == 0) {
        # Child
        while(<Reader>) {
            my $client = IO::Socket::UNIX->new(Peer => "/tmp/antsock",
                    Type => SOCK_DGRAM,
                    Timeout => 10 ) or die $@;
            $client->send($_);
        }
        exit(0);
    }
    return $fpid;
}

sub write {
    shift;
    my $what = shift;
    print "Put ".$what."\n";
    say Writer $what;
}

sub turn {
    my $self = shift;
#    print "Put turn $turn\n";
    say Writer "turn $turn";
    $turn++;
}

sub go {
    my $self = shift;
    print "Put go ------------------------------>\n";
    say Writer "go";
}

1;
