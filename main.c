#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <string.h>

#include <stdio.h>

#define BYTES 256

//#define P_NUMBERS

// ./RCD 192.168.1.1 index.txt

void sendfromfile(int socketid, const char* path)
{
    printf("SENDFROMFILE\n");
    FILE *file = fopen(path,"r");
    char sent[BYTES];
    memset(sent,(char)0,BYTES);
    if(file != NULL)
    {
        int counter = 0;
        do{
            sent[counter] = getc(file);
            counter++;

        }while(sent[counter-1]!=-1 && counter < BYTES);

        send(socketid, sent, BYTES, 0);
        printf("SENT:\n%s\n---------\n",sent);

        if(fclose(file) != 0) printf("File can't be closed properly.");
    }
    else printf("File can't be opened.");
}

int main(int argc, char *argv[]) {
    printf("Creating the socket.\n");

    char ip[15];
    memset(ip,(char)0,15);

    if (argc == 2)
    {
        printf("IP SET:'%s'\n",argv[1]);
        for (int i = 0; i < 15;i++) ip[i] = argv[1][i];
    }

    char opt = 'q';
    printf("quick(q) or live(l):");scanf("%c",&opt);

    int socketid = socket(AF_INET,SOCK_STREAM,0);
    if (socketid == 0) {
        printf("-1");
        return -1;
    }
    printf("Settings for socket.\n");
    int on_opt = 1;
    if (setsockopt(socketid,SOL_SOCKET,SO_REUSEADDR,&on_opt,sizeof(on_opt)) != 0){
        printf("-2");
        return -2;
    }
    if (setsockopt(socketid,SOL_SOCKET,SO_OOBINLINE,&on_opt,sizeof(on_opt)) != 0){
        printf("-3");
        return -3;
    }
    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(80);
    myaddr.sin_addr.s_addr = inet_addr(ip);
    if (connect(socketid, (struct sockaddr*)&myaddr,sizeof(myaddr)) < 0){
        if (errno == 101) printf("Connection error, network is unreachable.\n");
        return -4;
    }

    char rbuf[BYTES];

    if (opt == 'q')
    {
        sendfromfile(socketid,"thefile.txt");

        int readcount = 0, temp_readcount = BYTES;

        printf("Reading...\n");

        do
        {
            memset(rbuf,(char)0,BYTES);
            temp_readcount = read(socketid,rbuf,BYTES);
            printf("%s",rbuf);
            readcount += temp_readcount;
        }while (temp_readcount == BYTES);

        printf("Readed Bytes:%i",readcount);
    }

    char cmd;


    while (opt == 'l')
    {
        cmd = (char)0;
        printf("enter new input(e) or load from 'thefile.txt'(f)\nCMD:"); scanf("%c",&cmd);
        if (cmd == 'f') sendfromfile(socketid,"thefile.txt");
        else if(cmd == 'e')
        {
            char data[BYTES];
            memset(data,(char)0,BYTES);
            printf("DATA:");scanf("%s",data);
            printf("SENT:%i",send(socketid,data,BYTES,0));
        }

        printf("Reading...");
        int readcount = 0,temp_readcount = BYTES;

        do
        {
            memset(rbuf,(char)0,BYTES);
            temp_readcount = read(socketid,rbuf,BYTES);
            printf("%s",rbuf);
            readcount += temp_readcount;
        }while (temp_readcount == BYTES);

        printf("\nFetched Bytes:%i\n",readcount);


        printf("\n---------------\n\n");

    }



    close(socketid);

}
