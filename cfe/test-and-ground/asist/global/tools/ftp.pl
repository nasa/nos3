#!/usr/bin/perl

use Net::FTP;
use Env qw(ACCOUNT);
use Env qw(WORK);

foreach $arg (@ARGV)
{
    $arg_cnt++;
}
if ($arg_cnt != 5)
{
   print "Incorrect number of arguments: $arg_cnt\n";
   die "Please enter <source directory> <file name> <destination file name> <xfermode> <ip address>.\nAll arguments are REQUIRED !!!.\nUse \"\" in place of the <destination file name> if you want the name to be automatically generated with a time tag.\n";
}

print "\n\nStart perl ftp script execution. \n\n";

$sourcedirectory = $ARGV[0];
$sourcefilename = $ARGV[1];
$destfilename = $ARGV[2];
$xfermode = $ARGV[3];
$ipaddress = $ARGV[4];
$ipaddress = "$ipaddress";

# print "$ARGV[0]\n";
# print "$ARGV[1]\n";
# print "$ARGV[2]\n";
# print "$ARGV[3]\n";
# print "$ARGV[4]\n;

# ===================================================
# get time and date. Prepend with a '0' if necessary
# ===================================================
#
($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime();

if (length($hour) == 1) {
    $hour = "0$hour" };
if (length($min) == 1) {
    $min = "0$min" };
if (length($sec) == 1) {
    $sec = "0$sec" };
if (length($mday) == 1) {
    $mday = "0$mday" };
$year = $year + 1900 ;
$mon = $mon+1;
if (length($mon) == 1) {
    $mon = "0$mon" };

$date = "-$mon-$mday-$year:$hour:$min:$sec";

# =====================================================
# Build $destfilename if parameter $ARGV[2] is blank
# =====================================================
#
if (length($ARGV[2]) == "0") {
    $destfilename = "$WORK/image";
    $destfilename = "$destfilename/$sourcefilename$date"}
else {
    $destfilename = "$WORK/image/$destfilename"};

# ======================================================
# Remove old copy of file 
# ======================================================
system "rm $destfilename";
print "\n";

#=======================================================
# List Parameters passed to FTP
# ======================================================

print "Parameters passed to FTP\n";
print "------------------------\n";
print "Source Directory: $sourcedirectory\n";
print "Source Filename: $sourcefilename\n";
print "Destination Filename: $destfilename\n";
print "FTP Transfer Mode: $xfermode\n";
print "IP Address: $ipaddress\n";

# ===================================================
# Generate ftp structure and execute same
# ===================================================
#
$username = 'target';
$userpasswd = 'password';

$ftp=Net::FTP->new($ipaddress,Timeout=>15,Passive=>1);
die "Could not connect: $!" unless $ftp;
$ftp->login($username,$userpasswd);
$ftp->$xfermode;
$ftp->cwd ($sourcedirectory);
# ===========================================
# Get and print file names & sizes
# ===========================================
@entries = $ftp->dir();
print "\nFilename(s) and size(s) on IP Address->$ipaddress Directory->$sourcedirectory \n";
foreach $filename (@entries)
{
    print "$filename\n";
} 
print "\n";
# ===========================================

$ftp->get($sourcefilename,$destfilename);
$ftp->quit();

# ===========================================
# Print file name and size as ftp'd 
# ===========================================
print "\n";
print "\Filename and size as retrieved from IP Address->$ipaddress Directory->$sourcedirectory\n";
system "ls -g $destfilename";
print "\n";
print "End perl ftp script execution.\n";

