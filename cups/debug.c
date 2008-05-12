/*
 * "$Id$"
 *
 *   Debugging functions for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 2008 by Apple Inc.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   which should have been included with this file.  If this file is
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 *
 * Contents:
 *
 *   _cups_debug_printf() - Write a formatted line to the log.
 *   _cups_debug_puts()   - Write a single line to the log.
 */

/*
 * Include necessary headers...
 */

#include "globals.h"
#include "debug.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef DEBUG
/*
 * 'debug_vsnprintf()' - Format a string into a fixed size buffer.
 */

int					/* O - Number of bytes formatted */
debug_vsnprintf(char       *buffer,	/* O - Output buffer */
                size_t     bufsize,	/* O - Size of output buffer */
	        const char *format,	/* I - printf-style format string */
	        va_list    ap)		/* I - Pointer to additional arguments */
{
  char		*bufptr,		/* Pointer to position in buffer */
		*bufend,		/* Pointer to end of buffer */
		sign,			/* Sign of format width */
		size,			/* Size character (h, l, L) */
		type;			/* Format type character */
  int		width,			/* Width of field */
		prec;			/* Number of characters of precision */
  char		tformat[100],		/* Temporary format string for sprintf() */
		*tptr,			/* Pointer into temporary format */
		temp[1024];		/* Buffer for formatted numbers */
  char		*s;			/* Pointer to string */
  int		bytes;			/* Total number of bytes needed */


 /*
  * Loop through the format string, formatting as needed...
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;
  bytes  = 0;

  while (*format)
  {
    if (*format == '%')
    {
      tptr = tformat;
      *tptr++ = *format++;

      if (*format == '%')
      {
        if (bufptr && bufptr < bufend) *bufptr++ = *format;
        bytes ++;
        format ++;
	continue;
      }
      else if (strchr(" -+#\'", *format))
      {
        *tptr++ = *format;
        sign = *format++;
      }
      else
        sign = 0;

      if (*format == '*')
      {
       /*
        * Get width from argument...
	*/

	format ++;
	width = va_arg(ap, int);

	snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", width);
	tptr += strlen(tptr);
      }
      else
      {
	width = 0;

	while (isdigit(*format & 255))
	{
	  if (tptr < (tformat + sizeof(tformat) - 1))
	    *tptr++ = *format;

	  width = width * 10 + *format++ - '0';
	}
      }

      if (*format == '.')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        format ++;

        if (*format == '*')
	{
         /*
	  * Get precision from argument...
	  */

	  format ++;
	  prec = va_arg(ap, int);

	  snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", prec);
	  tptr += strlen(tptr);
	}
	else
	{
	  prec = 0;

	  while (isdigit(*format & 255))
	  {
	    if (tptr < (tformat + sizeof(tformat) - 1))
	      *tptr++ = *format;

	    prec = prec * 10 + *format++ - '0';
	  }
	}
      }
      else
        prec = -1;

      if (*format == 'l' && format[1] == 'l')
      {
        size = 'L';

	if (tptr < (tformat + sizeof(tformat) - 2))
	{
	  *tptr++ = 'l';
	  *tptr++ = 'l';
	}

	format += 2;
      }
      else if (*format == 'h' || *format == 'l' || *format == 'L')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        size = *format++;
      }

      if (!*format)
        break;

      if (tptr < (tformat + sizeof(tformat) - 1))
        *tptr++ = *format;

      type  = *format++;
      *tptr = '\0';

      switch (type)
      {
	case 'E' : /* Floating point formats */
	case 'G' :
	case 'e' :
	case 'f' :
	case 'g' :
	    if ((width + 2) > sizeof(temp))
	      break;

	    sprintf(temp, tformat, va_arg(ap, double));

            bytes += (int)strlen(temp);

            if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

        case 'B' : /* Integer formats */
	case 'X' :
	case 'b' :
        case 'd' :
	case 'i' :
	case 'o' :
	case 'u' :
	case 'x' :
	    if ((width + 2) > sizeof(temp))
	      break;

#ifdef HAVE_LONG_LONG
            if (size == 'L')
	      sprintf(temp, tformat, va_arg(ap, long long));
	    else
#endif /* HAVE_LONG_LONG */
            if (size == 'l')
	      sprintf(temp, tformat, va_arg(ap, long));
	    else
	      sprintf(temp, tformat, va_arg(ap, int));

            bytes += (int)strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

	case 'p' : /* Pointer value */
	    if ((width + 2) > sizeof(temp))
	      break;

	    sprintf(temp, tformat, va_arg(ap, void *));

            bytes += (int)strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

        case 'c' : /* Character or character array */
	    bytes += width;

	    if (bufptr)
	    {
	      if (width <= 1)
	        *bufptr++ = va_arg(ap, int);
	      else
	      {
		if ((bufptr + width) > bufend)
		  width = (int)(bufend - bufptr);

		memcpy(bufptr, va_arg(ap, char *), (size_t)width);
		bufptr += width;
	      }
	    }
	    break;

	case 's' : /* String */
	    if ((s = va_arg(ap, char *)) == NULL)
	      s = "(null)";

           /*
	    * Copy the C string, replacing control chars and \ with
	    * C character escapes...
	    */

            for (bufend --; *s && bufptr < bufend; s ++)
	    {
	      if (*s == '\n')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = 'n';
	      }
	      else if (*s == '\r')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = 'r';
	      }
	      else if (*s == '\t')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = 't';
	      }
	      else if (*s == '\\')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = '\\';
	      }
	      else if (*s == '\'')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = '\'';
	      }
	      else if (*s == '\"')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = '\"';
	      }
	      else if ((*s & 255) < ' ')
	      {
	        *bufptr++ = '\\';
		*bufptr++ = '0';
		*bufptr++ = '0' + *s / 8;
		*bufptr++ = '0' + (*s & 7);
	      }
	      else
	        *bufptr++ = *s;
            }

            bufend ++;
	    break;

	case 'n' : /* Output number of chars so far */
	    *(va_arg(ap, int *)) = bytes;
	    break;
      }
    }
    else
    {
      bytes ++;

      if (bufptr && bufptr < bufend)
        *bufptr++ = *format;

      format ++;
    }
  }

 /*
  * Nul-terminate the string and return the number of characters needed.
  */

  *bufptr = '\0';

  return (bytes);
}


