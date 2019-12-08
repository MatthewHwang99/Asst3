#include"DUMB.h"

//send a message given a socket descriptor
int sendMessage(int sd, char* msg){
	int valread = 0; int i = 0;
	while((valread = write(sd, &msg[i], 1)) == 1){
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

//receive a message | msg is predefined (buffer)
char* readMessage(int sd, char* msg){
	int valread = 0; int i = 0;
	while((valread = read(sd, &msg[i], 1)) == 1){
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

int checkExistingBoxName(char* name){
	struct messageBox temp = boxList;
	if(temp == NULL){
		
	}
}
