#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define PORTNUM 18213

int main(void) {
	FILE *fd;
	int sd;
	int fn, i = 0, num = 0;
	char buf[256], filename[10], numsize[10];
	struct sockaddr_in sin;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //소켓을 만들고 통신할 준비를 한다.
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("210.117.182.61");

	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("connect");
		exit(1);
	}
	
	if (recv(sd, filename, sizeof(filename), 0) == -1) { //server에서 보낸 filename을 받아서 저장한다.
                perror("numsize");
                exit(1);
        }
	
	if (recv(sd, numsize, sizeof(numsize), 0) == -1) { //server에서 보낸 numsize를 받아서 저장한다.
		perror("numsize");
		exit(1);
	}

	fn = open(filename, O_CREAT | O_TRUNC, 0644); //파일을 생성하고 이미 파일이 있다면 덮어쓴다.
	close(fn);
	num = atoi(numsize); //server에서 받은 numsize를 int형으로 변환한다.
	char fbuf[256];
	
	for (i = 0; i < num; i++) { //server에서 보낸 나눌개수만큼 for문으로 반복한다.
		memset(&fbuf, 0x00, 256); //fbuf는 계속해서 쓰이기 때문에 리셋해준다.
		if (recv(sd, fbuf, sizeof(fbuf), 0) == -1) { //file내용을 받아서 fbuf에 저장한다.
			perror("fbuf");
			exit(1);
		}
		if ((fd = fopen(filename, "a")) == NULL) { //파일을 추가용으로 연다 (맨 마지막에 추가되기위해서)
			perror("fopen");
			exit(1);
		}
		fprintf(fd, "%s", fbuf); //fprintf로 fbuf에 저장되있는 내용을 파일에 써준다.
		fclose(fd);
	}

	printf("%s 파일 전송이 완료되었습니다.\n", filename);
	close(sd); //통신한 소켓을 닫는다.
	return 0;
}
