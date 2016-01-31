/**********************************************************************
 *
 *  charclass.c
 *
 *  Author: Stephen V. Rice
 *  
 * Copyright 1996 The Board of Regents of the Nevada System of Higher
 * Education, on behalf, of the University of Nevada, Las Vegas,
 * Information Science Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 **********************************************************************/

#include "charclass.h"

static short num_classes;
static char *class_name[MAX_CHARCLASSES];
static Charclass class[NUM_CHARVALUES];

static struct range
{
    Charvalue start, stop;
    char *name;
} range[] =
{
    0x0000, 0x0000, "Unassigned",
    0x0000, 0x0008, "ASCII Control Codes",
    0x0009, 0x000D, "ASCII Spacing Characters",
    0x000E, 0x001F, "ASCII Control Codes",
    0x0020, 0x0020, "ASCII Spacing Characters",
    0x0021, 0x002F, "ASCII Special Symbols",
    0x0030, 0x0039, "ASCII Digits",
    0x003A, 0x0040, "ASCII Special Symbols",
    0x0041, 0x005A, "ASCII Uppercase Letters",
    0x005B, 0x0060, "ASCII Special Symbols",
    0x0061, 0x007A, "ASCII Lowercase Letters",
    0x007B, 0x007E, "ASCII Special Symbols",
    0x007F, 0x007F, "ASCII Control Codes",
    0x0080, 0x009F, "Latin1 Control Codes",
    0x00A0, 0x00A0, "Latin1 Spacing Characters",
    0x00A1, 0x00BF, "Latin1 Special Symbols",
    0x00C0, 0x00D6, "Latin1 Uppercase Letters",
    0x00D7, 0x00D7, "Latin1 Special Symbols",
    0x00D8, 0x00DE, "Latin1 Uppercase Letters",
    0x00DF, 0x00F6, "Latin1 Lowercase Letters",
    0x00F7, 0x00F7, "Latin1 Special Symbols",
    0x00F8, 0x00FF, "Latin1 Lowercase Letters",
    0x0100, 0x017F, "Latin Extended-A",
    0x0180, 0x024F, "Latin Extended-B",
    0x0250, 0x02AF, "IPA Extensions",
    0x02B0, 0x02FF, "Spacing Modifier Letters",
    0x0300, 0x036F, "Combining Diacritical Marks",
    0x0370, 0x03CF, "Basic Greek",
    0x03D0, 0x03FF, "Greek Symbols and Coptic",
    0x0400, 0x04FF, "Cyrillic",
    0x0530, 0x058F, "Armenian",
    0x0590, 0x05CF, "Hebrew Extended-A",
    0x05D0, 0x05EA, "Basic Hebrew",
    0x05EB, 0x05FF, "Hebrew Extended-B",
    0x0600, 0x0652, "Basic Arabic",
    0x0653, 0x06FF, "Arabic Extended",
    0x0900, 0x097F, "Devanagari",
    0x0980, 0x09FF, "Bengali",
    0x0A00, 0x0A7F, "Gurmukhi",
    0x0A80, 0x0AFF, "Gujarati",
    0x0B00, 0x0B7F, "Oriya",
    0x0B80, 0x0BFF, "Tamil",
    0x0C00, 0x0C7F, "Telugu",
    0x0C80, 0x0CFF, "Kannada",
    0x0D00, 0x0D7F, "Malayalam",
    0x0E00, 0x0E7F, "Thai",
    0x0E80, 0x0EFF, "Lao",
    0x10A0, 0x10CF, "Georgian Extended",
    0x10D0, 0x10FF, "Basic Georgian",
    0x1100, 0x11FF, "Hanguljamo",
    0x1E00, 0x1EFF, "Latin Extended Additional",
    0x1F00, 0x1FFF, "Greek Extended",
    0x2000, 0x206F, "General Punctuation",
    0x2070, 0x209F, "Superscripts and Subscripts",
    0x20A0, 0x20CF, "Currency Symbols",
    0x20D0, 0x20FF, "Combining Diacritical Marks for Symbols",
    0x2100, 0x214F, "Letterlike Symbols",
    0x2150, 0x218F, "Number Forms",
    0x2190, 0x21FF, "Arrows",
    0x2200, 0x22FF, "Mathematical Operators",
    0x2300, 0x23FF, "Miscellaneous Technical",
    0x2400, 0x243F, "Control Pictures",
    0x2440, 0x245F, "Optical Character Recognition",
    0x2460, 0x24FF, "Enclosed Alphanumerics",
    0x2500, 0x257F, "Box Drawing",
    0x2580, 0x259F, "Block Elements",
    0x25A0, 0x25FF, "Geometric Shapes",
    0x2600, 0x26FF, "Miscellaneous Symbols",
    0x2700, 0x27BF, "Dingbats",
    0x3000, 0x303F, "CJK Symbols and Punctuation",
    0x3040, 0x309F, "Hiragana",
    0x30A0, 0x30FF, "Katakana",
    0x3100, 0x312F, "Bopomofo",
    0x3130, 0x318F, "Hangul Compatibility Jamo",
    0x3190, 0x319F, "CJK Miscellaneous",
    0x3200, 0x32FF, "Enclosed CJK Letters and Months",
    0x3300, 0x33FF, "CJK Compatibility",
    0x3400, 0x3D2D, "Hangul",
    0x3D2E, 0x44B7, "Hangul Supplementary-A",
    0x44B8, 0x4DFF, "Hangul Supplementary-B",
    0x4E00, 0x9FFF, "CJK Unified Ideographs",
    0xE000, 0xF8FF, "Private Use Area",
    0xF900, 0xFAFF, "CJK Compatibility Ideographs",
    0xFB00, 0xFB4F, "Alphabetic Presentation Forms",
    0xFB50, 0xFDFF, "Arabic Presentation Forms-A",
    0xFE20, 0xFE2F, "Combining Half Marks",
    0xFE30, 0xFE4F, "CJK Compatibility Forms",
    0xFE50, 0xFE6F, "Small Form Variants",
    0xFE70, 0xFEFE, "Arabic Presentation Forms-B",
    0xFF00, 0xFFEF, "Halfwidth and Fullwidth Forms",
    0xFFF0, 0xFFFD, "Specials"
};

/**********************************************************************/

static void initialize_charclass()
{
    long i, j, k;
    for (i = 0; i < sizeof(range) / sizeof(struct range); i++)
    {
	for (j = 0; j < num_classes &&
	strcmp(range[i].name, class_name[j]) != 0; j++);
	if (j == num_classes)
	{
	    if (num_classes == MAX_CHARCLASSES)
		error("too many character classes");
	    num_classes++;
	    class_name[j] = range[i].name;
	}
	for (k = range[i].start; k <= range[i].stop; k++)
	    class[k] = j;
    }
}
/**********************************************************************/

Charclass charclass(value)
Charvalue value;
{
    if (num_classes == 0)
	initialize_charclass();
    return(class[value]);
}
/**********************************************************************/

char *charclass_name(class)
Charclass class;
{
    if (num_classes == 0)
	initialize_charclass();
    if (class >= num_classes)
	error("invalid character class");
    return(class_name[class]);
}
