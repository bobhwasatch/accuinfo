AccuInfo
========

AccuInfo and AccuWhite watch faces for the Pebble smart watch.

To build, the file "style.h" must be created in the top directory. This file
selects which style and must contain either "#define INVERTED 1" or
"#define INVERTED 0", without the quotes.

The first case builds with black text on a white background, the second does
the reverse. The script "build-all.sh" will build both styles, placing them
in the top directory with names of "accuinfo.pbw" and "accuwhite.pbw". The
two versions have different UUID's and so are considered different apps by
the Pebble.
