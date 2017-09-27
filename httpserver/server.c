/**
 * ALL OF THIS CODE WAS WRITTEN BY SEAN GALLAGHER
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <string.h>

pthread_mutex_t countLock;

struct entry{
	char filename[100];
	int count;
};

struct threadArgs{
		int socketfd;
		struct sockaddr_in addr;
		socklen_t length;
		struct entry *dict;
		int num;
		//pthread_mutex_t *lock;
};

char *getMIME(char *filename){
	int i, lineNum, start, j;
	char ext[10], temp[100];
	char *type;
	char *mimeFile = "/etc/mime.types";
	FILE *fp = fopen(mimeFile, "r");

	type = malloc(100);

	if(fp == NULL){
		printf("Wrong file\n");
	}
	start = 0;
	for(i = 0; i < strlen(filename); i++){
		if(filename[i] == '.'){
			start = i;
		}
	}
	if(start == 0){
		strcpy(type, "application/octet-stream");
	}else{
		start++;
		j = 0;
		for(i = start; i < strlen(filename); i++){
			ext[j] = filename[i];
			j++;
		}
		ext[j] = '\0';

		char *type1, *ext1;
		int done = 0;
		while((fgets(temp, sizeof(temp), fp)) != NULL){
			type1 = strtok(temp,"\t");
			strcpy(type,type1);
			type1 = strtok(NULL,"\t");
			ext1 = strtok(type1," ");
			while(ext1 != NULL){
				if(ext1[strlen(ext1)-1] == '\n'){
					ext1[strlen(ext1)-1] = '\0';
				}
				if(strcmp(ext1,ext) == 0){
					done = 1;
					break;
				}
				ext1 = strtok(NULL," ");
			}
			if(done){
				break;
			}
		}
	}
	//printf("%s\n", type);
	return type;
}

struct entry *createDictionary(int *numEntries){
	int num, i;
	struct entry *dictionary;
	struct dirent *dp;
	DIR *dfp;

	char *dir = "www";

	if((dfp = opendir(dir)) == NULL){
		perror("Directory Open Error\n");
		exit(1);
	}

	num = 0;
	while((dp = readdir(dfp)) != NULL){
		num++;
	}
	*numEntries = num;
	closedir(dfp);

	dictionary = malloc(num * sizeof(struct entry));
	
	if((dfp = opendir(dir)) == NULL){
		perror("Directory Open Error\n");
		exit(1);
	}

	i = 0;
	while((dp = readdir(dfp)) != NULL){
		strcpy((dictionary[i].filename), (dp->d_name));
		dictionary[i].count = 0; 
		i++;
	}	
	closedir(dfp);
	
	return dictionary;
}

char *getRFCDate(struct tm tm){
	char *day, *month;
	char mday[32],year[32],time[32];

	char *date = malloc(100);	

	char *days[] = {" Sun, "," Mon, "," Tue, "," Wed, "," Thu, "," Fri, "," Sat, "};
	char *months[] = {" Jan "," Feb "," Mar "," Apr "," May "," Jun "," Jul "," Aug "," Sep "," Oct "," Nov "," Dec "};
	month = months[tm.tm_mon];
	day = days[tm.tm_wday];
	
	strcpy(date,day);
	sprintf(mday,"%d",tm.tm_mday);
	strcat(date,mday);
	strcat(date, month);
	sprintf(year, "%d ",tm.tm_year + 1900);
	strcat(date, year);
	sprintf(time, "%d:%d:%d GMT\n",tm.tm_hour,tm.tm_min,tm.tm_sec);
	strcat(date, time);

	return date;
}

void *threadStart(void *arg){
	int i, start, temp, size, index;
	char buff[512], request[128],filename[128], header[256];
	void *reply;
		
	struct stat fileStat;
	FILE *fp;
	
	struct threadArgs *args = (struct threadArgs*) arg;
	
	int new_sock = args->socketfd;
	//struct entry *dict = args->dict;	
	//pthread_mutex_t *countLock = args->lock;

	temp = recv(new_sock, &buff, sizeof(buff), 0);
	if(temp == -1){
		printf("%s\n", strerror(errno));
	}	
	//printf("%s\n", buff);
	sprintf(buff,"\0");
		
	for(i = 0; i < sizeof(buff); i++){
		if(buff[i] == '/'){
			start = i;
			break;
		}
	}
	int j = 0;
	start++;
	while(buff[start] != ' '){
		request[j] = buff[start];
		j++;
		start++;
	}
	request[j] = '\0';

	int numEntries = args->num; 
	for(i = 0; i < numEntries; i++){
		if(strcmp(args->dict[i].filename, request) == 0){
			break;
		}
	}
	index = i;

	strcpy(filename, "www/");
	strcat(filename, request);	

	fp = fopen(filename, "rb");

	char *empty = "\n";
	char *failReply = "HTTP/1.1 404 Not Found";

	if(fp == NULL){
		write(new_sock, failReply, strlen(failReply));
		write(new_sock, empty, strlen(empty));
		char *reply2 = "What do you think this is, please request a resource we have.";
		write(new_sock, reply2, strlen(reply2));
	}else{
		time_t t = time(NULL);
		struct tm currTime = *gmtime(&t);	
		
		fclose(fp);

		stat(filename, &fileStat);
		strcpy(header, "HTTP/1.1 200 OK\n");
		strcat(header, "Date:");
		
		char *date = getRFCDate(currTime);
		strcat(header, date);
		strcat(header, "Server: cever\n");
		strcat(header, "Last-Modified:");

		time_t *modTime = &fileStat.st_mtime;

		struct tm tm;
		tm = *gmtime(modTime);
		date = getRFCDate(tm);
		strcat(header, date);
		size = fileStat.st_size;
	
		char ssize[16],*type;
		sprintf(ssize,"%d\n",size);
		strcat(header, "Content-Length: ");
		strcat(header, ssize);
		strcat(header, "Content-Type: ");	
		type = getMIME(request);
		strcat(header,type); 
		strcat(header, "\n");
	
		write(new_sock,header,strlen(header));	
		write(new_sock, empty,strlen(empty));
	
		fp = fopen(filename, "rb");

		pthread_mutex_lock(&countLock);
		args->dict[index].count++;
		pthread_mutex_unlock(&countLock);

		int count = 0;
		int written = 0;

		reply = malloc(32);
	
		while(written < size){
			fread(reply, sizeof(reply), 1,fp);
			count = send(new_sock,reply, sizeof(reply), 0);
			written += count;
		}
	}

	char *ip;
	struct sockaddr_in clientAddr = args->addr;
	struct in_addr inAddr= clientAddr.sin_addr;
	ip = inet_ntoa(inAddr);
	int port = clientAddr.sin_port;
	socklen_t addrLen = args->length;
	printf("/%s|%s|%d|%d\n",request,ip,port,args->dict[index].count);
	close(new_sock);
}

int server_init(){
	int sockfd, new_socket, temp;
	struct sockaddr_in address;
	struct entry *dict;
	//pthread_mutex_t countLock;
	socklen_t addrLen = sizeof(address);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_port = htons(0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	int num;
	dict = createDictionary(&num);

	if(bind(sockfd,(struct sockaddr*) &address, sizeof(address)) < 0){
		perror("Bind Failed");
		exit(1);
	}

	if(listen(sockfd, 1) < 0){	
		perror("Listen Failure");
		exit(1);
	}

	temp = getsockname(sockfd,(struct sockaddr*) &address, &addrLen);
	
	char name[100];
	if(temp == 0){
		gethostname(name, 100);
		printf("Host Name:%s\n",name);
		printf("Port Number:%d\n:", ntohs(address.sin_port));
	}	

	struct threadArgs args;
	pthread_t threads[1000];
	int threadCnt = 0;
	
	while((new_socket = accept(sockfd,(struct sockaddr*) &address, &addrLen)) > 0){
		temp = getsockname(new_socket,(struct sockaddr*) &address, &addrLen);	

		args.socketfd = new_socket;
		args.addr = address;
		args.length = addrLen;
		args.dict = dict;
		args.num = num;
		//args.lock = &countLock;
		pthread_create(&threads[threadCnt],NULL,&threadStart,&args);
		//pthread_join(threads[threadCnt],NULL);
		threadCnt++;
	}

	close(sockfd);
}

int main(){
	server_init();
	return 0;
}
