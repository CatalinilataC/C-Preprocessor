# C-Preprocessor
A mini preprocessor for C that implements the following directives:
#define <SYMBOL> <MAPPING>
#if <COND> / #elif <COND> / #else / #endif
#ifdef <SYMBOL> / #ifndef <SYMBOL> / #else / #endif
#include “HEADER”.
  The exacutable has the following signature: so-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>]
