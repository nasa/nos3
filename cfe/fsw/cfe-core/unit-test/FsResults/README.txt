The cFE File Service (FS) unit tests were run in the following pc-linux environment:

Distributor ID: RedHatEnterpriseClient
Release:        5.11

Linux gs582w-cfelnx 2.6.18-407.el5PAE #1 SMP Fri Oct 16 12:08:56 EDT 2015 
i686 i686 i386 GNU/Linux i386

Unit tests built with gcc (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)

The expected FS unit test pass/fail test results are located in the ut_cfe_fs_log.txt file

The FS unit test results are expected to have the following coverage results for each of 
the FS source files:

gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/fs/cfe_fs_api.c' 100.00%  98
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/fs/cfe_fs_decompress.c' 88.21%  441
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/fs/cfe_fs_priv.c' 100.00%  19

==========================================================================

gcov: 'cfe_fs_api.c' 100.00% coverage

==========================================================================

gcov: 'cfe_fs_priv.c' 100.00% coverage

==========================================================================

cfe_fs_decompress.c - 88.21% coverage

cfe_fs_decompress contains the code from a public domain copy of the gzip
utility that has been sightly modified to use the cFE variable types and
return codes. Other than that, the decompress algorithm is very old and
obtuse code. Most of what was not covered was because of macros that are
defined and use in the implementation. There are also lots of one letter
variables with no documentation.
==========================================================================


int32 FS_gz_eat_header_Reentrant( CFE_FS_Decompress_State_t *State )


        1:  229:                if ( (flags & CONTINUATION) != 0 ) {
    #####:  230:                        NEXTBYTE();
    #####:  231:                        if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  232:                        NEXTBYTE();
    #####:  233:                        if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  234:                }
        -:  235:                
        1:  236:                if ( (flags & EXTRA_FIELD) != 0 ) {
        -:  237:                        uint32 len;
    #####:  238:                        len  = NEXTBYTE();
    #####:  239:                        if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  240:                        len |= NEXTBYTE() << 8;
    #####:  241:                        if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  242:                        while (len--) {
    #####:  243:                                NEXTBYTE();
    #####:  244:                                if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  245:                        }
        -:  246:                }
         1:  265:               if ( (flags & COMMENT) != 0 ) {
    #####:  266:                        keep_going = TRUE;
    #####:  267:                        while ( keep_going ) {
        -:  268:
        -:  269:                                /* Get the next byte */
    #####:  270:                                thisByte = NEXTBYTE();
        -:  271:
        -:  272:                                /* Header failure when end of file is reached or a read failure occurs */
    #####:  273:                                if ((thisByte == EOF) || (State->Error != CFE_SUCCESS)) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  274:
        -:  275:                                /* End of string was found */
    #####:  276:                                if (thisByte == 0) keep_going = FALSE;
        -:  277:                        }
        3:  311:        if ( len == OS_FS_ERROR ) 
        -:  312:   {
    #####:  313:                State->Error = CFE_FS_GZIP_READ_ERROR;
    #####:  314:                return EOF;
        -:  315:        }

int32 FS_gz_huft_build_Reentrant( CFE_FS_Decompress_State_t *State, uint32 * b, uint32 n, uint32 s, uint16 * d, uint16 * e, int32 * m )

       20:  414:   if (c[0] == n) 
        -:  415:   {                    /* null input--all zero length codes */
    #####:  416:                *m = 0;
    #####:  417:                return CFE_SUCCESS;
        -:  418:        }
     2197:  545:                        if( (q == (HufTable *)NULL) && (j < z) )
        -:  546:                        {
    #####:  547:                                return CFE_FS_GZIP_BAD_CODE_BLOCK;
        -:  548:                        }
        2:  599:        while ( State->bk >= 8 ) {
    #####:  600:                State->bk -= 8;
    #####:  601:                State->inptr--;
        -:  602:        }
    #####:  651:        else               res = CFE_FS_GZIP_BAD_CODE_BLOCK;
    #####:  706:                 return CFE_FS_GZIP_INDEX_ERROR;
    14203:  718:         if ( w == WSIZE ) 
        -:  719:         {
    #####:  720:            State->outcnt = w;
    #####:  721:            FS_gz_flush_window_Reentrant(State);
    #####:  722:            w = 0;
        -:  723:         }
    #####:  753:                  return CFE_FS_GZIP_INDEX_ERROR;
    
int32 FS_gz_inflate_dynamic_Reentrant( CFE_FS_Decompress_State_t *State )
    
        -:  943:      else 
        -:  944:      {                 /* j == 18: 11 to 138 zero length codes */
    #####:  945:         NEEDBITS(7);
    #####:  946:         j = 11 + ( (uint32)b & 0x7f );
    #####:  947:         DUMPBITS(7);
    #####:  948:         if ( (uint32)i + j > n ) return CFE_FS_GZIP_BAD_DATA;
    #####:  949:         while (j--) ll[i++] = 0;
    #####:  950:         l = 0;
        -:  951:                }
        
int32 FS_gz_inflate_stored_Reentrant( CFE_FS_Decompress_State_t *State )

    #####: 1070:        n = ( (uint32)b & 0xffff );
    #####: 1071:        DUMPBITS(16);
        -: 1072:
    #####: 1073:        NEEDBITS(16);
    #####: 1074:        if ( n != (uint32)( (~b) & 0xffff) )  return CFE_FS_GZIP_BAD_DATA;    /* error in compressed data */
    #####: 1075:        DUMPBITS(16);
        -: 1076:
        -: 1077:
        -: 1078:        /*  read and output the compressed data */
    #####: 1079:        while (n--) 
        -: 1080:   {
    #####: 1081:                NEEDBITS(8);
    #####: 1082:      State->window[w++] = (uint8)b;
    #####: 1083:                if ( w == WSIZE ) 
        -: 1084:      {
    #####: 1085:                State->outcnt = w;
    #####: 1086:                FS_gz_flush_window_Reentrant(State);
    #####: 1087:                w = 0;
        -: 1088:                }
    #####: 1089:                DUMPBITS(8);
        -: 1090:        }
        -: 1091:
        -: 1092:        /*  restore the globals from the locals */
    #####: 1093:        State->outcnt = w;                                      /* restore global window pointer */
    #####: 1094:        State->bb     = b;                                      /* restore global bit buffer */
    #####: 1095:        State->bk     = k;
        -: 1096:
    #####: 1097:        return CFE_SUCCESS;
        
==========================================================================