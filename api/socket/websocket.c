#include "fetch.h"
#include "../../lib/json.h"
#include <string.h>
#include <stdbool.h>

int websocket_connect(struct Client* client, const char* domain, const char* port, const char* path) {
	client_connect(client, domain, port);
}

int websocket_send(SSL* ssl, const char* msg, size_t size) {
	char* buffer = malloc(14 + size); // 14 bytes is the biggest frame header

	char* ptr = buffer;

	// send opcode
	*ptr++ = 0x81; // FIN, TEXT
	
	// send payload_len
	if (size < 126) {
		*ptr++ = 0x80 + (char)size; // 0x80 = MASK
	} else {
		int payload_len_len = (size > 0xffff) ? 8 : 2;

		*ptr++ = 0x80 + (payload_len_len == 8) ? 127 : 126; // 0x80 = MASK

		for (int i = payload_len_len; i > 0; i--) {
			*ptr++ = (char)(size >> ((i - 1) * 8));
		}
	}

	// send mask
	char* pmask = ptr;

	int mask = rand();
	for (int i = 4; i > 0; i--) {
		*ptr++ = (char)(mask >> ((i - 1) * 8));
	}

	// send payload
	for (size_t i = 0; i < size; i++) {
		*ptr++ = msg[i] ^ pmask[i & 3];
	}

	SSL_write(ssl, buffer, ptr - buffer);

	free(buffer);
}

json_value* json_get_by_name(json_value* json, const char* name) {
	printf("json_get_by_name\n");
	printf("json->type=%d\n", json->type);
	if (json == NULL || name == NULL || json->type != json_object) {
		return NULL;
	}
	printf("json->u.object.length = %u\n", json->u.object.length);
	for (int i = 0; i < json->u.object.length; i++) {
		printf("json->u.object.values[%d].name = %s\n", i, json->u.object.values[i].name);
		if (!strcmp(name, json->u.object.values[i].name)) {
			return &json->u.object.values[i];
			printf("json_get_by_name2\n");
		}
	}
	return NULL;
}

int process_frame(const char* frame, size_t buffer_size) {
	printf("start of process_frame\n");
	const char* ptr = frame;
	bool fin = *ptr & 0x80;
	int opcode = *ptr & 0x0f;
	ptr++;
	bool mask = *ptr & 0x80;
	size_t payload_len = frame[1] & 0x7f;
	ptr++;
	if (payload_len >= 126) {
		int payload_len_len = (payload_len == 126) ? 2 : 8;

		payload_len = *ptr++;
		for (int i = 1; i < payload_len_len; i++) {
			payload_len = (payload_len << 8) + *ptr++;
		}
	}

	json_value* payload = json_parse(ptr, payload_len);
	printf("start of getting values\n");
	if (payload->type == json_object) {
		json_value* t = json_get_by_name(&payload->u.object, "t");
		json_value* op = json_get_by_name(&payload->u.object, "op");
		json_value* s = json_get_by_name(&payload->u.object, "s");
		json_value* d = json_get_by_name(&payload->u.object, "d");
	
		switch(op->u.integer) {
			case 10:
				printf("op = 10\n");\
				break;
			case 0:
				printf("op = 0\n");
				break;
		}
	}

	json_value_free(payload);
	return 0;
}

int main(int argc, const char** argv) {
	struct Client client;
	
	if (argc  != 4) {
		printf("Usage : %s <Domain> <Port> <Path>\n", argv[0]);
		return 1;
	}
	const char* path = argv[3];
	const char* domain = argv[1];
	const char* port = argv[2];

	if (!websocket_connect(&client, domain, port, path)) {
		printf("websocket_connect failed.\n");
		return 1;
	}

	const char* nonce = "x3JJHMbDL1EzLkh9GBhXDw==";
	const char* template =
		"GET %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: %s\r\n"
		"Sec-WebSocket-Protocol: chat, superchat\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";

	size_t path_size = strlen(path);
	size_t domain_size = strlen(domain);
	size_t nonce_size = strlen(nonce);
	char* request = malloc(path_size + domain_size + nonce_size + strlen(template)); /* FIX LATER */
	sprintf(request, template, path, domain, nonce);
	int request_size = strlen(request);
	printf("%s\n", request);
	printf("%d\n", request_size);
	printf("%p\n", client.ssl);
	printf("%p\n", client.ctx);

	/* Send Request */
	if (SSL_write(client.ssl, request, request_size) < 1) {
		printf("SSL_write failed.\n");
		return 2;
	}

	/* Read Response */
	char buffer[1024] = {0};
	while (1) {		
		int bytes_read = SSL_read(client.ssl, buffer, 1023);

		if (bytes_read == 0) {
			int error = SSL_get_error(client.ssl, bytes_read);
			if (error != SSL_ERROR_ZERO_RETURN) {
				printf("SSL_read failed.\n");
				return 3;
			}
			break;
		}

		buffer[bytes_read] = '\0';
		printf("%d: %s\n",bytes_read, buffer);

		static bool first_time = true;
		if (first_time) {
			first_time = false;

			printf("Sending identity\n");
			
			const char token[] = DISCORD_TOKEN;
			const int intents = 513;

			const char* identify_template =
			"{"
				"\"op\":2,"
				"\"d\": {"
					"\"token\": \"%s\","
					"\"intents\": %d,"
					"\"properties\": {"
						"\"os\": \"linux\","
						"\"browser\": \"aspectminors turtle\","
						"\"device\": \"aspectminors turtle\""
					"}"
				"}"
			"}";

			char* identify_message = malloc(strlen(identify_template) + strlen(token) + 10);
			sprintf(identify_message, identify_template, token, intents);
			websocket_send(client.ssl, identify_message, strlen(identify_message));
			free(identify_message);
		} else {
			process_frame(buffer, bytes_read);
		}

		/*TODO handle split packets*/

		/*Parse json
		json_value* payload = json_parse(json_text, bytes_read); */
	}

	client_disconnect(&client);
}

