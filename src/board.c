#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAX_USER_COUNT 100
#define MAX_ROOM_COUNT 4

typedef struct user{
	int id;
	char name[100];
	int mainAction;
	int cards[7];
	int online;
	int cardCount;
	int coin;
}user;
typedef struct Board{
	unsigned int dealerCard[7]; // 딜러 카드 정보 배열
	user users[3]; // 유저들 정보
	unsigned int budget[4]; // 남은 자금
	unsigned int status[4]; // 유저 상태
	int numOfUser;
	int sync;
}Board;
typedef struct user_info{
	int bet;
	int action;
}user_info;

user users[MAX_USER_COUNT];
int userCount = 0;

void sig_chld(int signo){
	pid_t pid;
	int stat;
	while((pid=waitpid(-1, &stat, WNOHANG))>0){
		printf("child %d terminated.\n", pid);
		userCount--;
	}
		
	return;
}

int main(int argc, char* argv[]){
	printf("ok");
	int sockfd, i;
	struct sockaddr_in addr;
	int rec_sock;
	char* address="172.17.0.18";
	int id = 0;
	int shmid;
	key_t keyval;
	void *shared_memory = (void *)0;
	keyval = 12;
	shmid = shmget(keyval, sizeof(Board), IPC_CREAT | 0666); 
	if (shmid == -1)
	{
		return -1;
	}
	shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1)
    {
        perror("shmat failed : ");
        exit(0);
    }
	
	Board* curBoard = (Board* )shared_memory;
	curBoard->numOfUser=0;
	curBoard->sync=0;
	struct sigaction sa;
	sa.sa_handler = sig_chld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	sigaction(SIGCHLD, &sa, NULL);
	
	
	
	if((sockfd=socket(AF_INET,SOCK_STREAM, 0))<0){
		perror("error");
			exit(1);
	}
	
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr=inet_addr(address);
	addr.sin_family=PF_INET;
	addr.sin_port=htons(atoi(argv[1]));
	if((bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)))<0){
		perror("error");
		exit(1);
	}
	
	if(listen(sockfd, 5)<0){
		perror("error");
		exit(1);
	}
	while(1){
		write(1,"listening new client...\n",24);
		if(curBoard->numOfUser==2)
			break;
		if((rec_sock=accept(sockfd, NULL,NULL))<0){
			if(errno == EINTR)
				continue;
			else{
				perror("accept");
				exit(1);
			}
			
		}else{
			if(fork()==0)
			{
				shmid = shmget(keyval, sizeof(Board), 0);
				if (shmid == -1)
				{
					perror("shmget failed : ");
					exit(0);
				}
				shared_memory = shmat(shmid, (void *)0, 0666|IPC_CREAT);
				if (shared_memory == (void *)-1)
				{
					perror("shmat failed : ");
					exit(0);
				}
				Board* cb = (Board*)shared_memory;
				close(sockfd);
				int curUser = cb->numOfUser;
				struct user newUser={getpid(),"dummy1",-1,0,0};
				// 이름입력
				char buf[100];
				int read_bytes=read(rec_sock,buf, sizeof(char)*100);

				if(read_bytes<=0){
					break;
				}else{
					strcpy(newUser.name, buf);
				}
				// 메인행동입력
				read_bytes=read(rec_sock,buf, sizeof(char)*100);
				if(read_bytes<=0){
					break;
				}else{
					newUser.mainAction=atoi(buf);
					printf("user %d %s : %d\n", newUser.id, newUser.name, newUser.mainAction);
					printf("ok\n");
					cb->users[cb->numOfUser++]=newUser;
					printf("%d\n", cb->numOfUser);
					printf("ok\n");

				}
				if(cb->users[curUser].mainAction==1){
					cb->users[curUser].online=1;
				}else{
					
				}
				printf("selection ok\n");
				// 게임플레이 시작 -> 보드 정보 각각 유저에게 전달
				// while(1){
				// 	if(cb->sync==1){
				// 		write(rec_sock, cb, sizeof(Board));
				// 		break;
				// 	}
				// }
				// //카드 2장 부여
				// srand(time(NULL));
				// int card1 = rand()%13;
				// int card2=rand()%13;
				// cb->users[curUser].cards[cb->users[curUser].cardCount++]=card1;
				// cb->users[curUser].cards[cb->users[curUser].cardCount++]=card2;
				// write(rec_sock, cb, sizeof(Board));
				// 	break;
				// // 유저 힛/스테이 판별
				// user_info* userInfo=(user_info*)malloc(sizeof(user_info));
				// read(rec_sock, userInfo, sizeof(user_info));
				// while(1){
					
					
				// }
				
				while(1){
					
				}
				close(rec_sock);
				cb->numOfUser--;
				exit(0);
			}else{
				close(rec_sock);	
			}
		}
	}
	printf("player waiting\n");
	while(1){
		if(curBoard->users[0].online==1 &&curBoard->users[1].online==1
	  &&curBoard->users[2].online==1){
			printf("playter game ready\n");
			break;
		}
	}
	
	
	
}