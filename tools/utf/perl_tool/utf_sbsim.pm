
#
# File: utf_sbsim.pm
#
# Purpose:
#   This package provides functions used to generate input files for the Unit Test Framework (aka Steve's SBSIM).
#   The following functions are provided:
#     cmd(cmd_name, cmd_parameter, ...)   = outputs a CCSDS command packet defined in the cmd_packet_table
#     tlm(tlm_name)                       = outputs a CCSDS telemetry packets defined in the tlm_packet_table
#     tlm2(tlm_name, tlm_parameter, ...)  = outputs a CCSDS telemetry packets defined in the tlm2_packet_table
#     pipe_empty()                        = outputs a pipe empty special character
#     label("text")                       = outputs a label special character followed by the labels text string
#     comment("text")                     = outputs a comment special character followed by the comments text string
#     special_cmd("name", arg1, ...)      = outputs a special command special character followed by the name of the command
#
# References:
#
# Assumptions and Notes:
#
# $Date: 2008/04/17 07:09:34GMT-05:00 $
# $Revision: 1.1 $
# $Log: utf_sbsim.pm  $
# Revision 1.1 2008/04/17 07:09:34GMT-05:00 ruperera 
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/perl_tool/project.pj
# Revision 1.2 2006/04/10 15:44:17EDT sslegel 
# Changed code to format time as a 6 byte timestamp instead of a 8 byte timestamp in telemetry packets
# Revision 1.1 2006/03/21 16:49:12EST sslegel 
# Initial revision
# Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/perl_tool/project.pj
# Revision 1.3 2005/12/01 14:14:48EST sslegel 
# Added new code to support both big and little endian output
# Added new TEXT parameter type for the cmd function
# Reorganized code so the functions that build packets could be reused for other things
# Revision 1.1 2004/12/21 15:00:56EST sslegel
# Initial revision
# Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
# Revision 1.8  2004/10/19 21:21:10  sslegel
# Fixed subseconds conversion
#
# Revision 1.7  2004/06/11 19:19:49  sslegel
# Fixed copy and paste bug in tlm2 where I missed changing the tlm to tlm2 for the check for the secondary header.
#
# Revision 1.6  2004/06/11 16:14:27  sslegel
# Changed code so that you can now specify how to increment time in each telemetry packet in the tlm and tlm2 tables
#
# Revision 1.5  2004/06/10 17:40:53  sslegel
# Separated out the user defined tables from the main perl module
#
# Revision 1.4  2004/06/07 22:02:49  sslegel
# Added support for DOUBLE parameter types
# Added new tlm2 command that builds a telemetry packet based on passed in parameters
# Cleaned up many routines and hopefully made them easier to understand and more efficient
#
# Revision 1.3  2004/05/11 14:48:47  sslegel
# Converted the default file to not use the utf_include stuff since I figure most people may not want to mess with it.
# Also fixed the time stamp in the tlm secondary header, we now define the time structure to be 4 16 bit words instead of 2 32 bit words.  So I needed to fix the byte ordering.
#
# Revision 1.2  2004/05/07 19:30:36  sslegel
# Fix revision history tag
#
#
package utf_sbsim;

{
    # the tables that define the users command and telemetry packets
    require utf_sbsim_custom;

    use Config;
    use Getopt::Long;

    # set the default byte order to be the same as the machine perl is running on
    if ($Config{byteorder} == "1234" || $Config{byteorder} == "12345678") {
        $endian = 0;    #little endian
    }
    else {
        $endian = 1;    #big endian
    }

    # command line option allows user to override output byte order
    GetOptions("endian=i" => \$endian);

    # if (the native byte order is little endian and we want little endian or
    #     the native byte order is big endian and we want big endian) then
    #    use the native byte order
    # else
    #    byte swap the data
    # endif
    if ((($Config{byteorder} == "1234" || $Config{byteorder} == "12345678") && $endian == 0) ||
        (($Config{byteorder} == "4321" || $Config{byteorder} == "87654321") && $endian == 1)) {
        $byte_swap_data = 0;    #FALSE
    }
    else {
        $byte_swap_data = 1;    #TRUE
    }
}

