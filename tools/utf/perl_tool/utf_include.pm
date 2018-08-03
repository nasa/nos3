#####################################################################
# Library       Include.pm
# Title         C::Include
# Version       1.30
# Author        Albert N. MICHEEV [Albert@f80.n5049.z2.fidonet.org]
# Description   Miscelaneous routines for working with C/C++ like 
#               header files as native Perl deep structs.
#####################################################################
# REVISION HISTORY
#
# 1.00 (01 Mar 2000)  First pilot variant
#
# 1.10 (21 Apr 2000)  Extended module call method & added func INC
#                        use C::Include qw(headers/config.h -cache);
#                     Fixed for compatibility with Perl v5.6
#
# 1.20 (04 Jun 2000)  Fixed for compatibility with ‘++  8-)
#
# 1.30 (24 Jun 2000)  make_struct modificated
#
# 1.40 (30 May 2001)  allowed anonymous enum 
#                     allowed synonym defining via 'typedef' keyword

package C::Include::Struct;
package C::Include;

use 5.005;
use strict;
use IO::File;
use Storable 0.6.7.1;
use vars qw/$VERSION %TYPES %ALIASES %INC $TEST/;

$VERSION = 1.40;

$TEST = 0;

%TYPES = (
    'string',             ['a',1],
    'sstring',            ['A',1],
    'zstring',            ['Z',1],
    'char',               ['c',1],
    'unsigned char',      ['C',1],
    'short',              ['s',2],
    'unsigned short',     ['S',2],
    'int',                ['i',4],
    'unsigned int',       ['I',4],
    'long',               ['l',4],
    'unsigned long',      ['L',4],
    'long long',          ['q',8],
    'unsigned long long', ['Q',8],
    'float',              ['f',4],
    'double',             ['d',8],
    'pointer',            ['p',4],
    'nullbyte',           ['x',1],
    'neganull',           ['X',1]
);

%ALIASES = (
    'short int'             => 'short',
    'long int'              => 'long',
    'long long int'         => 'long long',
    'unsigned'              => 'unsigned long',
    'unsigned long int'     => 'unsigned long',
    'unsigned short int'    => 'unsigned short',
    'unsigned long long int'=> 'unsigned long long',
    'byte'                  => 'unsigned char',
    'word'                  => 'unsigned short',
    'dword'                 => 'unsigned long',
);


sub new {
    my $class = shift;
    my $self = {};
    my %param;

    # Params processing
    {
        local $_; my $param;

        while( $_ = shift ){
            $param{ $param=$1 } = '', next if /^\-(.+)$/;

            $param = 'src' unless $param;
            die "Invalid parametr: '$param'" if $param ne 'src' and $param ne 'cache';
            $param{$param} = $_;
            undef $param;
        }

        $param{cache} = $1.'.hp' if exists $param{cache} and
                                   not $param{cache} and
                                   not defined fileno( $param{src} ) and
                                   $param{src} =~ /^(.*)\.h$/io;
    }

    # Reading compiled from cache file
    if( $param{cache} and -f $param{cache} and -M $param{cache} < -M $param{src} ){
        $self = Storable::retrieve( $param{cache} );
        die "Version error!" unless $self->{''}{VERSION} == $VERSION;

    # Parsing new source and store compiled to cache file
    }else{
        unless( defined fileno $param{src} ){
            my $file = $param{src};
            $param{src} = new IO::File( $file ) or die "Can't open file '$file'!\n";
        }
        read $param{src}, $self->{source}, (stat $param{src})[7], 0;
        parse( $self );
        Storable::store($self, $param{cache}) if $param{cache};
    }

    bless $self, $class;
}


