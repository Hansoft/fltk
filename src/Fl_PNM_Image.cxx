//
// "$Id: Fl_PNM_Image.cxx,v 1.1.2.6 2002/04/11 11:52:41 easysw Exp $"
//
// Fl_PNM_Image routines.
//
// Copyright 1997-2002 by Easy Software Products.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//
// Contents:
//
//   Fl_PNM_Image::Fl_PNM_Image() - Load a PNM image...
//

//
// Include necessary header files...
//

#include <FL/Fl.H>
#include <FL/Fl_PNM_Image.H>
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "flstring.h"
#include <ctype.h>


//
// 'Fl_PNM_Image::Fl_PNM_Image()' - Load a PNM image...
//

Fl_PNM_Image::Fl_PNM_Image(const char *name)	// I - File to read
  : Fl_RGB_Image(0,0,0) {
  FILE		*fp;		// File pointer
  int		x, y;		// Looping vars
  char		line[1024],	// Input line
		*lineptr;	// Pointer in line
  uchar		*ptr,		// Pointer to pixel values
		byte,		// Byte from file
		bit;		// Bit in pixel
  int		format,		// Format of PNM file
		val,		// Pixel value
		maxval;		// Maximum pixel value


  if ((fp = fopen(name, "rb")) == NULL) return;

  //
  // Read the file header in the format:
  //
  //   Pformat
  //   # comment1
  //   # comment2
  //   ...
  //   # commentN
  //   width
  //   height
  //   max sample
  //

  lineptr = fgets(line, sizeof(line), fp);
  if (!lineptr) {
    fclose(fp);
    Fl::error("Early end-of-file in PNM file \"%s\"!", name);
    return;
  }

  lineptr ++;

  format = atoi(lineptr);
  while (isdigit(*lineptr)) lineptr ++;

  while (lineptr != NULL && w() == 0) {
    if (*lineptr == '\0' || *lineptr == '#') {
      lineptr = fgets(line, sizeof(line), fp);
    } else if (isdigit(*lineptr)) {
      w(atoi(lineptr));
      while (isdigit(*lineptr))	lineptr ++;
    } else lineptr ++;
  }

  while (lineptr != NULL && h()) {
    if (*lineptr == '\0' || *lineptr == '#') {
      lineptr = fgets(line, sizeof(line), fp);
    } else if (isdigit(*lineptr)) {
      h(atoi(lineptr));
      while (isdigit(*lineptr)) lineptr ++;
    } else lineptr ++;
  }

  if (format != 1 && format != 4) {
    maxval = 0;

    while (lineptr != NULL && maxval == 0) {
      if (*lineptr == '\0' || *lineptr == '#') {
	lineptr = fgets(line, sizeof(line), fp);
      } else if (isdigit(*lineptr)) {
	maxval = atoi(lineptr);
	while (isdigit(*lineptr)) lineptr ++;
      } else lineptr ++;
    }
  } else maxval = 1;

  // Allocate memory...
  if (format == 1 || format == 2 || format == 4 || format == 5) d(1);
  else d(3);

  array = new uchar[w() * h() * d()];

  // Read the image file...
  for (y = 0; y < h(); y ++) {
    ptr = (uchar *)array + y * w() * d();

    switch (format) {
      case 1 :
      case 2 :
          for (x = w(); x > 0; x --)
            if (fscanf(fp, "%d", &val) == 1) *ptr++ = 255 * val / maxval;
          break;

      case 3 :
          for (x = w(); x > 0; x --) {
            if (fscanf(fp, "%d", &val) == 1) *ptr++ = 255 * val / maxval;
            if (fscanf(fp, "%d", &val) == 1) *ptr++ = 255 * val / maxval;
            if (fscanf(fp, "%d", &val) == 1) *ptr++ = 255 * val / maxval;
          }
          break;

      case 4 :
          for (x = w(), byte = getc(fp), bit = 128; x > 0; x --) {
	    if (byte & bit) *ptr++ = 255;
	    else *ptr++ = 0;

            if (bit > 1) bit >>= 1;
            else {
              bit  = 128;
              byte = getc(fp);
            }
          }
          break;

      case 5 :
      case 6 :
          fread(ptr, w(), d(), fp);
          break;
    }
  }

  fclose(fp);
}


//
// End of "$Id: Fl_PNM_Image.cxx,v 1.1.2.6 2002/04/11 11:52:41 easysw Exp $".
//
