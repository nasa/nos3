 /*************************************************************************
 ** File:
 **   $Id: sc_rts.h 1.4 2015/03/02 12:58:31EST sstrege Exp  $
 **
 **  Copyright © 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **  This software was created at NASA's Goddard Space Flight Center.
 **  This software is governed by the NASA Open Source Agreement and may be 
 **  used, distributed and modified only pursuant to the terms of that 
 **  agreement.
 **
 ** Purpose: 
 **   This file contains human readable definitions of all of the RTS's
 **   in Stored Command. This should be edited by the mission if it wishes
 **   to name it's RTS's
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_rts.h  $ 
 **   Revision 1.4 2015/03/02 12:58:31EST sstrege  
 **   Added copyright information 
 **   Revision 1.3 2011/03/15 17:30:47EDT lwalling  
 **   Delete RTS_ID_START_AUTO_EXEC, add RTS_ID_AUTO_POWER_ON and RTS_ID_AUTO_PROCESSOR 
 **   Revision 1.2 2009/01/05 08:26:41EST nyanchik  
 **   Check in after code review changes 
 *************************************************************************/

#ifndef _sc_rts_
#define _sc_rts_

/**
 ** \name RTS 'name for a mission
         This can be shorted or expanded or changed for each mission*/ 
/** \{ */

#define RTS_ID_AUTO_POWER_ON            1    /**< \brief Startup auto-exec after power-on reset */
#define RTS_ID_AUTO_PROCESSOR           2    /**< \brief Startup auto-exec after processor reset */

#define RTS_ID_Spare3                   3    
#define RTS_ID_Spare4                   4    
#define RTS_ID_Spare5                   5                     
#define RTS_ID_Spare6                   6
#define RTS_ID_Spare7                   7
#define RTS_ID_Spare8                   8
#define RTS_ID_Spare9                   9

#define RTS_ID_Spare10                  10
#define RTS_ID_Spare11                  11    
#define RTS_ID_Spare12                  12
#define RTS_ID_Spare13                  13    
#define RTS_ID_Spare14                  14
#define RTS_ID_Spare15                  15
#define RTS_ID_Spare16                  16
#define RTS_ID_Spare17                  17    
#define RTS_ID_Spare18                  18
#define RTS_ID_Spare19                  19    

#define RTS_ID_Spare20                  20
#define RTS_ID_Spare21                  21        
#define RTS_ID_Spare22                  22
#define RTS_ID_Spare23                  23
#define RTS_ID_Spare24                  24
#define RTS_ID_Spare25                  25
#define RTS_ID_Spare26                  26
#define RTS_ID_Spare27                  27
#define RTS_ID_Spare28                  28
#define RTS_ID_Spare29                  29

#define RTS_ID_Spare30                  30
#define RTS_ID_Spare31                  31
#define RTS_ID_Spare32                  32
#define RTS_ID_Spare33                  33
#define RTS_ID_Spare34                  34
#define RTS_ID_Spare35                  35
#define RTS_ID_Spare36                  36
#define RTS_ID_Spare37                  37
#define RTS_ID_Spare38                  38
#define RTS_ID_Spare39                  39

#define RTS_ID_Spare40                  40
#define RTS_ID_Spare41                  41
#define RTS_ID_Spare42                  42
#define RTS_ID_Spare43                  43
#define RTS_ID_Spare44                  44
#define RTS_ID_Spare45                  45
#define RTS_ID_Spare46                  46
#define RTS_ID_Spare47                  47
#define RTS_ID_Spare48                  48
#define RTS_ID_Spare49                  49

#define RTS_ID_Spare50                  50
#define RTS_ID_Spare51                  51
#define RTS_ID_Spare52                  52
#define RTS_ID_Spare53                  53
#define RTS_ID_Spare54                  54
#define RTS_ID_Spare55                  55
#define RTS_ID_Spare56                  56
#define RTS_ID_Spare57                  57
#define RTS_ID_Spare58                  58
#define RTS_ID_Spare59                  59

#define RTS_ID_Spare60                  60
#define RTS_ID_Spare61                  61
#define RTS_ID_Spare62                  62
#define RTS_ID_Spare63                  63
#define RTS_ID_Spare64                  64
#define RTS_ID_Spare65                  65
#define RTS_ID_Spare66                  66
#define RTS_ID_Spare67                  67
#define RTS_ID_Spare68                  68
#define RTS_ID_Spare69                  69

