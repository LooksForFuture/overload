#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "cc"
#define CFLAGS "-std=c17", "-g"
#define IFLAGS "-Iinclude/"
#define LFLAGS "-lm", "-lSDL2"
#define WARNINGS "-Wall", "-Wpedantic", "-Wextra"
#define SRC_FILES "entity.c", \
		"kb_input.c", \
		"transform.c", "srender.c", \
		"collider.c", "intent.c", "actuator.c", \
		"component_interfaces.c", "main.c"

#define OBJ_DIR "obj"
#define BIN_DIR "bin"
#define BIN_FILE "game"

#define CACHE_COMPILE(name) \
	if (!nob_file_exists(OBJ_DIR"/"#name".o") || \
nob_is_file_newer(#name".c", OBJ_DIR"/"#name".o") == 1) { \
	if (!NOB_CMD(CC, CFLAGS, WARNINGS, "-o", OBJ_DIR"/"#name".o", \
		     "-c", #name".c", IFLAGS, LFLAGS)) return 1; \
	}

#define TO_OBJ_FILE(name) OBJ_DIR"/"#name".o",

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(
		NOB_CMD("clang", "nob.c", "-o", "nob"),
		argc, argv);

	if (!nob_mkdir_if_not_exists(OBJ_DIR)) return 1;
	if (!nob_mkdir_if_not_exists(BIN_DIR)) return 1;
	CACHE_COMPILE(editor);
	CACHE_COMPILE(inspector);
	CACHE_COMPILE(r_main);
	CACHE_COMPILE(glut);
	if (!NOB_CMD(CC, CFLAGS, WARNINGS, SRC_FILES,
		     OBJ_DIR"/editor.o", OBJ_DIR"/inspector.o",
		     OBJ_DIR"/r_main.o", OBJ_DIR"/glut.o",
		     IFLAGS, LFLAGS,
		     "-o", BIN_DIR"/"BIN_FILE)) return 1;
	if (!nob_cd(BIN_DIR)) return 1;
	if (!NOB_CMD("./game")) return 1;
	return 0;
}
