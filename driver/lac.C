/*	program to test the firgelli actuator
	R. Treffers Starman Systems, LLC
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firgelli.h"

int main(int argc, char *argv[])
{
	int debug=0;
	int rank=1;
	Firgelli firgelli;

	if(argc==1){
		fprintf(stderr,"usage: lac: [debug] [rank=1] [write=loc[,value]]\n");
		exit(1);
	}
	for(int i=1; i<argc; i++){
		if(strcmp(argv[i],"debug")==0){
			debug=3;
			continue;
		}
		if(strncmp(argv[i],"rank=",5)==0){
			rank=atoi(argv[i]+5);
			continue;
		}
		if(strncmp(argv[i],"write=",6)==0)
			continue;

		fprintf(stderr,"lac: unknown option '%s'\n",argv[i]);
		exit(1);
	}

	if(debug)
		firgelli.SetDebug(debug);
	firgelli.Open(rank);

	if(debug)
		firgelli.Info();

	for(int i=1; i<argc; i++){
		if(strncmp(argv[i],"write=",6)==0){
			int loc,value=0;
			int n=sscanf(argv[i]+6,"%x,%d",&loc,&value);
			if(n<1){
				fprintf(stderr,"lac: two values not found\n");
				exit(1);
			}
			value=firgelli.WriteCode(loc,value);
			printf("value=%d\n",value);
			continue;
		}
	}
}
