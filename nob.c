#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "clang"
#define CFLAGS "-std=c17", "-g"
#define IFLAGS "-Iinclude/"
#define LFLAGS "-lm", "-lSDL2"
#define WARNINGS "-Wall", "-Wpedantic", "-Wextra"
#define SRC_FILES "entity.c", "glut.c", "r_main.c", "transform.c", \
		"srender.c", "main.c"

#define BIN_DIR "bin"
#define BIN_FILE "game"

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(NOB_CMD("clang", "nob.c", "-o", "nob"),
	                      argc, argv);

	if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;
	if (!NOB_CMD(CC, CFLAGS, WARNINGS, SRC_FILES, IFLAGS, LFLAGS,
		     "-o", BIN_DIR"/"BIN_FILE)) return 1;
	if (!nob_cd(BIN_DIR)) return 1;
	if (!NOB_CMD("./game")) return 1;
	return 0;
}
