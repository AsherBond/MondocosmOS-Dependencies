/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                S Y S D E P                               *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *         Copyright (C) 1992-2011, Free Software Foundation, Inc.          *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 3,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                          *
 * As a special exception under Section 7 of GPL version 3, you are granted *
 * additional permissions described in the GCC Runtime Library Exception,   *
 * version 3.1, as published by the Free Software Foundation.               *
 *                                                                          *
 * You should have received a copy of the GNU General Public License and    *
 * a copy of the GCC Runtime Library Exception along with this program;     *
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    *
 * <http://www.gnu.org/licenses/>.                                          *
 *                                                                          *
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/* This file contains system dependent symbols that are referenced in the
   GNAT Run Time Library */

#ifdef __vxworks
#include "ioLib.h"
#if ! defined (VTHREADS)
#include "dosFsLib.h"
#endif
#if ! defined (__RTP__) && (! defined (VTHREADS) || defined (__VXWORKSMILS__))
# include "nfsLib.h"
#endif
#include "selectLib.h"
#include "vxWorks.h"
#endif

#ifdef IN_RTS
#define POSIX
#include "tconfig.h"
#include "tsystem.h"
#include <fcntl.h>
#include <sys/stat.h>
#ifdef VMS
#include <unixio.h>
#endif
#else
#include "config.h"
#include "system.h"
#endif

#include <time.h>
#include <errno.h>

#if defined (sun) && defined (__SVR4) && !defined (__vxworks)
/* The declaration is present in <time.h> but conditionalized
   on a couple of macros we don't define.  */
extern struct tm *localtime_r(const time_t *, struct tm *);
#endif

#include "adaint.h"

/* Don't use macros versions of this functions on VxWorks since they cause
   imcompatible changes in some VxWorks versions */
#ifdef __vxworks
#undef getchar
#undef putchar
#undef feof
#undef ferror
#undef fileno
#endif

/*
   mode_read_text
   open text file for reading
   rt for DOS and Windows NT, r for Unix

   mode_write_text
   truncate to zero length or create text file for writing
   wt for DOS and Windows NT, w for Unix

   mode_append_text
   append; open or create text file for writing at end-of-file
   at for DOS and Windows NT, a for Unix

   mode_read_binary
   open binary file for reading
   rb for DOS and Windows NT, r for Unix

   mode_write_binary
   truncate to zero length or create binary file for writing
   wb for DOS and Windows NT, w for Unix

   mode_append_binary
   append; open or create binary file for writing at end-of-file
   ab for DOS and Windows NT, a for Unix

   mode_read_text_plus
   open text file for update (reading and writing)
   r+t for DOS and Windows NT, r+ for Unix

   mode_write_text_plus
   truncate to zero length or create text file for update
   w+t for DOS and Windows NT, w+ for Unix

   mode_append_text_plus
   append; open or create text file for update, writing at end-of-file
   a+t for DOS and Windows NT, a+ for Unix

   mode_read_binary_plus
   open binary file for update (reading and writing)
   r+b for DOS and Windows NT, r+ for Unix

   mode_write_binary_plus
   truncate to zero length or create binary file for update
   w+b for DOS and Windows NT, w+ for Unix

   mode_append_binary_plus
   append; open or create binary file for update, writing at end-of-file
   a+b for DOS and Windows NT, a+ for Unix

   Notes:

   (1) Opening a file with read mode fails if the file does not exist or
   cannot be read.

   (2) Opening a file with append mode causes all subsequent writes to the
   file to be forced to the then current end-of-file, regardless of
   intervening calls to the fseek function.

   (3) When a file is opened with update mode, both input and output may be
   performed on the associated stream.  However, output may not be directly
   followed by input without an intervening call to the fflush function or
   to a file positioning function (fseek, fsetpos, or rewind), and input
   may not be directly followed by output without an intervening call to a
   file positioning function, unless the input operation encounters
   end-of-file.

   The other target dependent declarations here are for the two functions
   __gnat_set_binary_mode and __gnat_set_text_mode:

      void __gnat_set_binary_mode (int handle);
      void __gnat_set_text_mode   (int handle);

   These functions have no effect in Unix (or similar systems where there is
   no distinction between binary and text files), but in DOS (and similar
   systems where text mode does CR/LF translation), these functions allow
   the mode of the stream with the given handle (fileno can be used to get
   the handle of a stream) to be changed dynamically. The returned result
   is 0 if no error occurs and -1 if an error occurs.

   Finally there is a boolean (character) variable

      char __gnat_text_translation_required;

   which is zero (false) in Unix mode, and one (true) in DOS mode, with a
   true value indicating that text translation is required on text files
   and that fopen supports the trailing t and b modifiers.

*/

