/*
 * "$Id: normalize.h,v 1.1.2.1 2002/08/19 01:15:20 mike Exp $"
 *
 *   Unicode normalization for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 1997-2002 by Easy Software Products.
 *
 *   These coded instructions, statements, and computer programs are
 *   the property of Easy Software Products and are protected by Federal
 *   copyright law.  Distribution and use rights are outlined in the
 *   file "LICENSE.txt" which should have been included with this file.
 *   If this file is missing or damaged please contact Easy Software
 *   Products at:
 *
 *       Attn: CUPS Licensing Information
 *       Easy Software Products
 *       44141 Airport View Drive, Suite 204
 *       Hollywood, Maryland 20636-3111 USA
 *
 *       Voice: (301) 373-9603
 *       EMail: cups-info@cups.org
 *         WWW: http://www.cups.org
 */

#ifndef _CUPS_NORMALIZE_H_
#  define _CUPS_NORMALIZE_H_

/*
 * Include necessary headers...
 */

#  include "transcode.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

/*
 * Types...
 */

typedef enum                    /**** Normalizataion Types ****/
{
  CUPS_NORM_NFD,                /* Canonical Decomposition */
  CUPS_NORM_NFKD,               /* Compatibility Decomposition */
  CUPS_NORM_NFC,                /* NFD, them Canonical Composition */
  CUPS_NORM_NFKC                /* NFKD, them Canonical Composition */
} cups_normalize_t;

typedef enum                    /**** Case Folding Types ****/
{
  CUPS_FOLD_SIMPLE,             /* Simple - no expansion in size */
  CUPS_FOLD_FULL                /* Full - possible expansion in size */
} cups_folding_t;

typedef enum                    /**** Unicode Char Property Types ****/
{
  CUPS_PROP_GENERAL_CATEGORY,   /* See 'cups_gencat_t' enum */
  CUPS_PROP_BIDI_CATEGORY,      /* See 'cups_bidicat_t' enum */
  CUPS_PROP_COMBINING_CLASS,    /* See 'cups_combclass_t' type */
  CUPS_PROP_BREAK_CLASS         /* See 'cups_breakclass_t' enum */
} cups_property_t;

/*
 * Note - parse Unicode char general category from 'UnicodeData.txt'
 * into sparse local table in 'normalize.c'.
 * Use major classes for logic optimizations throughout (by mask).
 */

typedef enum                    /**** Unicode General Category ****/
{
  CUPS_GENCAT_L  = 0x10, /* Letter major class */
  CUPS_GENCAT_LU = 0x11, /* Lu Letter, Uppercase */
  CUPS_GENCAT_LL = 0x12, /* Ll Letter, Lowercase */
  CUPS_GENCAT_LT = 0x13, /* Lt Letter, Titlecase */
  CUPS_GENCAT_LM = 0x14, /* Lm Letter, Modifier */
  CUPS_GENCAT_LO = 0x15, /* Lo Letter, Other */
  CUPS_GENCAT_M  = 0x20, /* Mark major class */
  CUPS_GENCAT_MN = 0x21, /* Mn Mark, Non-Spacing */
  CUPS_GENCAT_MC = 0x22, /* Mc Mark, Spacing Combining */
  CUPS_GENCAT_ME = 0x23, /* Me Mark, Enclosing */
  CUPS_GENCAT_N  = 0x30, /* Number major class */
  CUPS_GENCAT_ND = 0x31, /* Nd Number, Decimal Digit */
  CUPS_GENCAT_NL = 0x32, /* Nl Number, Letter */
  CUPS_GENCAT_NO = 0x33, /* No Number, Other */
  CUPS_GENCAT_P  = 0x40, /* Punctuation major class */
  CUPS_GENCAT_PC = 0x41, /* Pc Punctuation, Connector */
  CUPS_GENCAT_PD = 0x42, /* Pd Punctuation, Dash */
  CUPS_GENCAT_PS = 0x43, /* Ps Punctuation, Open (start) */
  CUPS_GENCAT_PE = 0x44, /* Pe Punctuation, Close (end) */
  CUPS_GENCAT_PI = 0x45, /* Pi Punctuation, Initial Quote */
  CUPS_GENCAT_PF = 0x46, /* Pf Punctuation, Final Quote */
  CUPS_GENCAT_PO = 0x47, /* Po Punctuation, Other */
  CUPS_GENCAT_S  = 0x50, /* Symbol major class */
  CUPS_GENCAT_SM = 0x51, /* Sm Symbol, Math */
  CUPS_GENCAT_SC = 0x52, /* Sc Symbol, Currency */
  CUPS_GENCAT_SK = 0x53, /* Sk Symbol, Modifier */
  CUPS_GENCAT_SO = 0x54, /* So Symbol, Other */
  CUPS_GENCAT_Z  = 0x60, /* Separator major class */
  CUPS_GENCAT_ZS = 0x61, /* Zs Separator, Space */
  CUPS_GENCAT_ZL = 0x62, /* Zl Separator, Line */
  CUPS_GENCAT_ZP = 0x63, /* Zp Separator, Paragraph */
  CUPS_GENCAT_C  = 0x70, /* Other (miscellaneous) major class */
  CUPS_GENCAT_CC = 0x71, /* Cc Other, Control */
  CUPS_GENCAT_CF = 0x72, /* Cf Other, Format */
  CUPS_GENCAT_CS = 0x73, /* Cs Other, Surrogate */
  CUPS_GENCAT_CO = 0x74, /* Co Other, Private Use */
  CUPS_GENCAT_CN = 0x75  /* Cn Other, Not Assigned */
} cups_gencat_t;

