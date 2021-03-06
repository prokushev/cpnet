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

#ifndef __main_h
#define __main_h

#define VERSION "0.9"

#define DEBUG_PACKET  0x01
#define DEBUG_DATA    0x02
#define DEBUG_MISC    0x04

#define CPNET_1_1     11
#define CPNET_1_2     12


void usage(char *pname);
void dump_data(unsigned char *buf, int len, int y);
int goto_drive(int drive);
int lst_output(int num, char *buf, int len);
int set_speed(int baud);
int get_baud(int speed);
int read_ini(char *fname);


#endif  /* __main_h */