#if defined(WINNT)
static const char *mode_read_text = "rt";
static const char *mode_write_text = "wt";
static const char *mode_append_text = "at";
static const char *mode_read_binary = "rb";
static const char *mode_write_binary = "wb";
static const char *mode_append_binary = "ab";
static const char *mode_read_text_plus = "r+t";
static const char *mode_write_text_plus = "w+t";
static const char *mode_append_text_plus = "a+t";
static const char *mode_read_binary_plus = "r+b";
static const char *mode_write_binary_plus = "w+b";
static const char *mode_append_binary_plus = "a+b";
const char __gnat_text_translation_required = 1;

void
__gnat_set_binary_mode (int handle)
{
  _setmode (handle, O_BINARY);
}

void
__gnat_set_text_mode (int handle)
{
  _setmode (handle, O_TEXT);
}

#ifdef __MINGW32__
#include <windows.h>

/* Return the name of the tty.   Under windows there is no name for
   the tty, so this function, if connected to a tty, returns the generic name
   "console".  */

char *
__gnat_ttyname (int filedes)
{
  if (isatty (filedes))
    return "console";
  else
    return NULL;
}

#ifndef RTX

int __gnat_is_windows_xp (void);

int
__gnat_is_windows_xp (void)
{
  static int is_win_xp=0, is_win_xp_checked=0;

  if (!is_win_xp_checked)
    {
      OSVERSIONINFO version;

      is_win_xp_checked = 1;

      memset (&version, 0, sizeof (version));
      version.dwOSVersionInfoSize = sizeof (version);

      is_win_xp = GetVersionEx (&version)
        && version.dwPlatformId == VER_PLATFORM_WIN32_NT
        && (version.dwMajorVersion > 5
            || (version.dwMajorVersion == 5 && version.dwMinorVersion >= 1));
    }
  return is_win_xp;
}

#endif

/* Get the bounds of the stack.  The stack pointer is supposed to be
   initialized to BASE when a thread is created and the stack can be extended
   to LIMIT before reaching a guard page.
   Note: for the main thread, the system automatically extend the stack, so
   LIMIT is only the current limit.  */

void
__gnat_get_stack_bounds (void **base, void **limit)
{
  NT_TIB *tib;

  /* We know that the first field of the TEB is the TIB.  */
  tib = (NT_TIB *)NtCurrentTeb ();

  *base = tib->StackBase;
  *limit = tib->StackLimit;
}

#endif /* !__MINGW32__ */

#else

static const char *mode_read_text = "r";
static const char *mode_write_text = "w";
static const char *mode_append_text = "a";
static const char *mode_read_binary = "r";
static const char *mode_write_binary = "w";
static const char *mode_append_binary = "a";
static const char *mode_read_text_plus = "r+";
static const char *mode_write_text_plus = "w+";
static const char *mode_append_text_plus = "a+";
static const char *mode_read_binary_plus = "r+";
static const char *mode_write_binary_plus = "w+";
static const char *mode_append_binary_plus = "a+";
const char __gnat_text_translation_required = 0;

/* These functions do nothing in non-DOS systems. */

void
__gnat_set_binary_mode (int handle ATTRIBUTE_UNUSED)
{
}

void
__gnat_set_text_mode (int handle ATTRIBUTE_UNUSED)
{
}
char *
__gnat_ttyname (int filedes)
{
#if defined (__vxworks) || defined (__nucleus)
  return "";
#else
  extern char *ttyname (int);

  return ttyname (filedes);
#endif /* defined (__vxworks) || defined (__nucleus) */
}
#endif

#if defined (linux) || defined (sun) || defined (sgi) \
  || (defined (__osf__) && ! defined (__alpha_vxworks)) || defined (WINNT) \
  || defined (__MACHTEN__) || defined (__hpux__) || defined (_AIX) \
  || (defined (__svr4__) && defined (i386)) || defined (__Lynx__) \
  || defined (__CYGWIN__) || defined (__FreeBSD__) || defined (__OpenBSD__) \
  || defined (__GLIBC__) || defined (__APPLE__)

#ifdef __MINGW32__
#if OLD_MINGW
#include <termios.h>
#else
#include <conio.h>  /* for getch(), kbhit() */
#endif
#else
#include <termios.h>
#endif

