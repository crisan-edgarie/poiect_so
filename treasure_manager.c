#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct{
  float latitude,longitude;
}coord;

typedef struct{
  char id[4];
  char user_name[4];
  coord coordinates;
  char clue[100];
  unsigned int value;
}treasure;

void add_treasure(char *hunt_id)
{
  char hunt_path[50]="./";
  char treasure_path[59]="./";
  struct stat file_desc_obj;
  int treasure_fd;
  treasure t;
  char fuck_c;

  strcat(hunt_path,hunt_id);
  strcat(treasure_path,hunt_id);
  strcat(treasure_path,"/");
  strcat(treasure_path,"treasure");

  if(stat(hunt_path,&file_desc_obj)==-1 && errno==ENOENT){
      mkdir(hunt_path,0777);
  }

  printf("Input treasure id:");
  scanf("%3s",t.id);
  while((fuck_c=getchar())!='\n') {}
  printf("Input treaure owner:");
  scanf("%3s",t.user_name);
  while((fuck_c=getchar())!='\n') {}
  printf("Input treasure coordinates:");
  scanf("%f %f",&t.coordinates.latitude,&t.coordinates.longitude);
  while((fuck_c=getchar())!='\n') {}
  printf("Input treasure clue:");
  fgets(t.clue,100,stdin);
  t.clue[strlen(t.clue)-1]='\0';
  printf("Input treasure value:");
  scanf("%u",&t.value);

  if(stat(treasure_path,&file_desc_obj)==-1 && errno==ENOENT)
    treasure_fd=open(treasure_path, O_WRONLY | O_CREAT, 0777);
  else 
    treasure_fd=open(treasure_path, O_WRONLY | O_APPEND);

  write(treasure_fd,t.id,4*sizeof(char));
  write(treasure_fd,t.user_name,4*sizeof(char));
  write(treasure_fd,&t.coordinates.latitude,sizeof(float));
  write(treasure_fd,&t.coordinates.longitude,sizeof(float));
  write(treasure_fd,t.clue,100*sizeof(char));
  write(treasure_fd,&t.value,sizeof(unsigned int));
	
  close(treasure_fd);

}

void list_hunt(char *hunt_id)
{
  char paths[59]="./";
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	return;
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	return;
  }

  printf("Hunt name:%s\n",hunt_id);
  printf("Size of treasure:%ld doubloons\n",file_desc_obj.st_size);
  printf("Last time it was discovered:%ld(im not gonna format this :p)\n------------------------------------------------------------------\n",file_desc_obj.st_mtim.tv_sec);

  int treasure_fd = open(paths,O_RDONLY);  

  treasure t;
  while(read(treasure_fd,t.id,4*sizeof(char))) 
  {
	read(treasure_fd,t.user_name,4*sizeof(char));
	read(treasure_fd,&t.coordinates.latitude,sizeof(float));
	read(treasure_fd,&t.coordinates.longitude,sizeof(float));
	read(treasure_fd,t.clue,100*sizeof(char));
	read(treasure_fd,&t.value,sizeof(unsigned int));

	printf("Treasure id:%s\nTreasure owner:%s\nTreasure coordinates:(%f,%f)\nTreasure clue:%s\nTreasure value:%u\n\n",t.id,t.user_name,t.coordinates.latitude,t.coordinates.longitude,t.clue,t.value);
  }
 
  close(treasure_fd);
}

void view_treasure(char *hunt_id,char *id){

  char paths[59]="./";
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	return;
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	return;
  }
  
  int treasure_fd = open(paths,O_RDONLY);  

  treasure t;
  while(read(treasure_fd,t.id,4*sizeof(char))) 
  {
	read(treasure_fd,t.user_name,4*sizeof(char));
	read(treasure_fd,&t.coordinates.latitude,sizeof(float));
	read(treasure_fd,&t.coordinates.longitude,sizeof(float));
	read(treasure_fd,t.clue,100*sizeof(char));
	read(treasure_fd,&t.value,sizeof(unsigned int));
        
	if(!strcmp(t.id,id))
  		printf("Treasure id:%s\nTreasure owner:%s\nTreasure coordinates:(%f,%f)\nTreasure clue:%s\nTreasure value:%u\n\n",t.id,t.user_name,t.coordinates.latitude,t.coordinates.longitude,t.clue,t.value);
  }
 
  close(treasure_fd);
}

