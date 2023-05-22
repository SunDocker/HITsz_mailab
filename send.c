#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include "base64_utils.h"

#define MAX_SIZE 4095

char buf[MAX_SIZE + 1];

// receiver: mail address of the recipient
// subject: mail subject
// msg: content of mail body or path to the file containing mail body
// att_path: path to the attachment
void send_mail(const char *receiver, const char *subject, const char *msg, const char *att_path)
{
    const char *end_msg = "\r\n.\r\n";
    const char *host_name = "smtp.163.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 25;         // SMTP server port
    const char *user = "hitszblog@163.com"; // TODO: Specify the user
    const char *pass = "EARINXPCIVUJOTRE";  // TODO: Specify the password
    const char *from = "hitszblog@163.com"; // TODO: Specify the mail address of the sender
    char dest_ip[16];                       // Mail server IP address
    int s_fd;                               // socket file descriptor
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

    // TODO: Create a socket, return the file descriptor to s_fd, and establish a TCP connection to the mail server
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

    // Send EHLO command and print server response
    const char *EHLO = "EHLO 163.com\r\n"; // TODO: Enter EHLO command here
    if (send(s_fd, EHLO, strlen(EHLO), 0) == -1)
    {
        perror("send EHLO");
        exit(EXIT_FAILURE);
    }
    // TODO: Print server response to EHLO command
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Authentication. Server response should be printed out.
    // AUTH command
    const char *AUTH = "AUTH login\r\n";
    if (send(s_fd, AUTH, strlen(AUTH), 0) == -1)
    {
        perror("send AUTH");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // input user
    char *user_base64 = encode_str(user);
    if (send(s_fd, user_base64, strlen(user_base64), 0) == -1)
    {
        perror("send user");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // input password
    char *pass_bass64 = encode_str(pass);
    if (send(s_fd, pass_bass64, strlen(pass_bass64), 0) == -1)
    {
        perror("send pass");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    pause();
    // TODO: Send MAIL FROM command and print server response
    char *FROM;
    sprintf(FROM, "MAIL FROM: <%s>\r\n", from);
    if (send(s_fd, FROM, strlen(FROM), 0) == -1)
    {
        perror("FROM");
        exit(EXIT_FAILURE);
    }
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    pause();
    // TODO: Send RCPT TO command and print server response

    // TODO: Send DATA command and print server response

    // TODO: Send message data

    // TODO: Message ends with a single period

    // TODO: Send QUIT command and print server response

    close(s_fd);
}

int main(int argc, char *argv[])
{
    int opt;
    char *s_arg = NULL;
    char *m_arg = NULL;
    char *a_arg = NULL;
    char *recipient = NULL;
    const char *optstring = ":s:m:a:";
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 's':
            s_arg = optarg;
            break;
        case 'm':
            m_arg = optarg;
            break;
        case 'a':
            a_arg = optarg;
            break;
        case ':':
            fprintf(stderr, "Option %c needs an argument.\n", optopt);
            exit(EXIT_FAILURE);
        case '?':
            fprintf(stderr, "Unknown option: %c.\n", optopt);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Unknown error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
    {
        fprintf(stderr, "Recipient not specified.\n");
        exit(EXIT_FAILURE);
    }
    else if (optind < argc - 1)
    {
        fprintf(stderr, "Too many arguments.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        recipient = argv[optind];
        send_mail(recipient, s_arg, m_arg, a_arg);
        exit(0);
    }
}