#else
#if defined (VMS)
extern char *decc$ga_stdscr;
static int initted = 0;
#endif
#endif

/* Implements the common processing for getc_immediate and
   getc_immediate_nowait. */

extern void getc_immediate (FILE *, int *, int *);
extern void getc_immediate_nowait (FILE *, int *, int *, int *);
extern void getc_immediate_common (FILE *, int *, int *, int *, int);

/* Called by Get_Immediate (Foo); */

void
getc_immediate (FILE *stream, int *ch, int *end_of_file)
{
  int avail;

  getc_immediate_common (stream, ch, end_of_file, &avail, 1);
}

/* Called by Get_Immediate (Foo, Available); */

void
getc_immediate_nowait (FILE *stream, int *ch, int *end_of_file, int *avail)
{
  getc_immediate_common (stream, ch, end_of_file, avail, 0);
}

/* Called by getc_immediate () and getc_immediate_nowait () */

void
getc_immediate_common (FILE *stream,
                       int *ch,
                       int *end_of_file,
                       int *avail,
                       int waiting)
{
#if defined (linux) || defined (sun) || defined (sgi) \
    || (defined (__osf__) && ! defined (__alpha_vxworks)) \
    || defined (__CYGWIN32__) || defined (__MACHTEN__) || defined (__hpux__) \
    || defined (_AIX) || (defined (__svr4__) && defined (i386)) \
    || defined (__Lynx__) || defined (__FreeBSD__) || defined (__OpenBSD__) \
    || defined (__GLIBC__) || defined (__APPLE__)
  char c;
  int nread;
  int good_one = 0;
  int eof_ch = 4; /* Ctrl-D */
  int fd = fileno (stream);
  struct termios otermios_rec, termios_rec;

  if (isatty (fd))
    {
      tcgetattr (fd, &termios_rec);
      memcpy (&otermios_rec, &termios_rec, sizeof (struct termios));

      /* Set RAW mode, with no echo */
      termios_rec.c_lflag = termios_rec.c_lflag & ~ICANON & ~ECHO;

#if defined(linux) || defined (sun) || defined (sgi) \
    || defined (__osf__) || defined (__MACHTEN__) || defined (__hpux__) \
    || defined (_AIX) || (defined (__svr4__) && defined (i386)) \
    || defined (__Lynx__) || defined (__FreeBSD__) || defined (__OpenBSD__) \
    || defined (__GLIBC__) || defined (__APPLE__)
      eof_ch = termios_rec.c_cc[VEOF];

      /* If waiting (i.e. Get_Immediate (Char)), set MIN = 1 and wait for
         a character forever. This doesn't seem to effect Ctrl-Z or
         Ctrl-C processing.
         If not waiting (i.e. Get_Immediate (Char, Available)),
         don't wait for anything but timeout immediately. */
      termios_rec.c_cc[VMIN] = waiting;
      termios_rec.c_cc[VTIME] = 0;
#endif
      tcsetattr (fd, TCSANOW, &termios_rec);

      while (! good_one)
        {
          /* Read is used here instead of fread, because fread doesn't
             work on Solaris5 and Sunos4 in this situation.  Maybe because we
             are mixing calls that use file descriptors and streams. */
          nread = read (fd, &c, 1);
          if (nread > 0)
            {
              /* On Unix terminals, Ctrl-D (EOT) is an End of File. */
              if (c == eof_ch)
                {
                  *avail = 0;
                  *end_of_file = 1;
                  good_one = 1;
                }

              /* Everything else is ok */
              else if (c != eof_ch)
                {
                  *avail = 1;
                  *end_of_file = 0;
                  good_one = 1;
                }
            }

          else if (! waiting)
            {
              *avail = 0;
              *end_of_file = 0;
              good_one = 1;
            }
          else
	    good_one = 0;
        }

      tcsetattr (fd, TCSANOW, &otermios_rec);
      *ch = c;
    }

  else
#elif defined (VMS)
  int fd = fileno (stream);

  if (isatty (fd))
    {
      if (initted == 0)
	{
	  decc$bsd_initscr ();
	  initted = 1;
	}

      decc$bsd_cbreak ();
      *ch = decc$bsd_wgetch (decc$ga_stdscr);

      if (*ch == 4)
	*end_of_file = 1;
      else
	*end_of_file = 0;

      *avail = 1;
      decc$bsd_nocbreak ();
    }
  else
#elif defined (__MINGW32__)
  int fd = fileno (stream);
  int char_waiting;
  int eot_ch = 4; /* Ctrl-D */

  if (isatty (fd))
    {
      if (waiting)
	{
	  *ch = getch ();

	  if (*ch == eot_ch)
	    *end_of_file = 1;
	  else
	    *end_of_file = 0;

	  *avail = 1;
	}
      else /* ! waiting */
	{
	  char_waiting = kbhit();

	  if (char_waiting == 1)
	    {
	      *avail = 1;
	      *ch = getch ();

	      if (*ch == eot_ch)
		*end_of_file = 1;
	      else
		*end_of_file = 0;
	    }
	  else
	    {
	      *avail = 0;
	      *end_of_file = 0;
	    }
	}
    }
  else
#elif defined (__vxworks)
  /* Bit masks of file descriptors to read from.  */
  struct fd_set readFds;
  /* Timeout before select returns if nothing can be read.  */
  struct timeval timeOut;
  char c;
  int fd = fileno (stream);
  int nread;
  int option;
  int readable;
  int status;
  int width;

  if (isatty (fd))
    {
      /* If we do not want to wait, we have to set up fd in RAW mode. This
	 should be done outside this function as setting fd in RAW mode under
	 vxWorks flushes the buffer of fd. If the RAW mode was set here, the
	 buffer would be empty and we would always return that no character
	 is available */
      if (! waiting)
	{
	  /* Initialization of timeOut for its use with select.  */
	  timeOut.tv_sec  = 0;
	  timeOut.tv_usec = 0;

	  /* Initialization of readFds for its use with select;
	     FD is the only file descriptor to be monitored */
	  FD_ZERO (&readFds);
	  FD_SET (fd, &readFds);
	  width = 2;

	  /* We do all this processing to emulate a non blocking read.  */
	  readable = select (width, &readFds, NULL, NULL, &timeOut);
	  if (readable == ERROR)
	    *avail = -1, *end_of_file = -1;
	  /* No character available in input.  */
	  else if (readable == 0)
	    *avail = 0, *end_of_file = 0;
	  else
	    {
	      nread = read (fd, &c, 1);
	      if (nread > 0)
		*avail = 1, *end_of_file = 0;
	      /* End Of File. */
	      else if (nread == 0)
		*avail = 0, *end_of_file = 1;
	      /* Error.  */
	      else
		*avail = -1, *end_of_file = -1;
	    }
	}

      /* We have to wait until we get a character */
      else
	{
	  *avail = -1;
	  *end_of_file = -1;

	  /* Save the current mode of FD.  */
	  option = ioctl (fd, FIOGETOPTIONS, 0);

	  /* Set FD in RAW mode.  */
	  status = ioctl (fd, FIOSETOPTIONS, OPT_RAW);
	  if (status != -1)
	    {
	      nread = read (fd, &c, 1);
	      if (nread > 0)
		*avail = 1, *end_of_file = 0;
	      /* End of file.  */
	      else if (nread == 0)
		*avail = 0, *end_of_file = 1;
	      /* Else there is an ERROR.  */
	    }

	  /* Revert FD to its previous mode. */
	  status = ioctl (fd, FIOSETOPTIONS, option);
	}

      *ch = c;
    }
  else
#endif
    {
      /* If we're not on a terminal, then we don't need any fancy processing.
	 Also this is the only thing that's left if we're not on one of the
	 supported systems; which means that for non supported systems,
         get_immediate may wait for a carriage return on terminals. */
      *ch = fgetc (stream);
      if (feof (stream))
        {
          *end_of_file = 1;
          *avail = 0;
        }
      else
        {
          *end_of_file = 0;
          *avail = 1;
        }
    }
}

