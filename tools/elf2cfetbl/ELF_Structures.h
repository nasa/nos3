typedef void *             Elf32_Addr;
typedef unsigned short int Elf32_Half;
typedef unsigned int       Elf32_Off;
typedef signed int         Elf32_Sword;
typedef unsigned int       Elf32_Word;

#define EI_NIDENT 16 /**< \brief Size of e_ident[]       */

/** 
*    Elf Header Format
*/
typedef struct {
    unsigned char e_ident [EI_NIDENT] ;  /**< \brief Machine independent data to allow decoding of file */
    Elf32_Half    e_type;                /**< \brief Identifies object file type */
    Elf32_Half    e_machine;             /**< \brief Specifies required architecture for file */
    Elf32_Word    e_version;             /**< \brief Object file version */
    Elf32_Addr    e_entry;               /**< \brief Virtual start address for process */
    Elf32_Off     e_phoff;               /**< \brief File offset to beginning of Program Header Table */
    Elf32_Off     e_shoff;               /**< \brief File offset to beginning of Section Header Table */
    Elf32_Word    e_flags;               /**< \brief Processor specific flags */
    Elf32_Half    e_ehsize;              /**< \brief ELF Header's size, in bytes */
    Elf32_Half    e_phentsize;           /**< \brief Size, in bytes, of each Program Header Table Entry */
    Elf32_Half    e_phnum;               /**< \brief Number of entries the Program Header Table contains */
    Elf32_Half    e_shentsize;           /**< \brief Size, in bytes, of each Section Header Table Entry */
    Elf32_Half    e_shnum;               /**< \brief Number of entries the Section Header Table contains */
    Elf32_Half    e_shstrndx;            /**< \brief Section Header Table index for the Section Name String Table */
} Elf32_Ehdr;

/**
*    e_type values are as follows:
*/

#define ET_NONE   0       /**< \brief No file type        */
#define ET_REL    1       /**< \brief Relocatable file    */
#define ET_EXEC   2       /**< \brief Executable file     */
#define ET_DYN    3       /**< \brief Shared object file  */
#define ET_CORE   4       /**< \brief Core file           */
#define ET_LOPROC 0xff00  /**< \brief Processor-specific  */
#define ET_HIPROC 0xffff  /**< \brief Processor-specific  */

