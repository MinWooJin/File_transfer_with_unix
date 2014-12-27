#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define PORTNUM 18213

int main(void) {
	FILE *fd;
	char filename[10], numsize[10], c; //filename과 numsize를 보낼 buf
	struct sockaddr_in sin, cli;
	int sd, ns, clientlen = sizeof(cli);
	int fsize = 0, osize = 0, num = 0;
	int i, status;

	pid_t pid;

	printf("전송할 파일명을 입력하세요.\n");
	scanf("%s", &filename);
		
        if ((fd = fopen(filename, "r")) == NULL) {
                perror("파일을 찾을 수 없습니다.");
                exit(1);
        }

	printf("몇번으로 나누어 전송하겠습니까.\n");
        scanf("%s", &numsize);
	num = atoi(numsize); //char형 num값을 int형으로 바꿔줌

	printf("클라이언트과의 연결을 기다립니다.\n");

        fseek(fd, 0, SEEK_END);
        fsize = ftell(fd);
        osize = fsize / num;
        rewind(fd);
        fclose(fd); //file을 열어서 file의 크기와 한번에 보낼 크기를 계산

	char filecapy[num][256]; //file내용을 저장하고 send할 buf선언
	int n;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //소켓을 만들고 보낼준비를한다.
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("210.117.182.61");

	if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("bind");
		exit(1);
	}

	if (listen(sd, 5)) {
		perror("listen");
		exit(1);
	}
	
	if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
		perror("accept");
		exit(1);
	}

	if (send(ns, filename, sizeof(filename), 0) == -1) {
                perror("file");
                exit(1);
        } //클라이언트에  filename을 보낸다.

	
        if (send(ns, numsize, sizeof(numsize), 0) == -1) {
                perror("num");
                exit(1);
        } //클라이언트에 numsize를 보낸다.
	if (fsize % num != 0) osize++;

	for (i = 0; i < num; i++) { //나눌개수 만큼 for문이 돌아간다.
		memset(&filecapy[i], 0x00, 256); //filecapy라는 buf를 계속 쓰기 때문에 초기화 시켜줘야 한다.
		switch (pid = vfork()) { //vfork로 부모프로세르를 복사하지 않고 fork한다.
			case -1 :
				perror("fork");
				exit(1);
				break;
			case 0 :
				if ((fd = fopen(filename, "r")) == NULL) {
            				perror("open");
                			exit(1);
        			}
				fseek(fd, i*osize, SEEK_SET); //파일을 열어서 한번에 보낼만큼을 읽어서 저장한다.
				n = fread(filecapy[i], sizeof(char), osize, fd); 
				if (send(ns, filecapy[i], sizeof(filecapy[i]), 0) == -1) {
					perror("capysend");
					exit(1);
				} //file내용을 보낸다
				fclose(fd);
				break;
			default :
				while (wait(&status) != pid) //자식프로세스가 종료되기 전에 부모프로세스가 종료되면 안되기 때문에 wait해준다.
					continue;
				break;
		}
	}
	printf("성공적으로 전송 되었습니다.\n"); //성공적으로 파일을 보냈음을 보여준다.
	close(ns); //accept한걸 닫는다.
	close(sd); //socket을 닫는다.

	return 0;
}