/* The following definitions are provided in NT to support Windows based
   Ada programs.  */

#ifdef WINNT
#include <windows.h>

/* Provide functions to echo the values passed to WinMain (windows bindings
   will want to import these).  We use the same names as the routines used
   by AdaMagic for compatibility.  */

char *rts_get_hInstance (void);
char *rts_get_hPrevInstance (void);
char *rts_get_lpCommandLine (void);
int   rts_get_nShowCmd (void);

char *
rts_get_hInstance (void)
{
  return (char *)GetModuleHandleA (0);
}

char *
rts_get_hPrevInstance (void)
{
  return 0;
}

char *
rts_get_lpCommandLine (void)
{
  return GetCommandLineA ();
}

int
rts_get_nShowCmd (void)
{
  return 1;
}

#endif /* WINNT */
#ifdef VMS

/* This gets around a problem with using the old threads library on VMS 7.0. */

extern long get_gmtoff (void);

long
get_gmtoff (void)
{
  time_t t;
  struct tm *ts;

  t = time ((time_t) 0);
  ts = localtime (&t);
  return ts->tm_gmtoff;
}
#endif

/* This value is returned as the time zone offset when a valid value
   cannot be determined. It is simply a bizarre value that will never
   occur. It is 3 days plus 73 seconds (offset is in seconds). */

