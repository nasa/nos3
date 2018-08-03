#!/usr/bin/perl

use Net::FTP;
use Env qw(ACCOUNT);
use Env qw(WORK);

foreach $arg (@ARGV)
{
    $arg_cnt++;
}
if ($arg_cnt != 6)
{
   print "Incorrect number of arguments: $arg_cnt\n";
   die "Please enter <remote directory> <local file name> <remote file name> <xfermode> <ip address> <getorput>.\nAll arguments are REQUIRED !!!.\nUse \"\" in place of the <destination file name> if you want the name to be automatically generated with a time tag.\n";
}

# print "\n\nStart perl ftp file script execution - Version 2.1 04/04/06. \n\n";
print "\n\nStart perl ftp file script execution - Version 2.2 05/02/06. \n\n";

$cwddirectory = $ARGV[0];
$localfilename = $ARGV[1];
$remotefilename = $ARGV[2];
$xfermode = $ARGV[3];
$ipaddress = $ARGV[4];
$ipaddress = "$ipaddress";
$getorput = $ARGV[5];
$exit_status = 0;

# print "$ARGV[0]\n";
# print "$ARGV[1]\n";
# print "$ARGV[2]\n";
# print "$ARGV[3]\n";
# print "$ARGV[4]\n";
# print "$ARGV[5]\n";

# ======================================================
# Set up FTP Direction Parameter 
# ======================================================

if ($ARGV[5] eq "P" || $ARGV[5] eq "p") {
    $getorput = "put";
    $remotedirectory = $ARGV[0];
    $cwddirectory = $remotedirectory;
    $localdirectory = "$WORK/image"; 
    $localfilename = "$WORK/image/$localfilename";
    $working_filename = $localfilename;
}

if ($ARGV[5] eq "G" || $ARGV[5] eq "g") {
    $getorput = "get";
    $localdirectory = $ARGV[0];
    $cwddirectory = $localdirectory;
    $remotedirectory = "$WORK/image"; 
    $remotefilename = "$WORK/image/$remotefilename";
    $working_filename = $remotefilename;
}

if ($ARGV[5] eq "R" || $ARGV[5] eq "r") {
    $getorput = "delete";
    $remotedirectory = $ARGV[0];
    $cwddirectory = $remotedirectory;
    $remotefilename = "$WORK/image/$remotefilename";
    $working_filename = $remotefilename;
}

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

# ======================================================
# List Parameters passed to FTP
# ======================================================

print "Parameters passed to FTP\n";
print "------------------------\n";
print "Local Directory: $localdirectory\n";
print "Remote Directory: $remotedirectory\n";
print "Local Filename: $localfilename\n";
print "Remote Filename: $remotefilename\n";
print "FTP Transfer Mode: $xfermode\n";
print "IP Address: $ipaddress\n";
print "FTP Option: $getorput\n\n";
print "FTP Command Response(s)\n";
print "-----------------------\n";

# ===================================================
# Generate ftp structure and execute same
# ===================================================
#
$username = 'target';
$userpasswd = 'password';

$ftp=Net::FTP->new($ipaddress,Timeout=>25,Passive=>1)
or die "Could not connect: $!" unless $ftp;
$ftp->login($username,$userpasswd) 
or die "Cannot login: ", print $ftp->message;
$ftp->$xfermode;
$ftp->cwd ($cwddirectory) 
or die "Cannot change working directory ", print $ftp->message;

# ==============================================================
# Determine spacecraft file existence and handle accordingly
# ==============================================================

if ($getorput eq "delete") {
   if ($ftp->exists($ARGV[2])) {
       $ftp->$getorput($ARGV[2]);
       print "File '", $ARGV[2], "' DELETED !!!\n";
   }
   else
   {
       print "File '", $ARGV[2], "' does NOT exist !!!\n";
       $exit_status = 1;
   }
goto QUIT;
}

if ($getorput eq "get") {
   if ($ftp->exists($ARGV[1])) {
      print "File '", $ARGV[1], "' exists !!!\n";
      $ftp->$getorput($localfilename,$remotefilename);
      print "File '", $ARGV[1], "' ftp'd to ground.\n";
   }
   else
   {
       print "File '", $ARGV[1], "' does NOT exist !!!\n";
       $exit_status = 1;
   }
goto QUIT;
}

if ($getorput eq "put") {
   if ($file_exists eq TRUE) {
      print "File '", $ARGV[1], "' exists !!!\n";
      $ftp->$getorput($localfilename,$remotefilename);
      print "File '", $ARGV[1], "' ftp'd to spacecraft.\n";
   }
   else
   {
     print "File '", $ARGV[1], "' does NOT exist !!!\n";
     $exit_status = 1;
   }
goto QUIT;
}

QUIT:
$ftp->quit();
print "\n\nEnd perl ftp file script execution.\n";
exit $exit_status;

sub Net::FTP::exists { 
   my $ftp = shift;
   if(defined $ftp->size(@_)) { return 1; }
#   elsif($ftp->isdir(@_)) { return 1; }
   else { return 0; }
}
