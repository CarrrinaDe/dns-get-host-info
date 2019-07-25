#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;
	char res[80];

	// set hints
	memset((char *) &hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;

	// get addresses
	ret = getaddrinfo(name, NULL, &hints, &result);
	if (ret < 0)
		gai_strerror(ret);

	// iterate through addresses and print them
	for (p = result; p != NULL; p = p->ai_next) {
		if (p->ai_family == AF_INET) { // pentru IPv4
			struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
			memset(res, 0, 80);
			inet_ntop(AF_INET, (struct in_addr *)&addr->sin_addr, res, 80);
			printf("ip: %s\nport: %d\n", res, ntohs(addr->sin_port));
		}
		else { // for IPv6
			struct sockaddr_in6 *addr = (struct sockaddr_in6 *)p->ai_addr;
			memset(res, 0, 80);
			inet_ntop(AF_INET6, (struct in6_addr *)&addr->sin6_addr, res, 80);
			printf("ip: %s\nport: %d\n", res, ntohs(addr->sin6_port));
		}
	}

	// free allocated data
	freeaddrinfo(result);
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	char host[1024];
	char service[20];

	memset(host, 0, 1024);
	memset(service, 0, 20);

	// fill in address data
	memset((char *) &addr, 0, sizeof(addr));
	inet_aton("127.0.0.1", &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(20000);

	// get name and service
	ret = getnameinfo((struct sockaddr *)&addr, sizeof(struct sockaddr_in), host, 1024, service, 20, 0);
	if (ret < 0)
		gai_strerror(ret);

	// print name and service
	printf("host: %s\nservice: %s\n", host, service);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
