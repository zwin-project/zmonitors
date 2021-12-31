#include <sys/mman.h>
#include <unistd.h>
#include <zmonitors-util.h>

ZMS_EXPORT int
zms_util_create_shared_fd(size_t size, const char *name)
{
  int fd = memfd_create(name, MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd < 0) return fd;
  unlink(name);

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}
