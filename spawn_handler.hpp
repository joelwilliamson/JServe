
struct request;

/* Spawn_handler accepts the file descriptor of an opened socket, and manages
 * an HTTP session on that socket. It currently returns when the session is
 * complete, but should be switched to a multithreaded model in the future.
 */
int spawn_handler(int fd);