/**
*   e_machine values are as follows:
*/
#define EM_NONE         0 /**< \brief No machine                                                      */
#define EM_M32          1 /**< \brief AT&T WE 32100                                                   */
#define EM_SPARC        2 /**< \brief SPARC                                                           */
#define EM_386          3 /**< \brief Intel 80386                                                     */
#define EM_68K          4 /**< \brief Motorola 68000                                                  */
#define EM_88K          5 /**< \brief Motorola 88000                                                  */
#define EM_860          7 /**< \brief Intel 80860                                                     */
#define EM_MIPS         8 /**< \brief MIPS RS3000                                                     */
#define EM_S370         9 /**< \brief IBM System/370 Processor                                        */
#define EM_MIPS_RS3_LE 10 /**< \brief MIPS RS3000 Little-endian                                       */
#define EM_PARISC      15 /**< \brief Hewlett-Packard PA-RISC                                         */
#define EM_VPP500      17 /**< \brief Fujitsu VPP500                                                  */
#define EM_SPARC32PLUS 18 /**< \brief Enhanced instruction set SPARC                                  */
#define EM_960         19 /**< \brief Intel 80960                                                     */
#define EM_PPC         20 /**< \brief PowerPC                                                         */
#define EM_PPC64       21 /**< \brief 64-bit PowerPC                                                  */
#define EM_V800        36 /**< \brief NEC V800                                                        */
#define EM_FR20        37 /**< \brief Fujitsu FR20                                                    */
#define EM_RH32        38 /**< \brief TRW RH-32                                                       */
#define EM_RCE         39 /**< \brief Motorola RCE                                                    */
#define EM_ARM         40 /**< \brief Advanced RISC Machines ARM                                      */
#define EM_ALPHA       41 /**< \brief Digital Alpha                                                   */
#define EM_SH          42 /**< \brief Hitachi SH                                                      */
#define EM_SPARCV9     43 /**< \brief SPARC Version 9                                                 */
#define EM_TRICORE     44 /**< \brief Siemens Tricore embedded processor                              */
#define EM_ARC         45 /**< \brief Argonaut RISC Core, Argonaut Technologies Inc.                  */
#define EM_H8_300      46 /**< \brief Hitachi H8/300                                                  */
#define EM_H8_300H     47 /**< \brief Hitachi H8/300H                                                 */
#define EM_H8S         48 /**< \brief Hitachi H8S                                                     */
#define EM_H8_500      49 /**< \brief Hitachi H8/500                                                  */
#define EM_IA_64       50 /**< \brief Intel IA-64 processor architecture                              */
#define EM_MIPS_X      51 /**< \brief Stanford MIPS-X                                                 */
#define EM_COLDFIRE    52 /**< \brief Motorola ColdFire                                               */
#define EM_68HC12      53 /**< \brief Motorola M68HC12                                                */
#define EM_MMA         54 /**< \brief Fujitsu MMA Multimedia Accelerator                              */
#define EM_PCP         55 /**< \brief Siemens PCP                                                     */
#define EM_NCPU        56 /**< \brief Sony nCPU embedded RISC processor                               */
#define EM_NDR1        57 /**< \brief Denso NDR1 microprocessor                                       */
#define EM_STARCORE    58 /**< \brief Motorola Star*Core processor                                    */
#define EM_ME16        59 /**< \brief Toyota ME16 processor                                           */
#define EM_ST100       60 /**< \brief STMicroelectronics ST100 processor                              */
#define EM_TINYJ       61 /**< \brief Advanced Logic Corp. TinyJ embedded processor family            */
#define EM_FX66        66 /**< \brief Siemens FX66 microcontroller                                    */
#define EM_ST9PLUS     67 /**< \brief STMicroelectronics ST9+ 8/16 bit microcontroller                */
#define EM_ST7         68 /**< \brief STMicroelectronics ST7 8-bit microcontroller                    */
#define EM_68HC16      69 /**< \brief Motorola MC68HC16 Microcontroller                               */
#define EM_68HC11      70 /**< \brief Motorola MC68HC11 Microcontroller                               */
#define EM_68HC08      71 /**< \brief Motorola MC68HC08 Microcontroller                               */
#define EM_68HC05      72 /**< \brief Motorola MC68HC05 Microcontroller                               */
#define EM_SVX         73 /**< \brief Silicon Graphics SVx                                            */
#define EM_ST19        74 /**< \brief STMicroelectronics ST19 8-bit microcontroller                   */
#define EM_VAX         75 /**< \brief Digital VAX                                                     */
#define EM_CRIS        76 /**< \brief Axis Communications 32-bit embedded processor                   */
#define EM_JAVELIN     77 /**< \brief Infineon Technologies 32-bit embedded processor                 */
#define EM_FIREPATH    78 /**< \brief Element 14 64-bit DSP Processor                                 */
#define EM_ZSP         79 /**< \brief LSI Logic 16-bit DSP Processor                                  */
#define EM_MMIX        80 /**< \brief Donald Knuth's educational 64-bit processor                     */
#define EM_HUANY       81 /**< \brief Harvard University machine-independent object files             */
#define EM_PRISM       82 /**< \brief SiTera Prism                                                    */
                                                                                                      
/**                                                                                                   
*    e_version values are as follows:                                                                 
*/                                                                                                    
#define EV_NONE    0 /**< \brief version         */                                                   
#define EV_CURRENT 1 /**< \brief Current version */                                                   
                                                                                                      
/**                                                                                                   
*    e_ident[] index values are as follows:                                                           
*/                                                                                                    
#define EI_MAG0    0 /**< \brief File identification     */                                           
#define EI_MAG1    1 /**< \brief File identification     */                                           
#define EI_MAG2    2 /**< \brief File identification     */                                           
#define EI_MAG3    3 /**< \brief File identification     */
#define EI_CLASS   4 /**< \brief File class              */
#define EI_DATA    5 /**< \brief Data encoding           */
#define EI_VERSION 6 /**< \brief File version            */
#define EI_PAD     7 /**< \brief Start of padding bytes  */

