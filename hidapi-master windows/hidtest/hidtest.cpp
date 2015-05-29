#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <string.h>

#define MAX_STR 255
#define DATAPNT 50
short getShort(unsigned char* array, int offset)
{
    return (short)(((short)array[offset]) << 8) | array[offset + 1];
}

int main(int argc, char* argv[])
{
	FILE *ofp;
	int res;
	unsigned char buf[65];
	//char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i,k=0,buf_len;
	char message[50],output[5];
	short accels[3],x[DATAPNT],y[DATAPNT],z[DATAPNT];
	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	printf("Enter row: \n");
	scanf("%s",message);
	
	//put message into buf
	while (message[k]){
		buf[2+k]=message[k];
		k++;
	}
	
	buf_len=strlen(message)+2;
	printf("Enter a string \n");
	scanf("%s",message);
	//fgets(message,200,stdin);
	//printf("%d\n",strlen(message));
	buf[buf_len]=strlen(message);
	buf_len=buf_len+1;
	//buf_len=buf_len+strlen(message);
	//printf("%d\n",buf_len);
	
	k=0;
	while(message[k]){
		buf[buf_len+k]=message[k];
		k++;
	}
	
	//printf("%d \n", atoi(message));
	
	res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	int j=0;
	while(j<DATAPNT){
		buf[0] = 0x0;
		buf[1] = 0x81;
		res = hid_write(handle, buf, 65);
		// Read requested state
		res = hid_read(handle, buf, 65);
		if (buf[1]){
			//printf("read data!! %d \n",buf[1]);
			for (i = 0; i < 3; i++){
				accels[i]=getShort(buf, 2*i+2);
				//printf("accels[%d]: %d\n",i,accels[i]);
			}
			x[j]=accels[0];
			y[j]=accels[1];
			z[j]=accels[2];
			j++;
		}
		res = hid_exit();
	}
	// Print out the returned buffer.

	// Finalize the hidapi library
	
	ofp=fopen("accels.txt","w");
	for(i=0; i<DATAPNT; i++){
		fprintf(ofp,"%d  %d  %d \r\n",x[i],y[i],z[i]);
	}
	return 0;
}
