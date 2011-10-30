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
            print $self->getpos($i, $j)." ";
        }
        print "\n";
    }
}

sub getpos {
    shift;
    my ($x, $y) = @_;
    return $map[$x][$y];
}

sub makemove {
    shift;
    my ($x, $y, $dir) = @_;

    my $newx = $x;
    my $newy = $y;

    if ($dir =~ /S/) {
        $newy++;
    } elsif ($dir =~ /N/) {
        $newy--;
    } elsif ($dir =~ /E/) {
        $newx++;
    } elsif ($dir =~ /W/) {
        $newx--;
    }

    print "[$x, $y] --> [$newx, $newy]\n";

    $map[$newx][$newy] = $map[$x][$y];
    $map[$x][$y] = ".";
}

sub nrows { return $rows; }
sub ncols { return $cols; }

1;
