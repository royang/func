#ifndef _WRAPSOCK_H_
#define _WRAPSOCK_H_


int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);
const char *Inet_ntop(int fd, void *src, char *dst, socklen_t size);
void Inet_pton(int family, const char *src, void *dst);
void Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen);
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr);




#endif

