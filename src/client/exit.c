#include "oeuf_socket.h"

void ra_client_exit(ra_clt_ctx_t *ctx) {
	if (ctx->fd != INVALID_SOCKET)
		SOCK_CLOSE(ctx->fd);
	ctx->fd = INVALID_SOCKET;
}
