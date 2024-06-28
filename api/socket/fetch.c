#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <string.h>
#include <stdio.h>
#include<unistd.h>

struct Client {
	int sockfd;
	SSL_CTX* ctx;
	SSL* ssl;
};

int client_connect(struct Client* client, const char* url, const char* port);
void client_disconnect(struct Client* client);

void fetch(const char* domain, const char* port, const char* request, size_t request_size) {
	struct Client client;

	client_connect(&client, domain, port);

	/* Send Request */
	SSL_write(client.ssl, request, request_size);

	/* Read Response */
	char buffer[1024] = {0};
	while (1) {		
		int bytes_read = SSL_read(client.ssl, buffer, 1023);

		if (bytes_read == 0) {
			break;
		}

		buffer[bytes_read] = '\0';
		printf("%s", buffer);
	}

	client_disconnect(&client);
}

void client_disconnect(struct Client* client) {
	/* Clean Up */
	SSL_free(client->ssl);
	SSL_CTX_free(client->ctx);
	close(client->sockfd);
}

int client_connect(struct Client* client, const char* url, const char* port) {
	client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(url, port, &hints, &result)) {
        printf("\n Error : getaddrinfo \n");
        return 0;
    }

    if (connect(client->sockfd, result->ai_addr, (int)result->ai_addrlen) < 0) {
        printf("\n Error : Connect Failed \n");
        return 0;
    }

    freeaddrinfo(result);

	client->ctx = SSL_CTX_new(TLS_method());
	client->ssl = SSL_new(client->ctx);
	SSL_set_fd(client->ssl, client->sockfd);
	SSL_connect(client->ssl);

    return 1; 
}

void old_fetch(const char* domain, const char* port, const char* path) {
	const char* template = "GET %s HTTP/1.0\r\n\r\n";
	size_t path_size = strlen(path);
	char* request = malloc(path_size + strlen(template)); /* FIX LATER */
	sprintf(request, template, path);

	fetch(domain, port, request, strlen(request));
	
	free(request);
}

void main2(int argc, char** argv) {
	if (argc  != 4) {
		printf("Usage : %s <Domain> <Port> <Path>\n", argv[0]);
		return;
	}

	old_fetch(argv[1], argv[2], argv[3]);
}