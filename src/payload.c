#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>

void* foo(void *arg) {
  const char *name = _dyld_get_image_name(1);
  if (strstr(name, "Player") != NULL)
    return NULL;

  const struct mach_header *header = _dyld_get_image_header(0);

  return NULL;
}

__attribute((constructor))
void init() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, foo, NULL) != 0) {
        printf("Error creating thread\n");
        exit(1);
    }

    pthread_detach(thread);
}
