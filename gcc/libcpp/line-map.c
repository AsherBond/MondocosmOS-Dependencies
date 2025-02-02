/* Map logical line numbers to (source file, line number) pairs.
   Copyright (C) 2001, 2003, 2004, 2007, 2008, 2009
   Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include "config.h"
#include "system.h"
#include "line-map.h"

static void trace_include (const struct line_maps *, const struct line_map *);

/* Initialize a line map set.  */

void
linemap_init (struct line_maps *set)
{
  set->maps = NULL;
  set->allocated = 0;
  set->used = 0;
  set->trace_includes = false;
  set->depth = 0;
  set->cache = 0;
  set->highest_location = RESERVED_LOCATION_COUNT - 1;
  set->highest_line = RESERVED_LOCATION_COUNT - 1;
  set->max_column_hint = 0;
  set->reallocator = 0;
}

/* Check for and warn about line_maps entered but not exited.  */

void
linemap_check_files_exited (struct line_maps *set)
{
  struct line_map *map;
  /* Depending upon whether we are handling preprocessed input or
     not, this can be a user error or an ICE.  */
  for (map = &set->maps[set->used - 1]; ! MAIN_FILE_P (map);
       map = INCLUDED_FROM (set, map))
    fprintf (stderr, "line-map.c: file \"%s\" entered but not left\n",
	     map->to_file);
}
 
/* Free a line map set.  */

void
linemap_free (struct line_maps *set)
{
  if (set->maps)
    {
      linemap_check_files_exited (set);

      free (set->maps);
    }
}

/* Add a mapping of logical source line to physical source file and
   line number.

   The text pointed to by TO_FILE must have a lifetime
   at least as long as the final call to lookup_line ().  An empty
   TO_FILE means standard input.  If reason is LC_LEAVE, and
   TO_FILE is NULL, then TO_FILE, TO_LINE and SYSP are given their
   natural values considering the file we are returning to.

   FROM_LINE should be monotonic increasing across calls to this
   function.  A call to this function can relocate the previous set of
   maps, so any stored line_map pointers should not be used.  */

const struct line_map *
linemap_add (struct line_maps *set, enum lc_reason reason,
	     unsigned int sysp, const char *to_file, linenum_type to_line)
{
  struct line_map *map;
  source_location start_location = set->highest_location + 1;

  if (set->used && start_location < set->maps[set->used - 1].start_location)
    abort ();

  if (set->used == set->allocated)
    {
      line_map_realloc reallocator
	= set->reallocator ? set->reallocator : xrealloc;
      set->allocated = 2 * set->allocated + 256;
      set->maps
	= (struct line_map *) (*reallocator) (set->maps,
					      set->allocated
					      * sizeof (struct line_map));
      memset (&set->maps[set->used], 0, ((set->allocated - set->used)
					 * sizeof (struct line_map)));
    }

  map = &set->maps[set->used];

  if (to_file && *to_file == '\0' && reason != LC_RENAME_VERBATIM)
    to_file = "<stdin>";

  if (reason == LC_RENAME_VERBATIM)
    reason = LC_RENAME;

  if (set->depth == 0 && reason == LC_RENAME)
    abort ();

  if (reason == LC_LEAVE)
    {
      struct line_map *from;
      bool error;

      if (MAIN_FILE_P (map - 1))
	{
	  if (to_file == NULL)
	    {
	      set->depth--;
	      return NULL;
	    }
	  error = true;
          reason = LC_RENAME;
          from = map - 1;
	}
      else
	{
	  from = INCLUDED_FROM (set, map - 1);
	  error = to_file && filename_cmp (from->to_file, to_file);
	}

      /* Depending upon whether we are handling preprocessed input or
	 not, this can be a user error or an ICE.  */
      if (error)
	fprintf (stderr, "line-map.c: file \"%s\" left but not entered\n",
		 to_file);

      /* A TO_FILE of NULL is special - we use the natural values.  */
      if (error || to_file == NULL)
	{
	  to_file = from->to_file;
	  to_line = SOURCE_LINE (from, from[1].start_location);
	  sysp = from->sysp;
	}
    }

  map->reason = reason;
  map->sysp = sysp;
  map->start_location = start_location;
  map->to_file = to_file;
  map->to_line = to_line;
  set->cache = set->used++;
  map->column_bits = 0;
  set->highest_location = start_location;
  set->highest_line = start_location;
  set->max_column_hint = 0;

  if (reason == LC_ENTER)
    {
      map->included_from = set->depth == 0 ? -1 : (int) (set->used - 2);
      set->depth++;
      if (set->trace_includes)
	trace_include (set, map);
    }
  else if (reason == LC_RENAME)
    map->included_from = map[-1].included_from;
  else if (reason == LC_LEAVE)
    {
      set->depth--;
      map->included_from = INCLUDED_FROM (set, map - 1)->included_from;
    }

  return map;
}