/**
*   e_ident[ELFMAG0...ELFMAG3] values are as follows:
*/
#define ELFMAG0 0x7f /**< \brief e_ident[EI_MAG0] */
#define ELFMAG1 'E'  /**< \brief e_ident[EI_MAG1] */
#define ELFMAG2 'L'  /**< \brief e_ident[EI_MAG2] */
#define ELFMAG3 'F'  /**< \brief e_ident[EI_MAG3] */

/**
*   e_ident[EI_CLASS] values are as follows:
*/
#define ELFCLASSNONE 0 /**< \brief Invalid class  */
#define ELFCLASS32   1 /**< \brief 32-bit objects */
#define ELFCLASS64   2 /**< \brief 64-bit objects */

/**
*   e_ident[EI_DATA] values are as follows:
*/
#define ELFDATANONE 0 /**< \brief Invalid data encoding  */
#define ELFDATA2LSB 1 /**< \brief Little-Endian storage  */
#define ELFDATA2MSB 2 /**< \brief Big-Endian storage     */

/**
*   Special Section Indexes:
*/

#define SHN_UNDEF      0       /**< \brief Marks an undefined, missing, irrelevant, or otherwise meaningless section */
#define SHN_LORESERVE  0xff00  /**< \brief Specifies the lower bound of the range of reserved indexes */
#define SHN_LOPROC     0xff00  /**< \brief Values >= than this are reserved for processor-specific semantics */
#define SHN_HIPROC     0xff1f  /**< \brief Values <= than this are reserved for processor-specific semantics */
#define SHN_ABS        0xfff1  /**< \brief Specifies absolute values for the corresponding reference */
#define SHN_COMMON     0xfff2  /**< \brief Symbols defined relative to this section are common symbols */
#define SHN_HIRESERVE  0xffff  /**< \brief Specifies the upper bound of the range of reserved indexes */



/**
*    Section Header Format
*/
typedef struct {
    Elf32_Word    sh_name;         /**< \brief Index into Section Header String Table giving location of section name */
    Elf32_Word    sh_type;         /**< \brief Section type */
    Elf32_Word    sh_flags;        /**< \brief Section attributes */
    Elf32_Addr    sh_addr;         /**< \brief Address at which first byte of section should reside */
    Elf32_Off     sh_offset;       /**< \brief File offset to first byte of Section */
    Elf32_Word    sh_size;         /**< \brief Section size, in bytes */
    Elf32_Word    sh_link;         /**< \brief Section Header Table index link (interpretation depends upon Section Type) */
    Elf32_Word    sh_info;         /**< \brief Extra information (interpretation depends upon Section Type) */
    Elf32_Word    sh_addralign;    /**< \brief Section memory address alignment constraints */
    Elf32_Word    sh_entsize;      /**< \brief Size of an entry for a Section containing a table of fixed size entries */
} Elf32_Shdr;

/**
*    sh_type values are as follows:
*/
#define SHT_NULL      0             /**< \brief Marks Section Header as inactive */
#define SHT_PROGBITS  1             /**< \brief Section contains information defined by program */
#define SHT_SYMTAB    2             /**< \brief Section contains symbols for link editing */
#define SHT_STRTAB    3             /**< \brief Section contains a string table */
#define SHT_RELA      4             /**< \brief Section contains relocation entries with explicit addends */
#define SHT_HASH      5             /**< \brief Section contains a symbol hash table */
#define SHT_DYNAMIC   6             /**< \brief Section contains information for dynamic linking */
#define SHT_NOTE      7             /**< \brief Section contains information for marking the file */
#define SHT_NOBITS    8             /**< \brief Section occupies no file space but otherwise resembles #SHT_PROGBITS */
#define SHT_REL       9             /**< \brief Section contains relocation entries without explicit addends */
#define SHT_SHLIB    10             /**< \brief Reserved Section type */
#define SHT_DYNSYM   11             /**< \brief Section contains minimal set of dynamic linking symbols */
#define SHT_INIT_ARRAY    14        /**< \brief Array of pointers to initialization functions */
#define SHT_FINI_ARRAY    15        /**< \brief Array of pointers to termination functions */
#define SHT_PREINIT_ARRAY 16        /**< \brief Array of pointers to functions invoked prior to all initialization functions */
#define SHT_GROUP         17        /**< \brief A set of sections that are related and must be treated specially by the linker */
#define SHT_SYMTAB_SHNDX  18        /**< \brief An array of values corresponding (one-to-one) with symbol table entries */