/*
 * '_cups_debug_printf()' - Write a formatted line to the log.
 */

void
_cups_debug_printf(const char *format,	/* I - Printf-style format string */
                   ...)			/* I - Additional arguments as needed */
{
  va_list		ap;		/* Pointer to arguments */
  struct timeval	curtime;	/* Current time */
  char			buffer[2048];	/* Output buffer */
  size_t		bytes;		/* Number of bytes in buffer */
  const char		*cups_debug_log;/* CUPS_DEBUG_LOG environment variable */
  _cups_globals_t	*cg = _cupsGlobals();
					/* Global data */


 /*
  * See if we need to do any logging...
  */

  if (!cg->debug_init)
  {
    cg->debug_init = 1;

    if ((cups_debug_log = getenv("CUPS_DEBUG_LOG")) == NULL)
      cg->debug_fd = -1;
    else if (!strcmp(cups_debug_log, "-"))
      cg->debug_fd = 2;
    else
      cg->debug_fd = open(cups_debug_log, O_WRONLY | O_APPEND | O_CREAT, 0644);
  }

  if (cg->debug_fd < 0)
    return;

 /*
  * Format the message...
  */

  gettimeofday(&curtime, NULL);
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d ",
	   (int)((curtime.tv_sec / 3600) % 24),
	   (int)((curtime.tv_sec / 60) % 60),
	   (int)(curtime.tv_sec % 60), (int)(curtime.tv_usec / 1000));

  va_start(ap, format);
  debug_vsnprintf(buffer + 13, sizeof(buffer) - 14, format, ap);
  va_end(ap);

  bytes = strlen(buffer);
  if (buffer[bytes - 1] != '\n')
  {
    buffer[bytes] = '\n';
    bytes ++;
    buffer[bytes] = '\0';
  }

 /*
  * Write it out...
  */

  write(cg->debug_fd, buffer, bytes);
}


/*
 * '_cups_debug_puts()' - Write a single line to the log.
 */

void
_cups_debug_puts(const char *s)		/* I - String to output */
{
  _cups_debug_printf("%s\n", s);
}


#elif defined(__APPLE__)
/* Mac OS X needs these stubbed since we reference them in the libcups.exp file */
void	_cups_debug_printf(const char *format, ...) {}
void	_cups_debug_puts(const char *s) {}
#endif /* DEBUG */


/*
 * End of "$Id$".
 */