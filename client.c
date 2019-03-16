#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define PSIZE 256

int main(int argc, char *argv[])
{
	int sockfd, portno, n, i, j, l;
	struct sockaddr_in serv_addr;
	struct hostent *server;
    int comm_num;
    char filename [80];
    char command [200];
    FILE* outdes;

	//setup
    unsigned char buffer[256], sent[PSIZE];
	if (argc < 3) {
	 fprintf(stderr,"usage %s hostname port\n", argv[0]);
	 exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) fprintf(stderr, "ERROR opening socket");
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);
	serv_addr.sin_port = htons(portno);
	if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		fprintf(stderr, "ERROR connecting\n");
	}
    
    //actual communication
	for(i = 0; i < 5; i++){
        //sending command
		bzero(sent, PSIZE);
        printf ("Type command number (13, 14, 15, 250)");
        scanf ("%d", &comm_num);
        switch (comm_num){
            case 13:
                sent[0] = 0;
                sent[1] = 13;
                sent[2] = 0;
                break;
            case 14:
                sent [0] = 0;
                sent [1] = 14;
                sent [2] = 20;
                break;
            case 15:
                sent[0] = 0;
                sent [1] = 15;
                sent [2] = 30;
                break;
            case 16:
                sent [0] = 0;
                sent [1] =16;
                sent [2] = 30;
            case 250:
                sent [0] = 0;
                sent [1] = 250;
                break;
            case 251:
                sent [0] = 0;
                sent [1] = 251;
                break;
            default:
                printf ("unsupported command\n");
                break;
        }
		n = write(sockfd, sent, PSIZE);
		if(n < 0) fprintf(stderr, "ERROR writing to socket");
        
        //testing acknowledgement
        bzero (buffer, PSIZE);
        n = read(sockfd, buffer, PSIZE);
        if(n < 0) fprintf(stderr, "ERROR reading from socket");
        if (strcmp ((char*)buffer, (char*)sent) != 0) {
            printf ("error in communication\n");
            exit (1);
        }
        else {
            printf ("acknowledgment received\n");
        }
        if (buffer [1] == 250 || buffer [1] == 251) goto exitnow;
        
        switch (comm_num){
            case 13:
                n = read(sockfd, buffer, PSIZE); //receiving start transfer command
                if (buffer [1] != 10){
                    printf ("Error: no start transfer command\n");
                    goto exitnow;
                }
                else {
                    for (j = 0; j <= buffer[0]; j++){
                        filename [j] = buffer [10+j];
                    }
                    filename [0] = 'Z';
                    outdes = fopen (filename, "wb");
                    n = write (sockfd, buffer, PSIZE); //sending acknowledgement
                }
                
                while (1){
                    n = read(sockfd, buffer, PSIZE); //receiving data
                    /*if (buffer[1] == 101){
                        fwrite (buffer+10, 1, buffer[0], outdes-buffer[0]);
                    }*/
                    fwrite (buffer+10, 1, buffer[0], outdes);

                    n = write (sockfd, buffer, PSIZE); //sending acknowledgement
                    if (buffer [1] == 11){
                        fclose (outdes);
                        break;
                    }
                }
                sprintf (command, "qlmanage -p %s", filename);
                system (command);
                break;
            case 14:
                n = read(sockfd, buffer, PSIZE);
                if (buffer [1] != 10){
                    printf ("Error: no start transfer command\n");
                    goto exitnow;
                }
                else {
                    for (j = 0; j <= buffer[0]; j++){
                        filename [j] = buffer [10+j];
                    }
                    filename [0] = 'Z';
                    outdes = fopen (filename, "wb");
                    n = write (sockfd, buffer, PSIZE);
                }
                
                while (1){
                    n = read(sockfd, buffer, PSIZE);
                    fwrite (buffer+10, 1, buffer[0], outdes);
                    n = write (sockfd, buffer, PSIZE);
                    if (buffer [1] == 11){
                        fclose (outdes);
                        break;
                    }
                }
                fclose (outdes);
                fprintf (stderr, "audio file can now be played\n");
                sprintf (command, "sox %s -d 2> /dev/null\n", filename);
                system (command);
                break;
            case 15:
                break;
            case 16:
                n = read(sockfd, buffer, PSIZE);
                for (j = 0; j <= buffer[0]; j++){
                    filename [j] = buffer [10+j];
                }
                fprintf (stderr, "motion detected, filename %s. enter next command\n", filename);
                break;
            default:
                goto exitnow;
                break;
        }
        
        //receiving pic
		/*bzero(buffer, 256);
		n = read(sockfd, buffer, 255);
		if(n < 0) fprintf(stderr, "ERROR reading from socket");
		printf("Message received: %s", buffer);*/
	}
exitnow:
	close(sockfd);
	return 0;
}
