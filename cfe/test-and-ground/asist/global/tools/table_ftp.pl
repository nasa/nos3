#!/usr/bin/perl

use Net::FTP;
# use Env qw(ACCOUNT);
use Env qw(WORK);

foreach $arg (@ARGV)
{
    $arg_cnt++;
}
if ($arg_cnt != 4)
{
   print "Incorrect number of arguments: $arg_cnt\n";
   die "Please enter <ip address> <file name> <directory> <cpu> <\n";
}

print "\n\nStart TBL perl ftp script execution - Version 1.1 07/13/06. \n\n";

$ipaddress = $ARGV[0];
$filename = $ARGV[1];
$directory = $ARGV[2];
$cpu = $ARGV[3];
$exit_status = 0;

# print "$ARGV[0]\n";
# print "$ARGV[1]\n";
# print "$ARGV[2]\n";
# print "$ARGV[3]\n";

$filename = "$WORK/image/$filename";
$working_filename = $filename;

# ======================================================
# Determine if ground file to be FTP'd exists
# ======================================================

if (-e $working_filename) {
  $file_exists = TRUE;
}
else
{
 $file_exists = FALSE;
}

# ==================================================

print "Parameters passed to FTP\n";
print "------------------------\n";
print "Filename: $filename\n";
print "IP Address: $ipaddress\n";
print "Directory: $directory\n";
print "CPU: $cpu\n";

# ===================================================
# Generate ftp structure and execute same
# ===================================================
#
$username = 'target';
$userpasswd = 'password';

$ftp=Net::FTP->new($ipaddress,Timeout=>25,Passive=>1);
die "Could not connect: $!" unless $ftp;
$ftp->login($username,$userpasswd) 
or die "Cannot login: ", print $ftp->message;
$ftp->binary;
$ftp->cwd($directory)
or die "Cannot change working directory ", print $ftp->message;

# ==============================================================
# Determine spacecraft file existence and handle accordingly
# ==============================================================

   if ($file_exists eq TRUE) {
      print "File '", $ARGV[1], "' exists !!!\n";
      $ftp->put($filename);
      print "File '", $ARGV[1], "' ftp'd to spacecraft.\n";
   }
   else
   {
     print "File '", $ARGV[1], "' does NOT exist !!!\n";
     $exit_status = 1;
     goto QUIT;
   }

# ===========================================
# Get and print file names & sizes
# ===========================================
@entries = $ftp->dir();
print "\nFilename(s) and size(s) on IP Address->$ipaddress Directory->$directory \n";
foreach $filename (@entries)
{
     print "$filename\n";
} 
print "\n";
# ===========================================

QUIT:
$ftp->quit();
print "End TBL perl ftp script execution.\n";
exit $exit_status;

sub Net::FTP::exists { 
   my $ftp = shift;
   if(defined $ftp->size(@_)) { return 1; }
#   elsif($ftp->isdir(@_)) { return 1; }
   else { return 0; }
}

