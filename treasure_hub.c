#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#define PWD_SIZE 257
#define SIG_C_EVENT_1 SIGUSR1+24 //view hunts sig
#define SIG_C_EVENT_2 SIGUSR1+25 //list treasures sig
#define SIG_C_EVENT_3 SIGUSR1+26 //view treasure sig

unsigned int monitor_on=0;

void view_hunts(int num)
{
	char root[257];
	char channel_path[513];
	
	struct stat file_desc_obj;
	
	printf("miaumiau\n");
}

void list_treasure(int num)
{

	char process_cmd[127]="./";
	char hunt_id[50]="";
	char buffer_headache=0;	

	printf("Input hunt from which to list treasures:");
	scanf("%49s",hunt_id);
	while((buffer_headache=getchar())!='\n') {}

	printf("\n");
	
	strcat(process_cmd,"treasure_manager --list ");
	strcat(process_cmd,hunt_id);

	system(process_cmd);

	usleep(150);

	kill(getppid(),SIGUSR1);
	
}

void view_treasure(int num)
{
	char process_cmd[140]="./";
	char hunt_id[50]="";
	char treasure_id[4]="";
	char buffer_headache=0;

	printf("Input hunt from which to list treasures:");
	scanf("%49s",hunt_id);
	while((buffer_headache=getchar())!='\n') {}
	printf("Input treasure id to be viewed:");
	scanf("%3s",treasure_id);
	while((buffer_headache=getchar())!='\n') {}

	printf("\n");

	strcat(process_cmd,"treasure_manager --view_treasure ");
	strcat(process_cmd,hunt_id);
	strcat(process_cmd," ");
	strcat(process_cmd,treasure_id);

	system(process_cmd);
	
	usleep(150);

	kill(getppid(),SIGUSR1);
}

void stop_monitor(int num)
{
	wait();
	sleep(4);
	printf("Monitor clossing...\n");
	exit(0);
}

void arm_monitor_h2h()
{
	struct sigaction view_hunts_sa,list_treasures_sa,view_treasure_sa,stop_monitor_sa;
	
	view_hunts_sa.sa_handler = view_hunts;
	list_treasures_sa.sa_handler = list_treasure;
	view_treasure_sa.sa_handler = view_treasure;
	stop_monitor_sa.sa_handler = stop_monitor;
	
	sigaction(SIG_C_EVENT_1,&view_hunts_sa, NULL);
	sigaction(SIG_C_EVENT_2,&list_treasures_sa, NULL);
	sigaction(SIG_C_EVENT_3,&view_treasure_sa, NULL);
	sigaction(SIGUSR2,&stop_monitor_sa, NULL);
}

void send_view_hunts(int c_pid)
{
	kill(c_pid,SIG_C_EVENT_1);
}

void send_list_treasures(int c_pid)
{
	kill(c_pid,SIG_C_EVENT_2);
}

void send_view_treasure(int c_pid)
{
	kill(c_pid,SIG_C_EVENT_3);
}

void send_stop_monitor(int c_pid)
{
	int status,zombie_pid=0;

	kill(c_pid,SIGUSR2);
	
	zombie_pid=wait(&status);
	
	if(zombie_pid==-1)
		errno=status|0x0C00;
}

//function that will be used only as a handler for a signal that is sent from to monitor telling main to regain focus from stdin
void return_focus()
{
	return;
}

void arm_hub_h2h()
{
	struct sigaction await_focus_sa;

	await_focus_sa.sa_handler = return_focus;

	sigaction(SIGUSR1,&await_focus_sa,NULL);
}

int main(int argc, char **argv)
{
	printf("Proces barbat parinte:%d\n",getpid());
	char cwd[PWD_SIZE]="";
	char *cmds[]={"start_monitor","list_hunts","list_treasures","view_treasure","stop_monitor","exit"};
	char user_cmd[513]="";
	char using_hub_cmd=0,buffer_headache=0;
	int slave_pid = 1;

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

			scanf("%512s",user_cmd);
			while((buffer_headache=getchar())!='\n'){}
				
			for(unsigned int i=0;i<6;i++)
			{
				if(strstr(user_cmd,cmds[i]))
				{
					using_hub_cmd = 1;
					switch(i){
						case 0: {if(!monitor_on) {slave_pid=fork(); if(!slave_pid) arm_monitor_h2h(); else if(slave_pid!=-1){monitor_on=1; arm_hub_h2h(); }} else printf("Monitor already on...\n"); break; }
						case 1: {if(monitor_on) send_view_hunts(slave_pid); else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 2: {if(monitor_on) {send_list_treasures(slave_pid); pause();} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 3: {if(monitor_on) {send_view_treasure(slave_pid); pause();} else printf("The monitor has to be started before using any hub function...\n"); break;}
						case 4: {if(monitor_on) {monitor_on=0; send_stop_monitor(slave_pid);} else printf("The monitor has to be started before using any hub function...\n"); break; }
						case 5: {if(monitor_on) printf("Monitor process forcefully stopped. Errors may have occured!\n"); monitor_on=0; kill(slave_pid,SIGKILL); printf("gata\n"); exit(0); break; }
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
