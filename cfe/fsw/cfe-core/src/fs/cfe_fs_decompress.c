/*
** $Id: cfe_fs_decompress.c 1.14 2014/04/14 15:21:52GMT-05:00 lwalling Exp  $
**
**   File: CFE_FS_decompress.c
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
** 
**
**
**   Author:   A. Cudmore,    NASA/GSFC Code 582
**             Ported from JWST, which was ported from Swift/BAT, 
**             which was ported from the GNU zip sources.
**
**   Purpose: cFE Port of the gzunzip software ported from Triana->Swift->JWST 
**
** $Log: cfe_fs_decompress.c  $
** Revision 1.14 2014/04/14 15:21:52GMT-05:00 lwalling 
** Delete unsuccessful decompression output file
** Revision 1.13 2012/02/08 14:09:20EST lwalling 
** Remove unused variable
** Revision 1.12 2012/01/13 12:11:28EST acudmore 
** Changed license text to reflect open source
** Revision 1.11 2012/01/10 18:37:46EST aschoeni 
** CFE return codes set directly; FS_gz_huft_build return code is now properly redefined
** Revision 1.10 2011/09/02 14:59:15EDT jmdagost 
** Added new-line characters where missing in syslog calls.
** Revision 1.9 2011/07/13 18:24:32EDT lwalling 
** Initialized local variables r.b, r.e, r.v.n and r.v.t
** Revision 1.8 2011/07/13 17:42:22EDT lwalling 
** Initialized local variable huft_index_t
** Revision 1.7 2011/01/19 18:00:59EST lwalling 
** Restore use of heritage error codes
** Revision 1.6 2011/01/19 15:09:17EST lwalling 
** Change calls from memset() to CFE_PSP_MemSet()
** Revision 1.5 2010/10/25 18:26:06EDT jmdagost 
** Added test to while-loop to make sure array index does not go negative.
**
*/

/* Original Credit:
 *
 * inflate.c -- Not copyrighted 1992 by Mark Adler version c10p1, 10 January
 * 1993
 *
 * You can do whatever you like with this source file, though I would prefer
 * that if you modify it and redistribute it that you include comments to
 * that effect with your name and the date.  Thank you. [The history has been
 * moved to the file ChangeLog.]
 */

/*
** Includes
*/
#include "cfe_fs_decompress.h"

/*
** Global data -- Note: The following Global Data should be removed. The CFE_FS_Decompress API is not 
**                re-entrant! In order to make sure that two apps will not corrupt the data
**                the API is guarded with a Mutex. This will have serious implications on 
**                real time performance if an application blocks while another is in the API.
**               
*/
CFE_FS_Decompress_State_t CFE_FS_Decompress_State_NR;

static uint32     trace[ 3 ];

/*
** Code
*/

/* Main API entry point to decompress a file -
 * Uses a global state buffer but protects the global by a mutex, so it may block if
 * more than one thread tries to do this at any given time.
 */
int32 CFE_FS_Decompress( const char * srcFileName, const char * dstFileName )
{

   int32 rc;

   /*
   ** Lock the API with the FS Mutex
   */
   CFE_FS_LockSharedData(__func__);

   rc = CFE_FS_Decompress_Reentrant(&CFE_FS_Decompress_State_NR, srcFileName, dstFileName );

   /*
   ** Unlock FS Shared data mutex
   */
   CFE_FS_UnlockSharedData(__func__);

   return rc;
}


int32 CFE_FS_Decompress_Reentrant(CFE_FS_Decompress_State_t *State, const char * srcFileName, const char * dstFileName )
{

   int32 guzerror;

   State->Error = CFE_SUCCESS; /*Initialize this variable.  */

   /*
   ** initialize max number of Huffman tables allocated to zero 
   */
   State->max_hufts = 0;

   /*  
   ** Open input file 
   */
   State->srcFile_fd = OS_open( srcFileName, OS_READ_ONLY, 0 );

   /*
   ** if input file could not be opened, return cFE error code 
   */
   if ( State->srcFile_fd < 0 )
   {
      CFE_ES_WriteToSysLog("CFE_FS_Decompress: Cannot open source file: %s\n",
                            srcFileName);
      return (CFE_FS_GZIP_OPEN_INPUT);
   }

   /*
   ** open output file 
   */
   State->dstFile_fd = OS_creat( dstFileName, OS_WRITE_ONLY);

   /*
   ** if output file could not be opened, return cFE error code
   */ 
   if ( State->dstFile_fd < 0 )
   {
      CFE_ES_WriteToSysLog("CFE_FS_Decompress: Cannot open destination file: %s\n",
                            dstFileName);

      /* close the source file before bailing out */
      OS_close( State->srcFile_fd );

      return (CFE_FS_GZIP_OPEN_OUTPUT);
   }

   CFE_PSP_MemSet( State->hufTable,  0, MAX_HUF_TABLES * sizeof(HufTable) );
   CFE_PSP_MemSet( State->window, 0, WSIZE_X2 );
   CFE_PSP_MemSet( trace,     0, 3 * sizeof(uint32) ); 
		
   /* 
   ** uncompress the file 
   */
   guzerror = FS_gz_unzip_Reentrant(State);

   /* 
   ** close input and output files 
   */
   OS_close( State->dstFile_fd );
   OS_close( State->srcFile_fd );

   /* 
   ** delete output file after error
   */
   if (guzerror != CFE_SUCCESS)
   {
      OS_remove(dstFileName);
   }


   /* 
   ** return cFE error code
   */
   return(guzerror);
}