#define RTS_ID_Spare70                  70
#define RTS_ID_Spare71                  71
#define RTS_ID_Spare72                  72
#define RTS_ID_Spare73                  73
#define RTS_ID_Spare74                  74
#define RTS_ID_Spare75                  75
#define RTS_ID_Spare76                  76
#define RTS_ID_Spare77                  77
#define RTS_ID_Spare78                  78
#define RTS_ID_Spare79                  79

#define RTS_ID_Spare80                  80
#define RTS_ID_Spare81                  81
#define RTS_ID_Spare82                  82
#define RTS_ID_Spare83                  83
#define RTS_ID_Spare84                  84
#define RTS_ID_Spare85                  85
#define RTS_ID_Spare86                  86
#define RTS_ID_Spare87                  87
#define RTS_ID_Spare88                  88
#define RTS_ID_Spare89                  89

#define RTS_ID_Spare90                  90
#define RTS_ID_Spare91                  91
#define RTS_ID_Spare92                  92
#define RTS_ID_Spare93                  93
#define RTS_ID_Spare94                  94
#define RTS_ID_Spare95                  95
#define RTS_ID_Spare96                  96
#define RTS_ID_Spare97                  97
#define RTS_ID_Spare98                  98
#define RTS_ID_Spare99                  99

#define RTS_ID_Spare100                 100
#define RTS_ID_Spare101                 101
#define RTS_ID_Spare102                 102
#define RTS_ID_Spare103                 103
#define RTS_ID_Spare104                 104
#define RTS_ID_Spare105                 105
#define RTS_ID_Spare106                 106  
#define RTS_ID_Spare107                 107  
#define RTS_ID_Spare108                 108  
#define RTS_ID_Spare109                 109  

#define RTS_ID_Spare110                 110  
#define RTS_ID_Spare111                 111  
#define RTS_ID_Spare112                 112  
#define RTS_ID_Spare113                 113  
#define RTS_ID_Spare114                 114  
#define RTS_ID_Spare115                 115  
#define RTS_ID_Spare116                 116  
#define RTS_ID_Spare117                 117  
#define RTS_ID_Spare118                 118  
#define RTS_ID_Spare119                 119  

#define RTS_ID_Spare120                 120  
#define RTS_ID_Spare121                 121  
#define RTS_ID_Spare122                 122  
#define RTS_ID_Spare123                 123  
#define RTS_ID_Spare124                 124  
#define RTS_ID_Spare125                 125  
#define RTS_ID_Spare126                 126  
#define RTS_ID_Spare127                 127  
#define RTS_ID_Spare128                 128  
#define RTS_ID_Spare129                 129  

#define RTS_ID_Spare130                 130  
#define RTS_ID_Spare131                 131  
#define RTS_ID_Spare132                 132  
#define RTS_ID_Spare133                 133  
#define RTS_ID_Spare134                 134  
#define RTS_ID_Spare135                 135  
#define RTS_ID_Spare136                 136  
#define RTS_ID_Spare137                 137  
#define RTS_ID_Spare138                 138  
#define RTS_ID_Spare139                 139  

#define RTS_ID_Spare140                 140  
#define RTS_ID_Spare141                 141  
#define RTS_ID_Spare142                 142  
#define RTS_ID_Spare143                 143  
#define RTS_ID_Spare144                 144  
#define RTS_ID_Spare145                 145  
#define RTS_ID_Spare146                 146  
#define RTS_ID_Spare147                 147  
#define RTS_ID_Spare148                 148  
#define RTS_ID_Spare149                 149  

#define RTS_ID_Spare150                 150  
#define RTS_ID_Spare151                 151  
#define RTS_ID_Spare152                 152  
#define RTS_ID_Spare153                 153  
#define RTS_ID_Spare154                 154  
#define RTS_ID_Spare155                 155  
#define RTS_ID_Spare156                 156  
#define RTS_ID_Spare157                 157  
#define RTS_ID_Spare158                 158  
#define RTS_ID_Spare159                 159  