sub main::cmd
{
    my ($packet, $size, $description);

    ($packet, $size, $description) = main::build_cmd_packet(@_);

    # UTF reads packets as 16 bit words instead of bytes so if we are outputting
    # data in little endian byte order then we need to byte swap the data so it
    # comes out in the right bite order in UTF.
    if ($endian == 0) {
        $packet = swap_bytes($packet, $size);
    }

    # print the packet
    printf("%s\t;%s\n", $packet, $description);

}

sub main::tlm
{
    my ($packet, $size, $description);

    ($packet, $size, $description) = main::build_tlm_packet1(@_);

    # UTF reads packets as 16 bit words instead of bytes so if we are outputting
    # data in little endian byte order then we need to byte swap the data so it
    # comes out in the right bite order in UTF.
    if ($endian == 0) {
        $packet = swap_bytes($packet, $size);
    }

    # print the packet
    printf("%s\t;%s\n", $packet, $description);
}

sub main::tlm2
{
    my ($packet, $size, $description);

    ($packet, $size, $description) = main::build_tlm_packet2(@_);

    # UTF reads packets as 16 bit words instead of bytes so if we are outputting
    # data in little endian byte order then we need to byte swap the data so it
    # comes out in the right bite order in UTF.
    if ($endian == 0) {
        $packet = swap_bytes($packet, $size);
    }

    # print the packet
    printf("%s\t;%s\n", $packet, $description);
}

sub main::pipe_empty
{
    printf("!\t; Pipe Empty\n");
}

sub main::label
{
    my($string) = @_;
    printf("*\n");
    printf("*%s\n", $string);
    printf("*");
    for ($i=0; $i < length($string); $i++) {
        printf("-");
    }
    printf("\n");
}

sub main::comment
{
    my($string) = @_;
    printf(";%s\n", $string);
}

sub main::special_cmd
{
    my(@args) = @_;
    my $i;

    printf("@ ");
    for ($i=0; $i < @args; $i++) {
        printf("%s ", $args[$i]);
    }
    printf("\t;Special Command\n");
}

