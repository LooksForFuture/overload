#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "cc"
#define CFLAGS "-std=c17", "-g"
#define IFLAGS "-Iinclude/"
#define LFLAGS "-lm", "-lSDL2"
#define WARNINGS "-Wall", "-Wpedantic", "-Wextra"
#define SRC_FILES "entity.c", "glut.c", "r_main.c", \
		"transform.c", "srender.c", "main.c"

#define OBJ_DIR "obj"
#define BIN_DIR "bin"
#define BIN_FILE "game"

int main(int argc, char **argv)
{
	if (!nob_mkdir_if_not_exists(OBJ_DIR)) return 1;
	if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;
	if (!nob_file_exists(OBJ_DIR"/editor.o") ||
	    nob_is_file_newer("editor.c", OBJ_DIR"/editor.o") == 1) {
		if (!NOB_CMD(CC, CFLAGS, WARNINGS,
			     "-o", OBJ_DIR"/editor.o",
			     "-c", "editor.c", IFLAGS, LFLAGS)) return 1;
	}
	if (!NOB_CMD(CC, CFLAGS, WARNINGS, SRC_FILES,
		     OBJ_DIR"/editor.o", IFLAGS, LFLAGS,
		     "-o", BIN_DIR"/"BIN_FILE)) return 1;
	if (!nob_cd(BIN_DIR)) return 1;
	if (!NOB_CMD("./game")) return 1;
	return 0;
}
