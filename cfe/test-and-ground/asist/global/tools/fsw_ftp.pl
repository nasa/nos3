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

print "\n\nStart FSW perl ftp script execution. \n\n";


$ipaddress = $ARGV[0];
$filename = $ARGV[1];
$directory = $ARGV[2];
$cpu = $ARGV[3];

# print "$ARGV[0]\n";
# print "$ARGV[1]\n";
# print "$ARGV[2]\n";
# print "$ARGV[3]\n";

$filename = "$WORK/apps/cpu$cpu/$filename";

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

$ftp=Net::FTP->new($ipaddress,Timeout=>15,Passive=>1);
die "Could not connect: $!" unless $ftp;
$ftp->login($username,$userpasswd);
$ftp->binary;
$ftp->cwd($directory);
$ftp->put($filename);

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

$ftp->quit();

print "End FSW perl ftp script execution.\n";

