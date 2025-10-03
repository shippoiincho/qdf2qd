#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const char *templatefile = "Blank.qd";
uint8_t buffer[0x10000];
FILE *fout,*fin,*ftemplate;

#define HEADER 0x3a90
//#define HEADER 0x4591

void main(int argc,char **argv){

    // File Open

    int bytecount=0;
    char lastbit=0;
    char clockbit=0;
    unsigned char indata,outdata;

    ftemplate=fopen(templatefile,"rb");

    if(argc!=3) {return;}

    fin=fopen(argv[1],"rb");
    fout=fopen(argv[2],"wb");

    // COPY HEADER bytes from Blank.qd

    fread(buffer,1,HEADER,ftemplate);
    fwrite(buffer,1,HEADER,fout);

    // SKIP QDF Header

    fseek(fin,16,SEEK_SET);
    lastbit=0;

    // MFM ENCODE from qdf file

    while(fread(buffer,1,1,fin)>0) {
        indata=buffer[0];

        for(int i=0;i<8;i++) {

            if(indata&1) {  // 1
                clockbit=0;
                lastbit=1;
            } else {    // 0
                if(lastbit==0) {
                    clockbit=1;
                } else {
                    clockbit=0;
                }
                lastbit=0;
            }
            
            if(clockbit) outdata|=0x40;
            if(lastbit) outdata|=0x80;

            if(i==3) {
                buffer[0]=outdata;
//                printf("[%02x]",outdata);
                fwrite(buffer,1,1,fout);
                bytecount++;
            } else if(i==7) {
                buffer[0]=outdata;
//                printf("[%02x]",outdata);
                fwrite(buffer,1,1,fout);
                bytecount++;
            }

            indata>>=1;
            outdata>>=2;

        }

    }

    // DUMMY

    buffer[0]=0x11;
    for(int i=0;i<(0x32000-HEADER-bytecount);i++) {
        fwrite(buffer,1,1,fout);
    }

    fclose(fin);
    fclose(fout);
    fclose(ftemplate);

}