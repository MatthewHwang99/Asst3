#include"DUMB.h"

int sendMessage(int cd, char* msg){
	int valread = 0; int i = 0;
	while((valread = write(cd, &msg[i], 1)) == 1){
		if(valread == -1 || valread == 0){
			//error
			printf("Error while writing message. Socket may have closed.\n");
			exit(0);
		}else if(msg[i]=='\0'){
			break;
		}else{
			i++;
		}
	}
	
	return 1;
}

char* readMessage(int cd, char* msg){
	int valread = 0; int i = 0;
	while((valread = read(cd, &msg[i], 1)) == 1){
		if(valread == -1 || valread == 0){
			//error
			printf("Error while reading message. Socket may have closed.\n");
			exit(0);
		}else if(msg[i]=='\0'){
			break;
		}else{
			i++;
		}
	}
	
	return msg;
}
