#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define E_NOH 0xFA //error no hunt
#define E_NOT 0xFB //error no treasure
#define E_CRPF 0xA0 //error closing file
#define E_MKDIR 0xC0 //error when making the hunt dir
#define E_FDG 0x40 //group for file descriptor opening errors
#define E_FWG 0x60 //group for file descriptor writing errors
#define E_FRG 0x80 //group for file descriptor reading errors

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
  char log_path[59]="./";
  char log_message[11]="--add ";
  char check_id[4];
  char symlink_log_name[7]="log";
  struct stat file_desc_obj;
  int treasure_fd;
  int log;
  int treasure_idx=1;
  treasure t;
  char buffer_headache;

  strcat(hunt_path,hunt_id);
  strcat(treasure_path,hunt_id);
  strcat(treasure_path,"/");
  strcat(treasure_path,"treasure");
  strcat(log_path,hunt_id);
  strcat(log_path,"/log");
  strcat(log_message,hunt_id);
  strcat(log_message,"\n");
  strcat(symlink_log_name,hunt_id);

  if(stat(hunt_path,&file_desc_obj)==-1 && errno==ENOENT){
      mkdir(hunt_path,0777);
  }

  errno=0;

  printf("Input treasure id:");
  scanf("%3s",t.id);
  while((buffer_headache=getchar())!='\n') {}
  printf("Input treaure owner:");
  scanf("%3s",t.user_name);
  while((buffer_headache=getchar())!='\n') {}
  printf("Input treasure coordinates:");
  scanf("%f %f",&t.coordinates.latitude,&t.coordinates.longitude);
  while((buffer_headache=getchar())!='\n') {}
  printf("Input treasure clue:");
  fgets(t.clue,100,stdin);
  t.clue[strlen(t.clue)-1]='\0';
  printf("Input treasure value:");
  scanf("%u",&t.value);

  if(stat(treasure_path,&file_desc_obj)==-1 && errno==ENOENT)
    {
	treasure_fd=open(treasure_path, O_WRONLY | O_CREAT, 0777);
	log=open(log_path, O_WRONLY | O_CREAT, 0777);
	symlink(log_path,symlink_log_name);
    }
  else 
    {
	treasure_fd=open(treasure_path, O_RDWR | O_APPEND);
	log=open(log_path, O_WRONLY | O_APPEND );

	lseek(treasure_fd,4,SEEK_SET);

	while(read(treasure_fd,check_id,4*sizeof(char)))
	{
		
		if(!strcmp(check_id,t.user_name))
		{
			printf("Treasure wasnt added:same user.You have already deposited a treasure here, go get a life %s\n",t.user_name);
			return;
		}
		lseek(treasure_fd,treasure_idx*120+4,SEEK_SET);
		treasure_idx++;
	}
    }
  lseek(treasure_fd,0,SEEK_SET);

  errno=0;   

  if(errno!=0){
	printf("Error while opening file:%s\n",treasure_path);
	exit(E_FDG | errno);
  }

  write(treasure_fd,t.id,4*sizeof(char)); if(errno != 0){printf("Error while writing to file\n"); exit(E_FWG | errno);}
  write(treasure_fd,t.user_name,4*sizeof(char)); if(errno != 0) {printf("Error while writing to file\n"); exit(E_FWG | errno);}
  write(treasure_fd,&t.coordinates.latitude,sizeof(float)); if(errno != 0) {printf("Error while writing to file\n"); exit(E_FWG | errno);}
  write(treasure_fd,&t.coordinates.longitude,sizeof(float)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  write(treasure_fd,t.clue,100*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  write(treasure_fd,&t.value,sizeof(unsigned int)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  write(log,log_message,11*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}

  close(treasure_fd);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
  close(log);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }

}

void list_hunt(char *hunt_id)
{
  char paths[59]="./";
  char log_path[59]="./";
  char log_message[12]="--list ";
  int treasure_fd;
  int log;
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  strcat(log_path,hunt_id);
  strcat(log_path,"/log");
  strcat(log_message,hunt_id);
  strcat(log_message,"\n");
  
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	exit(E_NOH);
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	exit(E_NOT);
  }

  printf("Hunt name:%s\n",hunt_id);
  printf("Size of treasure:%ld doubloons\n",file_desc_obj.st_size);
  printf("Last time it was discovered:%s\n------------------------------------------------------------------\n",ctime(&file_desc_obj.st_mtime));

  treasure_fd = open(paths,O_RDONLY); 
  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
        exit(E_FDG | errno);
  } 

  log=open(log_path, O_WRONLY | O_APPEND);
  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
        exit(E_FDG | errno);
  } 

  treasure t;
  while(read(treasure_fd,t.id,4*sizeof(char))) 
  {
	if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,t.user_name,4*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.coordinates.latitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.coordinates.longitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,t.clue,100*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.value,sizeof(unsigned int)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}

	printf("Treasure id:%s\nTreasure owner:%s\nTreasure coordinates:(%f,%f)\nTreasure clue:%s\nTreasure value:%u\n\n",t.id,t.user_name,t.coordinates.latitude,t.coordinates.longitude,t.clue,t.value);
  }

  write(log,log_message,12*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
 
  close(treasure_fd);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
  
  close(log);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
}