sub main::build_cmd_packet
{
    my($name, @params) = @_;

    my($packet, $i, $j, $type, $size);

    # search the packet table for a matching packet name
    for ($i=0; $i < @cmd_packet_table; $i++) {

        if ($cmd_packet_table[$i]{NAME} eq $name) {

            $packet = "";

            # primary CCSDS header
            put_word(\$packet, 0, $cmd_packet_table[$i]{APID});
            put_word(\$packet, 2, $cmd_packet_table[$i]{SEQ}++ | 0xC000);
            put_word(\$packet, 4, 0);                                 #packet length placeholder
            $byte_offset = 6;

            # secondary CCSDS header
            put_word(\$packet, 6, shift_left($cmd_packet_table[$i]{FC}, 8));    #the checksum is or'ed in later
            $byte_offset += 2;

            # parameters - passed number of parameters must match the number of parameters defined in the table
            if (@{$cmd_packet_table[$i]{PARAMS}} != @params) { printf("; ERROR: Command %s Wrong Number Of Params %d!\n", $name, @params); }

            for ($j=0; $j < @{$cmd_packet_table[$i]{PARAMS}}; $j++) {

                # split each table parameter into data type and size - size only applies to text snd string types
                ($type, $size) = split(/:/, $cmd_packet_table[$i]{PARAMS}[$j]);

                if ($type eq "BYTE") {
                    put_byte(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 1;
                }
                elsif ($type eq "WORD") {
                    put_word(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 2;
                }
                elsif ($type eq "DWORD") {
                    put_dword(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 4;
                }
                elsif ($type eq "FLOAT") {
                    put_float(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 4;
                }
                elsif ($type eq "DOUBLE") {
                    put_double(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 8;
                }
                elsif ($type eq "STRING") {
                    put_string(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += length($params[$j]) / 2;
                }
                elsif ($type eq "TEXT") {
                    if ($size == 0 || $size % 2 != 0) { printf("; ERROR: Command %s Invalid TEXT Parameter Size %d!\n", $name, $size); }
                    if ($size <= length($params[$j])) { printf("; ERROR: Command %s TEXT Parameter Too Long '%s'!\n", $name, $params[$j]); }
                    put_string(\$packet, $byte_offset, unpack('H*', (pack("a".$size, $params[$j]))), $size);
                    $byte_offset += $size;
                }
                else {
                    printf("; ERROR: Command %s Invalid Parameter Type %s!\n", $name, $cmd_packet_table[$i]{PARAMS}[$j]);
                }
            }

            # add packet length
            if ($byte_offset % 2 != 0) { printf("; ERROR: Command %s Invalid Packet Length %d!\n", $name, $byte_offset); }
            put_word(\$packet, 4, $byte_offset - 7);

            # add packet checksum
            put_word(\$packet, 6, get_word(\$packet, 6) | byte_checksum(\$packet)); # this or's in the checksum with the function code

            return($packet, $byte_offset, $cmd_packet_table[$i]{DESC});
        }
    }
    printf("; ERROR: Command %s Not Found!\n", $name);
    return("", 0, "");

}

sub main::build_tlm_packet1
{
    my($name, $param) = @_;

    my($packet, $i, $j, $seconds, $subseconds);

    # search the packet table for a matching packet name
    for ($i=0; $i < @tlm_packet_table; $i++) {

        if ($tlm_packet_table[$i]{NAME} eq $name) {

            $packet = "";

            # primary CCSDS header
            put_word(\$packet, 0, $tlm_packet_table[$i]{APID});
            put_word(\$packet, 2, $tlm_packet_table[$i]{SEQ}++ | 0xC000);
            put_word(\$packet, 4, 0);                                              #packet length placeholder
            $byte_offset = 6;

            # secondary CCSDS header
            if ($tlm_packet_table[$i]{SEC}) {

                $tlm_packet_table[$i]{TIME} += $tlm_packet_table[$i]{INCREMENT};
                $seconds = int($tlm_packet_table[$i]{TIME});
                $subseconds = 2**32 * ($tlm_packet_table[$i]{TIME} - int($tlm_packet_table[$i]{TIME}));

                # time is now stored as 6 8 bit bytes so I split it up into seperate writes to make the
                # bytes come out in the right order
                put_byte(\$packet, 6,  ($seconds & 0x000000FF));          #time seconds
                put_byte(\$packet, 7,  ($seconds & 0x0000FF00) >> 8);     #time seconds
                put_byte(\$packet, 8,  ($seconds & 0x00FF0000) >> 16);    #time seconds
                put_byte(\$packet, 9,  ($seconds & 0xFF000000) >> 24);    #time seconds
                put_byte(\$packet, 10, ($subseconds & 0x00FF0000) >> 16); #time subseconds
                put_byte(\$packet, 11, ($subseconds & 0xFF000000) >> 24); #time subseconds
                $byte_offset += 6;
            }

            # data
            if ($tlm_packet_table[$i]{DATA} eq "AA") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xAA);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "BB") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xBB);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "CC") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xCC);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "DD") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xDD);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "EE") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xEE);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "FF") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, 0xFF);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "COUNT") {
                for ($j=0; $j < $tlm_packet_table[$i]{LENGTH}; $j++) {
                    put_byte(\$packet, $byte_offset, $j);
                    $byte_offset += 1;
                }
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "STRING") {
                if ((length($param) / 2) != $tlm_packet_table[$i]{LENGTH}) { printf("; ERROR: Telemetry Packet %s Invalid Packet Length %d!\n" , $name, (length($param) / 2)); }
                put_string(\$packet, $byte_offset, $param);
                $byte_offset += length($param) / 2;
            }
            elsif ($tlm_packet_table[$i]{DATA} eq "TEXT") {
                put_string(\$packet, $byte_offset, unpack('H*', (pack("a".$tlm_packet_table[$i]{LENGTH}, $param))), $tlm_packet_table[$i]{LENGTH});
                $byte_offset += $tlm_packet_table[$i]{LENGTH};
            }
            else {
                printf("; ERROR: Telemetry %s Invalid Pattern Type %s!\n", $tlm_packet_table[$i]{NAME}, $tlm_packet_table[$i]{DATA});
            }

            # add packet length
            if ($byte_offset % 2 != 0) { printf("; ERROR: Telemetry Packet %s Odd Packet Length %d!\n", $name, $byte_offset); }
            put_word(\$packet, 4, $byte_offset - 7);

            return($packet, $byte_offset, $tlm_packet_table[$i]{DESC});
        }
    }
    printf("; ERROR: Telemetry Packet %s Not Found!\n", $name);
    return("", 0, "");
}

