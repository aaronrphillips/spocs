#!/usr/bin/perl -w

use strict;
use Cwd ();
use File::Basename;
use File::Find ();
use Data::Dumper;
use Getopt::Long;


# Set the variable $File::Find::dont_use_nlink if you're using AFS,
# since AFS cheats.

# for the convenience of &wanted calls, including -eval statements:
use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;


my %JSON_NAMES;
my $CWD = Cwd::cwd();
my $CLIQUES_DIR;
my $OUTPUT_DIR;


sub usage($;$)
{
  my ($retval, $message) = @_; 

  if ($retval)
  {
    print(STDERR "\n\nERROR: $message\n\n");
  }

  print(STDERR "\n");
  print(STDERR "  Usage: $0 [-h] | -p prefix\n\n");
  print(STDERR "    -h = Help\n");
  print(STDERR "    -p = Prefix to look for output files\n");
  print(STDERR "\n");
  print(STDERR "  Example: $0 -p /lustre/cliques/clique8\n");
  print(STDERR "\n");

  exit($retval);
}


sub process_command_args()
{
  my $help       = 0;
  my $prefix_dir = "/NEVER_TO_EXIST";

  GetOptions(
    'h|help|?' => \$help,
    "p=s"      => \$prefix_dir,
  );

  usage(0) if ($help);
  usage(1, "Bad prefix, cannot find ${prefix_dir}_cliques or ${prefix_dir}_out_dir") if (! (-e "${prefix_dir}_cliques" && -e "${prefix_dir}_out_dir"));

  $CLIQUES_DIR = "${prefix_dir}_cliques";
  $OUTPUT_DIR  = "${prefix_dir}_out_dir";

  return;
}


sub wanted
{
    /^.*\.html\z/s && dowork();
}


sub dowork ()
{
  # Looking for lines like:
  #   <script>var json = JSON.parse('{"nodes": [{"name":"Sfri_0740","group":1,"species":"Shewanella_frigidimarina_NCIMB_400"},{"name":"Shew_0958","group":1,"species":"Shewanella_loihica_PV-4"}],"links":[{"source":0,"target":1,"value":140}]}');

#print("CWD:$CWD:FILE:$name:\n");
  chdir $CWD; #sigh
  open(FH, "$name");
  while (<FH>)
  {
    next unless (/JSON/);
    s/^.*?[:]\s*\[?//;
    s/\][,]["].*//;
    chomp();
    my @junk = split('},{', $_);
    foreach my $item (@junk)
    {
      $item =~ s/.*?[:]["]?//;
      $item =~ s/["].*//;
#print $item . "\n";
      $JSON_NAMES{$item}++;
    }
    last;
  }
  close(FH);
  chdir $File::Find::dir;
  return(1);
}


sub check_paralogs()
{
  opendir(DH, "$OUTPUT_DIR") || die("Can't open directory, $OUTPUT_DIR: $!");
  my @files = grep { /\.paralogs/ } readdir(DH);
  closedir(DH);

  my %paralog_nodes;
  foreach my $file (@files)
  {
    open(FH, "$OUTPUT_DIR/$file") || die("Can't open file, $OUTPUT_DIR/$file: $!");
    while (<FH>)
    {
      chomp();
      # 19      2224         AFE_0369 1.000        Lferr_0539 1.000
      my($num1, $num2, @rest) = split(/\s+/, $_);
      while (scalar(@rest))
      {
        my $key1 = shift(@rest);
        my $key2 = shift(@rest);
        if ($key2 >= 1.0)
        {
          $paralog_nodes{$key1}++;
        }
      }
    }
    close(FH);
  }

  my $result = "PASSED";  # Assume things are good
  foreach my $node (sort(keys(%paralog_nodes)))
  {
    if (! exists($JSON_NAMES{$node}))
    {
      print("REPORT  :missing $node:\n");
      $result = "FAILED";
      #print("$node\n");
    }
    #print("WARN:$node:" . $paralog_nodes{$node} . ":\n") if ($paralog_nodes{$node} > 1);
  }

  foreach my $name (sort(keys(%JSON_NAMES)))
  {
    if (! exists($paralog_nodes{$name}))
    {
      print("HITS    :missing $name:\n");
      $result = "FAILED";
    }
  }

  print("CHECK FOR MISSING PARALOG NODES:        $result\n");

  return;
}


sub add_rejected($)
{
  my($file) = @_;
  open(FH, "$file") || die("Can't open file, $file: $!");
  my $header = <FH>;
  while (<FH>)
  {
    chomp();
    foreach (split(/\t/, $_))
    {   
      next if (/^\d/);
      next if (/^[-]/);
      next if (length($_) < 3); 
      $JSON_NAMES{$_}++;
    }   
  }
  close(FH);

  return;
}


sub check_for_doubles($)
{
  my($file) = @_;
  my %names;

  open(FH, "$file") || die("Can't open file, $file: $!");
  my $header = <FH>;
  while (<FH>)
  {
    chomp();
    foreach (split(/\t/, $_))
    {
      next if (/^\d/);
      next if (/^[-]/);
      next if (length($_) < 3);
      $names{$_}++;
    }
  }
  close(FH);

  my %counter;
  foreach (sort(keys(%names)))
  {
    next if ($names{$_} < 2);
    $counter{$names{$_}}++;
    print("NAME:$_:" . $names{$_} . ":\n");
  }
  
  my $result = "PASSED";  # Assume things are good
  foreach (sort { $a <=> $b } (keys(%counter)))
  {
    print("COUNTER:$_:" . $counter{$_} . ":\n");
    $result = "FAILED";
  }

  print("CHECK FOR DUPLICATES IN cliques.report: $result\n");

  return;
}


process_command_args();
check_for_doubles("$CLIQUES_DIR/cliques.report");
add_rejected("$CLIQUES_DIR/cliques.report.rejected");

# Traverse desired filesystems
File::Find::find({wanted => \&wanted}, "$CLIQUES_DIR/graph");

check_paralogs();

exit;
