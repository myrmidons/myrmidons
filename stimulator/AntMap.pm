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
    my ($r, $c) = @_;
    return $map[$r][$c];
}

sub makemove {
    shift;
    my ($r, $c, $dir) = @_;

    my $newr = $r;
    my $newc = $c;

    if ($dir =~ /S/) {
        $newr++;
    } elsif ($dir =~ /N/) {
        $newr--;
    } elsif ($dir =~ /E/) {
        $newc++;
    } elsif ($dir =~ /W/) {
        $newc--;
    }

    $newc = ($cols-1) if (-1 == $newc);
    $newr = ($rows-1) if (-1 == $newr);

    # TODO fix collision detection
    if ($map[$newr][$newc] == '.' ||
        $map[$newr][$newc] =~ /\d/) {
        print "[$r, $c] --> [$newr, $newc]\n";
        $map[$newr][$newc] = 'a';
        $map[$r][$c] = '.';
    } elsif ($map[$newr][$newc] == 'a') {
        $map[$newr][$newc] = 'a';
        $map[$r][$c] = 'a';
    }
}

sub nrows { return $rows; }
sub ncols { return $cols; }

1;
