#include "oeuf_socket.h"

void ra_exit() {
	#if defined(_WIN32)
		WSACleanup();
	#endif
}
