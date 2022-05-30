#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]){
	
	int userCount=4;
	char path[1000];
	getcwd(path, 1000);
	strcat(path,"/board");
	char buf[100];
	int fd[2];
		if(pipe(fd)<0){
			printf("error");
				exit(1);
		}
	for(;;){	
			// printf("%d\n", userCount);
			if(fork()==0){
				if(userCount==4){
					userCount=0;
					char str[5];
					sprintf(str, "%d", fd[1]);
					printf("%d",fd[1]);
					if(execl(path, "board", argv[1], str, (char* )NULL)==-1){
						perror("error1");
						exit(1);
					}
			}else{
				close(fd[1]);
				read(fd[0],buf,100);
				userCount=atoi(buf);
				printf("%d", userCount);
			}
		}
	}
		
}