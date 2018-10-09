/**************************************************************************

    This file is part of the CP/NET 1.1/1.2 server emulator for Win32
    systems. Copyright (C) 2004,2005, Hector Peraza.
    Portions copyright (C) 2005-2017. Teunis van Beelen (teuniz@gmail.com)
    Portions copyright (C) 2018, Yuri Prokushev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/

#ifndef __inifile_h
#define __inifile_h

#define INI_MAX_LINE_LEN   1024

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif


FILE *ini_openr(const char *filename);
FILE *ini_openw(const char *filename);
int ini_close(FILE *ini);
int ini_rewind(FILE *ini);
int ini_get_next(FILE *ini, char *section);
char *ini_get_item(FILE *ini, const char *item, char *value);
int ini_get_bool(FILE *ini, const char *item, int default_val);
int ini_put_next(FILE *ini, const char *section);
int ini_put_item(FILE *ini, const char *item, const char *value);
int ini_put_bool(FILE *ini, const char *item, int value);
int ini_put_newline(FILE *ini);


#endif  // __inifile_h
