#include <openssl/ssl.h>

struct Client {
	int sockfd;
	SSL_CTX* ctx;
	SSL* ssl;
};


int client_connect(struct Client* client, const char* url, const char* port);
void client_disconnect(struct Client* client);

int fetch(const char* url, const char* port, const char* msg);