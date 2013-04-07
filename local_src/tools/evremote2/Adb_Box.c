/*
 * Adb_Box.c
 *
 * (c) 2010 duckbox project
 *
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#include "global.h"
#include "map.h"
#include "remotes.h"
#include "Adb_Box.h"

#define ADB_BOX_LONGKEY

#ifdef ADB_BOX_LONGKEY
static tLongKeyPressSupport cLongKeyPressSupport = {
//  140, 100,
    250, 45,
};
#endif

//extern int KeyPressDown;

/* B4Team ADB_BOX RCU */
static tButton cButtonsADB_BOX[] = {

    {"POWER"          	, "01", KEY_POWER},
    {"VOD"            	, "02", KEY_AUX},
    {"N.Button"       	, "03", KEY_V},

    {"EPG"            	, "04", KEY_EPG},
    {"HOME"           	, "05", KEY_BACK}, //HOME
    {"BACK"           	, "06", KEY_HOME}, //BACK
    {"INFO"           	, "07", KEY_INFO}, //THIS IS WRONG SHOULD BE KEY_INFO

    {"OPT"            	, "08", KEY_MENU},

    {"VOLUMEUP"		, "09", KEY_VOLUMEUP},
    {"VOLUMEDOWN"	, "0a", KEY_VOLUMEDOWN},
    {"CHANNELUP"	, "0b", KEY_PAGEUP},
    {"CHANNELDOWN"	, "0c", KEY_PAGEDOWN},

    {"OK"             	, "0d", KEY_OK},

    {"UP"          	, "0e", KEY_UP},
    {"DOWN"        	, "0f", KEY_DOWN},
    {"LEFT"       	, "10", KEY_LEFT},
    {"RIGHT"       	, "11", KEY_RIGHT},
 
    {"STOP"           	, "12", KEY_STOP},
    {"REWIND"         	, "13", KEY_REWIND},
    {"FASTFORWARD"    	, "14", KEY_FASTFORWARD},
    {"PLAY"           	, "15", KEY_PLAY},
    {"PAUSE"          	, "16", KEY_PAUSE},
    {"RECORD"         	, "17", KEY_RECORD},

    {"MUTE"           	, "18", KEY_MUTE},

    {"TV/RADIO/@"     	, "19", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCH BUTTON
    {"TEXT"           	, "1a", KEY_TEXT},
    {"LIST"           	, "1b", KEY_FAVORITES},

    {"RED"            	, "1c", KEY_RED},
    {"GREEN"          	, "1d", KEY_GREEN},
    {"YELLOW"         	, "1e", KEY_YELLOW},
    {"BLUE"           	, "1f", KEY_BLUE},

    {"1BUTTON"        	, "20", KEY_1},
    {"2BUTTON"        	, "21", KEY_2},
    {"3BUTTON"        	, "22", KEY_3},
    {"4BUTTON"        	, "23", KEY_4},
    {"5BUTTON"        	, "24", KEY_5},
    {"6BUTTON"        	, "25", KEY_6},
    {"7BUTTON"        	, "26", KEY_7},
    {"8BUTTON"        	, "27", KEY_8},
    {"9BUTTON"        	, "28", KEY_9},
    {"0BUTTON"        	, "29", KEY_0},

    {"AUDIO/SETUP"    	, "2a", KEY_AUDIO},

    {"TIMER/APP"      	, "2b", KEY_TIME},

    {"STAR"         	, "2c", KEY_HELP},

//------long

    {"POWER"          	, "41", KEY_POWER},
    {"VOD"            	, "42", KEY_AUX},
    {"N.Button"       	, "43", KEY_V},

    {"EPG"            	, "44", KEY_EPG},
    {"HOME"           	, "45", KEY_BACK}, //HOME
    {"BACK"           	, "46", KEY_HOME}, //BACK
    {"INFO"           	, "47", KEY_INFO}, //THIS IS WRONG SHOULD BE KEY_INFO

    {"OPT"            	, "48", KEY_MENU},

    {"VOLUMEUP"		, "49", KEY_VOLUMEUP},
    {"VOLUMEDOWN"	, "4a", KEY_VOLUMEDOWN},
    {"CHANNELUP"	, "4b", KEY_PAGEUP},
    {"CHANNELDOWN"	, "4c", KEY_PAGEDOWN},

    {"OK"             	, "4d", KEY_OK},

    {"UP"          	, "4e", KEY_UP},
    {"DOWN"        	, "4f", KEY_DOWN},
    {"LEFT"       	, "50", KEY_LEFT},
    {"RIGHT"       	, "51", KEY_RIGHT},
 
    {"STOP"           	, "52", KEY_STOP},
    {"REWIND"         	, "53", KEY_REWIND},
    {"FASTFORWARD"    	, "54", KEY_FASTFORWARD},
    {"PLAY"           	, "55", KEY_PLAY},
    {"PAUSE"          	, "56", KEY_PAUSE},
    {"RECORD"         	, "57", KEY_RECORD},

    {"MUTE"           	, "58", KEY_MUTE},

    {"TV/RADIO/@"     	, "59", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCH BUTTON
    {"TEXT"           	, "5a", KEY_TEXT},
    {"LIST"           	, "5b", KEY_FAVORITES},

    {"RED"            	, "5c", KEY_RED},
    {"GREEN"          	, "5d", KEY_GREEN},
    {"YELLOW"         	, "5e", KEY_YELLOW},
    {"BLUE"           	, "5f", KEY_BLUE},

    {"1BUTTON"        	, "60", KEY_1},
    {"2BUTTON"        	, "61", KEY_2},
    {"3BUTTON"        	, "62", KEY_3},
    {"4BUTTON"        	, "63", KEY_4},
    {"5BUTTON"        	, "64", KEY_5},
    {"6BUTTON"        	, "65", KEY_6},
    {"7BUTTON"        	, "66", KEY_7},
    {"8BUTTON"        	, "67", KEY_8},
    {"9BUTTON"        	, "68", KEY_9},
    {"0BUTTON"        	, "69", KEY_0},

    {"AUDIO/SETUP"    	, "6a", KEY_AUDIO},

    {"TIMER/APP"      	, "6b", KEY_TIME},

    {"STAR"         	, "6c", KEY_HELP},

    {""               	, ""  , KEY_NULL},
};
/* fixme: move this to a structure and
 * use the private structure of RemoteControl_t
 */
static struct sockaddr_un  vAddr;

static int pInit(Context_t* context, int argc, char* argv[]) {

    int vHandle;

    vAddr.sun_family = AF_UNIX;
    // in new lircd its moved to /var/run/lirc/lircd by default and need use key to run as old version
    
    strcpy(vAddr.sun_path, "/var/run/lirc/lircd");

    vHandle = socket(AF_UNIX,SOCK_STREAM, 0);

    if(vHandle == -1)  {
        perror("socket");
        return -1;
    }

    if(connect(vHandle,(struct sockaddr *)&vAddr,sizeof(vAddr)) == -1)
    {
        perror("connect");
        return -1;
    }

    return vHandle;
}

int pShutdown(Context_t* context ) {

    close(context->fd);

    return 0;
}

#ifndef ADB_BOX_LONGKEY
static int pRead(Context_t* context ) {
    char                vBuffer[128];
    char                vData[10];
    const int           cSize         = 128;
    int                 vCurrentCode  = -1;

    read (context->fd, vBuffer, cSize);


    vData[0] = vBuffer[14];
    vData[1] = vBuffer[15];
    vData[2] = '\0';

/*
    printf("[ ADB_BOX RCU ] key: %s\n", &vBuffer);   			//move to DEBUG
    printf("[ ADB_BOX RCU ] key: %s -> %s\n", vData, &vBuffer[20]);   	//move to DEBUG
*/
    vCurrentCode = getInternalCode(cButtonsADB_BOX, vData);

    printf("[ ADB_BOX RCU ] key: vCC -> %i\n", vCurrentCode);

    return vCurrentCode;
}

#else

static int gNextKey = 0;

static int pRead(Context_t* context) {
    char                vBuffer[128];
    char         	vData[3];
    const int    	cSize		= 128;
    int          	vCurrentCode	= -1;
    char 		*buffer;

    memset(vBuffer, 0, 128);
    
    //wait for new command
    read (context->fd, vBuffer, cSize);




  vData[0] = vBuffer[17];
  vData[1] = vBuffer[18];
  vData[2] = '\0';
  //printf("[RCU ADB_BOX] key: %s -> %s\n", vData, &vBuffer[0]);

  vData[0] = vBuffer[14];
  vData[1] = vBuffer[15];
  vData[2] = '\0';
  printf("[RCU ADB_BOX] key: %s -> %s\n", vData, &vBuffer[0]);


  vCurrentCode = getInternalCode((tButton*)((RemoteControl_t*)context->r)->RemoteControl, vData);
  
  //printf("[ADB_BOX RCU] key: vCC -> %i\n", vCurrentCode);
  //printf("[ADB_BOX RCU] [17|18]: %i,    {"CHANNELUP"	, "0b", KEY_PAGEUP},
 
	if(vCurrentCode != 0) {
		static int nextflag = 0;
		if (('0' == vBuffer[17]) && ('0' == vBuffer[18]))
		{
		    nextflag++;
		}
//		printf("[ADB_BOX RCU] nextflag: nf -> %i\n", nextflag);
		vCurrentCode += (nextflag << 16);
	}


//   printf("[ADB_BOX RCU] key: vCC -> %i\n", vCurrentCode); 

return vCurrentCode;


}
#endif


static int pNotification(Context_t* context, const int cOn) {

    struct adb_box_ioctl_data vfd_data;
    int ioctl_fd = -1;

	struct {
		unsigned char start;
		unsigned char data[64];
		unsigned char length;
	} data;
        

    if(cOn)
    {

       ioctl_fd = open("/dev/vfd", O_RDONLY);

       	data.start = 0x00;
    	data.data[0] = 35;
    	data.data[4] = 1;
    	data.length = 5;
    	ioctl(ioctl_fd, VFDICONDISPLAYONOFF, &data);

       close(ioctl_fd);
    }
    else
    {
       usleep(100000);

       ioctl_fd = open("/dev/vfd", O_RDONLY);

       	data.start = 0x00;
    	data.data[0] = 35;
    	data.data[4] = 0;
    	data.length = 5;
    	ioctl(ioctl_fd, VFDICONDISPLAYONOFF, &data);

       close(ioctl_fd);
    }

    return 0;
}

RemoteControl_t Adb_Box_RC = {
	"Adb_Box RemoteControl",
	Adb_Box,
	&pInit,
	&pShutdown,
	&pRead,
	&pNotification,
	cButtonsADB_BOX,
	NULL,
        NULL,
#ifndef ADB_BOX_LONGKEY
    	0,
    	NULL,
#else
    	1,
    	&cLongKeyPressSupport,
#endif
};
