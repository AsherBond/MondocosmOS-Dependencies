/* Handling of compile-time options that influence the library.
   Copyright (C) 2005, 2007, 2009, 2010 Free Software Foundation, Inc.

This file is part of the GNU Fortran runtime library (libgfortran).

Libgfortran is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Libgfortran is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "libgfortran.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif


/* Useful compile-time options will be stored in here.  */
compile_options_t compile_options;


volatile sig_atomic_t fatal_error_in_progress = 0;

/* A signal handler to allow us to output a backtrace.  */
void
backtrace_handler (int signum)
{
  /* Since this handler is established for more than one kind of signal, 
     it might still get invoked recursively by delivery of some other kind
     of signal.  Use a static variable to keep track of that. */
  if (fatal_error_in_progress)
    raise (signum);
  fatal_error_in_progress = 1;

  show_backtrace();

  /* Now reraise the signal.  We reactivate the signal's
     default handling, which is to terminate the process.
     We could just call exit or abort,
     but reraising the signal sets the return status
     from the process correctly. */
  signal (signum, SIG_DFL);
  raise (signum);
}


/* Helper function for set_options because we need to access the
   global variable options which is not seen in set_options.  */
static void
maybe_find_addr2line (void)
{
  if (options.backtrace == -1)
    find_addr2line ();
}

/* Set the usual compile-time options.  */
extern void set_options (int , int []);
export_proto(set_options);

void
set_options (int num, int options[])
{
  if (num >= 1)
    compile_options.warn_std = options[0];
  if (num >= 2)
    compile_options.allow_std = options[1];
  if (num >= 3)
    compile_options.pedantic = options[2];
  /* options[3] is the removed -fdump-core option. It's place in the
     options array is retained due to ABI compatibility. Remove when
     bumping the library ABI.  */
  if (num >= 5)
    compile_options.backtrace = options[4];
  if (num >= 6)
    compile_options.sign_zero = options[5];
  if (num >= 7)
    compile_options.bounds_check = options[6];
  if (num >= 8)
    compile_options.range_check = options[7];

  /* If backtrace is required, we set signal handlers on the POSIX
     2001 signals with core action.  */
#if defined(HAVE_SIGNAL) && (defined(SIGQUIT) || defined(SIGILL) \
			     || defined(SIGABRT) || defined(SIGFPE) \
			     || defined(SIGSEGV) || defined(SIGBUS) \
			     || defined(SIGSYS) || defined(SIGTRAP) \
			     || defined(SIGXCPU) || defined(SIGXFSZ))
  if (compile_options.backtrace)
    {
#if defined(SIGQUIT)
      signal (SIGQUIT, backtrace_handler);
#endif

#if defined(SIGILL)
      signal (SIGILL, backtrace_handler);
#endif

#if defined(SIGABRT)
      signal (SIGABRT, backtrace_handler);
#endif

#if defined(SIGFPE)
      signal (SIGFPE, backtrace_handler);
#endif

#if defined(SIGSEGV)
      signal (SIGSEGV, backtrace_handler);
#endif

#if defined(SIGBUS)
      signal (SIGBUS, backtrace_handler);
#endif

#if defined(SIGSYS)
      signal (SIGSYS, backtrace_handler);
#endif

#if defined(SIGTRAP)
      signal (SIGTRAP, backtrace_handler);
#endif

#if defined(SIGXCPU)
      signal (SIGXCPU, backtrace_handler);
#endif

#if defined(SIGXFSZ)
      signal (SIGXFSZ, backtrace_handler);
#endif

      maybe_find_addr2line ();
    }
#endif

}


/* Default values for the compile-time options.  Keep in sync with
   gcc/fortran/options.c (gfc_init_options).  */
void
init_compile_options (void)
{
  compile_options.warn_std = GFC_STD_F95_DEL | GFC_STD_LEGACY;
  compile_options.allow_std = GFC_STD_F95_OBS | GFC_STD_F95_DEL
    | GFC_STD_F2003 | GFC_STD_F2008 | GFC_STD_F95 | GFC_STD_F77
    | GFC_STD_F2008_OBS | GFC_STD_GNU | GFC_STD_LEGACY;
  compile_options.pedantic = 0;
  compile_options.backtrace = 0;
  compile_options.sign_zero = 1;
  compile_options.range_check = 1;
}

/* Function called by the front-end to tell us the
   default for unformatted data conversion.  */

extern void set_convert (int);
export_proto (set_convert);

void
set_convert (int conv)
{
  compile_options.convert = conv;
}

extern void set_record_marker (int);
export_proto (set_record_marker);


void
set_record_marker (int val)
{

  switch(val)
    {
    case 4:
      compile_options.record_marker = sizeof (GFC_INTEGER_4);
      break;

    case 8:
      compile_options.record_marker = sizeof (GFC_INTEGER_8);
      break;

    default:
      runtime_error ("Invalid value for record marker");
      break;
    }
}

extern void set_max_subrecord_length (int);
export_proto (set_max_subrecord_length);

void set_max_subrecord_length(int val)
{
  if (val > GFC_MAX_SUBRECORD_LENGTH || val < 1)
    {
      runtime_error ("Invalid value for maximum subrecord length");
      return;
    }

  compile_options.max_subrecord_length = val;
}
