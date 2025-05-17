#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PWD_SIZE 257
#define SIG_C_EVENT_1 SIGUSR1+24 //view hunts sig
#define SIG_C_EVENT_2 SIGUSR1+25 //list treasures sig
#define SIG_C_EVENT_3 SIGUSR1+26 //view treasure sig
#define SIG_C_EVENT_4 SIGUSR1+27 //do score hunt

unsigned int monitor_on=0;

void view_hunts(int num)
{
	char channel_path[PWD_SIZE+512];
	char cwd[PWD_SIZE]="";

	unsigned int is_hunts=0;

	struct stat file_desc_obj;

	getcwd(cwd,PWD_SIZE);

	DIR *current_dir=opendir(cwd);
	struct dirent *cascade_dir=NULL;

	if(current_dir==NULL){
		printf("Error while opening current dir for parsing...");
		perror("opendir");
		return;
	}
	printf("------------------------------------------------\n");

	while((cascade_dir=readdir(current_dir)))
	{
		if(cascade_dir->d_type == DT_DIR)
		{
			strcpy(channel_path,cwd);
			strcat(channel_path,"/");
			strcat(channel_path,cascade_dir->d_name);
			strcat(channel_path,"/");
			strcat(channel_path,"treasure");
 
			if(stat(channel_path,&file_desc_obj)!=-1)
			{
				is_hunts=1;
				printf("Hunt %s with %ld treasures\n",cascade_dir->d_name,file_desc_obj.st_size/120);
			}
		}
	}

	if(!is_hunts)
		printf("No hunts...No money...\n");

	printf("------------------------------------------------\n");
	
	if(closedir(current_dir)==-1)
	{	
		printf("There was an error whilst closing the dir file...\n");
		perror("closedir");
	}
}

void list_treasure(int num)
{

	char process_cmd[127]="./";
	char hunt_id[50]="";

	FILE *fin=fopen("treasure_inputs.in","rb");

	fscanf(fin,"%s",&hunt_id);
	//while((buffer_headache=getchar())!='\n') {}

	printf("\n");
	
	strcat(process_cmd,"treasure_manager --list ");
	strcat(process_cmd,hunt_id);

	system(process_cmd);

	usleep(150);
	fclose(fin);

	kill(getppid(),SIGUSR1);
	
}

void view_treasure(int num)
{
	char process_cmd[140]="./";
	char hunt_id[50]="";
	char treasure_id[4]="";
	
	FILE *fin=fopen("treasure_inputs.in","rb");
	
	fscanf(fin,"%s",hunt_id);
	printf("%s\n",hunt_id);
	fscanf(fin,"%s",treasure_id);

	printf("\n");

	strcat(process_cmd,"treasure_manager --view_treasure ");
	strcat(process_cmd,hunt_id);
	strcat(process_cmd," ");
	strcat(process_cmd,treasure_id);

	system(process_cmd);
	
	usleep(150);

	fclose(fin);

	kill(getppid(),SIGUSR1);
}

void score_hunts(int num)
{
	char channel_path[PWD_SIZE+512];
	char cwd[PWD_SIZE]="";
	int status=0,zombie_pid=0;
	int c_pid;

	struct stat file_desc_obj;

	getcwd(cwd,PWD_SIZE);

	DIR *current_dir=opendir(cwd);
	struct dirent *cascade_dir=NULL;

	if(current_dir==NULL){
		printf("Error while opening current dir for parsing...");
		perror("opendir");
		return;
	}

	while((cascade_dir=readdir(current_dir)))
	{
		if(cascade_dir->d_type == DT_DIR)
		{
			strcpy(channel_path,cwd);
			strcat(channel_path,"/");
			strcat(channel_path,cascade_dir->d_name);
			strcat(channel_path,"/");
			strcat(channel_path,"treasure");
 
			if(stat(channel_path,&file_desc_obj)!=-1)
			{
				c_pid=fork();
				if(c_pid==-1){
					perror("frok");
					exit(errno);
				}
				else if(!c_pid)
				{
					char *args[]={"./score_hunt",cascade_dir->d_name,NULL};
					execvp(args[0],args);
					exit(0);
				}		
				else
				{
				zombie_pid=waitpid(c_pid,&status,0); 
				if(zombie_pid==-1) 
					errno=status|0x0C00;	
				}
			}
		}
	}

	if(closedir(current_dir)==-1)
	{	
		printf("There was an error whilst closing the dir file...\n");
		perror("closedir");
	}

	kill(getppid(),SIGUSR1);
}

void stop_monitor(int num)
{
	sleep(4);
	printf("Monitor clossing...\n");
	kill(getppid(),SIGUSR2);
	exit(0);
}

void arm_monitor_h2h()
{
	struct sigaction view_hunts_sa,list_treasures_sa,view_treasure_sa,score_hunt_sa,stop_monitor_sa;
	
	view_hunts_sa.sa_handler = view_hunts;
	list_treasures_sa.sa_handler = list_treasure;
	view_treasure_sa.sa_handler = view_treasure;
	score_hunt_sa.sa_handler = score_hunts;
	stop_monitor_sa.sa_handler = stop_monitor;
	
	sigaction(SIG_C_EVENT_1,&view_hunts_sa, NULL);
	sigaction(SIG_C_EVENT_2,&list_treasures_sa, NULL);
	sigaction(SIG_C_EVENT_3,&view_treasure_sa, NULL);
	sigaction(SIG_C_EVENT_4,&score_hunt_sa,NULL);
	sigaction(SIGUSR2,&stop_monitor_sa, NULL);
}

