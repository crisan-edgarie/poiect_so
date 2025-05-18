#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>	
#include <stdlib.h>
#include <fcntl.h>

#define E_CRPF 0xA0
#define E_FDG 0x40
#define E_FRG 0x80

int main(int argc,char **argv)
{
	char *hunt = argv[1];
	char cwd[512]="";
	char hunt_path[613]="";

	getcwd(cwd,512);

	strcat(hunt_path,cwd);
	strcat(hunt_path,"/");
	strcat(hunt_path,hunt);
	strcat(hunt_path,"/");
	strcat(hunt_path,"treasure");

	int treasure_file = open(hunt_path,O_RDONLY);
	
	if(errno!=0){
		printf("Error while opening file:%s\n",hunt_path);
		exit(E_FDG | errno);
	}

	char treasure_owner[4]="";
	char *users[65535]={0};
	unsigned int user_score[65535]={0};
	unsigned short int known_users=0;

	lseek(treasure_file,4,SEEK_SET);
	while(read(treasure_file,treasure_owner,4*sizeof(char)))
	{
		if(errno!=0){
			printf("Error while reading from file\n"); exit(E_FRG | errno);
		}
	
		for(unsigned int i=0;i<known_users;i++)
		{
			if(!strcmp(users[i],treasure_owner)){
				user_score[i]++;
				lseek(treasure_file,116,SEEK_CUR);
				continue;
			}
		}
		users[known_users]=malloc(4*sizeof(char));
		strcpy(users[known_users],treasure_owner);
		user_score[known_users]=1;
		known_users++;				
		lseek(treasure_file,116,SEEK_CUR);
	}
	
	printf("Scores for hunt:%s\n------------------------\n",hunt);
	for(unsigned int i=0;i<known_users;i++)
	{
		printf("User:%s\tScore:%d\n",users[i],user_score[i]);
		free(users[i]);
	}
	printf("------------------------\n");
	
	return 0;	
}
