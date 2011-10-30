package AntBot;
use FileHandle;
use IPC::Open2;
use AntMap;
use IPC::SysV qw(IPC_CREAT);

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
        my $firstcmd = 0;
        # Child
        while(<Reader>) {
            if (0 == $firstcmd) {
                $self->takesem(1);
                $firstcmd = 1;
            }
            print "Got $_";

            if (/o (\d+) (\d+) (.)/) {
                AntMap->makemove($1, $2, $3)
            }

            # Bot is finished with turn
            if (/go/) {
                AntMap->print_map();
                print "Got go <------------------------------\n";
                $self->givesem(0);
                $firstcmd = 0;
            }
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
    $self->takesem(0) unless (0 == $turn);
    say Writer "turn $turn";
    $turn++;
}

sub go {
    my $self = shift;
    print "Put go ------------------------------>\n";
    say Writer "go";
    $self->givesem(1);
}

sub createsem {
    my $self = shift;
    my $IPC_KEY0 = 1234;
    my $IPC_KEY1 = $IPC_KEY0 + 1;
    my $id0 = semget($IPC_KEY0, 10, 0666 | IPC_CREAT);
    my $id1 = semget($IPC_KEY1, 10, 0666 | IPC_CREAT);
    defined($id0)                    || die "shmget: $!";
    defined($id1)                    || die "shmget: $!";

    #print "shm key $id0\n";
    #print "shm key $id1\n";
}

sub takesem {
    shift;
    my $semid = shift;
#    print "\t" if (1 == $semid);
#    print "\tTaking semaphore $semid\n";
# create a semaphore
    my $IPC_KEY = 1234 + $semid;
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
}

sub givesem {
    shift;
    my $semid = shift;
#    print "\t" if (1 == $semid);
#    print "\tGiving semaphore $semid\n";
# "give" the semaphore
# run this in the original process and you will see
# that the second process continues
    my $IPC_KEY = 1234 + $semid;
    my $id = semget($IPC_KEY, 0, 0);
    die unless defined($id);
    my $semnum  = 0;
    my $semflag = 0;
# Decrement the semaphore count
    my $semop = -1;
    my $opstring = pack("s!s!s!", $semnum, $semop, $semflag);
    semop($id, $opstring)   || die "semop: $!";
}

1;