void FS_gz_clear_bufs_Reentrant( CFE_FS_Decompress_State_t *State )
{
	State->outcnt = 0;
	State->insize = 0;
	State->inptr  = 0;

	State->bytes_in  = 0L;
	State->bytes_out = 0L;
}


int32 FS_gz_eat_header_Reentrant( CFE_FS_Decompress_State_t *State )
{
	uint8   flags;                  /* compression flags */
	int8    magic[2];               /* magic header      */
	int8    thisByte;				     /* temporary holder for current byte in a string read */
	uint32  stamp;                  /* time stamp        */
	boolean keep_going;

	/*  read input buffer (check for read error) */
	magic[0] = (int8)NEXTBYTE();
	if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;

	magic[1] = (int8)NEXTBYTE();
	if ( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;

	if ( memcmp( magic, GZIP_MAGIC, 2 ) == 0 || memcmp( magic, OLD_GZIP_MAGIC, 2 ) == 0 ) 
   {
		
		NEXTBYTE();
		if ( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		
		flags = NEXTBYTE();
		if ( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		
		stamp  = (uint32)NEXTBYTE();
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		stamp |= (uint32)NEXTBYTE() <<  8;
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		stamp |= (uint32)NEXTBYTE() << 16;
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		stamp |= (uint32)NEXTBYTE() << 24;
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		
		NEXTBYTE();		/* Ignore extra flags for the moment */
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		NEXTBYTE();		/* Ignore OS type for the moment */ 
		if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		
		if ( (flags & CONTINUATION) != 0 ) {
			NEXTBYTE();
			if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
			NEXTBYTE();
			if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
		}
		
		if ( (flags & EXTRA_FIELD) != 0 ) {
			uint32 len;
			len  = NEXTBYTE();
			if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
			len |= NEXTBYTE() << 8;
			if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
			while (len--) {
				NEXTBYTE();
				if( State->Error != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
			}
		}
		
		/* Get original file name if it was truncated */
		if ( (flags & ORIG_NAME) != 0 ) {
			keep_going = TRUE;
			while ( keep_going ) {

				/* Get the next byte */
				thisByte = NEXTBYTE();

				/* Header failure when end of file is reached or a read failure occurs */
				if ((thisByte == EOF) || (State->Error != CFE_SUCCESS)) return CFE_FS_GZIP_READ_ERROR_HEADER;

				/* End of string was found */
				if (thisByte == 0) keep_going = FALSE;
			}
		}
		
		/*  Discard file comment if any  */
		if ( (flags & COMMENT) != 0 ) {
			keep_going = TRUE;
			while ( keep_going ) {

				/* Get the next byte */
				thisByte = NEXTBYTE();

				/* Header failure when end of file is reached or a read failure occurs */
				if ((thisByte == EOF) || (State->Error != CFE_SUCCESS)) return CFE_FS_GZIP_READ_ERROR_HEADER;

				/* End of string was found */
				if (thisByte == 0) keep_going = FALSE;
			}
		}
	}
	else
	{
		State->Error = CFE_FS_GZIP_NON_ZIP_FILE;
	}

	return State->Error;
}

/*
** Fill the input buffer. This is called only when the buffer is empty. 
*/
int16 FS_gz_fill_inbuf_Reentrant( CFE_FS_Decompress_State_t *State )
{
	int32 len;

	/*  Read as much as possible */
	State->insize = 0;

	do 
   {
		len = OS_read( State->srcFile_fd, (int8*)State->inbuf + State->insize, INBUFSIZ - State->insize );
		
		if ( len == 0 || len == EOF || len == OS_FS_ERROR ) break;
		
		State->insize += len;
		
	} while ( State->insize < INBUFSIZ );


	if ( State->insize == 0 ) return EOF;

	if ( len == OS_FS_ERROR ) 
   {
		State->Error = CFE_FS_GZIP_READ_ERROR;
		return EOF;
	}

	State->bytes_in += (uint32)State->insize;
	State->inptr = 1;

	return State->inbuf[0];
}


/*  Write the output window window[0..State->outcnt-1] and update crc and */
/*   State->bytes_out. (Used for the decompressed data only.) */
/*  merged original State->flush_window & State->write_buf together - glw */
/* fix for infinite loop when file store is full -dds */
void FS_gz_flush_window_Reentrant( CFE_FS_Decompress_State_t *State )
{

	int32  n;
	uint32  cnt;
	uint8   *gz_window_position;

	if ( State->outcnt == 0 ) return;

	FS_gz_updcrc( State->window, State->outcnt );

	cnt = State->outcnt;
	gz_window_position = State->window;

	while ( State->Error != CFE_FS_GZIP_WRITE_ERROR  &&
		    ( n = OS_write(State->dstFile_fd, (int8 *)gz_window_position, cnt) ) != cnt )
	{ 
		if ( n <= 0 ) State->Error = CFE_FS_GZIP_WRITE_ERROR;
		
		cnt -= n;
		gz_window_position += n;
	}

	State->bytes_out += (uint32)State->outcnt;

	State->outcnt = 0;
}


/*  Given a list of code lengths and a maximum table size, make a set of */
/*  tables to decode that set of codes.  Return zero on success, one if the */
/*  given code set is incomplete (the tables are still built in this case), */
/*  two if the input is invalid (all zero length codes or an oversubscribed */
/*  set of lengths), and three if not enough memory. */

/*  INPUT:  */
/*	uint32    *b;      */             /* code lengths in bits (all assumed <= BMAX) */
/*	uint32     n;      */             /* number of codes (assumed <= N_MAX)         */
/*	uint32     s;       */            /* number of simple-valued codes (0..s-1)     */
/*	uint16    *d;       */            /* list of base values for non-simple codes   */
/*	uint16    *e;       */            /* list of extra bits for non-simple codes    */
/*	int32     *m;       */            /* maximum lookup bits, returns actual        */

int32 FS_gz_huft_build_Reentrant( CFE_FS_Decompress_State_t *State, uint32 * b, uint32 n, uint32 s, uint16 * d, uint16 * e, int32 * m )
{

	uint32  a;              /* counter for codes of length k        */
	uint32  c[BMAX + 1];    /* bit length count table               */
	uint32  f;              /* i repeats in table every f entries   */
	int32   g;              /* maximum code length                  */
	int32   h;              /* table level                          */
	register uint32 i;      /* counter, current code                */
	register uint32 j;      /* counter                              */
	register int32  k;      /* number of bits in current code       */
	int32  l;               /* bits per table (returned in m)       */
	uint32 lu;              /* l unsigned                           */
	register uint32 *p;     /* pointer into c[], b[], or v[]        */
	register HufTable *q;   /* points to current table              */
	HufTable r;             /* table entry for structure assignment */
	HufTable *u[BMAX];      /* table stack                          */
	uint32 v[N_MAX];        /* values in order of bit length        */
	register int32 w;       /* bits before this table == (l * h)    */
	uint32 x[BMAX + 1];     /* bit offsets, then code stack         */
	uint32 *xp;             /* pointer into x                       */
	int32 y;                /* number of dummy codes added          */
	uint32 z;               /* number of entries in current table   */

	uint32 huft_index_q;
	uint32 huft_index_t = 0;
	boolean   not_first_table = FALSE;
	huft_index_t = 0;

	/*  Generate counts for each bit length  */
	CFE_PSP_MemSet( (void*)(c), 0, sizeof(c) );
	r.b=0;
	r.e=0;
	r.v.n=0;
	r.v.t=0;
	p = b;
	i = n;
	do 
   {
		c[*p]++;
		p++;
	} while (--i);
	
   if (c[0] == n) 
   {			/* null input--all zero length codes */
		*m = 0;
		return CFE_SUCCESS;
	}


	/*  Find minimum and maximum length, bound *m by those */
	l = *m;
	for ( j = 1; j <= BMAX; j++ ) if ( c[j] ) break;
	k = j;
	lu = (uint32)l;				   		               /* minimum code length */
	if ( lu < j ) { l = j; lu = (uint32)l; }
	for ( i = BMAX; i; i-- ) if (c[i]) break;
	g = i;						                       /* maximum code length */
	if ( lu > i ) { l = i; lu = (uint32)l; }
	*m = l;


	/*  Adjust last length count to fill out codes, if needed */
	for ( y = 1 << j; j < i; j++, y <<= 1 )
	if ( ( y -= c[j] ) < 0 )  return CFE_FS_GZIP_BAD_CODE_BLOCK;    /* bad input: more codes than bits */
	if ( (y -= c[i] ) < 0 )  return CFE_FS_GZIP_BAD_CODE_BLOCK;
	c[i] += y;


	/*  Generate starting offsets into the value table for each length */
	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	while (--i) {					/* note that i == g from above */
		j += *p++;
		*xp++ = j;
	}


	/*  Make a table of values in order of bit lengths */
	p = b;
	i = 0;
	do {
		if ( (j = *p++) != 0 ) v[x[j]++] = i;
	} while ( ++i < n );


	/*  Generate the Huffman codes and for each, make the table entries  */
	x[0] = i = 0;				  /* first Huffman code is zero   */
	p = v;						  /* grab values in bit order     */
	h = -1;						  /* no tables yet--level -1      */
	w = -l;						  /* bits decoded == (l * h)      */
	u[0] = (HufTable *)NULL;      /* just to keep compilers happy */
	q = (HufTable *)NULL;         /* ditto */
	z = 0;                        /* ditto */


	/*  go through the bit lengths (k already is bits in shortest code) */
	for (; k <= g; k++) {
		a = c[k];
		while (a--) {
			
			/*      here i is the Huffman code of length k bits for value *p */
			/*      make tables up to required level */
	        while (k > w + l) {
				h++;
				w += l;			/* previous table always l bits */
				
				/*        compute minimum size table less than or equal to l bits */
				z = g - w;
				z = ( z > lu ? l : z );                        /* upper limit on table size */
				j = k - w;
				f = 1<<j;
				if ( f > a + 1 )  {                            /* try a k-w bit table */
					
					/*          too few codes for k-w bit table  */
					f -= a + 1;                         /* deduct codes from patterns left */
					xp = c + k;
					while ( ++j < z )  {                /* try smaller tables up to z bits */
						f <<= 1;
						if ( f <= *++xp ) break;	        /* enough codes to use up j bits   */
						f -= *xp;                         /* else deduct codes from patterns */
					}
					
				}
				z = 1 << j;                           /* table entries for j-bit table   */
				
				/*        if space remains in the Huffman table memory array, "allocate" and link in new table */
				if ( State->hufts + (z + 1) > MAX_HUF_TABLES ) return CFE_FS_GZIP_NO_MEMORY;
				
				q = &State->hufTable[State->hufts];
				huft_index_q = State->hufts;
				State->hufts += z + 1;
				if ( State->hufts > State->max_hufts ) State->max_hufts = State->hufts;
				
				if ( not_first_table ) State->hufTable[huft_index_t].v.t = huft_index_q + 1;
				else                   not_first_table = TRUE;
				
				huft_index_t = huft_index_q;
				State->hufTable[huft_index_t].v.t = 0xffff;               /* stand-in for NULL */
				
				u[h] = ++q;		                        /* table starts after link */
				huft_index_q++;
				
				/*        connect to last table, if there is one */
				if ( h ) {
					x[h] = i;                /* save pattern for backing up    */
					r.b = (uint8)l;     	  /* bits to dump before this table */
					r.e = (uint8)(16 + j);	  /* bits in this table             */
					/*             r.t = q;  */               /* pointer to this table          */
					r.v.t = huft_index_q;    /* pointer to this table          */
					j = i >> (w - l);        /* (get around Turbo C bug)       */
					u[h - 1][j] = r;         /* connect to last table          */ 
				}
				
	        }
			
			/*      set up table entry in r */
	        r.b = (uint8)(k - w);
	        if (p >= v + n) {
				r.e = 99;                             /* out of values--invalid code     */
	        } else if ( *p < s ) {
				r.e = (uint8)( *p < 256 ? 16 : 15 );	/* 256 is end-of-block code        */
				r.v.n = *p;          	                /* simple code is just the value   */
				p++;                                  /* one compiler does not like *p++ */
	        } else {
				r.e = (uint8)( e[*p - s] );           /* non-simple--look up in lists */
				r.v.n = d[*p++ - s];
	        }
			
			/*      fill code-like entries with r */
	        f = 1 << (k - w);
			j = i >> w;

			if( (q == (HufTable *)NULL) && (j < z) )
			{
				return CFE_FS_GZIP_BAD_CODE_BLOCK;
			}
			else
			{
	        	for ( j = i >> w; j < z; j += f ) q[j] = r;
			}
			
			/*      backwards increment the k-bit code i */
	        for ( j = 1 << (k - 1); i & j; j >>= 1 ) i ^= j;
	        i ^= j;
			
			/*      backup over finished tables */
	        while (( h >= 0 ) && ( ( i & ((1 << w) - 1) ) != x[h] )) {
				h--;                                       /* don't need to update q */
				w -= l;
	        }
			
		}
		
	}

	/*  Return true (1) if we were given an incomplete table  */
	if((y != 0 && g != 1))
	{
		return CFE_FS_GZIP_BAD_DATA;
	}

	return CFE_SUCCESS;

}



int32 FS_gz_inflate_Reentrant( CFE_FS_Decompress_State_t *State )
{

	int32           e;        /* last block flag */
	int32   r;        /* result code     */

	/*  initialize window, bit buffer */
	State->outcnt = 0;
	State->bk     = 0;
	State->bb     = 0;

	/*  decompress until the last block */
	do {
		r = FS_gz_inflate_block_Reentrant( State, &e);
		if ( r != CFE_SUCCESS ) return r;
	} while (!e);

	/*  Undo too much lookahead. The next read will be byte aligned so we can */
	/*  discard unused bits in the last meaningful byte. */
	while ( State->bk >= 8 ) {
		State->bk -= 8;
		State->inptr--;
	}

	/*  flush out sliding window */
	FS_gz_flush_window_Reentrant(State);

	/*  return success */
	return CFE_SUCCESS;

}


/*  
** decompress an inflated block 
*/
int32 FS_gz_inflate_block_Reentrant( CFE_FS_Decompress_State_t *State, int32 * e )
{

	uint32  t;          /* block type */
	uint32  b;          /* bit buffer */
	uint32  k;          /* number of bits in bit buffer */

	int32 res;

	/*  make local bit buffer */
	b = State->bb;
	k = State->bk;

	/*  read in last block bit */
	NEEDBITS(1)
	*e = (int32)b & 1;
	DUMPBITS(1)


	/*  read in block type */
	NEEDBITS(2)
	t = (uint32)b & 3;
	DUMPBITS(2)


	/*  restore the global bit buffer */
	State->bb = b;
	State->bk = k;


	/*  inflate that block type */
	if      ( t == 0 ) { res = FS_gz_inflate_stored_Reentrant(State);  trace[0]++; }
	else if ( t == 1 ) { res = FS_gz_inflate_fixed_Reentrant(State);   trace[1]++; }
	else if ( t == 2 ) { res = FS_gz_inflate_dynamic_Reentrant(State); trace[2]++; }

	else               res = CFE_FS_GZIP_BAD_CODE_BLOCK;    

	return res;

}

/* Inflate (decompress) the codes in a deflated (compressed) block.  */
/* Return an error code or zero if it all goes ok. */
int32 FS_gz_inflate_codes_Reentrant( CFE_FS_Decompress_State_t *State, HufTable * tl, HufTable * td, int32 bl, int32 bd )
{

	static uint16 mask_bits[] = {
		0x0000,
			0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
			0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
	};


	uint32      e;          /* table entry flag/number of extra bits  */
	uint32      n, d;       /* length and index for copy              */
	uint32      w;          /* current window position                */
	HufTable   *t;          /* pointer to table entry                 */
	uint32      ml, md;     /* masks for bl and bd bits               */
	uint32      b;          /* bit buffer                             */
	uint32      k;          /* number of bits in bit buffer           */

	uint32 index;

	/*  make local copies of globals  */
	b = State->bb;					/* initialize bit buffer */
	k = State->bk;
	w = State->outcnt;				/* initialize window position */

	/*  inflate the coded data */
	ml = mask_bits[bl];			/* precompute masks for speed */
	md = mask_bits[bd];

	for (;;) 
   {					/* do until end of block */
		
		NEEDBITS((uint32)bl);
      t = tl + ((uint32)b & ml);
		e = t->e;
		if ( e > 16 ) 
      {
	        do 
           {
              if (e == 99) return CFE_FS_GZIP_BAD_DATA;
              DUMPBITS(t->b);
              e -= 16;
				  NEEDBITS(e);
              index = t->v.t + ( (uint32)b & mask_bits[e] );
				  if ( index >= 0  &&  index < State->hufts )
                 t = &( State->hufTable[index] );
				  else  
                 return CFE_FS_GZIP_INDEX_ERROR;
				  e = t->e;
	        } while ( e > 16 );
		}
		
		DUMPBITS(t->b);
			
      if ( e == 16 ) 
      {                 /* then it's a literal */
				
         /* gz_window[w++] = (uint8)(t->n); */
         State->window[w++] = (uint8)(t->v.n);
         if ( w == WSIZE ) 
         {
            State->outcnt = w;
            FS_gz_flush_window_Reentrant(State);
            w = 0;
         }
				
      } 
      else 
      {                        /* it's an EOB or a length */
				
         /*  exit if end of block  */
         if ( e == 15 ) break;
				
         /*  get length of block to copy */
         NEEDBITS(e);
         n = t->v.n + ( (uint32)b & mask_bits[e] );
         DUMPBITS(e);
				
         /*  decode distance of block to copy  */
         NEEDBITS((uint32)bd);
         t = td + ((uint32)b & md);
         e = t->e;
         if ( e > 16 ) 
         {
            do 
            {
               if (e == 99) return CFE_FS_GZIP_BAD_DATA;
               DUMPBITS(t->b);
               e -= 16;
               NEEDBITS(e);
               index = t->v.t + ( (uint32)b & mask_bits[e] );
               if ( index >= 0  &&  index < State->hufts )
                  t = &( State->hufTable[index] );
               else  
                  return CFE_FS_GZIP_INDEX_ERROR;
               e = t->e;
            } while ( e > 16 );
         }
         DUMPBITS(t->b);
					
         NEEDBITS(e);
         d = w - t->v.n - ((uint32)b & mask_bits[e]);
         DUMPBITS(e);
					
         /* do the copy */
         do 
         {
						
            d &= WSIZE - 1;
            e = WSIZE - ( d > w ? d : w);
            e = ( e > n ? n : e );
            n -= e;
						
            if ( w - d >= e ) 
            { /* (this test assumes unsigned comparison) */
							
               memcpy( State->window + w, State->window + d, e );
               w += e;
               d += e;
							
            } 
            else 
            { /* do it slow to avoid memcpy() overlap */
							
               do 
               {
                  State->window[w++] = State->window[d++];
               } while (--e);
							
            }
						
            if ( w == WSIZE ) 
            {
               State->outcnt = w;
               FS_gz_flush_window_Reentrant(State);
               w = 0;
            }
						
         } while (n);
					
      }
			
   }

	/*  restore the globals from the locals */
	State->outcnt = w;				/* restore global window pointer */
	State->bb = b;					/* restore global bit buffer */
	State->bk = k;

	return CFE_SUCCESS;
}

/*
** removed gz_huft_free, no longer needed - glw 
** -- decompress an inflated type 2 (dynamic Huffman codes) block. 
*/
int32 FS_gz_inflate_dynamic_Reentrant( CFE_FS_Decompress_State_t *State )
{

	static int32 lbits = 9;      /* bits in base literal/length lookup table */
	static int32 dbits = 6;      /* bits in base distance lookup table */

	/*  Order of the bit length code lengths */
	static uint32 border[] = {
		16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
	};

	/*  Copy lengths for literal codes 257..285 */
	static uint16 cplens[] = {
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
			35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
	};

	/*  Extra bits for literal codes 257..285    ( 99==invalid ) */
	static uint16 cplext[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
			3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
	};
 
	/*  Copy offsets for distance codes 0..29 */
	static uint16 cpdist[] = {
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
			257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
			8193, 12289, 16385, 24577
	};

	/*  Extra bits for distance codes */
	static uint16 cpdext[] = {
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
			7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
	};

	static uint16 mask_bits[] = {
		0x0000,
			0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
			0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
	};


	int32      i;               /* temporary variables                      */
	uint32     j;
	uint32     l;               /* last length                              */
	uint32     m;               /* mask for bit lengths table               */
	uint32     n;               /* number of lengths to get                 */
	HufTable  *tl;              /* literal/length code table                */
	HufTable  *td;              /* distance code table                      */
	int32      bl;              /* lookup bits for tl                       */
	int32      bd;              /* lookup bits for td                       */
	uint32     nb;              /* number of bit length codes               */
	uint32     nl;              /* number of literal/length codes           */
	uint32     nd;              /* number of distance codes                 */
	uint32     ll[286 + 30];    /* literal/length and distance code lengths */

	uint32     b;               /* bit buffer                   */
	uint32     k;               /* number of bits in bit buffer */

	int32 error_code;

	/*  make local bit buffer */
	b = State->bb;
	k = State->bk;

	/*  read in table lengths */ 
	NEEDBITS(5);
	nl = 257 + ((uint32)b & 0x1f);	/* number of literal/length codes */
	DUMPBITS(5);

	NEEDBITS(5);
	nd = 1 + ((uint32) b & 0x1f);	/* number of distance codes */
	DUMPBITS(5);

	NEEDBITS(4);
	nb = 4 + ((uint32) b & 0xf);	/* number of bit length codes */
	DUMPBITS(4);

	if (nl > 286 || nd > 30)  return CFE_FS_GZIP_BAD_DATA;				/* bad lengths */

	/*  read in bit-length-code lengths */
	for (j = 0; j < nb; j++) {
		NEEDBITS(3);
			ll[border[j]] = (uint32)b & 7;
		DUMPBITS(3);
	}
	for (; j < 19; j++) ll[border[j]] = 0;

	/*  build decoding table for trees--single level, 7 bit lookup  */
	bl = 7;
	State->hufts = 0;             /* initialize Huff Table memory */
	tl = &State->hufTable[1];        /* set known position given alloc removed - glw */
	error_code = FS_gz_huft_build_Reentrant(State, ll, 19, 19, (uint16 *)NULL, (uint16 *)NULL, &bl );
	if ( error_code != CFE_SUCCESS )  return error_code; 

	/*  read in literal and distance code lengths */
	n = nl + nd;
	m = mask_bits[bl];
	i = l = 0;
	while ( (uint32)i < n ) 
   {
		NEEDBITS((uint32)bl);
      td = tl + ((uint32)b & m);
		j = td->b;
		DUMPBITS(j);
      j = td->v.n;
		if ( j < 16 ) 
      {            /* length of code in bits (0..15) */
         ll[i++] = l = j;         /* save last length in l */
		} 
      else if ( j == 16 ) 
      {    /* repeat last length 3 to 6 times */
         NEEDBITS(2);
         j = 3 + ( (uint32)b & 3 );
         DUMPBITS(2);
         if ( (uint32)i + j > n ) return CFE_FS_GZIP_BAD_DATA;
         while (j--) ll[i++] = l;
		} 
      else if (j == 17) 
      {    /* 3 to 10 zero length codes */
         NEEDBITS(3);
         j = 3 + ( (uint32)b & 7 );
         DUMPBITS(3);
         if ( (uint32)i + j > n ) return CFE_FS_GZIP_BAD_DATA;
         while (j--) ll[i++] = 0;
         l = 0;
		} 
      else 
      {                 /* j == 18: 11 to 138 zero length codes */
         NEEDBITS(7);
         j = 11 + ( (uint32)b & 0x7f );
         DUMPBITS(7);
         if ( (uint32)i + j > n ) return CFE_FS_GZIP_BAD_DATA;
         while (j--) ll[i++] = 0;
         l = 0;
		}
	}

	/*  restore the global bit buffer  */
	State->bb = b;
	State->bk = k;

	/*  build the decoding tables for literal/length and distance codes */
	bl = lbits;
	State->hufts = 0;                 /* initialize Huff Table memory */
	tl = &State->hufTable[1];            /* set known position given alloc removed - glw */
	error_code = FS_gz_huft_build_Reentrant(State, ll, nl, 257, cplens, cplext, &bl );
	if ( error_code != CFE_SUCCESS ) return error_code;

	bd = dbits;
	td = &State->hufTable[State->hufts+1];   /* set known position given alloc removed - glw */
	error_code = FS_gz_huft_build_Reentrant(State, ll + nl, nd, 0, cpdist, cpdext, &bd );
	if ( error_code != CFE_SUCCESS ) return error_code;

	/*  decompress until an end-of-block code */
	error_code = FS_gz_inflate_codes_Reentrant(State, tl, td, bl, bd );
   
	return error_code;
}


/* Decompress an inflated type 1 (fixed Huffman codes) block.   */
/*  We should either replace this with a custom decoder,  */
/*  or at least precompute the Huffman tables. */

/* removed gz_huft_free, no longer needed - glw */
int32 FS_gz_inflate_fixed_Reentrant( CFE_FS_Decompress_State_t *State )
{
	/*  Copy lengths for literal codes 257..285 */
	static uint16 cplens[] = {
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
			35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
	};

	/*  Extra bits for literal codes 257..285    ( 99==invalid ) */
	static uint16 cplext[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
			3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
	};

	/*  Copy offsets for distance codes 0..29 */
	static uint16 cpdist[] = {
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
			257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
			8193, 12289, 16385, 24577
	};

	/*  Extra bits for distance codes */
	static uint16 cpdext[] = {
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
			7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
	};

	int32     i;            /* temporary variable            */
	HufTable *tl;           /* literal/length code table     */
	HufTable *td;           /* distance code table           */
	int32     bl;           /* lookup bits for tl            */
	int32     bd;           /* lookup bits for td            */
	uint32    l[288];       /* length list for gz_huft_build */

	int32 error_code;

	/*  set up literal table */
	for (i = 0; i < 144; i++) l[i] = 8;
	for (; i < 256; i++)      l[i] = 9;
	for (; i < 280; i++)      l[i] = 7;
	for (; i < 288; i++)      l[i] = 8;       /* make a complete, but wrong code set */

	bl = 7;
	State->hufts = 0;              /* initialize huff table "allocation" position  */
	tl = &State->hufTable[1];         /* set known position given alloc removed - glw */
	error_code = FS_gz_huft_build_Reentrant(State, l, 288, 257, cplens, cplext, &bl );

	if ( error_code != CFE_SUCCESS ) return error_code;

	/*  set up distance table */
	for ( i = 0; i < 30; i++ ) l[i] = 5;      /* make an incomplete code set */
	bd = 5;
	td = &State->hufTable[State->hufts+1];               /* set known position given alloc removed - glw */
	error_code = FS_gz_huft_build_Reentrant(State, l, 30, 0, cpdist, cpdext, &bd );

	if ( error_code != CFE_SUCCESS && error_code != CFE_FS_GZIP_BAD_DATA ) return error_code;
	/*  --- seems odd that BAD DATA is let through, but original did so --- */
	/*  decompress until an end-of-block code  */
	error_code = FS_gz_inflate_codes_Reentrant(State, tl, td, bl, bd );

	return error_code;

}

/* 
** "decompress" an inflated type 0 (stored) block. 
*/
int32 FS_gz_inflate_stored_Reentrant( CFE_FS_Decompress_State_t *State )
{

	uint32          n;     /* number of bytes in block */
	uint32          w;     /* current window position */
	register uint32 b;     /* bit buffer */
	register uint16 k;     /* number of bits in bit buffer */


	/*  make local copies of globals */
	b = State->bb;             /* initialize bit buffer */
	k = State->bk;
	w = State->outcnt;         /* initialize window position */


	/*  go to byte boundary */
	n = k & 7;
	DUMPBITS(n);

	/*  get the length and its complement */
	NEEDBITS(16);
	n = ( (uint32)b & 0xffff );
	DUMPBITS(16);

	NEEDBITS(16);
	if ( n != (uint32)( (~b) & 0xffff) )  return CFE_FS_GZIP_BAD_DATA;    /* error in compressed data */
	DUMPBITS(16);


	/*  read and output the compressed data */
	while (n--) 
   {
		NEEDBITS(8);
      State->window[w++] = (uint8)b;
		if ( w == WSIZE ) 
      {
	        State->outcnt = w;
	        FS_gz_flush_window_Reentrant(State);
	        w = 0;
		}
		DUMPBITS(8);
	}

	/*  restore the globals from the locals */
	State->outcnt = w;					/* restore global window pointer */
	State->bb     = b;					/* restore global bit buffer */
	State->bk     = k;

	return CFE_SUCCESS;

}


/* > merged original gunzip & unzip into a single routine */
/* > replaced Read & Write indirection */

int32 FS_gz_unzip_Reentrant( CFE_FS_Decompress_State_t *State )
{
	/*  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- */
	/*  Unzip in to out.  This routine works on both gzip files. */

	/*  IN assertions: the buffer State->inbuf contains already the beginning of the */
	/*   compressed data, from offsets State->inptr to State->insize-1 included. The magic */
	/*   header has already been checked. The output buffer is cleared. */

	int32          guzerror;
	uint32         orig_crc = 0;        /* original crc */
	uint32         orig_len = 0;        /* original uncompressed length */
	int32          n;
	uint8          buf[EXTHDR];         /* extended local header */
	int32          res;

	FS_gz_clear_bufs_Reentrant(State);

	/*  Snarf up the header for the zip file, check for read error */
	guzerror = FS_gz_eat_header_Reentrant(State);

	if( guzerror != CFE_SUCCESS ) return guzerror;

	FS_gz_updcrc(NULL, 0);	                /* initialize crc */

	res = FS_gz_inflate_Reentrant(State);

	if ( res != CFE_SUCCESS ) return res;

	/*  Get the crc and original length */
	/*    crc32  (see algorithm.doc) uncompressed input size modulo 2^32 */
	for ( n = 0; n < 8; n++ ) 
   {
		buf[n] = NEXTBYTE();      
		if ( State->Error != CFE_SUCCESS ) return State->Error;
	}
	orig_crc = LG(buf);
	orig_len = LG(buf + 4);

	/*  Validate decompression */
	if ( orig_crc != FS_gz_updcrc( State->outbuf, 0 ) ) return CFE_FS_GZIP_CRC_ERROR;
	if ( orig_len != (uint32)State->bytes_out )      return CFE_FS_GZIP_LENGTH_ERROR;

	return CFE_SUCCESS;

}


/*  Run a set of bytes through the crc shift register.  If s is a NULL */
/*   pointer, then initialize the crc shift register contents instead. */ 
/*   Return the current crc in either case. */

/*  input: */
/*	  s  pointer to bytes to pump through */
/*	  n	 number of bytes in s[] */
uint32 FS_gz_updcrc( uint8 * s, uint32 n )
{
	static uint32 crc_32_tab[] = {
			0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
			0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
			0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
			0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
			0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
			0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
			0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
			0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
			0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
			0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
			0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
			0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
			0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
			0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
			0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
			0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
			0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
			0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
			0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
			0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
			0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
			0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
			0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
			0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
			0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
			0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
			0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
			0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
			0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
			0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
			0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
			0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
			0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
			0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
			0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
			0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
			0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
			0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
			0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
			0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
			0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
			0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
			0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
			0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
			0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
			0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
			0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
			0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
			0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
			0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
			0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
			0x2d02ef8dL
	};

	register uint32 c;                           /* temporary variable      */

	static uint32 crc = (uint32)0xffffffffL;     /* shift register contents */

	if (s == NULL) 
   {
		c = 0xffffffffL;
	} 
   else 
   {
		
		c = crc;
		if (n) 
      {
         do
         {
				c = crc_32_tab[ ( (int16)c ^ (*s++) ) & 0xff ] ^ (c >> 8);
				
         } while (--n);			
		}		
	}

	crc = c;

	return c ^ 0xffffffffL;      /* (instead of ~c for 64-bit machines) */

}