long __gnat_invalid_tzoff = 259273;

/* Definition of __gnat_localtime_r used by a-calend.adb */

#if defined (__MINGW32__)

#ifdef CERT

/* For the Cert run times on native Windows we use dummy functions
   for locking and unlocking tasks since we do not support multiple
   threads on this configuration (Cert run time on native Windows). */

void dummy (void) {}

void (*Lock_Task) ()   = &dummy;
void (*Unlock_Task) () = &dummy;

#else

#define Lock_Task system__soft_links__lock_task
extern void (*Lock_Task) (void);

#define Unlock_Task system__soft_links__unlock_task
extern void (*Unlock_Task) (void);

#endif

/* Reentrant localtime for Windows. */

extern void
__gnat_localtime_tzoff (const time_t *, long *);

static const unsigned long long w32_epoch_offset = 11644473600ULL;
void
__gnat_localtime_tzoff (const time_t *timer, long *off)
{
  union
  {
    FILETIME ft_time;
    unsigned long long ull_time;
  } utc_time, local_time;

  SYSTEMTIME utc_sys_time, local_sys_time;
  TIME_ZONE_INFORMATION tzi;

  BOOL  status = 1;
  DWORD tzi_status;

  (*Lock_Task) ();

#ifdef RTX

  tzi_status = GetTimeZoneInformation (&tzi);
  *off = tzi.Bias;
  if (tzi_status == TIME_ZONE_ID_STANDARD)
     /* The system is operating in the range covered by the StandardDate
        member. */
     *off = *off + tzi.StandardBias;
  else if (tzi_status == TIME_ZONE_ID_DAYLIGHT)
     /* The system is operating in the range covered by the DaylightDate
        member. */
     *off = *off + tzi.DaylightBias;
  *off = *off * -60;

#else

  /* First convert unix time_t structure to windows FILETIME format.  */
  utc_time.ull_time = ((unsigned long long) *timer + w32_epoch_offset)
                      * 10000000ULL;

  tzi_status = GetTimeZoneInformation (&tzi);

  /* If GetTimeZoneInformation does not return a value between 0 and 2 then
     it means that we were not able to retrieve timezone informations.
     Note that we cannot use here FileTimeToLocalFileTime as Windows will use
     in always in this case the current timezone setting. As suggested on
     MSDN we use the following three system calls to get the right information.
     Note also that starting with Windows Vista new functions are provided to
     get timezone settings that depend on the year. We cannot use them as we
     still support Windows XP and Windows 2003.  */
  status = (tzi_status >= 0 && tzi_status <= 2)
     && FileTimeToSystemTime (&utc_time.ft_time, &utc_sys_time)
     && SystemTimeToTzSpecificLocalTime (&tzi, &utc_sys_time, &local_sys_time)
     && SystemTimeToFileTime (&local_sys_time, &local_time.ft_time);

  if (!status)
     /* An error occurs so return invalid_tzoff.  */
     *off = __gnat_invalid_tzoff;
  else
     if (local_time.ull_time > utc_time.ull_time)
        *off = (long) ((local_time.ull_time - utc_time.ull_time) / 10000000ULL);
     else
        *off = - (long) ((utc_time.ull_time - local_time.ull_time) / 10000000ULL);

#endif

  (*Unlock_Task) ();
}

#else

/* On Lynx, all time values are treated in GMT */

#if defined (__Lynx__)

/* As of LynxOS 3.1.0a patch level 040, LynuxWorks changes the
   prototype to the C library function localtime_r from the POSIX.4
   Draft 9 to the POSIX 1.c version. Before this change the following
   spec is required. Only use when ___THREADS_POSIX4ad4__ is defined,
   the Lynx convention when building against the legacy API. */

extern void
__gnat_localtime_tzoff (const time_t *, long *);

void
__gnat_localtime_tzoff (const time_t *timer, long *off)
{
  *off = 0;
}

#else

/* VMS does not need __gnat_localtime_tzoff */

