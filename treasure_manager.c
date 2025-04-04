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
  char clue[101];
  unsigned int value;
}treasure;

void add_treasure(char *hunt_id)
{
  char hunt_path[6]="./";
  char treasure_path[15]="./";
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
  fgets(t.clue,101,stdin);
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
  char paths[15]="./";
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

  char paths[15]="./";
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

int main(int argc,char **argv)
{
  if(argc == 1){
	printf("Wrong usage of script.Usage:treasure_manager --<op> <*args>\nFor a more specific description of all operations use:treasure_manager --help\n" );
	exit(-5);
  }
  char *op = argv[1];
  switch(op[2]){
	case 'a':{ add_treasure(argv[2]); break;}
	case 'l':{ list_hunt(argv[2]); break;}
	case 'v':{ view_treasure(argv[2],argv[3]); break;}
 }
  return 0;
}
