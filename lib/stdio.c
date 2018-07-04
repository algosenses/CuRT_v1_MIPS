/*
 * stdio.c
 */

#include "lib/stdio.h"

extern void SerialOutputByte(const char);
extern int SerialIsReadyChar( void );
extern char SerialIsGetChar( void );

/* internal helper functions */
static void _PrintChar(char *fmt, char c);
static void _PrintDec(char *fmt, int value);
static void _PrintHex(char *fmt, int value);
static void _PrintString(char *fmt, char *cptr);

void *memcpy(void *dest, const void *src, int cnt)
{
	char *s1 = dest;
	const char *s2 = src;
	char *endptr = (char *) dest + cnt;

	while (s1 < endptr)
		*s1++ = *s2++;
	return dest;
}

void *memset(void *dest, int c, int len)
{
	char *s = dest;
	char *limit = (char *) dest + len;

	while (s < limit)
		*s++ = c;
	return dest;
}

int strcmp(char *s1, char *s2)
{
	while (*s1 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
	}
	return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

static bool DecToLong(char *s, long *retval)
{
	long remainder;
	if (!s || !s[0])
		return false;

	for (*retval = 0; *s; s++) {
		if (*s < '0' || *s > '9')
			return false;
		remainder = *s - '0';
		*retval = *retval * 10 + remainder;
	}

	return true;
}

/**
 * @brief printf() family produce output according to a format
 * The simple implementation in CuRT supports the following format:
 *   "%s", "%c", "%d", and "%x"
 * and the correspoding variants: "%08x", "%8x".
 */
#if 0
int printf(const char *fmt, ...)
{
	int i;
	va_list args;
	const char *s = fmt;
	char format[10];

	va_start(args, fmt);
	while (*s) {
		if (*s == '%') {
			s++;
			/* s in "% 08lx" format to get the record format. */
			format[0] = '%';
			for (i = 1; i < 10; ) {
				if (*s=='c' || *s=='d' || *s=='x' ||
				    *s=='s' || *s=='%') {
					format[i++] = *s;
					format[i] = '\0';
					break;
				}
				else {
					format[i++] = *s++;
				}
			}
			/* "% s", "% c", "% d", "% x" to find the print
			 * function calls */
			switch (*s++) {
				case 'c' :
					_PrintChar(format, va_arg(args, int));
					break;
				case 'd' :
					_PrintDec(format, va_arg(args, int));
					break;
				case 'x' :
					_PrintHex(format, va_arg(args, int));
					break;
				case 's' :
					_PrintString(format, va_arg(args, char *));
					break;
				case '%' :
					_PrintChar("%c", '%');
					break;
				default:
					/* it shall not happend. */
					break;
			}
		}
		else {
			_PrintChar("%c", *s);
			s++;
		}
	}
	va_end(args);

	/* FIXME: Upon successful return, printf function shall return the
	 * number of characters printed.
	 */
	return 1;
}
#else
typedef __signed char s8;
typedef unsigned char u8;

typedef __signed short s16;
typedef unsigned short u16;

typedef __signed int s32;
typedef unsigned int u32;

#define _UPC    0x1
#define _LWR    0x2
#define _DIG    0x4
#define _SPC    0x8
#define _PUN    0x10
#define _CTR    0x20
#define _HEX    0x40
#define _BLK    0x80


#define LF      0x0A
#define CR      0x0D


/* for ctype.h */
char _ctype[] = {
    /* 0 */ 0,
    /* 1 */ _CTR,
    /* 2 */ _CTR,
    /* 3 */ _CTR,
    /* 4 */ _CTR,
    /* 5 */ _CTR,
    /* 6 */ _CTR,
    /* 7 */ _CTR,
    /* 8 */ _CTR,
    /* 9 */ _SPC+_CTR,
    /* 10 */ _SPC+_CTR,
    /* 11 */ _SPC+_CTR,
    /* 12 */ _SPC+_CTR,
    /* 13 */ _SPC+_CTR,
    /* 14 */ _CTR,
    /* 15 */ _CTR,
    /* 16 */ _CTR,
    /* 17 */ _CTR,
    /* 18 */ _CTR,
    /* 19 */ _CTR,
    /* 20 */ _CTR,
    /* 21 */ _CTR,
    /* 22 */ _CTR,
    /* 23 */ _CTR,
    /* 24 */ _CTR,
    /* 25 */ _CTR,
    /* 26 */ _CTR,
    /* 27 */ _CTR,
    /* 28 */ _CTR,
    /* 29 */ _CTR,
    /* 30 */ _CTR,
    /* 31 */ _CTR,
    /* 32 */ _SPC+_BLK,
    /* 33 */ _PUN,
    /* 34 */ _PUN,
    /* 35 */ _PUN,
    /* 36 */ _PUN,
    /* 37 */ _PUN,
    /* 38 */ _PUN,
    /* 39 */ _PUN,
    /* 40 */ _PUN,
    /* 41 */ _PUN,
    /* 42 */ _PUN,
    /* 43 */ _PUN,
    /* 44 */ _PUN,
    /* 45 */ _PUN,
    /* 46 */ _PUN,
    /* 47 */ _PUN,
    /* 48 */ _DIG,
    /* 49 */ _DIG,
    /* 50 */ _DIG,
    /* 51 */ _DIG,
    /* 52 */ _DIG,
    /* 53 */ _DIG,
    /* 54 */ _DIG,
    /* 55 */ _DIG,
    /* 56 */ _DIG,
    /* 57 */ _DIG,
    /* 58 */ _PUN,
    /* 59 */ _PUN,
    /* 60 */ _PUN,
    /* 61 */ _PUN,
    /* 62 */ _PUN,
    /* 63 */ _PUN,
    /* 64 */ _PUN,
    /* 65 */ _UPC+_HEX,
    /* 66 */ _UPC+_HEX,
    /* 67 */ _UPC+_HEX,
    /* 68 */ _UPC+_HEX,
    /* 69 */ _UPC+_HEX,
    /* 70 */ _UPC+_HEX,
    /* 71 */ _UPC,
    /* 72 */ _UPC,
    /* 73 */ _UPC,
    /* 74 */ _UPC,
    /* 75 */ _UPC,
    /* 76 */ _UPC,
    /* 77 */ _UPC,
    /* 78 */ _UPC,
    /* 79 */ _UPC,
    /* 80 */ _UPC,
    /* 81 */ _UPC,
    /* 82 */ _UPC,
    /* 83 */ _UPC,
    /* 84 */ _UPC,
    /* 85 */ _UPC,
    /* 86 */ _UPC,
    /* 87 */ _UPC,
    /* 88 */ _UPC,
    /* 89 */ _UPC,
    /* 90 */ _UPC,
    /* 91 */ _PUN,
    /* 92 */ _PUN,
    /* 93 */ _PUN,
    /* 94 */ _PUN,
    /* 95 */ _PUN,
    /* 96 */ _PUN,
    /* 97 */ _LWR+_HEX,
    /* 98 */ _LWR+_HEX,
    /* 99 */ _LWR+_HEX,
    /* 100 */ _LWR+_HEX,
    /* 101 */ _LWR+_HEX,
    /* 102 */ _LWR+_HEX,
    /* 103 */ _LWR,
    /* 104 */ _LWR,
    /* 105 */ _LWR,
    /* 106 */ _LWR,
    /* 107 */ _LWR,
    /* 108 */ _LWR,
    /* 109 */ _LWR,
    /* 110 */ _LWR,
    /* 111 */ _LWR,
    /* 112 */ _LWR,
    /* 113 */ _LWR,
    /* 114 */ _LWR,
    /* 115 */ _LWR,
    /* 116 */ _LWR,
    /* 117 */ _LWR,
    /* 118 */ _LWR,
    /* 119 */ _LWR,
    /* 120 */ _LWR,
    /* 121 */ _LWR,
    /* 122 */ _LWR,
    /* 123 */ _PUN,
    /* 124 */ _PUN,
    /* 125 */ _PUN,
    /* 126 */ _PUN,
    /* 127 */ _CTR,
    /* 128 */ 0,
    /* 129 */ 0,
    /* 130 */ 0,
    /* 131 */ 0,
    /* 132 */ 0,
    /* 133 */ 0,
    /* 134 */ 0,
    /* 135 */ 0,
    /* 136 */ 0,
    /* 137 */ 0,
    /* 138 */ 0,
    /* 139 */ 0,
    /* 140 */ 0,
    /* 141 */ 0,
    /* 142 */ 0,
    /* 143 */ 0,
    /* 144 */ 0,
    /* 145 */ 0,
    /* 146 */ 0,
    /* 147 */ 0,
    /* 148 */ 0,
    /* 149 */ 0,
    /* 150 */ 0,
    /* 151 */ 0,
    /* 152 */ 0,
    /* 153 */ 0,
    /* 154 */ 0,
    /* 155 */ 0,
    /* 156 */ 0,
    /* 157 */ 0,
    /* 158 */ 0,
    /* 159 */ 0,
    /* 160 */ 0,
    /* 161 */ _PUN,
    /* 162 */ _PUN,
    /* 163 */ _PUN,
    /* 164 */ _PUN,
    /* 165 */ _PUN,
    /* 166 */ _PUN,
    /* 167 */ _PUN,
    /* 168 */ _PUN,
    /* 169 */ _PUN,
    /* 170 */ _PUN,
    /* 171 */ _PUN,
    /* 172 */ _PUN,
    /* 173 */ _PUN,
    /* 174 */ _PUN,
    /* 175 */ _PUN,
    /* 176 */ _PUN,
    /* 177 */ _PUN,
    /* 178 */ _PUN,
    /* 179 */ _PUN,
    /* 180 */ _PUN,
    /* 181 */ _PUN,
    /* 182 */ _PUN,
    /* 183 */ _PUN,
    /* 184 */ _PUN,
    /* 185 */ _PUN,
    /* 186 */ _PUN,
    /* 187 */ _PUN,
    /* 188 */ _PUN,
    /* 189 */ _PUN,
    /* 190 */ _PUN,
    /* 191 */ _PUN,
    /* 192 */ _PUN,
    /* 193 */ _PUN,
    /* 194 */ _PUN,
    /* 195 */ _PUN,
    /* 196 */ _PUN,
    /* 197 */ _PUN,
    /* 198 */ _PUN,
    /* 199 */ _PUN,
    /* 200 */ _PUN,
    /* 201 */ _PUN,
    /* 202 */ _PUN,
    /* 203 */ _PUN,
    /* 204 */ _PUN,
    /* 205 */ _PUN,
    /* 206 */ _PUN,
    /* 207 */ _PUN,
    /* 208 */ _PUN,
    /* 209 */ _PUN,
    /* 210 */ _PUN,
    /* 211 */ _PUN,
    /* 212 */ _PUN,
    /* 213 */ _PUN,
    /* 214 */ _PUN,
    /* 215 */ _PUN,
    /* 216 */ _PUN,
    /* 217 */ _PUN,
    /* 218 */ _PUN,
    /* 219 */ _PUN,
    /* 220 */ _PUN,
    /* 221 */ _PUN,
    /* 222 */ _PUN,
    /* 223 */ _PUN,
    /* 224 */ 0,
    /* 225 */ 0,
    /* 226 */ 0,
    /* 227 */ 0,
    /* 228 */ 0,
    /* 229 */ 0,
    /* 230 */ 0,
    /* 231 */ 0,
    /* 232 */ 0,
    /* 233 */ 0,
    /* 234 */ 0,
    /* 235 */ 0,
    /* 236 */ 0,
    /* 237 */ 0,
    /* 238 */ 0,
    /* 239 */ 0,
    /* 240 */ 0,
    /* 241 */ 0,
    /* 242 */ 0,
    /* 243 */ 0,
    /* 244 */ 0,
    /* 245 */ 0,
    /* 246 */ 0,
    /* 247 */ 0,
    /* 248 */ 0,
    /* 249 */ 0,
    /* 250 */ 0,
    /* 251 */ 0,
    /* 252 */ 0,
    /* 253 */ 0,
    /* 254 */ 0,
    /* 255 */ 0};

#define isalpha(c)  ((_ctype)[(unsigned char)(c)]&(_UPC|_LWR))
#define isupper(c)  ((_ctype)[(unsigned char)(c)]&_UPC)
#define islower(c)  ((_ctype)[(unsigned char)(c)]&_LWR)
#define isdigit(c)  ((_ctype)[(unsigned char)(c)]&_DIG)
#define isxdigit(c) ((_ctype)[(unsigned char)(c)]&(_DIG|_HEX))
#define isspace(c)  ((_ctype)[(unsigned char)(c)]&_SPC)
#define ispunct(c)  ((_ctype)[(unsigned char)(c)]&_PUN)
#define isalnum(c)  ((_ctype)[(unsigned char)(c)]&(_UPC|_LWR|_DIG))
#define isprint(c)  ((_ctype)[(unsigned char)(c)]&(_PUN|_UPC|_LWR|_DIG|_BLK))
#define isgraph(c)  ((_ctype)[(unsigned char)(c)]&(_PUN|_UPC|_LWR|_DIG))
#define iscntrl(c)  ((_ctype)[(unsigned char)(c)]&_CTL)
#define isascii(c)  ((unsigned)(c)<=0x7f)
#define toascii(c)  ((unsigned char)(c)&0x7f)

/* ********************************************************************* */
/* File local definitions */

/* for _doprintx */
#define BUF 32

#define ARG()                                           \
            _ulong =                                    \
            (flags & SHORTINT) ?                        \
                (u32)((s16)((u16)va_arg(argp, u32))) :  \
            (u32)va_arg(argp, u32);

#define todigit(c)  ((c) - '0')
#define tochar(n)   ((n) + '0')

/* have to deal with the negative buffer count kludge */
#define NEGATIVE_COUNT_KLUDGE

#define LONGINT     0x01        /* long integer         */
#define LONGDBL     0x02        /* long double; unimplemented   */
#define SHORTINT    0x04        /* short integer        */
#define ALT         0x08        /* alternate form       */
#define LADJUST     0x10        /* left adjustment      */
#define ZEROPAD     0x20        /* zero (as opposed to blank) pad */
#define HEXPREFIX   0x40        /* add 0x or 0X prefix      */

/* for strtol */
#define  max_allowable(A)    (MAXINT(sizeof(S32))/A - 1)

/* ********************************************************************* */
/* Local functions */

static int _doprntx(
    char        *fmt0,
    va_list     argp,
    void        (*putc)(),
    char        **pca)
{
    u8      *fmt;       /* format string                            */
    int     ch;         /* character from fmt                       */
    int     cnt;        /* return value accumulator                 */
    int     n;          /* random handy integer                     */
    char    *t;         /* buffer pointer                           */
    u32     _ulong;     /* integer arguments %[diouxX]              */
    int     base;       /* base for [diouxX] conversion             */
    int     dprec;      /* decimal precision in [diouxX]            */
    int     fieldsz;    /* field size expanded by sign, etc         */
    int     flags;      /* flags as above                           */
    int     fpprec;     /* `extra' floating precision in [eEfgG]    */
    int     prec;       /* precision from format (%.3d), or -1      */
    int     realsz;     /* field size expanded by decimal precision */
    int     size;       /* size of converted field or string        */
    int     width;      /* width from format (%8d), or 0            */
    char    sign;       /* sign prefix (' ', '+', '-', or \0)       */
    char    *digs;      /* digits for [diouxX] conversion           */
    char    buf[BUF];   /* space for %c, %[diouxX], %[eEfgG]        */

    fmt = (u8*)fmt0;
    digs = "0123456789abcdef";

    for (cnt = 0; ; ++fmt) {
        for (; (ch = *fmt) && ch != '%'; ++cnt, ++fmt) {
            if (ch!='\n') {
                putc(ch, pca);
            } else {
                putc(CR, pca);
                putc(LF, pca);
                ++cnt;
            }
        }

        if (!ch)
            return (cnt);

        flags = 0; dprec = 0; fpprec = 0; width = 0;
        prec = -1;
        sign = '\0';

rflag:
        switch (*++fmt) {
        case ' ':
            /*
             * ``If the space and + flags both appear, the space
             * flag will be ignored.''
             *  -- ANSI X3J11
             */
            if (!sign)
                sign = ' ';
            goto rflag;
        case '#':
            flags |= ALT;
            goto rflag;
        case '*':
            /*
             * ``A negative field width argument is taken as a
             * - flag followed by a  positive field width.''
             *  -- ANSI X3J11
             * They don't exclude field widths read from args.
             */
            if ((width = va_arg(argp, int)) >= 0)
                goto rflag;
            width = -width;
            /* FALLTHROUGH */
        case '-':
            flags |= LADJUST;
            goto rflag;
        case '+':
            sign = '+';
            goto rflag;
        case '.':
            if (*++fmt == '*')
                n = va_arg(argp, int);
            else {
                n = 0;
                while (isascii(*fmt) && isdigit(*fmt))
                    n = 10 * n + todigit(*fmt++);
                --fmt;
            }
            prec = n < 0 ? -1 : n;
            goto rflag;
        case '0':
            /*
             * ``Note that 0 is taken as a flag, not as the
             * beginning of a field width.''
             *  -- ANSI X3J11
             */
            flags |= ZEROPAD;
            goto rflag;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = 0;
            do {
                n = 10 * n + todigit(*fmt);
            } while (isascii(*++fmt) && isdigit(*fmt));
            width = n;
            --fmt;
            goto rflag;
        case 'L':
            flags |= LONGDBL;
            goto rflag;
        case 'h':
            flags |= SHORTINT;
            goto rflag;
        case 'l':
            flags |= LONGINT;
            goto rflag;
        case 'c':
            *(t = buf) = va_arg(argp, int);
            size = 1;
            sign = '\0';
            goto pforw;
        case 'D':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'd':
        case 'i':
            ARG();
            if ((long)_ulong < 0) {
                _ulong = -_ulong;
                sign = '-';
            }
            base = 10;
            goto number;
        case 'n':
            if (flags & LONGINT)
                *va_arg(argp, long *) = cnt;
            else if (flags & SHORTINT)
                *va_arg(argp, short *) = cnt;
            else
                *va_arg(argp, int *) = cnt;
            break;
        case 'O':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'o':
            ARG();
            base = 8;
            goto nosign;
        case 'p':
            /*
             * ``The argument shall be a pointer to void.  The
             * value of the pointer is converted to a sequence
             * of printable characters, in an implementation-
             * defined manner.''
             *  -- ANSI X3J11
             */
            /* NOSTRICT */
            _ulong = (u32)va_arg(argp, void *);
            base = 16;
            goto nosign;
        case 's':
            if (!(t = va_arg(argp, char *)))
                t = "(null)";
            if (prec >= 0) {
                /*
                 * can't use strlen; can only look for the
                 * NUL in the first `prec' characters, and
                 * strlen() will go further.
                 */
                char *p;

                if ((p = (void *)memchr(t, 0, prec))) {
                    size = p - t;
                    if (size > prec)
                        size = prec;
                } else
                    size = prec;
            } else
                size = strlen(t);
            sign = '\0';
            goto pforw;
        case 'U':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'u':
            ARG();
            base = 10;
            goto nosign;
        case 'X':
            digs = "0123456789ABCDEF";
            /* FALLTHROUGH */
        case 'x':
            ARG();
            base = 16;
            /* leading 0x/X only if non-zero */
            if (flags & ALT && _ulong != 0)
                flags |= HEXPREFIX;

            /* unsigned conversions */
nosign:     sign = '\0';
            /*
             * ``... diouXx conversions ... if a precision is
             * specified, the 0 flag will be ignored.''
             *  -- ANSI X3J11
             */
number:     if ((dprec = prec) >= 0)
                flags &= ~ZEROPAD;

            /*
             * ``The result of converting a zero value with an
             * explicit precision of zero is no characters.''
             *  -- ANSI X3J11
             */
            t = buf + BUF;
            if (_ulong != 0 || prec != 0) {
                do {
                    *--t = digs[_ulong % base];
                    _ulong /= base;
                } while (_ulong);
                digs = "0123456789abcdef";
                if (flags & ALT && base == 8 && *t != '0')
                    *--t = '0'; /* octal leading 0 */
            }
            size = buf + BUF - t;

pforw:
            /*
             * All reasonable formats wind up here.  At this point,
             * `t' points to a string which (if not flags&LADJUST)
             * should be padded out to `width' places.  If
             * flags&ZEROPAD, it should first be prefixed by any
             * sign or other prefix; otherwise, it should be blank
             * padded before the prefix is emitted.  After any
             * left-hand padding and prefixing, emit zeroes
             * required by a decimal [diouxX] precision, then print
             * the string proper, then emit zeroes required by any
             * leftover floating precision; finally, if LADJUST,
             * pad with blanks.
             */

            /*
             * compute actual size, so we know how much to pad
             * fieldsz excludes decimal prec; realsz includes it
             */
            fieldsz = size + fpprec;
            if (sign)
                fieldsz++;
            if (flags & HEXPREFIX)
                fieldsz += 2;
            realsz = dprec > fieldsz ? dprec : fieldsz;

            /* right-adjusting blank padding */
            if ((flags & (LADJUST|ZEROPAD)) == 0 && width)
                for (n = realsz; n < width; n++)
                    putc(' ', pca);
            /* prefix */
            if (sign)
                putc(sign, pca);
            if (flags & HEXPREFIX) {
                putc('0', pca);
                putc((char)*fmt, pca);
            }
            /* right-adjusting zero padding */
            if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
                for (n = realsz; n < width; n++)
                    putc('0', pca);
            /* leading zeroes from decimal precision */
            for (n = fieldsz; n < dprec; n++)
                putc('0', pca);

            /* the string or number proper */
            n = size;
            while (--n >= 0)
              putc(*t++, pca);
            /* trailing f.p. zeroes */
            while (--fpprec >= 0)
                putc('0', pca);
            /* left-adjusting padding (always blank) */
            if (flags & LADJUST)
                for (n = realsz; n < width; n++)
                    putc(' ', pca);
            /* finally, adjust cnt */
            cnt += width > realsz ? width : realsz;
            break;
        case '\0':  /* "%?" prints ?, unless ? is NULL */
            return (cnt);
        default:
            putc((char)*fmt, pca);
            cnt++;
        }
    }
    /* NOTREACHED */
}

static void putch(char ch, char **pca)
{
    *(*pca)++ = ch;
}

static void _putc(char ch, char **pca)
{
    serial_putc(ch);
}

int printf(const char *fmt, ...)
{
    int ret;
    va_list args;
    va_start(args,fmt);
    ret = _doprntx((char *)fmt, args, (void (*)())_putc, 0);
    va_end(args);

    return ret;
}
#endif


static void _PrintChar(char *fmt, char c)
{
	SerialOutputByte(c);
	return;
}

static void _PrintDec(char *fmt, int l)
{
	char tol[10];
	int remainder;
	int count = 0;
	if (l == 0) {
		SerialOutputByte('0');
		return;
	}
	if (l < 0) {
		SerialOutputByte('-');
		l = -l;
	}
	/* FIXME: we don't intend to handle the number bigger than 100000. */
	if (l > 100000)
		return;

	for ( ; count < 10; count++)
		tol[count] = '\0';
	count = 7;
	do {
		remainder = l % 10;
		tol[count--] = '0' + remainder;
		l = (l - remainder) / 10;
	} while (count > 0);
	tol[0] = ' ';

	_PrintString(fmt, tol);
}

static void _PrintHex(char *fmt, int l)
{
	int i, j;
	char c, *s = fmt, tol[10];
	bool flag0 = false, flagl = false;
	long flagcnt = 0;
	bool leading_zero = true;
	char uHex, lHex;
	int cnt;	/* note: the format like "% 5x" only prints
			   the number of 5. */

	/* format like "%081" is interpreted for '0', '8', 'l'
	 * individually. */
	for (i = 0; (c = s[i]) != 0; i++) {
		if (c == 'x')
			break;
		else if (c >= '1' && c <= '9') {
			for (j = 0; s[i] >= '0' && s[i] <= '9'; j++) {
				tol[j] = s[i++];
			}
			tol[j] = '\0';
			i--;
			DecToLong(tol, &flagcnt);
		}
		else if (c == '0')
			flag0 = true;
		else if (c == 'l')
			flagl = true;
		else
			continue;
	}

	s = (char *)(&l);
	l = SWAP32(l);	/* depends on the endianess. */

	/* output, based on the flag */
	if (flagcnt) {
		if (flagcnt & 0x01) {	/* upper ignored, lower the output. */
			c = s[(8 - (flagcnt + 1)) / 2];

			/* check if lower 4 bits becomes ASCII code. */
			lHex = ((c >> 0) & 0x0f);
			if (lHex != 0)
				leading_zero = false;
			if (lHex < 10)
				lHex += '0';
			else         lHex += 'A' - 10;

			/* lower 4 bits */
			if (leading_zero) {
				if (flag0)
					SerialOutputByte('0');
				else
					SerialOutputByte(' ');
			}
			else SerialOutputByte(lHex);

			flagcnt--;
		}

		/* byte-level data, the output Hex */
		for (cnt = 0, i = (8 - flagcnt) / 2; i < 4; i++) {
			c = s[i];

			/* get upper 4 bits and lower 4 bits. */
			uHex = ((c >> 4) & 0x0f);
			lHex = ((c >> 0) & 0x0f);

			/* upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'.
			   upper 4 bits: ascii code */
			if (uHex != 0)
				leading_zero = false;
			if (uHex < 10)
				uHex += '0';
			else
				uHex += 'A' - 10;

			/* upper 4 bits */
			if (leading_zero) {
				if (flag0)
					SerialOutputByte('0');
				else
					SerialOutputByte(' ');
			}
			else SerialOutputByte(uHex);

			/* lower 4 bits: ascii code */
			if (lHex != 0)
				leading_zero = false;
			if (lHex < 10)
				lHex += '0';
			else
				lHex += 'A' - 10;

			/* lower 4 bits */
			if (leading_zero) {
				if (flag0)
					SerialOutputByte('0');
				else
					SerialOutputByte(' ');
			}
			else
				SerialOutputByte(lHex);
		}
	}
	else {
		for (i = 0; i < 4; i++){
			c = s[i];

			/* get upper 4 bits and lower 4 bits. */
			uHex = ((c >> 4) & 0x0f);
			lHex = ((c >> 0) & 0x0f);

			/* upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'. */
			if (uHex != 0)
				leading_zero = false;
			if (uHex < 10)
				uHex += '0';
			else
				uHex += 'A' - 10;
			if (!leading_zero)
				SerialOutputByte(uHex);

			if (lHex != 0 || i == 3)
				leading_zero = false;
			if (lHex < 10)
				lHex += '0';
			else
				lHex += 'A' - 10;
			if (!leading_zero)
				SerialOutputByte(lHex);
		}
	}
	return;
}

static void _PrintString(char *fmt, char *s)
{
	if (!fmt || !s)
		return;
	while (*s)
		SerialOutputByte(*s++);
	return;
}

int getc(void)
{
	return serial_getc();
}

int gets(char *s)
{
	int cnt = 0;
	char  c;

	while ((c = getc()) != CR) {
		if (c != BS) {
			cnt++;
			*s++ = c;
			printf("%c",c );
		}
		else {
			if (cnt > 0) {
				cnt--;
				*s-- = ' ';
				printf("\b \b");
			}
		}
	}
	*s = 0;
	return cnt;
}

int putchar(int c)
{
	SerialOutputByte(c);
	return c;
}

int puts(const char *s)
{
	while (*s)
		SerialOutputByte(*s++);
	return 1;
}

static char * ___strtok;
char * strtok(char * s,const char * ct)
{
	char *sbegin, *send;

	sbegin  = s ? s : ___strtok;
	if (!sbegin) {
		return NULL;
	}
	sbegin += strspn(sbegin, ct);
	if (*sbegin == '\0') {
		___strtok = NULL;
		return( NULL );
	}
	send = strpbrk(sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';
	___strtok = send;
	return (sbegin);
}

unsigned int strspn(const char *s, const char *accept)
{
	const char *p;
	const char *a;
	unsigned int count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (a = accept; *a != '\0'; ++a) {
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}
	return count;
}

char * strpbrk(const char * cs,const char * ct)
{
	const char *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1) {
		for (sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (char *) sc1;
		}
	}
	return NULL;
}

unsigned long strtoul(const char *str, char **endptr, int requestedbase)
{
	unsigned long num = 0;
	char c;
	unsigned char digit;
	int base = 10;
	int nchars = 0;
	int leadingZero = 0;
	unsigned char strtoul_err = 0;

	while ((c = *str) != 0) {
		if (nchars == 0 && c == '0') {
			leadingZero = 1;
			goto step;
		}
		else if (leadingZero && nchars == 1) {
			if (c == 'x') {
				base = 16;
				goto step;
			}
			else if (c == 'o') {
				base = 8;
				goto step;
			}
		}
		if (c >= '0' && c <= '9') {
			digit = c - '0';
		}
		else if (c >= 'a' && c <= 'z') {
			digit = c - 'a' + 10;
		}
		else if (c >= 'A' && c <= 'Z') {
			digit = c - 'A' + 10;
		}
		else {
			strtoul_err = 3;
			return 0;
		}
		if (digit >= base) {
			strtoul_err = 4;
			return 0;
		}
		num *= base;
		num += digit;
step:
		str++;
		nchars++;
	}
	return num;
}

void *memchr(const void *s, int c, int n)
{
	const unsigned char *p = s;
	while (n-- != 0) {
        	if ((unsigned char)c == *p++) {
			return (void *)(p - 1);
		}
	}
	return NULL;
}

int strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}
