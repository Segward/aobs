#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <mach-o/dyld.h>

int save_to_file(const char* filename, uintptr_t start, uintptr_t end) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return -1;
    }

    size_t size = end - start;
    if (fwrite((void*)start, 1, size, file) != size) {
        perror("fwrite");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

void* foo(void* arg) {
  const char* name = _dyld_get_image_name(1);
  if (strstr(name, "Player") == NULL) {
    return NULL;
  }

  const struct mach_header* header = _dyld_get_image_header(1);
  if (header == NULL) {
    return NULL;
  }

  uintptr_t header_base = (uintptr_t)header;
  printf("Base address of Player: 0x%lx\n", header_base);

  const struct mach_header* end = _dyld_get_image_header(_dyld_image_count() - 1);
  if (end == NULL) {
    return NULL;
  }

  uintptr_t end_base = (uintptr_t)end;
  printf("End address of Player: 0x%lx\n", end_base);

  int result = save_to_file("/tmp/aobs.bin", header_base, end_base);
  if (result != 0) {
    printf("Failed to save memory to file\n");
    return NULL;
  }

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