void remove_hunt(char *hunt_id)
{
  char hunt_path[50]="./";
  char treasure_path[59]="./";
  struct stat file_desc_obj;

  strcat(hunt_path,hunt_id);
  strcat(treasure_path,hunt_id);
  strcat(treasure_path,"/treasure");
 
  if(stat(hunt_path,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	return;
  }

  unlink(treasure_path);
  unlinkat(AT_FDCWD,hunt_path,AT_REMOVEDIR);
  
}

void remove_treasure(char *hunt_id,char *treasure_id)
{
  
  char paths[59]="./";
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	return;
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	return;
  }
  
  treasure *wanted_treasure = (treasure *)malloc(file_desc_obj.st_size);

  int fd = open(paths,O_RDONLY);
  int treasure_index=0;

  while(read(fd,wanted_treasure[treasure_index].id,4*sizeof(char)))
  {
	read(fd,wanted_treasure[treasure_index].user_name,4*sizeof(char));
	read(fd,&wanted_treasure[treasure_index].coordinates.latitude,sizeof(float));
	read(fd,&wanted_treasure[treasure_index].coordinates.longitude,sizeof(float));
	read(fd,wanted_treasure[treasure_index].clue,100*sizeof(char));
	read(fd,&wanted_treasure[treasure_index].value,sizeof(unsigned int));
	treasure_index++;
  }
  
  close(fd);
  unlink(paths);
  fd = open(paths,O_WRONLY | O_CREAT, 0777);

  for(unsigned int i=0;i<treasure_index;i++)
  {
	if(strcmp(wanted_treasure[i].id,treasure_id)!=0){
		write(fd,wanted_treasure[i].id,4*sizeof(char));
		write(fd,wanted_treasure[i].user_name,4*sizeof(char));
		write(fd,&wanted_treasure[i].coordinates.latitude,sizeof(float));
		write(fd,&wanted_treasure[i].coordinates.longitude,sizeof(float));
		write(fd,wanted_treasure[i].clue,100*sizeof(char));
  		write(fd,&wanted_treasure[i].value,sizeof(unsigned int));
	}
  }
  
  close(fd);
  free(wanted_treasure);
}

void help()
{
  printf("Usage:./treasure_manager --<op> <*args>\n\n");
  printf("./treasure_manager --add <hunt_id>:{Adds one treasure to the selected hunt.If there is no such hunt one will be created}\n");
  printf("./treasure_manager --list <hunt_id>:{Prints the id, size and last time of modification of the selected hunt, then prints all the treasure inside. If there is no such hunt or there is no treasure inside, an appropiate message will be displayed}\n");
  printf("./treasure_manager --view_treasure <hunt_id> <treasure_id>:{Prints all of the treasures with the selected id from the selected hunt.  If there is no such hunt or there is no treasure inside, an appropiate message will be displayed}\n");
  printf("./treasure_manager --remove_hunt <hunt_id>:{Removes the selected hunt if it exists}\n");
  printf("./treasure_manager --remove_treasure <hunt_id> <treasure_id>:{Removes the selected treasure fromt the selected hunt.  If there is no such hunt or there is no treasure inside, an appropiate message will be displayed}\n}");
  printf("----------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

int main(int argc,char **argv)
{
  if(argc == 1){
	printf("Wrong usage of script.Usage:treasure_manager --<op> <*args>\nFor a more specific description of all operations use:treasure_manager --help\n" );
	exit(-5);
  }
  if(!strcmp(argv[1],"--add")){ add_treasure(argv[2]); return 0;}
  if(!strcmp(argv[1],"--list")){ list_hunt(argv[2]); return 0;}
  if(!strcmp(argv[1],"--view_treasure")){if(argc<4) printf("Insufficient number of arguments...\n"); else view_treasure(argv[2],argv[3]); return 0;}
  if(!strcmp(argv[1],"--remove_hunt")){ remove_hunt(argv[2]); return 0;} 
  if(!strcmp(argv[1],"--remove_treasure")){if(argc<4) printf("Insufficient number of arguments...\n"); else remove_treasure(argv[2],argv[3]); return 0;}
  if(!strcmp(argv[1],"--help")) {help(); return 0;}
 
  printf("No such command: %s\n",argv[1]);
  return 0;
}
