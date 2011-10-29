package AntMap;

use vars qw($rows, $cols, @map);
$rows = $cols = 0;
@map = ();

sub readfile {
    shift;
    my $file = shift;
    open(MAP, $file);
    while(<MAP>) {
        $rows = $1 if (/rows (\d+)/);
        $cols = $1 if (/cols (\d+)/);

        if (s/m //) {
            push(@map, [split(//)]);
        }
    }
    close(MAP);
}

sub print_map {
    my $self = shift;
    for (my $i=0; $i<$rows; $i++) {
        for (my $j=0; $j<$cols; $j++) {
            print $self->getpos($j, $i)." ";
        }
        print "\n";
    }
}

sub getpos {
    shift;
    my ($x, $y) = @_;
    return $map[$x][$y];
}

sub nrows { return $rows; }
sub ncols { return $cols; }

sub update {
    shift;
    my $what = shift;
    print "Got: ".$what;

    # o 4 2 S
    if ($what =~ /o (\d+) (\d+) (.)/) {

    }
}

1;