###########################
# TEXT PARSER
sub parse() {
    my $self = shift;
    my %define;

    # Define defaults
    $self->{''}{VERSION} = $VERSION;
    $define{__PERL__} = '';
    %{$self->{typedef}} = %TYPES;
    $self->{typedef}{$_[0]} = $self->{typedef}{$_[1]} while @_ = each %ALIASES;

    # Kill comments
    $self->{source} =~ tr/\x0a\x0d/\x00/d;
    $self->{source} =~ s{//.*?\x00|/\*.*?\*/}{\x00}go;

    # Split to lines
    my @lines = grep { /\S/ } split /\x00+/, $self->{source};
    delete $self->{source};


    # Local evalution func
    local *evalute = sub($){
        my ($code, $key, $value) = shift;
        $code =~ s/ \b (0x[0-9a-f]+) [ul]{0,2} \b /$1/gix;
        while( ($key, $value) = each %define ){
           $value = 1 unless defined $value;  # $value ??= 1;
           $code =~ s/\b$key\b/$value/g;
        }
        return eval $code;
    };


    # Local structs allocation func
    local *alloc_structs = sub($$$){
        my ($template, $parent, $count, $result) = @_;

        if( $count == 1 ){
            $result = $parent ?
                      Storable::dclone( $template ) :
                      new C::Include::Struct( $template );

            if( $parent ){
                push @{$$parent{''}{buffers}}, @{$$result{''}{buffers}};
                push @{$$parent{''}{bitsets}}, @{$$result{''}{bitsets}} if $$result{''}{bitsets};
                delete $$result{''} ;
            }
        }else{
            for ( 0..($count-1) ){
                $$result[$_] =  $parent ?
                                Storable::dclone( $template ) :
                                new C::Include::Struct( $template );

                next unless $parent;
                push @{$$parent{''}{buffers}}, @{$$result[$_]{''}{buffers}};
                push @{$$parent{''}{bitsets}}, @{$$result[$_]{''}{bitsets}}
                    if $$result[$_]{''}{bitsets};
                delete $$result[$_]{''};
            }
        }

        if( $parent ){
            $$parent{''}{mask}   .= $$template{''}{mask} x $count;
            $$parent{''}{length} += $$template{''}{length} * $count;
        }

        return $result;
    };

    local $\ = "\n" if $TEST;

    # Parsing
    my ($skip, $code);

    # Evaluating prepocessor directives
    while ( my $line = shift @lines ) {
        $line =~ s/^\s+//o; $line =~ s/\s+$//o;

        if ($line =~ /^\#\s*(\w+)\s*(\S.*?)?\s*$/o){
            my ($word, $value) = ($1, $2);
            
            if ($word eq 'define') {
                next if $skip;

                ($word, $value) = split /\s+/, $value, 2;
                if( $value and exists $self->{struct}{$value} ){
                    $self->{struct}{$word} = $self->{struct}{$value};
                }elsif( $value and exists $self->{typedef}{$value} ){
                    $self->{typedef}{$word} = $self->{typedef}{$value};
                }else{
                    $define{$word} = defined $value ? evalute($value) : undef;
                }
            }
            elsif ($word eq 'pragma') {
            }
            elsif ($word eq 'ifdef') {
                $skip++, next if $skip;
                $skip++ unless exists $define{$value} or exists $self->{typedef}{$value};
            }
            elsif ($word eq 'ifndef') {
                $skip++, next if $skip;
                $skip++ if exists $define{$value} or exists $self->{typedef}{$value};
            }
            elsif ($word eq 'endif') {
                $skip-- if $skip;
            }
            elsif ($word eq 'else') {
                $skip += $skip ? -1 : 1;
            }

            next;
        }

        $code .= $line unless $skip;
    }

    @lines = split /;|(?<=\{)/, $code; 
    undef $code;

    my @structs;

    while( my $line = shift @lines ){
        $line =~ s/^\s+//o; $line =~ s/\s+$//o;

#        if( $line =~ /^typedef \s+ (.+?) \s+ (\w+) $ /xo ){
#            print 'TYPEDEF: ', $2, ' == ', $1 if $TEST;
#
#            if( exists $self->{struct}{$1} ){
#                $self->{struct}{$2} = $self->{struct}{$1};
#            }elsif( exists $self->{typedef}{$1} ){
#                $self->{typedef}{$2} = $self->{typedef}{$1};
#            }
#        }

        # ENUM definition 
        if( $line =~ /^ (?:typedef \s+)? enum (?: \s+(\w+) )? \s* \{$ /xo ){
#        elsif( $line =~ /^ (?:typedef \s+)? enum (?: \s+(\w+) )? \s* \{$ /xo ){
            print 'ENUM: ', $1||'<anonymous>' if $TEST;

            $self->{typedef}{$1} = $self->{typedef}{int} if $1;

            my $items = shift @lines;
            $items =~ s/\}\s*$//o; $items =~ s/\s+//go;

            my $counter = 0;
            foreach my $item ( split ',', $items ){
                $counter = evalute($1) if $item =~ s/=(.+)$//o;
                print 'ENUM VALUE: ', $item, '=', $counter if $TEST;
                $define{$item} = $counter++;
            }
        }

        # BEGIN STRUCT definition 
#        elsif( $line =~ /^ (typedef \s+)? struct (?: \s+(\w+) )? \s* \{$ /xo ){
#            unshift @structs, {};
#            $structs[0]{''}{typedef}++ if $1;
#            $structs[0]{''}{tag} = $2 if $2;
#            print 'STRUCT BEGIN: ', $line if $TEST;
#        }
#
#        # END STRUCT definition 
#        elsif( $line =~ s/^\}//o ){
#            print 'STRUCT END: ', $line if $TEST;
#            die "WARNING: found end of struct without begin" unless @structs;
#
#            my ($struct, @items, $name, $repeat) = shift @structs;
#
#            $line =~ s/\s+//go;
#            @items = map {
#                $repeat = s/\[(.+?)\]$//o ? evalute($1) : 1;
#                [$_, $repeat];
#            } grep { !/\*/ } split /,/, $line;
#
#            # Compacting mask by counting repeated chars
#            1 while $$struct{''}{mask} =~ s/(?<=([b-y]))(\d+)?(\1+)(\d+)?/($2?$2:1)+length($3)+($4?$4-1:0)/geix;
#
#            if( $$struct{''}{typedef} ){
#                die "WARNING: Unnamed typedef found!\n" unless @items;
#
#                $name = @{shift @items}[0];
#                if( $$struct{''}{tag} ){
#                    $self->{struct}{ $name } = $struct;
#                }else{
#                    $$struct{''}{tag} = $name;
#                }
#
#                splice @items;
#                delete $$struct{''}{typedef};
#            }
#
#            if( $$struct{''}{tag} ){
#                #$name = join '::', map{ $$_{''}{tag} }(reverse(@structs), $struct);
#                #die "WARNING: Can't define struct while anonymous parent exists '$name'!"
#                #    if index($name, ':::') > -1;
#                #$self->{struct}{$name} = $struct;
#                $self->{struct}{ $$struct{''}{tag} } = $struct;
#            }
#
#            while( @items and ($name, $repeat) = @{shift @items} ){
#                my $item;
#
#                if( $name ){
#                    if( scalar @structs ){
#                        $structs[0]{$name} = alloc_structs($struct, $structs[0], $repeat);
#                    }else{
#                        $$self{$name} = alloc_structs($struct, undef, $repeat);
#                    }
#                }else{
#                    die "WARNING: Parent struct not fount for aggregating anonymous struct: '$name'!"
#                        unless scalar @structs;
#                    die "WARNING: Can't aggregate to parent more than 1 anonymous struct: '$name' - $repeat!"
#                        if $repeat > 1;
#
#                    die "WARNING: In current anonymous struct aggregation not supported!";
#                }
#
#                print "STRUCT: name:$name \t repeat:$repeat \t tag:$$struct{''}{tag} \t length:$$struct{''}{length} \t mask:$$struct{''}{mask}" if $TEST;
#            }
#        }
#
#        # STRUCT ITEM definition 
#        elsif( @structs ){
#            print 'STRUCT ITEM: ', $line if $TEST;
#
#            # Bitfields
#            if( $line =~ /:/o ){
#                my $empty = 0;
#                while( $line =~ /^(.+?) (?: \s+ (\w+) )? \s* : \s* (\d+)$/xo ){
#                    my ($type, $name, $len) = ($1, $2, $3);
#
#                    if( !$len or (defined $len and $len > $empty) ){
#                        push @{$structs[0]{''}{bitsets}[0]{mask}}, 0-$empty if $empty;
#
#                        unshift @{$structs[0]{''}{bitsets}}, {};
#                        #$structs[0]{''}{bitsets}[0]{buffer} = '';  # !* Storable bug fixer
#                        push @{$structs[0]{''}{buffers}}, \$structs[0]{''}{bitsets}[0]{buffer};
#
#                        $type = $self->{typedef}{$type};
#                        $structs[0]{''}{mask}   .= $$type[0];
#                        $structs[0]{''}{length} += $$type[1];
#
#                        $empty = $$type[1] * 8;
#                    }
#                    next unless $len;
#
#                    #$structs[0]{$name} = '';     # !* Storable bug fixer
#                    push @{$structs[0]{''}{bitsets}[0]{fields}}, \$structs[0]{$name};
#                    push @{$structs[0]{''}{bitsets}[0]{mask}}, $len;
#
#                    $empty -= $len;
#                }continue{
#                    $line = shift @lines;
#                    unshift(@lines,$line), last if $line =~ /^\}/o;
#                }
#
#                @{$structs[0]{''}{bitsets}} = reverse @{$structs[0]{''}{bitsets}};
#            }
#
#            # Normal item
#            elsif( $line =~ s/^(?: struct \s+)? (.+?) \s+ (?= \w+ (?: \s* \[ \s* (.+?) \s* \] )?  \s* (?:,|$) )//ox ){
#                my ($type, $name, $repeat) = $1;
#                $line =~ s/\s+//go;
#
#                my @items = map {
#                    $repeat = s/\[(.+?)\]$//o ? evalute($1) : 1;
#                    [$_, $repeat];
#                } split /,/, $line;
#
#                # sample type
#                if( exists $self->{typedef}{$type} ){
#                    my $typedef = $self->{typedef}{$type};
#                    my $strdef  = $type eq 'char' ? $self->{typedef}{zstring}[0] : undef;
#                    $strdef = $$typedef[0] if not $strdef and ( $$typedef[0] eq 'a' or
#                                                                $$typedef[0] eq 'A' or
#                                                                $$typedef[0] eq 'Z' );
#
#                    while( @items and ($name, $repeat) = @{shift @items} ){
#                        my $typedef = $typedef;
#                        $structs[0]{''}{length} += $$typedef[1] * $repeat;
#                        $typedef = [$strdef.$repeat, 1], $repeat=1 if $repeat>1 and $strdef;
#                        $structs[0]{''}{mask} .= $$typedef[0];
#                        $structs[0]{''}{mask} .= $repeat if $repeat>1;
#                        next if $$typedef[0] eq 'x' or $$typedef[0] eq 'X';
#                        if( $repeat > 1 ){
#                            #$structs[0]{$name}[$_] = '', # !* Storable bug fixer
#                            push @{$structs[0]{''}{buffers}}, \$structs[0]{$name}[$_] for 0..($repeat-1);
#                        }else{
#                            #$structs[0]{$name} = '';     # !* Storable bug fixer
#                            push @{$structs[0]{''}{buffers}}, \$structs[0]{$name};
#                        }
#                    }
#                }
#                
#                # struct type
#                elsif( exists $self->{struct}{$type} ){
#                    while( @items and ($name, $repeat) = @{shift @items} ){
#                        $structs[0]{$name} = alloc_structs( $self->{struct}{$type},
#                                                            $structs[0],
#                                                            $repeat );
#                    }
#                }
#
#                # unknown item type
#                else{ die "WARNING: Unknown struct item type '$type'\n" }
#            }
#
#            # Unknown struct item
#            else{ die "WARNING: Unknown struct item '$line'\n" }
#        }
#
#        # Unreashed code
#        else{ die "WARNING: Unreashed code '$line'\n" }
    }

    @$self{ keys %define } = values %define;
}


#sub sizeof($){
#    my ($self, $op) = @_;
#
#    return $self->{typedef}{$op}[1]         if exists $self->{typedef}{$op};
#    return $self->{struct}{$op}{''}{length} if exists $self->{struct}{$op};
#    return 0;
#}


#sub make_struct($){
#    my ($self, $struct) = @_;
#    die "WARNING: Can't make struct with unknown type: '$struct'!\n"
#        unless exists $self->{struct}{$struct};
#    return new C::Include::Struct( $self->{struct}{$struct} );
#}


sub defined($){ return exists shift->{$_[0]}; }
#sub INC{ return exists($INC{scalar($_[0] || caller)}) ? $INC{scalar($_[0] || caller)} : undef; }
#sub import{ shift; $INC{scalar(caller)} = new C::Include( @_ ) if @_; }


################################
## package C::Include::Struct ##
################################
package C::Include::Struct;

use 5.005;
use strict;
use Storable;

sub new($){
    my ($class, $self) = @_;
    $self = Storable::dclone( $self );
    bless $self, $class;
}

sub pack(;$){
    my ($self, $FILE, $data) = (shift, shift);

    if( $$self{''}{bitsets} ){
        for my $item( @{$$self{''}{bitsets}} ){
            my $i = $#{$$item{fields}};
            my $data;
            for my $size ( reverse @{$$item{mask}} ){
                $data <<= $size<1 ? 1-$size : $size;
                $data += ${$$item{fields}[$i--]} unless $size < 1;
            }
            $$item{buffer} = $data;
        }
    }
    $data = pack $$self{''}{mask}, map{ $$_ }@{$$self{''}{buffers}};

    print $FILE $data if $FILE and defined fileno $FILE;

    return $data;
}

sub unpack($;$$){
    my $self = shift;
    my $data = shift;
    
    if( defined fileno $data ){
        my( $pos, $len ) = ( shift || 0, shift || $$self{''}{length} );
        my $readed = read $data, $data, $len, $pos;
        return 0 if $readed < $len;
    }

    my $i = 0;
    ${$$self{''}{buffers}[$i++]} = $_ for unpack $$self{''}{mask}, $data;

    if( $$self{''}{bitsets} ){
        for my $item( @{$$self{''}{bitsets}} ){
            my $data = $$item{buffer} || 0;
            my $prev = $data;
            my $i = 0;
            for my $size ( @{$$item{mask}} ){
                $data >>= $size<1 ? 1-$size : $size;
                ${$$item{fields}[$i++]} = $prev - ($data << $size) unless $size < 1;
                $prev = $data;
            }
        }
    }

    return 1;
}

sub link($$){
    my ($self, $old, $new) = @_;

    $old = \$self->{$old} unless ref $old;
    $_ == $old and return $_ = $new for @{ $self->{''}{buffers} };
    die "Can't relink!";
}

sub size(){ return shift->{''}{length} }

1;

__END__

=head1 NAME

C::Include - Package to easy operate with binary data via describing they like C/C++ structs.

=head1 CLASSES AND PACKAGES

  C::Include         - Header file base parser class
  C::Include::Struct - Struct wraper class

=head1 USAGE

  use C::Include;

=head1 POSSIBILITY

 - skip comments;

 - valid preprocessor commands: define, ifdef, ifndef, endif, else;

 - supported bitset and enclosed structs, enums;

 - The compiled data may be cached in the external file and at repeated call not compiled
   any more, that will speed up operation.8);

 - may be defined and redefined type substitutions via #define
   (Ex: #define WORD word );

 - predefined standart types:
     char
     unsigned char
     short
     unsigned short
     int
     unsigned int
     long
     unsigned long
     long long
     unsigned long long
     float
     double
     pointer
     null
     neganull
     bit
     short int
     long int
     long long int
     unsigned
     unsigned long int
     unsigned short int
     unsigned long long int
     byte
     dword
     string  (null padded)
     sstring (space padded)
     zstring (null terminated, null padded)

 - predefined standart type aliases:
     'short int'              => 'short'
     'long int'               => 'long'
     'long long int'          => 'long long'
     'unsigned'               => 'unsigned long'
     'unsigned long int'      => 'unsigned long'
     'unsigned short int'     => 'unsigned short'
     'unsigned long long int' => 'unsigned long long'
     'byte'                   => 'unsigned char'
     'word'                   => 'unsigned short'
     'dword'                  => 'unsigned long'


=head1 CLASS C::Include

This class is a header file parser


=head2 FUNCTIONS


=over 4

=item * new - New object constructor

=item * sizeof - Return length of type in bytes

=item * make_struct - Return instance of struct wrapper object C::Include::Struct

=item * defined - Return TRUE if parameter is described in header

=item * INC - Return REF to alternative constructed instance C::Include

=back



=head2 REFERENCE

=over 4

=item new ( params )

As params there should be pairs (-name, value). If the name of the param
is absent - then it is accepted as 'source'.

Current version support two params types: -src and -cache.

-src param describe header file. May be \*FILEHANDLE or 'path/to/file.h'.

-cache say, that was cached the compiled header file. If the value of this parameter
is absent and -src the parameter describes path to the file, the cache the file,
will be created as 'Path/to/file.ph'. Pay attention that, while the value of this
parameter is absent and -src the parameter is \*FILEHANDLE, the cache the file
will not be created.

 Examples of usage:

    # Creating object from external header file
    my $include = new C::Include( -src, '/usr/src/include.h' );
    my $include = new C::Include( '/usr/src/include.h' );
    my $include = new C::Include( '/usr/src/include.h', -cache );
    my $include = new C::Include( '/usr/src/include.h', -cache, '/tmp/cache.ph' );

    # Create object from embended header file in Perl
    # script after __DATE__ directive
    my $include = new C::Include( \*main::DATA );
    my $include = new C::Include( \*main::DATA, -cache, '/tmp/cache.ph' );

    # Alternative constructor call
    use C::Include qw(/usr/src/include.h -cache);

=item sizeof

Return length of type in bytes.

 Examples of usage:

    my $size_of_int = $include->sizeof(int);
    my $size_of_struct = $include->sizeof(HEADER_STRUCT);

=item make_struct ( 'struct name' )

Returns a copy of the object C::Include::Struct - wrapper of structure.

 Examples of usage:

    # Create wraper class for struct described in $include as 'HEADER_STRUCT'.
    my $header = $include->make_struct( 'HEADER_STRUCT' );

=item INC( [module_name] )

Return REF to alternative constructed instance C::Include in module "module_name".
If "module_name" param not defined, then - current call module name;

Note: There should be only one created instance of C::Include for each calling unit module.

 Examples of usage:

    package My::Cfg;

    # Alternative constructor call
    use C::Include qw(/usr/src/include.h -cache);

    # Create wraper class for struct described in /usr/src/include.h
    # as 'HEADER_STRUCT'.
    my $header = INC->make_struct( 'HEADER_STRUCT' );

 in other scripts may be called instance of C::Include created in package My::Cfg as

    use C::Include;
    my $header = INC('My::Cfg')->make_struct( 'HEADER_STRUCT' );

=back


=head1 CLASS C::Include::Struct

This class is a struct wrapper.


=head2 FUNCTIONS


=over 4

=item * pack - Pack struct fields to binary and, if need, save it to file.

=item * unpack - Unpack binary data and fill with they self fields.

=item * size - Return length of struct in bytes.

=back



=head2 REFERENCE

=over 4

=item pack ( [\*FILEHANDLE] )

Pack struct fields to binary and, if need, save it to file.

As param may be REF to FILEHANDLE. If FILEHANDLE exists then packed binary data
will be saved to this file.

Function return resulted binary data as scalar.

 Examples of usage:

    $header_struct->pack( \*OUT_FILE );

    my data =  $header_struct->pack();

=item unpack

    unpack( data )
    unpack( \*FILEHANDLE, [offset, [size]] )

To unpack the binary informations, and to fill by them in the fields.

If first param is scalar, then it is binary data to unpacking. Otherwise
if first param is \*FILEHANDLE, then input binary data will be reader
from this file. If 'size' param not defined then from file will be readed
same counte of bytes as struct length. Param 'offset' describe count of bytes
to skip before reading binary data. Default offset equal 0;

 Examples of usage:

    $header_struct->unpack( $binary_data );
    $header_struct->unpack( \*CFG );
    $header_struct->unpack( \*CFG, 10, $header_struct->size );

=item size

Return length of struct in bytes.

 Examples of usage:

    # 8)
    print 'Size of struct HEADER_STRUCT: ', $header_struct->size(), ' bytes';

=back

=head1 EXAMPLE

    use C::Include;

    # Create instance of object and cache it to 'data/database.ph'
    # If 'data/database.ph' exists then only read compiled data from they.
    my $include = new C::Include( 'data/database.h', -cache )
        or die "Can't read include: $!";

    my $cfg = $include->make_struct('CONFIG')
        or die "Can't create instance of CONFIG: $!";

    # Open binary file
    open CFG, 'database.cfg';

    # Read from CFG length of struct wraped by $cfg and fill self fields
    $cfg->unpack( \*CFG );

    close CFG;


=head1 FAQ

  * Q: Where I may see module tested header file?
    A: Tested header file on which this package it
       tested included in distribudion as 'test.h'

  * Q: Why module work not correct?
    A: Probably in your computer the old version required(demanded) Storable.pm
       with an error is installed.


=head1 INTERNAL REMARKS

1. For all field definitions with type array of chars will be produced one string field:

    struct file{
      char path[__MAXPATH];   // first string field
      char name[12];          // second string field
    }

If you want to create array of bytes for definitions as array of chars, you must change type,
for example, to 'byte':

    struct array{
      char path[__MAXPATH];   // first string field
      byte flags[12];         // second field as array of unsigned chars
    }

    or

    #define BYTE char
    struct array{
      char path[__MAXPATH];   // first string field
      BYTE flags[12];         // second field as array of signed chars
    }

2. All properties of C::Include::Struct object be in element with key {''}.
Do not delete and do not change them!:

  delete $header_struct->{''};

3. You can describe in headers exclusive definitions like compiller sensivity

 Example:

    #ifdef __WATCOM_C__
        // some definitions
    #endif

    // __PERL__ is predefined in package default
    // and processed correctly
    #ifdef __PERL__
        #define skiptype neganull
    #endif

4. For optimizing memory usage and speed for pack/unpack machanizm:

  in this sample code:

    1   typedef struct HEADER{
    2       word revision;
    3       byte reserved[1024];  // in wraper object will be allocated
    4   };                        // array with 1024 elements and take CPU
                                  // to process in every pack/unpack operation
  line 3. best replace to

    char reserved[1024];      // will be created one string element
                              // with length 1024
  or

    #ifndef __PERL__
    #define nullbyte byte
    #endif
    nullbyte reserved[1024];  // this 1024 bytes will be skiped
                              // in operations pack/unpack and
                              // not allocated in wraper object
                              // as element

5. Note, that the current version processes of the preprocessor commands only once, before parsing of the main code!

  In this code:

    #define __MAXPATH 56
    struct file{
      char path[__MAXPATH];
      char name[12];
    }
    #define __MAXPATH 127

  Length of a field file.path will be, equal 127 bytes!

6. All defined values via '#define' and 'enum' are availablis:

    my $include = new C::Include( 'data/database.h', -cache )
        or die "Can't read include: $!";

    print $include->{__MAXPATH};

7. All copies of the predetermined structures also are availablis:

    struct FILE{
      char path[__MAXPATH];
      char name[12];
    }root, files[100];

    $\ = "\n";
    print $include->{root}{name};
    print $include->{files}[$_]{name} for 0..99;

8. For parsing progress indicating, need switch a flag 'TEST' to '1':

    use C::Include;
    $C::Include::TEST = 1;

    my $include = new C::Include( 'data/database.h', -cache )

=head1 AUTHOR

  Albert N. MICHEEV
  email: Albert@f80.n5049.z2.fidonet.org
  fido:  2:5049/80

=head1 COPYRIGHT

Copyright 2000, Albert N. MICHEEV (Albert@f80.n5049.z2.fidonet.org)

This library is free software; you can redistribute it and/or
modify it under the same terms as Perl itself.

=head1 AVAILABILITY

The latest version of this library is likely to be available from:

 http://www.perl.com/CPAN

=head1 SEE ALSO

Storable.

=cut
