#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const enum SUPPORTED_IMAGE_FORMATS {
  avif,
  dz,
  fits,
  gif,
  heif,
  input,
  jpeg,
  jpg,
  jp2,
  jxl,
  magick,
  openslide,
  pdf,
  png,
  ppm,
  raw,
  svg,
  tiff,
  tif,
  v,
  webp
};
const enum RECOGNIZED_FILES { html, css, scss, ts, js, tsx, jsx };

struct InputParameters {
  char workspaceDir[1000];
  char format[5];
  bool fixImports;
};

void printUsage() {
  printf("Usage: program.exe [workspaceDir] -f <format> -i\n");
}

struct InputParameters getCommandLineArguments(int argc, char *argv[]) {
  char opt;
  struct InputParameters results = {".", "webp", false};
  bool invalidArgument = false;

  while ((opt = getopt(argc, argv, "f:i")) != -1) {
    switch (opt) {
      case 'f':
        strcpy(results.format, optarg);
        break;
      case 'i':
        results.fixImports = true;
        break;
      case '?':
        invalidArgument = true;
        break;
    }
  }

  for (int i = optind; i < argc; ++i) {
    strcpy(results.workspaceDir, argv[i]);
    break;
  }

  if (invalidArgument) {
    printUsage();
    exit(EXIT_FAILURE);
  }
  return results;
}

int main(int argc, char *argv[]) {
  DIR *directory = opendir(".");
  struct dirent *entry;

  struct InputParameters cmdLineResults = getCommandLineArguments(argc, argv);

  printf("Starting Reduse using the following options: \n\n");
  printf("Workspace Directory: %s\n", cmdLineResults.workspaceDir);
  printf("Format: %s\n", cmdLineResults.format);
  printf("Fix Imports: %s\n", cmdLineResults.fixImports ? "true" : "false");
  return 0;
}
