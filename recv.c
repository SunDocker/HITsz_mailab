#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SIZE 65535

char buf[MAX_SIZE + 1];

void recv_mail()
{
    const char *host_name = "pop.163.com";  // TODO: Specify the mail server domain name
    const unsigned short port = 110;        // POP3 server port
    const char *user = "hitszblog@163.com"; // TODO: Specify the user
    const char *pass = "*****************";  // TODO: Specify the password
    char dest_ip[16];
    int s_fd; // socket file descriptor
    struct hostent *host;
    struct in_addr **addr_list;
    int i = 0;
    int r_size;

    // Get IP from domain name
    if ((host = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    addr_list = (struct in_addr **)host->h_addr_list;
    while (addr_list[i] != NULL)
        ++i;
    strcpy(dest_ip, inet_ntoa(*addr_list[i - 1]));

    // TODO: Create a socket,return the file descriptor to s_fd, and establish a TCP connection to the POP3 server
    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memset(sin.sin_zero, 0, 8);
    sin.sin_addr.s_addr = inet_addr(dest_ip);
    if (connect(s_fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)))
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Print welcome message
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Send user and password and print server response
    // input user
    char *pop_user = (char *)malloc(MAX_SIZE + 1);
    int user_len = sprintf(pop_user, "user %s\r\n", user);
    if (send(s_fd, pop_user, strlen(pop_user), 0) == -1)
    {
        perror("user");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    free(pop_user);
    // input password
    char *pop_pass = (char *)malloc(MAX_SIZE + 1);
    int pass_len = sprintf(pop_pass, "pass %s\r\n", pass);
    if (send(s_fd, pop_pass, strlen(pop_pass), 0) == -1)
    {
        perror("pass");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    free(pop_pass);
    // TODO: Send STAT command and print server response
    char *STAT = "stat\r\n";
    if (send(s_fd, STAT, strlen(STAT), 0) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Send LIST command and print server response
    char *LIST = "list\r\n";
    if (send(s_fd, LIST, strlen(LIST), 0) == -1)
    {
        perror("list");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Retrieve the first mail and print its content
    char *RETR1 = "retr 1\r\n";
    if (send(s_fd, RETR1, strlen(RETR1), 0) == -1)
    {
        perror("retr 1");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Send QUIT command and print server response
    char *QUIT = "quit\r\n";
    if (send(s_fd, QUIT, strlen(QUIT), 0) == -1)
    {
        perror("quit");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    putchar('\n');
    close(s_fd);
}

int main(int argc, char *argv[])
{
    recv_mail();
    exit(0);
}