void send_view_hunts(int c_pid)
{
	kill(c_pid,SIG_C_EVENT_1);
}

void send_list_treasures(int c_pid)
{
	char hunt[50]="";
	char buffer_headache=0;

	printf("Input hunt names:");
	scanf("%50s",&hunt);
	while((buffer_headache=getchar())!='\n') {}

	FILE *fout=fopen("treasure_inputs.in","w");

	fprintf(fout,"%s",hunt);

	fclose(fout);
	
	kill(c_pid,SIG_C_EVENT_2);
}

void send_view_treasure(int c_pid)
{
	char hunt[50]="";
	char id[4]="";
	char buffer_headache=0;

	FILE *fout=fopen("treasure_inputs.in","w");

	printf("Input hunt from which to list treasures:");
	scanf("%49s",hunt);
	while((buffer_headache=getchar())!='\n') {}

	printf("Input treasure id to be viewed:");
	scanf("%3s",id);
	while((buffer_headache=getchar())!='\n') {}

	fprintf(fout,"%s\0%s",hunt,id);

	fclose(fout);

	kill(c_pid,SIG_C_EVENT_3);
}

void send_score_hunts(int c_pid)
{
	kill(c_pid,SIG_C_EVENT_4);
}

void send_stop_monitor(int c_pid)
{
	
	kill(c_pid,SIGUSR2);	

	int pre_cpid=fork();

	if(pre_cpid==0){
		int status=0,zombie_pid=0;
		zombie_pid=waitpid(c_pid,&status,0); 
		if(zombie_pid==-1) 
			errno=status|0x0C00;
	}
}

//function that will be used only as a handler for a signal that is sent from to monitor telling main to regain focus from stdin
void return_focus(int num)
{
	return;
}

void return_cmd_input(int num)
{
	monitor_on=0;
}


void arm_hub_h2h()
{
	struct sigaction await_focus_sa,await_normal_input_sa;

	await_focus_sa.sa_handler = return_focus;
	await_normal_input_sa.sa_handler = return_cmd_input;

	sigaction(SIGUSR1,&await_focus_sa,NULL);
	sigaction(SIGUSR2,&await_normal_input_sa,NULL);
}

int main(int argc, char **argv)
{
	char cwd[PWD_SIZE]="";
	char *cmds[]={"start_monitor","list_hunts","list_treasures","view_treasure","score_hunts","stop_monitor","exit"};
	char user_cmd[513]="";
	char using_hub_cmd=0;
	int slave_pid = 1;

	printf("Starting hub subroutine. Listing commands bellow. If a command that is not listed will be sent, it will be proccesed by the os shell. Usage on windows will result in errors:\n1.start_monitor - start the monitor subroutine\n2.list_hunts - lists all the hunts in the current working directory\n3.list_treasures - same as view hunt in treasure_manager\n4.view_treasure - same as view_treasure in treasure_manager\n5.score_hunts - shows the scores for all hunts int he current waorking directory\n6.stop_monitor - stops the monito subroutine if one was created\n7.exit - forcefully stops any other subprocces and exits the hub process\n\n");

	while(1)
	{
	
	switch(slave_pid){
		
		case -1:
		{
			perror("Error while creating slave process:");
			exit(-1);
			break;		
		}

		case 0:
		{

			break;
		}

		default:
		{	
			using_hub_cmd=0;
			//recreating unix "gui" basically not restricting any call to any unix functionality whilst treasure_hub runs
			getcwd(cwd,PWD_SIZE);
			printf("%s#",cwd);

			fgets(user_cmd,513,stdin);

			if(monitor_on==2)
			{
				int status=0, zombie_pid=0;

				printf("Error:Input cannot be processed whilst monitor is closing...\n");
				pause();

				zombie_pid=wait(&status);
 
				if(zombie_pid==-1) 
					errno=status|0x0C00;
				monitor_on=0;
				continue;
			}
				
			for(unsigned int i=0;i<7;i++)
			{
				if(strstr(user_cmd,cmds[i]))
				{
					using_hub_cmd = 1;
					switch(i){
						case 0: {if(!monitor_on) {slave_pid=fork(); if(!slave_pid) arm_monitor_h2h(); else if(slave_pid!=-1){monitor_on=1; arm_hub_h2h(); }} else printf("Monitor already on...\n"); break; }
						case 1: {if(monitor_on) {send_view_hunts(slave_pid); sleep(1);} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 2: {if(monitor_on) {send_list_treasures(slave_pid); pause();} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 3: {if(monitor_on) {send_view_treasure(slave_pid); pause();} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 4: {if(monitor_on) {send_score_hunts(slave_pid); pause();} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 5: {if(monitor_on) {monitor_on=2; send_stop_monitor(slave_pid);} else printf("The monitor has to be started before using any hub function...\n"); break; }
						case 6: {if(monitor_on) printf("Monitor process forcefully stopped. Errors may have occured!\n"); monitor_on=0; kill(slave_pid,SIGKILL); exit(0); break; }
					}
					break;
				}
			}
			if(!using_hub_cmd)
				system((const char *)user_cmd);				
		}
	}
	}

	return 0;
}
