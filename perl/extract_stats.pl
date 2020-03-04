#!/usr/local/bin/perl

my @weights = ("throughput", "IDF", "rareBooks", "bookScore", "scorePotential", "signUpTime");
my %by_stats;
for (<STDIN>) {
    s/parameters;//;
    s/\n//;
    my ($score,@stats) = split(/;/);
    my $id_stat = join(',', @stats);
    #$by_score{$id_stat} = {} unless exists($by_score{$id_stat});
    # print $score;
    $by_stats{$id_stat}{$score} = 1;
}

foreach my $id_stat (keys %by_stats) {
    #print "$id_stat\n";
    my %hash_ref = %{$by_stats{$id_stat}};
    my @scores = sort(keys %hash_ref);
    my $count = $#scores + 1;
    my @current_stat = ();
    my @values = split(/,/, $id_stat);
    for my $i (0..$#weights) {
        push(@current_stat, "${weights[$i]}=${values[$i]}");
    }
    print "$count => @scores => @current_stat\n";
}

