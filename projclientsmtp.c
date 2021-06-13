
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#define DEFAULT_HOST "localhost"


void checkError(int status)
{
        if (status < 0)
        {
                printf("socket error: [%s]\n",strerror(errno));
                exit(-1);
        }
}


int socket_send(int sock_id, char* cmd, int cmd_len)
{
        int sent = 0;
        int current;
        while(sent < cmd_len)
        {
                current = send(sock_id, (cmd+sent), cmd_len-sent, 0);
                checkError(current);
                sent = sent + current;
        }
        return sent;
}

int main(int argc,char* argv[])
{
        char *servername = (argc > 1) ? argv[1] : DEFAULT_HOST;

        int sid = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in srv;
        srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = inet_addr ("192.168.43.14");
        srv.sin_port = htons(8025);

        int status = connect(sid,(struct sockaddr*)&srv,sizeof(srv));
        checkError(status);

        fd_set readfds;
        struct timeval timeout;

        while(1)
        {
                char cmd[1000];
                fgets(cmd, 1000, stdin);
                strcat(cmd, "\0");

                FD_ZERO(&readfds);
                FD_SET(sid, &readfds);
                timeout.tv_sec = 0;
                timeout.tv_usec = 100000;

                int active;
                socket_send(sid, cmd, strlen(cmd));

                active = select(10, &readfds, NULL, NULL, &timeout);
                if(active==0)
                {
                        continue;

                } else if(active>0){

                        char* buf= (char*)calloc(sizeof(char), 1000);
                        int buf_len = 1000;

                        read(sid, buf, buf_len);
                        printf("%s", buf);
                        free(buf);

                        if(cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't')
                        {
                                return 0;
                        }
                } else{
                        printf("select() failed");
                        break;
                }
        }
        close(sid);
        return 0;
}