/*
 * Note - parse Unicode char bidi category from 'UnicodeData.txt'
 * into sparse local table in 'normalize.c'.
 * Add bidirectional support to 'textcommon.c' - per Mike
 */

typedef enum                    /**** Unicode Bidi Category ****/
{
  CUPS_BIDI_L,   /* Left-to-Right (Alpha, Syllabic, Ideographic) */
  CUPS_BIDI_LRE, /* Left-to-Right Embedding (explicit) */
  CUPS_BIDI_LRO, /* Left-to-Right Override (explicit) */
  CUPS_BIDI_R,   /* Right-to-Left (Hebrew alphabet and most punct) */
  CUPS_BIDI_AL,  /* Right-to-Left Arabic (Arabic, Thaana, Syriac) */
  CUPS_BIDI_RLE, /* Right-to-Left Embedding (explicit) */
  CUPS_BIDI_RLO, /* Right-to-Left Override (explicit) */
  CUPS_BIDI_PDF, /* Pop Directional Format */
  CUPS_BIDI_EN,  /* Euro Number (Euro and East Arabic-Indic digits) */
  CUPS_BIDI_ES,  /* Euro Number Separator (Slash) */
  CUPS_BIDI_ET,  /* Euro Number Termintor (Plus, Minus, Degree, etc) */
  CUPS_BIDI_AN,  /* Arabic Number (Arabic-Indic digits, separators) */
  CUPS_BIDI_CS,  /* Common Number Separator (Colon, Comma, Dot, etc) */
  CUPS_BIDI_NSM, /* Non-Spacing Mark (category Mn / Me in UCD) */
  CUPS_BIDI_BN,  /* Boundary Neutral (Formatting / Control chars) */
  CUPS_BIDI_B,   /* Paragraph Separator */
  CUPS_BIDI_S,   /* Segment Separator (Tab) */
  CUPS_BIDI_WS,  /* Whitespace Space (Space, Line Separator, etc) */
  CUPS_BIDI_ON   /* Other Neutrals */
} cups_bidicat_t;

/*
 * Note - parse Unicode line break class from 'DerivedLineBreak.txt'
 * into sparse local table (list of class ranges) in 'normalize.c'.
 * Note - add state table from UAX-14, section 7.3 - Ira
 * Remember to do BK and SP in outer loop (not in state table).
 * Consider optimization for CM (combining mark).
 * See 'LineBreak.txt' (12,875) and 'DerivedLineBreak.txt' (1,350).
 */

