#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mach/mach.h>
#include <string.h>
#include <limits.h>

int main(const int argc, const char **argv) {
  FILE *f = popen("csrutil status", "r");
  if (f == NULL) {
      perror("popen failed");
      return -1;
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), f) != NULL) {
    if (strstr(buffer, "enabled") != NULL) {
      printf("SIP is enabled. Disable it in recovery mode with 'csrutil disable'\n");
      pclose(f);
      return 1;
    }
  }

  if (argc < 2) {
    printf("Usage: %s <process path>\n", argv[0]);
    return 1;
  } 

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd() error");
    return 1;
  }

  const char *process_path = argv[1];
  const char *payload = "libpayload.dylib";
  char payload_path[PATH_MAX];
  snprintf(payload_path, sizeof(payload_path), "%s/%s", cwd, payload);

  setenv("DYLD_INSERT_LIBRARIES", payload_path, 1);
  setenv("DYLD_FORCE_FLAT_NAMESPACE", "1", 1);

  char *const targv[] = { (char *)process_path, NULL};
  execvp(process_path, targv);

  return 0;
}
