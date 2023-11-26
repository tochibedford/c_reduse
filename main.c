#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_MAX 4096

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
  char workspaceDir[PATH_MAX];
  char format[5];
  bool fixImports;
};

bool isAbsolutePath(const char *path) {
#ifdef _WIN32
  // On Windows, check for a drive letter (e.g., "C:\")
  if (isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
    return true;
#else
  // On Unix-like systems, check for a leading '/'
  if (path[0] == '/') return true;
#endif
  return false;
}

void printUsage(char *argv[]) {
  printf("Usage: %s [workspaceDir] -f <format> -i\n", argv[0]);
}

void *concatenateArrays(const void *arr1, size_t len1, const void *arr2,
                        size_t len2, size_t elementSize, size_t *resultLen) {
  *resultLen = len1 + len2;

  void *result = malloc((*resultLen) * elementSize);

  if (result != NULL) {
    const char *src1 = (const char *)arr1;
    char *dest = (char *)result;

    for (size_t i = 0; i < len1 * elementSize; ++i) {
      dest[i] = src1[i];
    }

    const char *src2 = (const char *)arr2;
    dest += len1 * elementSize;

    for (size_t i = 0; i < len2 * elementSize; ++i) {
      dest[i] = src2[i];
    }
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

  // get rid of trailing slash
  for (int i = optind; i < argc; ++i) {
    if (strcmp(argv[i] + (strlen(argv[i]) - 1), "\\") == 0 ||
        strcmp(argv[i] + (strlen(argv[i]) - 1), "/") == 0) {
      strncpy(results.workspaceDir, argv[i], strlen(argv[i]) - 1);
    } else {
      strcpy(results.workspaceDir, argv[i]);
    }

    if (isAbsolutePath(results.workspaceDir) == false) {
      // convert to absolute path
      char cwd[PATH_MAX];
      getcwd(cwd, PATH_MAX);
      printf("%s\n", cwd);
    }
    break;
  }

  if (invalidArgument) {
    printUsage(argv);
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

typedef struct dir_stack {
  char **directories;
  size_t capacity;
  size_t length;
} dir_stack;

void listRelevantFiles(char *directory, const char *fileExtensions[],
                       dir_stack *dirStack) {
  DIR *dirStream = opendir(directory);
  struct dirent *entry;

  if (dirStream == NULL) {
    printf("Error opening directory %s\n", strerror(errno));
    printf("Error opening %s", directory);
    return;
  }

  while ((entry = readdir(dirStream)) != NULL) {
    char *currPath = malloc(strlen(directory) + 1 + strlen(entry->d_name) + 1);
    if (currPath == NULL) {
      perror("Error allocating memory");
      break;
    }

    sprintf(currPath, "%s/%s", directory, entry->d_name);
    if (entry->d_type == DT_REG) {
      if (dirStack->length == dirStack->capacity) {
        dirStack->capacity += 10;
        dirStack->directories =
            realloc(dirStack->directories, dirStack->capacity * sizeof(char *));
        if (dirStack->directories == NULL) {
          perror("Error reallocating memory");
          break;  // Exit the loop on memory reallocation failure
        }
      }
      dirStack->directories[dirStack->length] = currPath;
      dirStack->length++;
    } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
               strcmp(entry->d_name, "..") != 0) {
      listRelevantFiles(currPath, fileExtensions, dirStack);
      free(currPath);
    } else {
      free(currPath);
    }
  }

  if (closedir(dirStream) == -1) {
    printf("Error,Closing directory. \n");
    printf("Error closing %s\n", directory);
    return;
  }
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

  dir_stack dirStack;
  dirStack.capacity = 10;
  dirStack.directories = (char **)calloc(dirStack.capacity, sizeof(char *));
  dirStack.length = 0;

  listRelevantFiles(cmdLineResults.workspaceDir, SUPPORTED_FILES_STRINGS,
                    &dirStack);

  if (dirStack.directories != NULL) {
    printf("Array length: %zu\n", dirStack.length);

    for (size_t i = 0; i < dirStack.length; i++) {
      printf("File %zu: %s\n", i, dirStack.directories[i]);
      free(dirStack.directories[i]);
    }
  } else {
    fprintf(stderr, "Memory allocation failed\n");
  }
  free(dirStack.directories);

  if (dirStack.directories == NULL) {
    exit(EXIT_FAILURE);
  }

  return 0;
}
