#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct InputParameters {
  char workspaceDir[1000];
  char format[5];
  bool fixImports;
};

struct InputParameters getCommandLineArguments(int argc, char *argv[]) {
  char opt;
  struct InputParameters results = {".", "webp", false};

  while ((opt = getopt(argc, argv, "f:i")) != -1) {
    switch (opt) {
      case 'f':
        strcpy(results.format, optarg);
        printf("Option 'f' with value '%s'\n", optarg);
        break;
      case 'i':
        results.fixImports = true;
        printf("Option 'i'\n");
        break;
      case '?':
        break;
    }
  }

  for (int i = optind; i < argc; ++i) {
    strcpy(results.workspaceDir, argv[i]);
    printf("Non-option argument: %s\n", argv[i]);
    break;
  }

  return results;
}

int main(int argc, char *argv[]) {
  DIR *directory = opendir(".");
  struct dirent *entry;

  struct InputParameters cmdLineResults = getCommandLineArguments(argc, argv);

  printf("Workspace Directory: %s\n", cmdLineResults.workspaceDir);
  printf("Format: %s\n", cmdLineResults.format);
  printf("Fix Imports: %s\n", cmdLineResults.fixImports ? "true" : "false");
  return 0;
}