sub main::build_tlm_packet2
{
    my($name, @params) = @_;

    my($packet, $i, $j, $seconds, $subseconds);

    # search the packet table for a matching packet name
    for ($i=0; $i < @cmd_packet_table; $i++) {

        if ($tlm2_packet_table[$i]{NAME} eq $name) {

            $packet = "";

            # primary CCSDS header
            put_word(\$packet, 0, $tlm2_packet_table[$i]{APID});
            put_word(\$packet, 2, $tlm2_packet_table[$i]{SEQ}++ | 0xC000);
            put_word(\$packet, 4, 0);                                               #packet length placeholder
            $byte_offset = 6;

            # secondary CCSDS header
            if ($tlm2_packet_table[$i]{SEC}) {

                $tlm2_packet_table[$i]{TIME} += $tlm2_packet_table[$i]{INCREMENT};
                $seconds = int($tlm2_packet_table[$i]{TIME});
                $subseconds = 2**32 * ($tlm2_packet_table[$i]{TIME} - int($tlm2_packet_table[$i]{TIME}));

                # time is now stored as 6 8 bit bytes so I split it up into seperate writes to make the
                # bytes come out in the right order
                put_byte(\$packet, 6,  ($seconds & 0x000000FF));          #time seconds
                put_byte(\$packet, 7,  ($seconds & 0x0000FF00) >> 8);     #time seconds
                put_byte(\$packet, 8,  ($seconds & 0x00FF0000) >> 16);    #time seconds
                put_byte(\$packet, 9,  ($seconds & 0xFF000000) >> 24);    #time seconds
                put_byte(\$packet, 10, ($subseconds & 0x00FF0000) >> 16); #time subseconds
                put_byte(\$packet, 11, ($subseconds & 0xFF000000) >> 24); #time subseconds
                $byte_offset += 6;
            }

            # parameters
            if (@{$tlm2_packet_table[$i]{PARAMS}} != @params) { printf("; ERROR: Command %s Wrong Number Of Params %d!\n", $name, @params); }

            for ($j=0; $j < @{$tlm2_packet_table[$i]{PARAMS}}; $j++) {
                if ($tlm2_packet_table[$i]{PARAMS}[$j] eq "BYTE") {
                    put_byte(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 1;
                }
                elsif ($tlm2_packet_table[$i]{PARAMS}[$j] eq "WORD") {
                    put_word(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 2;
                }
                elsif ($tlm2_packet_table[$i]{PARAMS}[$j] eq "DWORD") {
                    put_dword(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 4;
                }
                elsif ($tlm2_packet_table[$i]{PARAMS}[$j] eq "FLOAT") {
                    put_float(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 4;
                }
                elsif ($tlm2_packet_table[$i]{PARAMS}[$j] eq "DOUBLE") {
                    put_double(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += 8;
                }
                elsif ($tlm2_packet_table[$i]{PARAMS}[$j] eq "STRING") {
                    put_string(\$packet, $byte_offset, $params[$j]);
                    $byte_offset += length($params[$j]) / 2;
                }
                else {
                    printf("; ERROR: Command %s Invalid Parameter Type %s!\n", $name, $tlm2_packet_table[$i]{PARAMS}[$j]);
                }
            }

            # add packet length
            if ($byte_offset % 2 != 0) { printf("; ERROR: Telemetry Packet %s Invalid Packet Length %d!\n", $name, $byte_offset); }
            put_word(\$packet, 4, $byte_offset - 7);

            return($packet, $byte_offset, $tlm2_packet_table[$i]{DESC});
        }
    }
    printf("; ERROR: Telemetry Packet %s Not Found!\n", $name);
    return("", 0, "");
}


sub put_byte
{
    my($buffer_ref, $offset, $data) = @_;

    substr($$buffer_ref, $offset * 2, 2) = unpack('H2', pack("C", $data));
}

sub put_word
{
    my($buffer_ref, $offset, $data) = @_;

    if (!$byte_swap_data) {
        substr($$buffer_ref, $offset * 2, 4) = unpack('H4', pack("S", $data)); #le
    } else {
        substr($$buffer_ref, $offset * 2, 4) = unpack('H4', reverse(pack("S", $data))); #be
    }
}

sub put_dword
{
    my($buffer_ref, $offset, $data) = @_;

    if (!$byte_swap_data) {
        substr($$buffer_ref, $offset * 2, 8) = unpack('H8', pack("L", $data)); #le
    } else {
        substr($$buffer_ref, $offset * 2, 8) = unpack('H8', reverse(pack("L", $data))); #be
    }
}

sub put_float
{
    my($buffer_ref, $offset, $data) = @_;

    if (!$byte_swap_data) {
        substr($$buffer_ref, $offset * 2, 8) = unpack('H8', pack("f", $data)); #le
    } else {
        substr($$buffer_ref, $offset * 2, 8) = unpack('H8', reverse(pack("f", $data))); #be
    }
}

sub put_double
{
    my($buffer_ref, $offset, $data) = @_;

    if (!$byte_swap_data) {
        substr($$buffer_ref, $offset * 2, 16) = unpack('H16', pack("d", $data)); #le
    } else {
        substr($$buffer_ref, $offset * 2, 16) = unpack('H16', reverse(pack("d", $data))); #be
    }
}

sub put_string
{
    my($buffer_ref, $offset, $data) = @_;

    substr($$buffer_ref, $offset * 2) = $data;
}

sub get_byte
{
    my($buffer_ref, $offset) = @_;

    return unpack('C', pack("H2", substr($$buffer_ref, $offset * 2, 2)));
}

sub get_word
{
    my($buffer_ref, $offset) = @_;

    if (!$byte_swap_data) {
        return unpack('S', pack("H4", substr($$buffer_ref, $offset * 2, 4), 2));
    } else {
        return unpack('S', reverse(pack("H4", substr($$buffer_ref, $offset * 2, 4), 2)));
    }
}

sub get_dword
{
    my($buffer_ref, $offset) = @_;

    if (!$byte_swap_data) {
        return unpack('L', pack("H8", substr($$buffer_ref, $offset * 2, 8), 4));
    }
    else {
        return unpack('L', reverse(pack("H8", substr($$buffer_ref, $offset * 2, 8), 4)));
    }

}

sub get_float
{
    my($buffer_ref, $offset) = @_;

    if (!$byte_swap_data) {
        return unpack('f', pack("H8", substr($$buffer_ref, $offset * 2, 8), 4));
    }
    else {
        return unpack('f', reverse(pack("H8", substr($$buffer_ref, $offset * 2, 8), 4)));
    }
}

sub get_double
{
    my($buffer_ref, $offset) = @_;

    if (!$byte_swap_data) {
        return unpack('d', pack("H16", substr($$buffer_ref, $offset * 2, 16), 8));
    }
    else {
        return unpack('d', reverse(pack("H16", substr($$buffer_ref, $offset * 2, 16), 8)));
    }
}

sub byte_checksum
{
    my($buffer_ref) = @_;

    my($checksum, $i);

    $checksum = 0xFF;
    for ($i=0; $i < (length($$buffer_ref) / 2); $i++) {
        $checksum ^= get_byte($buffer_ref, $i);
    }

    return ($checksum & 0x000000FF);
}

sub swap_bytes
{

    my($buffer, $bytes) = @_;

    my($i, $swapped);

    $swapped = "";

    if (length($buffer) % 2 != 0) { printf("; ERROR: Swap bytes with invalid length buffer:%s length:%d\n", $buffer, $bytes); }

    for ($i=0; $i < $bytes; $i+=2) {
        $swapped = join ('', $swapped, substr($buffer, $i * 2 + 2, 2), substr($buffer, $i * 2 + 0, 2));
    }

    return($swapped);
}

sub swap_words
{
    my($buffer, $bytes) = @_;

    my($i,$swapped);

    $swapped = "";

    if (length($buffer) % 4 != 0) { printf("; ERROR: Swap words with invalid length buffer:%s length:%d\n", $buffer, $bytes); }

    for ($i=0; $i < $bytes; $i+=4) {
        $swapped = join ('', $swapped, substr($buffer, $i * 2 + 4, 4), substr($buffer, $i * 2 + 0, 4));
    }

    return($swapped);
}

sub shift_left
{
    my($data, $bits) = @_;

    return($data << $bits);
}

1;
