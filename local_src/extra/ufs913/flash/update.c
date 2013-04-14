/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int  fd;
char micom_cmd[20];

int settermflags(int fd) 
{
	struct termios old_io;
	struct termios new_io;
	
	if((tcgetattr(fd, &old_io)) == 0) 
	{
  		new_io = old_io;
	
		cfmakeraw( &new_io );

		new_io.c_iflag &= ~( INPCK |ICRNL | ISTRIP | IXON/*| BRKINT*/);

		new_io.c_lflag &= ~( ECHO | ICANON | IEXTEN /*| ISIG*/);

              	/* c_oflags ->output flags*/
    		new_io.c_oflag &= ~(ONLCR);


		new_io.c_cflag &= ~(CSIZE | PARENB);
		new_io.c_cflag |= CS8;

		new_io.c_cc[VMIN] = 1;
		new_io.c_cc[VTIME] = 0;
		new_io.c_cc[VEOL] = '\r';	
		new_io.c_cc[VEOL2] = '\r';
		new_io.c_cc[VEOF] = 0xe0;	// Not used..

		cfsetispeed(&new_io, B19200); 		/* 115200 baud rates */

		if(tcsetattr(fd, TCSAFLUSH, &new_io) < 0)
		{
			printf( "tcsetattr error" );
			return -1;	
		}
	}
	return 0;
}

void setText(int p, char* text)
{
    micom_cmd[0] = 0x21;
    memset( micom_cmd + 1, ' ', 16);
    strncpy( micom_cmd + 1 + p, text, strlen(text));
    micom_cmd[17] = '\0';

    write(fd, micom_cmd, 17 );

}

int main(int argc, char* argv[])
{
    memset(micom_cmd, 0, 20);

    fd = open("/dev/ttyAS1", O_WRONLY);

    if (fd < 0)
    {
        printf("cannot open /dev/ttyAS1 (errno = %d)\n", errno);
        exit(1);
    }

    settermflags(fd);

    int p = 0;
    int i = 1;

    char* text = "UPDATE";

    if (argc > 1)
        text = argv[1];

    while (1)
    {
        setText(p, text);

        p = p + i;

        if (p + strlen(text) == 16)
        {
            i = -1;
        }
        else
        if (p < 0)
        {
            i = 1;
            p = 1;
        }
            
        
        sleep(1);
    }

	return 0;
}
