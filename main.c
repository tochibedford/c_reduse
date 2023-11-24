#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum SUPPORTED_IMAGE_FORMATS {
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
const char *SUPPORTED_IMAGE_FORMATS_STRINGS[] = {
    "avif", "dz",  "fits", "gif",    "heif",      "input", "jpeg",
    "jpg",  "jp2", "jxl",  "magick", "openslide", "pdf",   "png",
    "ppm",  "raw", "svg",  "tiff",   "tif",       "v",     "webp"};
enum SUPPORTED_FILES { html, css, scss, ts, js, tsx, jsx };
const char *SUPPORTED_FILES_STRINGS[] = {"html", "css", "scss", "ts",
                                         "js",   "tsx", "jsx"};

struct InputParameters {
  char workspaceDir[1000];
  char format[5];
  bool fixImports;
};

void printUsage() {
  printf("Usage: program.exe [workspaceDir] -f <format> -i\n");
}

char *concatenateStrings(const char *str1, const char *str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  size_t totalLength = len1 + len2 + 1;
  char *result = (char *)malloc(totalLength);

  if (result != NULL) {
    strcpy(result, str1);
    strcat(result, str2);
  }

  return result;
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

bool confirmDirectory(char *workspaceDir) {
  DIR *dir = opendir(workspaceDir);
  if (dir) {
    closedir(dir);
    return true;
  } else {
    return false;
  }
}

char **listRelevantFiles(char *directory, const char *fileExtensions[],
                         size_t *length) {
  DIR *dirStream = opendir(directory);
  struct dirent *entry;
  size_t stackSize = 10;
  char **dirStack = calloc(stackSize, sizeof(char *));
  size_t currIndex = 0;

  if (dirStack == NULL) {
    fprintf(stderr, "Not enough memory");
    return NULL;
  }

  if (directory == NULL) {
    printf("Error opening directory %s\n", strerror(errno));
    return false;
  }

  while ((entry = readdir(dirStream)) != NULL) {
    dirStack[currIndex] = concatenateStrings(directory, entry->d_name);
    if (entry->d_type == DT_REG) {
      printf("File: %s\n", entry->d_name);
    } else if (entry->d_type == DT_DIR) {
      printf("Folder: %s\n", entry->d_name);
    }
    if (currIndex == stackSize - 1) {
      stackSize += 1;
      dirStack = realloc(dirStack, stackSize * sizeof(char *));
    }
    currIndex += 1;
    *length = currIndex;
  }

  if (closedir(dirStream) == -1) {
    printf("Error,Closing directory. \n");
    return NULL;
  }

  return dirStack;
}

int main(int argc, char *argv[]) {
  struct InputParameters cmdLineResults = getCommandLineArguments(argc, argv);

  printf("Starting Reduse using the following options: \n\n");
  printf("Workspace Directory: %s\n", cmdLineResults.workspaceDir);
  printf("Format: %s\n", cmdLineResults.format);
  printf("Fix Imports: %s\n", cmdLineResults.fixImports ? "true" : "false");
  printf("\n");

  if (!confirmDirectory(cmdLineResults.workspaceDir)) {
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  size_t length = 0;
  char **dirStack = listRelevantFiles(cmdLineResults.workspaceDir,
                                      SUPPORTED_FILES_STRINGS, &length);

  if (dirStack != NULL) {
    printf("Array length: %zu\n", length);

    for (size_t i = 0; i < length; i++) {
      printf("File %zu: %s\n", i, dirStack[i]);
      free(dirStack[i]);
    }

    free(dirStack);
  }

  if (dirStack == NULL) {
    exit(EXIT_FAILURE);
  }

  return 0;
}