#define RTS_ID_Spare160                 160  
#define RTS_ID_Spare161                 161  
#define RTS_ID_Spare162                 162  
#define RTS_ID_Spare163                 163  
#define RTS_ID_Spare164                 164  
#define RTS_ID_Spare165                 165  
#define RTS_ID_Spare166                 166  
#define RTS_ID_Spare167                 167  
#define RTS_ID_Spare168                 168  
#define RTS_ID_Spare169                 169  

#define RTS_ID_Spare170                 170  
#define RTS_ID_Spare171                 171  
#define RTS_ID_Spare172                 172  
#define RTS_ID_Spare173                 173  
#define RTS_ID_Spare174                 174  
#define RTS_ID_Spare175                 175  
#define RTS_ID_Spare176                 176  
#define RTS_ID_Spare177                 177  
#define RTS_ID_Spare178                 178  
#define RTS_ID_Spare179                 179  

#define RTS_ID_Spare180                 180  
#define RTS_ID_Spare181                 181  
#define RTS_ID_Spare182                 182  
#define RTS_ID_Spare183                 183  
#define RTS_ID_Spare184                 184  
#define RTS_ID_Spare185                 185  
#define RTS_ID_Spare186                 186  
#define RTS_ID_Spare187                 187  
#define RTS_ID_Spare188                 188  
#define RTS_ID_Spare189                 189  

#define RTS_ID_Spare190                 190  
#define RTS_ID_Spare191                 191  
#define RTS_ID_Spare192                 192  
#define RTS_ID_Spare193                 193  
#define RTS_ID_Spare194                 194  
#define RTS_ID_Spare195                 195  
#define RTS_ID_Spare196                 196  
#define RTS_ID_Spare197                 197  
#define RTS_ID_Spare198                 198  
#define RTS_ID_Spare199                 199  

#define RTS_ID_Spare200                 200  
#define RTS_ID_Spare201                 201  
#define RTS_ID_Spare202                 202  
#define RTS_ID_Spare203                 203  
#define RTS_ID_Spare204                 204  
#define RTS_ID_Spare205                 205  
#define RTS_ID_Spare206                 206  
#define RTS_ID_Spare207                 207  
#define RTS_ID_Spare208                 208  
#define RTS_ID_Spare209                 209  

#define RTS_ID_Spare210                 210  
#define RTS_ID_Spare211                 211  
#define RTS_ID_Spare212                 212  
#define RTS_ID_Spare213                 213  
#define RTS_ID_Spare214                 214  
#define RTS_ID_Spare215                 215  
#define RTS_ID_Spare216                 216  
#define RTS_ID_Spare217                 217  
#define RTS_ID_Spare218                 218  
#define RTS_ID_Spare219                 219  

#define RTS_ID_Spare220                 220  
#define RTS_ID_Spare221                 221  
#define RTS_ID_Spare222                 222  
#define RTS_ID_Spare223                 223  
#define RTS_ID_Spare224                 224  
#define RTS_ID_Spare225                 225  
#define RTS_ID_Spare226                 226  
#define RTS_ID_Spare227                 227  
#define RTS_ID_Spare228                 228  
#define RTS_ID_Spare229                 229  

#define RTS_ID_Spare230                 230  
#define RTS_ID_Spare231                 231  
#define RTS_ID_Spare232                 232  
#define RTS_ID_Spare233                 233  
#define RTS_ID_Spare234                 234  
#define RTS_ID_Spare235                 235  
#define RTS_ID_Spare236                 236  
#define RTS_ID_Spare237                 237  
#define RTS_ID_Spare238                 238  
#define RTS_ID_Spare239                 239  

#define RTS_ID_Spare240                 240  
#define RTS_ID_Spare241                 241  
#define RTS_ID_Spare242                 242  
#define RTS_ID_Spare243                 243  
#define RTS_ID_Spare244                 244  
#define RTS_ID_Spare245                 245  
#define RTS_ID_Spare246                 246  
#define RTS_ID_Spare247                 247  
#define RTS_ID_Spare248                 248  
#define RTS_ID_Spare249                 249  

#define RTS_ID_Spare250                 250  
#define RTS_ID_Spare251                 251  
#define RTS_ID_Spare252                 252  
#define RTS_ID_Spare253                 253  
#define RTS_ID_Spare254                 254  
#define RTS_ID_Spare255                 255  
#define RTS_ID_Spare256                 256  
/** \} */


#endif /* _sc_rts_ */			    

/************************/
/*  End of File Comment */
/************************/

























