#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "cc"
#define CFLAGS "-std=c17", "-g", "-fsanitize=address,undefined", \
		"-flto=auto"
#define IFLAGS "-Iinclude/"
#define LFLAGS "-lm", "-lSDL2"
#define WARNINGS "-Wall", "-Wpedantic", "-Wextra", "-Wshadow"

#define OBJ_DIR "obj"
#define BIN_DIR "bin"
#define BIN_FILE "game"

static bool must_compile = false;
static bool must_link = false;

#define CHECK_HEADER(name) \
	if (nob_is_file_newer("include/"name, BIN_DIR"/"BIN_FILE)==1) { \
		must_compile = true; \
		must_link = true; \
		goto compile_stage; \
	}

#define CACHE_COMPILE(name) \
	if (must_compile || \
	    !nob_file_exists(OBJ_DIR"/"#name".o") || \
nob_is_file_newer(#name".c", OBJ_DIR"/"#name".o") == 1) { \
		must_link = true; \
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

	if (!nob_file_exists(BIN_DIR"/"BIN_FILE)) {
		must_compile = true;
		must_link = true;
		goto compile_stage;
	}

	CHECK_HEADER("actuator.h");
	CHECK_HEADER("all_components.h");
	CHECK_HEADER("collider.h");
	CHECK_HEADER("component.h");
	CHECK_HEADER("component_interfaces.h");
	CHECK_HEADER("dsa.h");
	CHECK_HEADER("editor.h");
	CHECK_HEADER("entity.h");
	CHECK_HEADER("gamedef.h");
	CHECK_HEADER("glut.h");
	CHECK_HEADER("gmath.h");
	CHECK_HEADER("input.h");
	CHECK_HEADER("inspector.h");
	CHECK_HEADER("intent.h");
	CHECK_HEADER("kb_input.h");
	CHECK_HEADER("nuklear.h");
	CHECK_HEADER("nuklear_sdl_renderer.h");
	CHECK_HEADER("physics.h");
	CHECK_HEADER("r_main.h");
	CHECK_HEADER("srender.h");
	CHECK_HEADER("transform.h");

compile_stage:
	CACHE_COMPILE(glut);
	CACHE_COMPILE(entity);
	CACHE_COMPILE(physics);
	CACHE_COMPILE(editor);
	CACHE_COMPILE(inspector);
	CACHE_COMPILE(r_main);
	CACHE_COMPILE(kb_input);
	CACHE_COMPILE(transform);
	CACHE_COMPILE(srender);
	CACHE_COMPILE(collider);
	CACHE_COMPILE(intent);
	CACHE_COMPILE(actuator);
	CACHE_COMPILE(component_interfaces);
	CACHE_COMPILE(main);
	if (must_link && !NOB_CMD(CC, CFLAGS, WARNINGS,
		     OBJ_DIR"/editor.o", OBJ_DIR"/inspector.o",
		     OBJ_DIR"/r_main.o", OBJ_DIR"/glut.o",
				  OBJ_DIR"/physics.o",
		     OBJ_DIR"/entity.o", OBJ_DIR"/kb_input.o",
		     OBJ_DIR"/transform.o", OBJ_DIR"/srender.o",
		     OBJ_DIR"/collider.o", OBJ_DIR"/intent.o",
		     OBJ_DIR"/actuator.o",
		     OBJ_DIR"/component_interfaces.o",
		     "main.c",
		     IFLAGS, LFLAGS,
		     "-o", BIN_DIR"/"BIN_FILE)) return 1;
	if (!nob_cd(BIN_DIR)) return 1;
	if (!NOB_CMD("./game")) return 1;
	return 0;
}