source_location
linemap_line_start (struct line_maps *set, linenum_type to_line,
		    unsigned int max_column_hint)
{
  struct line_map *map = &set->maps[set->used - 1];
  source_location highest = set->highest_location;
  source_location r;
  linenum_type last_line = SOURCE_LINE (map, set->highest_line);
  int line_delta = to_line - last_line;
  bool add_map = false;
  if (line_delta < 0
      || (line_delta > 10 && line_delta * map->column_bits > 1000)
      || (max_column_hint >= (1U << map->column_bits))
      || (max_column_hint <= 80 && map->column_bits >= 10))
    {
      add_map = true;
    }
  else
    max_column_hint = set->max_column_hint;
  if (add_map)
    {
      int column_bits;
      if (max_column_hint > 100000 || highest > 0xC0000000)
	{
	  /* If the column number is ridiculous or we've allocated a huge
	     number of source_locations, give up on column numbers. */
	  max_column_hint = 0;
	  if (highest >0xF0000000)
	    return 0;
	  column_bits = 0;
	}
      else
	{
	  column_bits = 7;
	  while (max_column_hint >= (1U << column_bits))
	    column_bits++;
	  max_column_hint = 1U << column_bits;
	}
      /* Allocate the new line_map.  However, if the current map only has a
	 single line we can sometimes just increase its column_bits instead. */
      if (line_delta < 0
	  || last_line != map->to_line
	  || SOURCE_COLUMN (map, highest) >= (1U << column_bits))
	map = (struct line_map *) linemap_add (set, LC_RENAME, map->sysp,
					       map->to_file, to_line);
      map->column_bits = column_bits;
      r = map->start_location + ((to_line - map->to_line) << column_bits);
    }
  else
    r = highest - SOURCE_COLUMN (map, highest)
      + (line_delta << map->column_bits);
  set->highest_line = r;
  if (r > set->highest_location)
    set->highest_location = r;
  set->max_column_hint = max_column_hint;
  return r;
}

source_location
linemap_position_for_column (struct line_maps *set, unsigned int to_column)
{
  source_location r = set->highest_line;
  if (to_column >= set->max_column_hint)
    {
      if (r >= 0xC000000 || to_column > 100000)
	{
	  /* Running low on source_locations - disable column numbers.  */
	  return r;
	}
      else
	{
	  struct line_map *map = &set->maps[set->used - 1];
	  r = linemap_line_start (set, SOURCE_LINE (map, r), to_column + 50);
	}
    }
  r = r + to_column;
  if (r >= set->highest_location)
    set->highest_location = r;
  return r;
}

/* Given a logical line, returns the map from which the corresponding
   (source file, line) pair can be deduced.  Since the set is built
   chronologically, the logical lines are monotonic increasing, and so
   the list is sorted and we can use a binary search.  */

const struct line_map *
linemap_lookup (struct line_maps *set, source_location line)
{
  unsigned int md, mn, mx;
  const struct line_map *cached;

  mn = set->cache;
  mx = set->used;
  
  cached = &set->maps[mn];
  /* We should get a segfault if no line_maps have been added yet.  */
  if (line >= cached->start_location)
    {
      if (mn + 1 == mx || line < cached[1].start_location)
	return cached;
    }
  else
    {
      mx = mn;
      mn = 0;
    }

  while (mx - mn > 1)
    {
      md = (mn + mx) / 2;
      if (set->maps[md].start_location > line)
	mx = md;
      else
	mn = md;
    }

  set->cache = mn;
  return &set->maps[mn];
}

/* Print an include trace, for e.g. the -H option of the preprocessor.  */

static void
trace_include (const struct line_maps *set, const struct line_map *map)
{
  unsigned int i = set->depth;

  while (--i)
    putc ('.', stderr);
  fprintf (stderr, " %s\n", map->to_file);
}
