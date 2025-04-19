#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define PWD_SIZE 513

void stop_monitor_process(int num)
{
	wait();
	printf("Monitor clossing\n");
	exit(0);
}

void arm_monitor_h2h()
{
	struct sigaction stop_monitor_sa;
	stop_monitor_sa.sa_handler = stop_monitor_process;
	
	sigaction(SIGUSR1,&stop_monitor_sa, NULL);
}

void stop_monitor(int c_pid)
{
	int zombie_pid=0;
	kill(c_pid,SIGUSR1);
	int status;

	zombie_pid=wait(&status);
	
	if(zombie_pid==-1)
		errno=status|0x0C00;
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

	printf("Id pt pfiecare process:%d",slave_pid);
	
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
						case 0: {slave_pid=fork(); if(!slave_pid) arm_monitor_h2h(); break; }
						case 1: break;
						case 2: break;
						case 3: break;
						case 4: {stop_monitor(slave_pid); break; }
						case 5: {printf("gata\n"); exit(0); break; }
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