#if defined (VMS)

/* Other targets except Lynx, VMS and Windows provide a standard localtime_r */

#else

#define Lock_Task system__soft_links__lock_task
extern void (*Lock_Task) (void);

#define Unlock_Task system__soft_links__unlock_task
extern void (*Unlock_Task) (void);

extern void
__gnat_localtime_tzoff (const time_t *, long *);

void
__gnat_localtime_tzoff (const time_t *timer, long *off)
{
  struct tm tp;

/* AIX, HPUX, SGI Irix, Sun Solaris */
#if defined (_AIX) || defined (__hpux__) || defined (sgi) || defined (sun)
{
  (*Lock_Task) ();

  localtime_r (timer, &tp);
  *off = (long) -timezone;

  (*Unlock_Task) ();

  /* Correct the offset if Daylight Saving Time is in effect */

  if (tp.tm_isdst > 0)
    *off = *off + 3600;
}

/* VxWorks */
#elif defined (__vxworks)
#include <stdlib.h>
{
  (*Lock_Task) ();

  localtime_r (timer, &tp);

  /* Try to read the environment variable TIMEZONE. The variable may not have
     been initialize, in that case return an offset of zero (0) for UTC. */

  char *tz_str = getenv ("TIMEZONE");

  if ((tz_str == NULL) || (*tz_str == '\0'))
    *off = 0;
  else
  {
    char *tz_start, *tz_end;

    /* The format of the data contained in TIMEZONE is N::U:S:E where N is the
       name of the time zone, U are the minutes difference from UTC, S is the
       start of DST in mmddhh and E is the end of DST in mmddhh. Extracting
       the value of U involves setting two pointers, one at the beginning and
       one at the end of the value. The end pointer is then set to null in
       order to delimit a string slice for atol to process. */

    tz_start = index (tz_str, ':') + 2;
    tz_end = index (tz_start, ':');
    tz_end = '\0';

    /* The Ada layer expects an offset in seconds. Note that we must reverse
       the sign of the result since west is positive and east is negative on
       VxWorks targets. */

    *off = -atol (tz_start) * 60;

    /* Correct the offset if Daylight Saving Time is in effect */

    if (tp.tm_isdst > 0)
      *off = *off + 3600;
  }

  (*Unlock_Task) ();
}

/* Darwin, Free BSD, Linux, Tru64, where component tm_gmtoff is present in
   struct tm */

#elif defined (__APPLE__) || defined (__FreeBSD__) || defined (linux) ||\
     (defined (__alpha__) && defined (__osf__)) || defined (__GLIBC__)
{
  localtime_r (timer, &tp);
  *off = tp.tm_gmtoff;
}

/* Default: treat all time values in GMT */

#else
  *off = 0;

#endif
}

#endif
#endif
#endif

#ifdef __vxworks

#include <taskLib.h>

/* __gnat_get_task_options is used by s-taprop.adb only for VxWorks. This
   function returns the options to be set when creating a new task. It fetches
   the options assigned to the current task (parent), so offering some user
   level control over the options for a task hierarchy. It forces VX_FP_TASK
   because it is almost always required. On processors with the SPE
   category, VX_SPE_TASK is needed to enable the SPE. */
extern int __gnat_get_task_options (void);

int
__gnat_get_task_options (void)
{
  int options;

  /* Get the options for the task creator */
  taskOptionsGet (taskIdSelf (), &options);

  /* Force VX_FP_TASK because it is almost always required */
  options |= VX_FP_TASK;
#if defined (__SPE__) && (! defined (__VXWORKSMILS__)) \
    && (! defined (VTHREADS))
  options |= VX_SPE_TASK;
#endif

  /* Mask those bits that are not under user control */
#ifdef VX_USR_TASK_OPTIONS
  return options & VX_USR_TASK_OPTIONS;
#else
  return options;
#endif
}

#endif

int
__gnat_is_file_not_found_error (int errno_val) {
   switch (errno_val) {
      case ENOENT:
#ifdef __vxworks
      /* In the case of VxWorks, we also have to take into account various
       * filesystem-specific variants of this error.
       */
#if ! defined (VTHREADS)
      case S_dosFsLib_FILE_NOT_FOUND:
#endif
#if ! defined (__RTP__) && (! defined (VTHREADS) || defined (__VXWORKSMILS__))
      case S_nfsLib_NFSERR_NOENT:
#endif
#endif
         return 1;

      default:
         return 0;
   }
}
