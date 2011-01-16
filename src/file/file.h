/* ASE - Allegro Sprite Editor
 * Copyright (C) 2001-2010  David Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FILE_FILE_H_INCLUDED
#define FILE_FILE_H_INCLUDED

#include <stdio.h>
#include <vector>
#include <string>

#define FILE_LOAD_SEQUENCE_NONE		0x00000001
#define FILE_LOAD_SEQUENCE_ASK		0x00000002
#define FILE_LOAD_SEQUENCE_YES		0x00000004
#define FILE_LOAD_ONE_FRAME		0x00000008

class Cel;
class Image;
class Layer;
class LayerImage;
class Mutex;
class Palette;
class Sprite;

class FileFormat;
class FormatOptions;

/* file operations */
typedef enum { FileOpLoad,
	       FileOpSave } FileOpType;

/* structure to load & save files */
struct FileOp
{
  FileOpType type;		/* operation type: 0=load, 1=save */
  FileFormat* format;
  Sprite* sprite;		/* loaded sprite, or sprite to be saved */
  std::string filename;		/* file-name to load/save */

  /* shared fields between threads */
  Mutex* mutex;			/* mutex to access to the next two fields */
  float progress;		/* progress (1.0 is ready) */
  std::string error;		/* error string */
  bool done : 1;		/* true if the operation finished */
  bool stop : 1;		/* force the break of the operation */
  bool oneframe : 1;		/* load just one frame (in formats
				   that support animation like
				   GIF/FLI/ASE) */

  /* data for sequences */
  struct {
    std::vector<std::string> filename_list; /* all file names to load/save */
    Palette* palette;		/* palette of the sequence */
    Image* image;		/* image to be saved/loaded */
    /* for the progress bar */
    float progress_offset;	/* progress offset from the current frame */
    float progress_fraction;	/* progress fraction for one frame */
    /* to load sequences */
    int frame;
    bool has_alpha;
    LayerImage* layer;
    Cel* last_cel;
    FormatOptions* format_options;
  } seq;

  bool has_error() const {
    return !this->error.empty();
  }

  bool is_sequence() const {
    return !this->seq.filename_list.empty();
  }

};

/* available extensions for each load/save operation */

void get_readable_extensions(char* buf, int size);
void get_writable_extensions(char* buf, int size);

/* high-level routines to load/save sprites */

Sprite* sprite_load(const char* filename);
int sprite_save(Sprite* sprite);

/* low-level routines to load/save sprites */

FileOp *fop_to_load_sprite(const char *filename, int flags);
FileOp *fop_to_save_sprite(Sprite* sprite);
void fop_operate(FileOp *fop);
void fop_done(FileOp *fop);
void fop_stop(FileOp *fop);
void fop_free(FileOp *fop);

void fop_sequence_set_format_options(FileOp *fop, FormatOptions* format_options);
void fop_sequence_set_color(FileOp *fop, int index, int r, int g, int b);
void fop_sequence_get_color(FileOp *fop, int index, int *r, int *g, int *b);
Image* fop_sequence_image(FileOp *fi, int imgtype, int w, int h);

void fop_error(FileOp *fop, const char *error, ...);
void fop_progress(FileOp *fop, float progress);

float fop_get_progress(FileOp *fop);
bool fop_is_done(FileOp *fop);
bool fop_is_stop(FileOp *fop);

int fgetw(FILE *file);
long fgetl(FILE *file);
int fputw(int w, FILE *file);
int fputl(long l, FILE *file);

#endif