void view_treasure(char *hunt_id,char *id){

  char paths[59]="./";
  char log_path[59]="./";
  char log_message[25]="--view_treasure ";
  int treasure_fd;
  int log;
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  strcat(log_path,hunt_id);
  strcat(log_path,"/log");
  strcat(log_message,hunt_id);
  strcat(log_message," ");
  strcat(log_message,id);
  strcat(log_message,"\n");
  
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	exit(E_NOH);
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	exit(E_NOT);
  }
  
  treasure_fd = open(paths,O_RDONLY);

  if(errno!=0){
	printf("Error while opening file:%s\n",paths);

	exit(E_FDG | errno);
  }

  log=open(log_path,O_WRONLY | O_APPEND);
  
  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
	exit(E_FDG | errno);
  }

  treasure t;
  while(read(treasure_fd,t.id,4*sizeof(char))) 
  {
	if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,t.user_name,4*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.coordinates.latitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.coordinates.longitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,t.clue,100*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(treasure_fd,&t.value,sizeof(unsigned int)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
        
	if(!strcmp(t.id,id))
  		printf("Treasure id:%s\nTreasure owner:%s\nTreasure coordinates:(%f,%f)\nTreasure clue:%s\nTreasure value:%u\n\n",t.id,t.user_name,t.coordinates.latitude,t.coordinates.longitude,t.clue,t.value);
  }

  write(log,log_message,25*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
 
  close(treasure_fd);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
  
  close(log);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
}

void remove_hunt(char *hunt_id)
{
  char hunt_path[50]="./";
  char treasure_path[59]="./";
  char log_path[59]="./";
  char log_message[22]="--remove_hunt";
  char linked_log_path[7]="log";
  struct stat file_desc_obj;
  int log;

  strcat(hunt_path,hunt_id);
  strcat(treasure_path,hunt_id);
  strcat(treasure_path,"/treasure");
  strcat(log_path,hunt_id);
  strcat(log_path,"/log");
  strcat(log_message," ");
  strcat(log_message,hunt_id);
  strcat(log_message,"\n");
  strcat(linked_log_path,hunt_id);
 
  if(stat(hunt_path,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	exit(E_NOH);
  }

  log = open(log_path,O_WRONLY | O_APPEND);

  if(errno!=0){
	printf("Error while opening file:%s\n",log_path);
	exit(E_FDG | errno);
  }
  
  write(log,log_message,22*sizeof(char));  if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
 
  unlink(linked_log_path);
  unlink(log_path);
  unlink(treasure_path);
  unlinkat(AT_FDCWD,hunt_path,AT_REMOVEDIR);
}
 

void remove_treasure(char *hunt_id,char *treasure_id)
{
  
  char paths[59]="./";
  char log_path[59]="./";
  char log_message[27]="--remove_treasure ";
  int fd;
  int log;
  struct stat file_desc_obj;

  strcat(paths,hunt_id);
  strcat(log_path,hunt_id);
  strcat(log_path,"/log");
  strcat(log_message,hunt_id);
  strcat(log_message," ");
  strcat(log_message,treasure_id);
  strcat(log_message,"\n");

  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
  	printf("There is no such hunt.Hmmm...Must've been the wind\n");
 	exit(E_NOH);
  }
  strcat(paths,"/treasure");
  if(stat(paths,&file_desc_obj)==-1 && errno==ENOENT){
	printf("Oopsie daisy there is no treasure here.....damn.....so empty....\n");
	exit(E_NOT);
  }
  
  treasure *wanted_treasure = (treasure *)malloc(file_desc_obj.st_size);
  if(wanted_treasure == NULL){
	printf("Error when deleting treasure\n");
	exit(-4 | errno);
  }

  fd = open(paths,O_RDONLY);

  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
	exit(E_FDG | errno);
  }  

  int treasure_index=0;

  while(read(fd,wanted_treasure[treasure_index].id,4*sizeof(char)))
  {	
	if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(fd,wanted_treasure[treasure_index].user_name,4*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(fd,&wanted_treasure[treasure_index].coordinates.latitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(fd,&wanted_treasure[treasure_index].coordinates.longitude,sizeof(float)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(fd,wanted_treasure[treasure_index].clue,100*sizeof(char)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	read(fd,&wanted_treasure[treasure_index].value,sizeof(unsigned int)); if(errno!= 0) { printf("Error while reading from file\n"); exit(E_FRG | errno);}
	treasure_index++;
  }
  
  close(fd);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }

  unlink(paths);
  fd = open(paths,O_WRONLY | O_CREAT, 0777);

  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
	exit(E_FDG | errno);
  }

  log=open(log_path,O_WRONLY | O_APPEND);

  if(errno!=0){
	printf("Error while opening file:%s\n",paths);
	exit(E_FDG | errno);
  }
  for(unsigned int i=0;i<treasure_index;i++)
  {
	if(strcmp(wanted_treasure[i].id,treasure_id)!=0){
		write(fd,wanted_treasure[i].id,4*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
		write(fd,wanted_treasure[i].user_name,4*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
		write(fd,&wanted_treasure[i].coordinates.latitude,sizeof(float)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
		write(fd,&wanted_treasure[i].coordinates.longitude,sizeof(float)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
		write(fd,wanted_treasure[i].clue,100*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  		write(fd,&wanted_treasure[i].value,sizeof(unsigned int)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
	}
  }

  write(log,log_message,27*sizeof(char)); if(errno != 0) {printf("Error while riting to file\n"); exit(E_FWG | errno);}
  
  close(fd);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
  
  close(log);
  if(errno!=0)
  {
	printf("Error while closing file\n");
	exit(E_CRPF | errno);
  }
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
