#include "raisin.h"

#if defined(_WIN32) || defined(__linux__)
void ra_server_remove(ra_srv_ctx_t *ctx, ra_srv_clt_t *clt) {
	if (clt->fd == INVALID_SOCKET)
		return ;
	ctx->on_disconnect(clt, ctx, 1);
	SOCK_CLOSE(clt->fd);
	clt->fd = INVALID_SOCKET;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
