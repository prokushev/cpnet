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

#ifndef __netio_h
#define __netio_h

#define SOH   1
#define STX   2
#define ETX   3
#define EOT   4
#define ENQ   5
#define ACK   6
#define NAK   0x15


void wait_for_packet();
int get_packet(char *data, int *len, int *fnc, int *sid);
int send_packet(int to, int fnc, char *data, int len);
int send_ok(int to, int fnc);
int send_error(int to, int fnc);


#endif  /* __netio_h */
