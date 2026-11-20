#include "raisin.h"
#if !defined(_WIN32)
#include <arpa/inet.h>
#endif

static ra_socket_t get_fd(const char *ip, int port, uint8_t flags) {
	ra_socket_t res = INVALID_SOCKET;
	struct addrinfo hints = {0};
	struct addrinfo *info = {0};

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(ip, NULL, &hints, &info))
		return INVALID_SOCKET;
	
	for (struct addrinfo *ptr = info; ptr; ptr = ptr->ai_next) {
		res = socket(AF_INET, SOCK_STREAM, 0);
		if (res == INVALID_SOCKET)
			continue;
		if (_ra_set_nonblock(res)) {
			SOCK_CLOSE(res);
			continue;
		}
		((struct sockaddr_in *)ptr->ai_addr)->sin_port = htons(port);
		int ret = connect(res, ptr->ai_addr, ptr->ai_addrlen);
		int is_fine = 0;
		if (ret) {
			#if defined(_WIN32)
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					is_fine = 1;
			#else
				if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
					is_fine = 1;
			#endif
		}
		if (is_fine || ret == 0) {
			freeaddrinfo(info);
			if (flags & OESO_FLAG_NODELAY) {
				int flag = 1;
				setsockopt(res, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));
			}
			return res;
		}
		SOCK_CLOSE(res);
	}
	freeaddrinfo(info);
	return INVALID_SOCKET;
}

int ra_client_init(ra_clt_ctx_t *ctx, const char *ip, int port, uint8_t flags) {
	ra_socket_t fd = get_fd(ip, port);
	if (fd == INVALID_SOCKET)
		return 1;
	ctx->fd = fd;
	return 0;
}
