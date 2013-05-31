
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <bpamem.h>

//bpa2dump 0x4a824000 28311552 1280 720

int main(int argc, char *argv[])
{
	int fd_bpa;
	int res;
	BPAMemMapMemData bpa_data;
	char bpa_mem_device[30];
	char *decode_surface;
	unsigned int w = 1280;
	unsigned int h = 720;

	fd_bpa = open("/dev/bpamem0", O_RDWR);
	
	if(fd_bpa < 0)
	{
		fprintf(stderr, "cannot access /dev/bpamem0! err = %d\n", fd_bpa);
		return 1;
	}
	
	bpa_data.bpa_part  = "LMI_VID";
	sscanf(argv[1], "%lx", &bpa_data.phys_addr); //0x4a824000;
	sscanf(argv[2], "%lu", &bpa_data.mem_size); //28311552;
	sscanf(argv[3], "%u", &w); //28311552;
	sscanf(argv[4], "%u", &h); //28311552;
	
	res = ioctl(fd_bpa, BPAMEMIO_MAPMEM, &bpa_data); // request memory from bpamem
	if(res)
	{
		fprintf(stderr, "cannot map required mem\n");
		return 1;
	}
	
	sprintf(bpa_mem_device, "/dev/bpamem%d", bpa_data.device_num);
	close(fd_bpa);
	
	fd_bpa = open(bpa_mem_device, O_RDWR);
	
	// if somebody forgot to add all bpamem devs then this gets really bad here
	if(fd_bpa < 0)
	{
		fprintf(stderr, "cannot access %s! err = %d\n", bpa_mem_device, fd_bpa);
		return 1;
	}
	
	decode_surface = (char *)mmap(0, bpa_data.mem_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd_bpa, 0);
	
	if(decode_surface == MAP_FAILED) 
	{
		fprintf(stderr, "could not map bpa mem");
		close(fd_bpa);
		return 1;
	}
#if 0
	for (i = 0; i < bpa_data.mem_size; i += 256)
	{
		fwrite(decode_surface + i, 256, 1, stdout);
	}
#else

/*
Wir haben 16x16pixel blöcke 0x100
d.h. erste zeile
0x0-0xf, 0x200-0x20f, ...
zweite zeile
0x10-0x1f, 0x210-0x21f, ...
...
16te zeile
0xe0-0xef, 0x2e0-0x2ef, ...

17te zeile
0x2000-0x200f,...

*/
#if 0
	{
		int i;
#if 0
		//luma
		for (i = 0x0; i < 0xdc100; i += 0x100)
		{
			fwrite(decode_surface + i, 0x100, 1, stdout);
		}
		for (i = 0xdc200; i < 0xe5f00; i += 0x200)
		{
			fwrite(decode_surface + i, 0x100, 1, stdout);
		}
#endif
		//chroma
		for (i = 0xe6000; i < 0x154100; i += 0x100)
		{
			fwrite(decode_surface + i, 0x100, 1, stdout);
		}
		for (i = 0x154200; i < 0x158f00; i += 0x200)
		{
			fwrite(decode_surface + i, 0x100, 1, stdout);
		}
	}
#else
	//luma
#define OUT(x) fputc((unsigned char)*(decode_surface + x)&0xFF, stdout)

#define OUT4(x) \
	OUT(x + 0x03); \
	OUT(x + 0x02); \
	OUT(x + 0x01); \
	OUT(x + 0x00);

#define OUT8(x) \
	OUT4(x + 0x04); \
	OUT4(x + 0x00);

#define OUT16A(x) \
	OUT8(x); \
	OUT8(x + 0x40);

#define OUT16A_C(x) \
	OUT4(x); \
	OUT4(x + 0x20); \
	OUT4(x + 0x80); \
	OUT4(x + 0xA0);

//x: macroblock address
//l: line 0-15
#define OUT16(x,l) \
	OUT16A(x + (l/4) * 0x10 + (l%2) * 0x80 + ((l/2)%2?0x00:0x08));

/*
0: 0x08
1: 0x48
2: 0x00
3: 0x40

4: 0x18
5: 0x58
6: 0x10
7: 0x50
*/

//x: macroblock address
//l: line 0-7
//b: 0=cr 1=cb
#define OUT16_C(x,l,b) \
	OUT16A_C(x + (l/4) * 0x10 + (l%2) * 0x40 + ((l/2)%2?0x00:0x08) + (b?0x04:0x00));

//x: first macroblock address
//l: line 0-15 of macroblock line
//s: stride by factor 16, e.g. 1280=80
#define OUTL(x,l,s) \
	{ \
		int iMacro; \
		for(iMacro = 0; iMacro < s; iMacro++) \
		{ \
			OUT16(x + iMacro*0x200, l); \
		} \
	}

//x: first macroblock address
//l: line 0-7
//s: stride by factor 16, e.g. 1280=80
//b: 0=cr 1=cb
#define OUTL_C(x,l,s, b) \
	{ \
		int iMacro; \
		for(iMacro = 0; iMacro < s; iMacro++) \
		{ \
			OUT16_C(x + iMacro*0x200, l, b); \
		} \
	}

//x: first macroblock address
//s: stride by factor 16, e.g. 1280=80
#define OUTL_16(x,s) \
	{ \
		int l; \
		for(l = 0; l < 16; l++) \
		{ \
			OUTL(x, l, s); \
		} \
	}

//x: first macroblock address
//s: stride by factor 16, e.g. 1280=80
//b: 0=cr 1=cb
#define OUTL_16_C(x,s,b) \
	{ \
		int l; \
		for(l = 0; l < 8; l++) \
		{ \
			OUTL_C(x, l, s, b); \
		} \
	}

	{
		int yblock, xblock, iyblock, yblockoffset, offset;

		//luma
		offset = 0;
		yblock = h/16;  //45
		xblock = w/16; //80
		yblockoffset = w*32; //0xA000
		
		for (iyblock = 0; iyblock < yblock; iyblock++)
		{
			OUTL_16(((iyblock/2) * yblockoffset) + (iyblock%2?0x0100:0x0000) + offset, xblock);
		}

		//chroma
		offset = ((w*h + yblockoffset/2 /*round up*/) / yblockoffset) * yblockoffset;
		//cb
		yblock = h/16; //45
		xblock = (w/2)/16; //40
		yblockoffset = (w/2)*8/*h in block*/*2/*y blocks*/*2/*cr cb*/; //16pixel per x block and 2 y blocks//0x5000
		
		for (iyblock = 0; iyblock < yblock; iyblock++)
		{
			OUTL_16_C(((iyblock/2) * yblockoffset) + (iyblock%2?0x0100:0x0000) + offset, xblock, 1);
		}

		//cr
		yblock = h/16; //45
		xblock = (w/2)/16; //40
		yblockoffset = (w/2)*8/*h in block*/*2/*y blocks*/*2/*cr cb*/; //16pixel per x block and 2 y blocks//0x5000
		
		for (iyblock = 0; iyblock < yblock; iyblock++)
		{
			OUTL_16_C(((iyblock/2) * yblockoffset) + (iyblock%2?0x0100:0x0000) + offset, xblock, 0);
		}
	}
	
	
#endif
#endif
	
	fflush(stderr);
	
	res = ioctl(fd_bpa, BPAMEMIO_UNMAPMEM); // request memory from bpamem
	if(res)
	{
		fprintf(stderr, "cannot unmap required mem\n");
		close(fd_bpa);
		return 1;
	}
	
	close(fd_bpa);
	return 0;
}