typedef enum                    /**** Unicode Line Break Class ****/
{
 /*
  * (A) - Allow Break AFTER
  * (XA) - Prevent Break AFTER
  * (B) - Allow Break BEFORE
  * (XB) - Prevent Break BEFORE
  * (P) - Allow Break For Pair
  * (XP) - Prevent Break For Pair
  */
  CUPS_BREAK_AI, /* Ambiguous (Alphabetic or Ideograph) */
  CUPS_BREAK_AL, /* Ordinary Alphabetic / Symbol Chars (XP) */
  CUPS_BREAK_BA, /* Break Opportunity After Chars (A) */
  CUPS_BREAK_BB, /* Break Opportunities Before Chars (B) */
  CUPS_BREAK_B2, /* Break Opportunity Before / After (B/A/XP) */
  CUPS_BREAK_BK, /* Mandatory Break (A) (normative) */
  CUPS_BREAK_CB, /* Contingent Break (B/A) (normative) */
  CUPS_BREAK_CL, /* Closing Punctuation (XB) */
  CUPS_BREAK_CM, /* Attached Chars / Combining (XB) (normative) */
  CUPS_BREAK_CR, /* Carriage Return (A) (normative) */
  CUPS_BREAK_EX, /* Exclamation / Interrogation (XB) */
  CUPS_BREAK_GL, /* Non-breaking ("Glue") (XB/XA) (normative) */
  CUPS_BREAK_HY, /* Hyphen (XA) */
  CUPS_BREAK_ID, /* Ideographic (B/A) */
  CUPS_BREAK_IN, /* Inseparable chars (XP) */
  CUPS_BREAK_IS, /* Numeric Separator (Infix) (XB) */
  CUPS_BREAK_LF, /* Line Feed (A) (normative) */
  CUPS_BREAK_NS, /* Non-starters (XB) */
  CUPS_BREAK_NU, /* Numeric (XP) */
  CUPS_BREAK_OP, /* Opening Punctuation (XA) */
  CUPS_BREAK_PO, /* Postfix (Numeric) (XB) */
  CUPS_BREAK_PR, /* Prefix (Numeric) (XA) */
  CUPS_BREAK_QU, /* Ambiguous Quotation (XB/XA) */
  CUPS_BREAK_SA, /* Context Dependent (South East Asian) (P) */
  CUPS_BREAK_SG, /* Surrogates (XP) (normative) */
  CUPS_BREAK_SP, /* Space (A) (normative) */
  CUPS_BREAK_SY, /* Symbols Allowing Break After (A) */
  CUPS_BREAK_XX, /* Unknown (XP) */
  CUPS_BREAK_ZW  /* Zero Width Space (A) (normative) */
} cups_breakclass_t;

typedef int cups_combclass_t;   /**** Unicode Combining Class ****/
                                /* 0=base / 1..254=combining char */

/*
 * Structures...
 */

typedef struct cups_normmap_str /**** Normalize Map Cache Struct ****/
{
  struct cups_normmap_str *next;        /* Next normalize in cache */
  int                   used;           /* Number of times entry used */
  cups_normalize_t      normalize;      /* Normalization type */
  int                   normcount;      /* Count of Source Chars */
  ucs2_t                *uni2norm;      /* Char -> Normalization */
                                        /* ...only supports UCS-2 */
} cups_normmap_t;

typedef struct cups_foldmap_str /**** Case Fold Map Cache Struct ****/
{
  struct cups_foldmap_str *next;        /* Next case fold in cache */
  int                   used;           /* Number of times entry used */
  cups_folding_t        fold;           /* Case folding type */
  int                   foldcount;      /* Count of Source Chars */
  ucs2_t                *uni2fold;      /* Char -> Folded Char(s) */
                                        /* ...only supports UCS-2 */
} cups_foldmap_t;

typedef struct cups_prop_str    /**** Char Property Struct ****/
{
  ucs2_t                ch;             /* Unicode Char as UCS-2 */
  unsigned char         gencat;         /* General Category */
  unsigned char         bidicat;        /* Bidirectional Category */
} cups_prop_t;

typedef struct                  /**** Char Property Map Struct ****/
{
  int                   used;           /* Number of times entry used */
  int                   propcount;      /* Count of Source Chars */
  cups_prop_t           *uni2prop;      /* Char -> Properties */
} cups_propmap_t;

typedef struct                  /**** Line Break Class Map Struct ****/
{
  int                   used;           /* Number of times entry used */
  int                   breakcount;     /* Count of Source Chars */
  ucs2_t                *uni2break;     /* Char -> Line Break Class */
} cups_breakmap_t;

