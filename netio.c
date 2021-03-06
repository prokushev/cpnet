/*************************************************************************

    This file is part of the CP/NET 1.1/1.2 server emulator for Win32
    systems. Copyright (C) 2004,2005, Hector Peraza.
    Portions copyright (C) 2005-2017. Teunis van Beelen (teuniz@gmail.com)
    Portions copyright (C) 2018, Yuri Prokushev

    This module implements the serial port communicarion protocol
    described in Appendix E of the CP/NET documentation.
  
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

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include <time.h>

#include "main.h"
#include "netio.h"
//#include "sio.h"
#include "rs232.h"

/*----------------------------------------------------------------------*/

extern int _level;     /* which CP/NET version is being emulated */
extern int _netID;     /* our server ID */
extern int _debug;     /* debug mask */

void wait_for_packet() {
  unsigned char buf[2];

  /* wait for ENQ byte... */
  while (1) {
    while (RS232_PollComport(0, buf, 1) < 1) {}
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[0]);
    if (buf[0] == ENQ) break;
  }
      
  /* send ACK back */
  buf[0] = ACK;
  RS232_SendBuf(0, buf, 1);
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[0]);
}

int get_packet(char *data, int *len, int *fnc, int *sid) {
  int i, n, did, siz;
  unsigned char buf[1024], cks;


  /* receive header */
  n = RS232_PollComport(0, buf, 7);

  cks = 0;
  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
    cks += buf[i];
  }
  if (buf[0] != SOH || n != 7) {
    if (_debug & DEBUG_PACKET) printf("Bad packet\n\n");
    return -1;
  }
  if (cks != 0) {
    if (_debug & DEBUG_PACKET) printf("Checksum error\n\n");
    return -1;
  }
  did = buf[2];
  *sid = buf[3];
  *fnc = buf[4];
  siz = buf[5];
  *len = siz + 1;
  if (did != _netID) {
    if (_debug & DEBUG_PACKET) printf("Not for us...\n\n");
    return -1;
  }
      
  /* send ACK */
  buf[0] = ACK;
  RS232_SendBuf(0, buf, 1);
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[0]);
      
  /* receive data part */
  n = RS232_PollComport(0, buf, siz+2);
  cks = 0;
  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
    cks += buf[i];
  }
  if (buf[0] != STX) {
    if (_debug & DEBUG_PACKET) printf("Bad packet\n\n");
    return -1;
  }
  if (n != siz+2) {
    if (_debug & DEBUG_PACKET) printf("Bad length\n\n");
    return -1;
  }
  for (i = 0; i < siz+1; ++i) {
    data[i] = buf[i+1];
  }
      
  /* receive checksum field */
  n = RS232_PollComport(0, buf, 2);

  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
    cks += buf[i];
  }
  if (buf[0] != ETX || n != 2) {
    if (_debug & DEBUG_PACKET) printf("Bad packet\n\n");
    return -1;
  }
  if (cks != 0) {
    if (_debug & DEBUG_PACKET) printf("Checksum error\n\n");
    return -1;
  }
      
  /* receive trailer */
  n = RS232_PollComport(0, buf, 1);

  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
  }
  if (buf[0] != EOT || n != 1) {
    if (_debug & DEBUG_PACKET) printf("Bad packet\n\n");
    return -1;
  }

  /* send ACK */
  buf[0] = ACK;
  RS232_SendBuf(0, buf, 1);
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[0]);
  
  return 0;
}

int send_packet(int to, int fnc, char *data, int len) {
  int i, n;
  unsigned char buf[1024], cks;

  if (_debug & DEBUG_DATA) fprintf(stderr,"Replying\n");
  dump_data(data, len, 17);

  if (len < 1 || len > 256) {
    fprintf(stderr, "Error: can't send packet with length %d\n", len);
    return -1;
  }

  while(1)
  {
            
  buf[0] = ENQ;
  RS232_SendBuf(0, buf, 1);
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[0]);

  /* wait for ACK */
  n = RS232_PollComport(0, buf, 1);
  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
  }

  if (n != 1) {
    if (_debug & DEBUG_PACKET) printf("No ACK?\n\n");
  }
      
  buf[0] = SOH;
  buf[1] = 1;          /* FMT */
  buf[2] = to;         /* DID */
  buf[3] = _netID;     /* SID */
  buf[4] = fnc;        /* FNC */
  buf[5] = len - 1;    /* SIZ */
  cks = 0;
  for (i = 0; i < 6; ++i) {
    cks += buf[i];
    if (_debug & DEBUG_PACKET) fprintf(stderr,"\t<< %02X\n", buf[i]);
  }
  buf[6] = -cks;       /* HCS */
  if (_debug & DEBUG_PACKET) fprintf(stderr,"\t<< %02X\n", buf[6]);
  RS232_SendBuf(0, buf, 7);

  /* wait for ACK */
  n = RS232_PollComport(0, buf, 1);
  for (i = 0; i < n; ++i) {
    if (_debug & DEBUG_PACKET) fprintf(stderr,">> %02X\n", buf[i]);
  }
  if (n != 1) {
    if (_debug & DEBUG_PACKET) fprintf(stderr,"No ACK?\n\n");
  }

  buf[0] = STX;
  for (i = 0; i < len; ++i) {
    buf[i+1] = data[i];
  }

  buf[len+1] = ETX;
  cks = 0;
  for (i = 0; i < len + 2; ++i) {
    cks += buf[i];
    if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[i]);
  }
  buf[len+2] = -cks;
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[len+2]);
  buf[len+3] = EOT;
  if (_debug & DEBUG_PACKET) printf("\t<< %02X\n", buf[len+3]);
  RS232_SendBuf(0, buf, len+4);

  /* wait for ACK */
  n = RS232_PollComport(0, buf, 1);

  if (n != 1) {
    if (_debug & DEBUG_PACKET) printf("No ACK?\n\n");
    break;
  } else {
    for (i = 0; i < n; ++i) {
      if (_debug & DEBUG_PACKET) printf(">> %02X\n", buf[i]);
    }

    if (buf[0]!=NAK) break;
  }
  }

  return 0;
}

int send_ok(int to, int fnc) {
  unsigned char uc = 0;
  return send_packet(to, fnc, &uc, 1);
}

int send_error(int to, int fnc) {
  if (_level == CPNET_1_1) {
    unsigned char buf[2] = { 0xff, 0xff };
    return send_packet(to, fnc, buf, 1);  /*2*/
  } else {
    unsigned char buf[2] = { 0xff, 0x0c };
    return send_packet(to, fnc, buf, 2);
  }
}
