#ifndef SOCKET_OEUF_H
#define SOCKET_OEUF_H

#include <stdint.h>
#include <stdlib.h>

#if defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>

	typedef SOCKET ra_socket_t;	
	#define SOCK_CLOSE closesocket

#elif defined(__linux__)
	#include <netdb.h>
	#include <poll.h>
	#include <sys/socket.h>
	#include <time.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stddef.h>
	#include <errno.h>
	#include <netinet/tcp.h>

	typedef int ra_socket_t;
	#undef INVALID_SOCKET
	#define INVALID_SOCKET (-1)
	#define SOCK_CLOSE close

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif

typedef struct ra_srv_clt_t ra_srv_clt_t;
typedef struct ra_srv_ctx_t ra_srv_ctx_t;

#define RA_FLAG_NODELAY (1 << 0)

struct ra_srv_clt_t {
	ra_socket_t fd;

	struct sockaddr addr;
	socklen_t addr_len;

	void *data;
	uint8_t flags;
};

struct ra_srv_ctx_t {
	ra_socket_t fd;

	int (*on_connect)(ra_srv_clt_t *, ra_srv_ctx_t *); // return 1 to refuse
	void (*on_disconnect)(ra_srv_clt_t *, ra_srv_ctx_t *, int); // int == 0 if reset
	void (*on_recv)(ra_srv_clt_t *, ra_srv_ctx_t *, void *, size_t);
	void (*on_send_ready)(ra_srv_clt_t *, ra_srv_ctx_t *); // used by wait
	uint8_t need_send; // set to 1 to receiv on_send_ready when wait

	ra_srv_clt_t *clients;
	size_t clients_len;
	size_t clients_capacity;

	void *data;
	uint8_t flags;
};

// server side
int ra_server_init(ra_srv_ctx_t *ctx, int port, uint8_t flags);
void ra_server_update(ra_srv_ctx_t *ctx);
void ra_server_remove(ra_srv_ctx_t *ctx, ra_srv_clt_t *clt);
// return < 0 if connection ended ( == 0 if full for now)
ssize_t ra_server_send(ra_srv_ctx_t *ctx, ra_srv_clt_t *clt, void *buf, size_t len);
// can call ctx->on_send_ready
void ra_server_wait(ra_srv_ctx_t *ctx, int at_least_ms);
void ra_server_exit(ra_srv_ctx_t *ctx);

typedef struct ra_clt_ctx_t ra_clt_ctx_t;

struct ra_clt_ctx_t {
	ra_socket_t fd;

	int is_connected;
	void (*on_connect)(ra_clt_ctx_t *ctx, int); // int == 0 if failed
	void (*on_disconnect)(ra_clt_ctx_t *ctx, int); // int == 0 if reset
	void (*on_recv)(ra_clt_ctx_t *ctx, void *, size_t);
	void (*on_send_ready)(ra_clt_ctx_t *ctx);
	uint8_t need_send;

	void *data;
};

// client side
int ra_client_init(ra_clt_ctx_t *ctx, const char *ip, int port, uint8_t flags);
void ra_client_update(ra_clt_ctx_t *ctx);
ssize_t ra_client_send(ra_clt_ctx_t *ctx, void *buf, size_t len);
// can call ctx->on_send_ready
void ra_client_wait(ra_clt_ctx_t *ctx, int at_least_ms);
void ra_client_exit(ra_clt_ctx_t *ctx);

int ra_init();
void ra_exit();


// internal stuff
int _ra_set_nonblock(ra_socket_t fd);
int64_t _ra_time_ms();
void _ra_check_accept(ra_srv_ctx_t *ctx);
void _ra_check_clients(ra_srv_ctx_t *ctx);
void _ra_update_client(ra_srv_clt_t *clt, ra_srv_ctx_t *ctx);
void _ra_clean_list(ra_srv_ctx_t *ctx);

#endif
