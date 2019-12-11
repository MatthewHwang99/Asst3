#include"DUMB.h"

//send a message given a socket descriptor
int sendMessage(int sd, char* msg){
  //printf("hi\n");
	int valread = 0; int i = 0;
	while((valread = write(sd, &msg[i], 1)) == 1){
	 //printf("hi\n");
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

int checkExistingBoxName(char* name, struct messageBox* boxList){
	struct messageBox* temp = boxList;
	if(temp == NULL){
		return -1; //name doesn't exist
	}
	
	while(temp!=NULL){
		if(strcmp(temp->boxName, name) == 0){
			return 1;
		}else{
			temp = temp->next;
		}
	}
	
	return -1;
}

//gets message from queue and sends to sd
void pop(int sd, struct messageBox* ptr){
  struct message* temp = ptr->mymsg;
  char* retval = "OK!";
  int len = strlen(temp->msg);
  char* length;
  sprintf(length, "%d", len);
  strcat(retval, length);
  strcat(retval, "!");
  strcat(retval, temp->msg);
  sendMessage(sd, retval);
  ptr->mymsg = temp->next;
  free(temp);
  return;
}

char* errorChecker(char* ret){
  char* returnval;
  if(strcmp(ret, "ER:WHAT?") == 0){
    returnval = "Your message is in some way broken or malformed.\n";
    return returnval;
  }else if(strcmp(ret, "ER:EXIST") == 0){
    returnval = "A box with that name already exists and it can not be created again.\n";
    return returnval;
  }if(strcmp(ret, "ER:NEXST") == 0){
    returnval = "A box with that name does not exist, so it can not be opened.\n";
    return returnval;
  }else if(strcmp(ret, "ER:OPEND") == 0){
    returnval = "A box with that name is currently opened by another user, so you can not open it.\n";
    return returnval;
  }if(strcmp(ret, "ER:EMPTY") == 0){
    returnval = "There are no messages left in this message box.\n";
    return returnval;
  }else if(strcmp(ret, "ER:NOOPN") == 0){
    returnval = "You currently have no message box open.\n";
    return returnval;
  }if(strcmp(ret, "ER:NOTMT") == 0){
    returnval = "The box you are attempting to delete is not empty and still has messages.\n";
    return returnval;
  }else{
    returnval = "OK!";
  }
  return returnval;
}

int nameCheck(char* name){
  int size = strlen(name);
  char first = name[0];
  if(size >= 5 && size <= 25 && ((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z'))){
    //valid name
    return 0;
  }
  //invalid name
  return 1;
}

void help(){
  printf("List of commands:\n");
  printf("--help\n--quit\n--create\n--open\n--next\n--put\n--delete\n--close\n");
  return;
}


char* createCommand(char* command, char* arg0){
	char* res = malloc(strlen(command)+strlen(arg0)+1);
	
	strcpy(res, command);
	strcat(res, arg0);
	
	return res;
}