typedef struct cups_comb_str    /**** Char Combining Class Struct ****/
{
  ucs2_t                ch;             /* Unicode Char as UCS-2 */
  unsigned char         combclass;      /* Combining Class */
  unsigned char         reserved;       /* Reserved for alignment */
} cups_comb_t;

typedef struct                  /**** Combining Class Map Struct ****/
{
  int                   used;           /* Number of times entry used */
  int                   combcount;      /* Count of Source Chars */
  cups_comb_t           *uni2comb;      /* Char -> Combining Class */
} cups_combmap_t;

/*
 * Globals...
 */

extern int      NzSupportUcs2;  /* Support UCS-2 (16-bit) mapping */
extern int      NzSupportUcs4;  /* Support UCS-4 (32-bit) mapping */

/*
 * Prototypes...
 */

/*
 * Utility functions for normalization module
 */
extern int      cupsNormalizeMapsGet(void);
extern int      cupsNormalizeMapsFree(void);
extern void     cupsNormalizeMapsFlush(void);

/*
 * Normalize UTF-8 string to Unicode UAX-15 Normalization Form
 * Note - Compatibility Normalization Forms (NFKD/NFKC) are
 * unsafe for subsequent transcoding to legacy charsets
 */
extern int      cupsUtf8Normalize(utf8_t *dest, /* O - Target string */
                    const utf8_t *src,          /* I - Source string */
                    const int maxout,           /* I - Max output */
                    const cups_normalize_t normalize);
                                                /* I - Normalization */

/*
 * Normalize UTF-32 string to Unicode UAX-15 Normalization Form
 * Note - Compatibility Normalization Forms (NFKD/NFKC) are
 * unsafe for subsequent transcoding to legacy charsets
 */
extern int      cupsUtf32Normalize(utf32_t *dest,
                                                /* O - Target string */
                    const utf32_t *src,         /* I - Source string */
                    const int maxout,           /* I - Max output */
                    const cups_normalize_t normalize);
                                                /* I - Normalization */

/*
 * Case Fold UTF-8 string per Unicode UAX-21 Section 2.3
 * Note - Case folding output is
 * unsafe for subsequent transcoding to legacy charsets
 */
extern int      cupsUtf8CaseFold(utf8_t *dest,  /* O - Target string */
                    const utf8_t *src,          /* I - Source string */
                    const int maxout,           /* I - Max output */
                    const cups_folding_t fold); /* I - Fold Mode */

/*
 * Case Fold UTF-32 string per Unicode UAX-21 Section 2.3
 * Note - Case folding output is
 * unsafe for subsequent transcoding to legacy charsets
 */
extern int      cupsUtf32CaseFold(utf32_t *dest,/* O - Target string */
                    const utf32_t *src,         /* I - Source string */
                    const int maxout,           /* I - Max output */
                    const cups_folding_t fold); /* I - Fold Mode */

/*
 * Compare UTF-8 strings after case folding
 */
extern int      cupsUtf8CompareCaseless(const utf8_t *s1,
                                                /* I - String1 */
                    const utf8_t *s2);          /* I - String2 */

/*
 * Compare UTF-32 strings after case folding
 */
extern int      cupsUtf32CompareCaseless(const utf32_t *s1,
                                                /* I - String1 */
                    const utf32_t *s2);         /* I - String2 */

/*
 * Compare UTF-8 strings after case folding and NFKC normalization
 */
extern int      cupsUtf8CompareIdentifier(const utf8_t *s1,
                                                /* I - String1 */
                    const utf8_t *s2);          /* I - String2 */

/*
 * Compare UTF-32 strings after case folding and NFKC normalization
 */
extern int      cupsUtf32CompareIdentifier(const utf32_t *s1,
                                                /* I - String1 */
                    const utf32_t *s2);         /* I - String2 */

/*
 * Get UTF-32 character property
 */
extern int      cupsUtf32CharacterProperty(const utf32_t ch,
                                                /* I - Source char */
                    const cups_property_t property);
                                                /* I - Char Property */

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif /* !_CUPS_NORMALIZE_H_ */

/*
 * End of "$Id: normalize.h,v 1.1.2.1 2002/08/19 01:15:20 mike Exp $"
 */