#define SHT_LOPROC   0x70000000     /**< \brief Values >= are reserved for Processor specific semantics */
#define SHT_HIPROC   0x7fffffff     /**< \brief Values <= are reserved for Processor specific semantics */
#define SHT_LOUSER   0x80000000     /**< \brief Values >= are reserved for application programs */
#define SHT_HIUSER   0xffffffff     /**< \brief Values <= are reserved for application programs */

/**
*    sh_flags values are as follows:
*/
#define SHF_WRITE      0x1         /**< \brief Section contains data that should be writable during execution */
#define SHF_ALLOC      0x2         /**< \brief Section occupies memory during process execution */
#define SHF_EXECINSTR  0x4         /**< \brief Section contains executable machine instructions */
#define SHF_MERGE      0x10        /**< \brief Section contains data that can be merged to eliminate duplication */
#define SHF_STRINGS    0x20        /**< \brief Section consists of null-terminated character strings */
#define SHF_INFO_LINK  0x40        /**< \brief sh_info field of this section contains section header table index */
#define SHF_LINK_ORDER 0x80        /**< \brief Section requires special ordering requirements for link editors */
#define SHF_OS_NONCONFORMING 0x100 /**< \brief Section requires special OS-specific processing */
#define SHF_GROUP      0x200       /**< \brief Section is a member of a section group */

#define SHF_MASKPROC   0xf0000000  /**< \brief Bits in this mask are reserved for processor specific semantics */

/**
*    Symbol Table Entry
*/
typedef struct {
    Elf32_Word    st_name;         /**< \brief String Table Index that gives the symbol name */
    Elf32_Addr    st_value;        /**< \brief Value of associated symbol */
    Elf32_Word    st_size;         /**< \brief Size, in bytes, of symbol */
    unsigned char st_info;         /**< \brief Symbol's type and binding attributes */
    unsigned char st_other;        /**< \brief  */
    Elf32_Half    st_shndx;        /**< \brief  */
} Elf32_Sym;

#define ELF32_ST_INFO (b,t) (((b) << 4 ) + ((t) & 0xf))

/**
*   Extraction and possible values of symbol table entry binding attributes in st_info
*/
#define ELF32_ST_BIND (i)   ((i) >> 4)

#define STB_LOCAL   0  /**< \brief Local symbols are not visible outside the object file */
#define STB_GLOBAL  1  /**< \brief Global symbols are visible to all object files being combined */
#define STB_WEAK    2  /**< \brief Same as Global but with lower precedence */

#define STB_LOPROC 13  /**< \brief Values >= are reserved for processor specific semantics */
#define STB_HIPROC 15  /**< \brief Values <= are reserved for processor specific semantics */

/**
*   Extraction and possible values of symbol table entry type attributes in st_info
*/
#define ELF32_ST_TYPE (i)   ((i) & 0xf)

#define STT_NOTYPE   0  /**< \brief The symbol's type is unspecified */
#define STT_OBJECT   1  /**< \brief Symbol associated with a data object (variable, array, etc) */
#define STT_FUNC     2  /**< \brief Symbol associated with a function or other executable code  */
#define STT_SECTION  3  /**< \brief Symbol associated with a section */
#define STT_FILE     4  /**< \brief Symbol's name gives the source file */
#define STT_COMMON   5  /**< \brief Symbol labels an uninitialized common block */

#define STT_LOPROC  13  /**< \brief Values >= are reserved for processor specific semantics */
#define STT_HIPROC  15  /**< \brief Values <= are reserved for processor specific semantics */

