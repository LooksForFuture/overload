#ifndef __NOB_H__
#define __NOB_H__

/*
  My own implementation of nob.h

  This has been written by LooksForFuture(Mohammad Armin Niknami),
  by watching "tsoding daily" videos and some personal explorations, during
  internet outage. This is more of a recreational programming project :D
  And isn't a fully personal innovation. Something like 50-50 between
  rexim and personal ideas.
  Maybe I would write my own build system some time later. I'm currently
  inspired by functional programming style. Who knows?
  Anyway, this project uses the same license that Tsoding's nob.h uses.
  Have fun.
 */

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif //_WIN32

#define nob_return_defer(val) do { result = (val); goto defer; } while (0)

typedef enum {
	NOB_INFO,
	NOB_WARNING,
	NOB_ERROR,
} Nob_Log_Level;

void nob_log(Nob_Log_Level, char *, ...);

#ifndef NOB_DA_INIT_CAP
#define NOB_DA_INIT_CAP 64
#endif

#define nob_da_append(da, item) \
	do {						\
		if ((da)->count >= (da)->capacity) {	\
			(da)->capacity = (da)->capacity == 0 ?		\
				NOB_DA_INIT_CAP : (da)->capacity*2;	\
			(da)->items = realloc((da)->items,		\
					      (da)->capacity*		\
					      sizeof(*(da)->items));	\
			assert((da)->items != NULL && "Failed to expand"); \
		}							\
		(da)->items[(da)->count++] = (item);		\
	} while (0)

#define nob_da_append_many(da, newItems, newItemsCount) \
	do {								\
		if ((da)->count + (newItemsCount) > (da)->capacity) {	\
			if ((da)->capacity == 0) {			\
				(da)->capacity = NOB_DA_INIT_CAP;	\
			}						\
			while ((da)->count + (newItemsCount) > (da)->capacity) { \
				(da)->capacity *= 2;		\
			}						\
			(da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
			assert((da)->items != NULL && "Failed to expand"); \
		}							\
		memcpy((da)->items + (da)->count, (newItems), (newItemsCount)*sizeof(*(da)->items)); \
		(da)->count += (newItemsCount);			\
	} while (0)

#define nob_da_foreach(type, item, da) \
	for (type *item = (da)->items; \
	     item < (da)->items + (da)->count; item++)

typedef struct {
	char *items;
	size_t count;
	size_t capacity;
} Nob_String;

#define nob_str_append_cstr(sb, cstr) \
	do { \
		const char *s = (cstr); \
		size_t n = strlen(cstr); \
		nob_da_append_many(sb, s, n); \
	} while (0)
#define nob_str_append_null(sb) nob_da_append_many(sb, "", 1)

bool nob_cd(const char *);

bool nob_mkdir_if_not_exists(const char *);

bool nob_copy_file(const char *, const char *);

bool nob_rename(const char *, const char *);

int nob_is_file_newer(const char *, const char *);

bool nob_write_entire_file(const char *, void *, size_t);

#ifdef _WIN32
typedef HANDLE Nob_Proc;
#define NOB_INVALID_PROC NULL
#else
typedef int Nob_Proc;
#define NOB_INVALID_PROC -1
#endif //_WIN32

bool nob_proc_wait(Nob_Proc);

typedef struct {
	const char **items;
	size_t count;
	size_t capacity;
} Nob_Cmd;

void nob_cmd_render(Nob_Cmd, Nob_String *);

void nob_cmd_append_null(Nob_Cmd *, ...);

#define nob_cmd_append(cmd, ...) nob_cmd_append_null(cmd, __VA_ARGS__, NULL)

Nob_Proc nob_cmd_run_async(Nob_Cmd);

bool nob_cmd_run_sync(Nob_Cmd);

bool nob_cmd(const char*, ...);

#define NOB_CMD(...) nob_cmd(__VA_ARGS__, NULL)

#define NOB_GO_REBUILD_URSELF(compile_command, argc, argv)	\
	do { \
		const char *src_path = __FILE__; \
		assert(argc >= 1); \
		const char *bin_path = argv[0]; \
		\
		int rebuild_is_needed = nob_is_file_newer(src_path, bin_path);\
		if (rebuild_is_needed == -2) exit(1); \
		if (rebuild_is_needed == 1) { \
			Nob_String str = {0}; \
			nob_str_append_cstr(&str, bin_path); \
			nob_str_append_cstr(&str, ".old"); \
			nob_str_append_null(&str); \
			\
			if (!nob_rename(bin_path, str.items)) exit(1); \
			if (!compile_command) { \
				nob_rename(str.items, bin_path); \
				exit(1); \
		        } \
			\
			Nob_Cmd cmd = {0}; \
			nob_da_append_many(&cmd, argv, argc); \
			if (!nob_cmd_run_sync(cmd)) exit(1); \
			exit(0); \
		} \
	} while (0)

#ifdef NOB_IMPLEMENTATION

void nob_log(Nob_Log_Level level, char *fmt, ...)
{
	FILE *restrict stream = NULL;
	char *header = NULL;
	switch (level) {
	case NOB_INFO:
		stream = stdout;
		header = "[INFO]";
		break;
	case NOB_WARNING:
		stream = stdout;
		header = "[WARNING]";
		break;
	case NOB_ERROR:
		stream = stderr;
		header = "[ERROR]";
		break;
	}
	fprintf(stream, "%s ", header);

	va_list args;
	va_start(args, fmt);
	vfprintf(stream, fmt, args);
	va_end(args);

	fprintf(stream, "\n");
}

bool nob_cd(const char *path)
{
	int result = chdir(path);
	if (result) {
		nob_log(NOB_ERROR, "could not change directory to '%s': %s",
			path, strerror(errno));
		return false;
	}
	nob_log(NOB_INFO, "changed directory to '%s'", path);
	return true;
}

bool nob_mkdir_if_not_exists(const char *path)
{
#ifdef _WIN32
	int result = mkdir(path);
#else
	int result = mkdir(path, 0755);
#endif
	if (result < 0) {
		if (errno == EEXIST) {
			nob_log(NOB_INFO, "directory '%s' already exists", path);
			return true;
		}
		nob_log(NOB_ERROR, "could not create directory '%s': %s", path, strerror(errno));
		return false;
	}

	nob_log(NOB_INFO, "created directory '%s'", path);
	return true;
}

bool nob_copy_file(const char *src_path, const char *dst_path)
{
	nob_log(NOB_INFO, "Copying %s -> %s", src_path, dst_path);
#ifdef _WIN32
	if (!CopyFile(src_path, dst_path, FALSE)) {
		nob_log(NOB_ERROR, "Could not copy file: %lu", GetLastError());
		return false;
	}
	return true;
#else
	int src_fd = -1;
	int dst_fd = -1;
	size_t buf_size = 32 * 1024;
	char *buf = realloc(NULL, buf_size);
	assert(buf != NULL && "Looks like you're out of memo");
	bool result = true;

	src_fd = open(src_path, O_RDONLY);
	if (src_fd < 0) {
		nob_log(NOB_ERROR, "Could not open file %s:%s",
			src_path, strerror(errno));
		nob_return_defer(false);
	}

	struct stat src_stat;
	if (fstat(src_fd, &src_stat) < 0) {
		nob_log(NOB_ERROR, "Could not get mode of file %s: %s", src_path, strerror(errno));
		nob_return_defer(false);
	}

	dst_fd = open(dst_path, O_CREAT, O_TRUNC, O_WRONLY, src_stat.st_mode);
	if (dst_fd < 0) {
		nob_log(NOB_ERROR, "Could not create file %s:%s",
			dst_path, strerror(errno));
		nob_return_defer(false);
	}

	while (true) {
		ssize_t n = read(src_fd, buf, buf_size);
		if (n == 0) break;
		if (n < 0) {
			nob_log(NOB_ERROR, "Could not read from file %s: %s",
				src_path, strerror(errno));
			nob_return_defer(false);
		}
		char *buf2 = buf;
		while (n > 0) {
			ssize_t m = write(dst_fd, buf2, n);
			if (m < 0) {
				nob_log(NOB_ERROR, "Could not write to file %s: %s", dst_path, strerror(errno));
				nob_return_defer(false);
			}
			n -= m;
			buf2 += m;
		}
	}

defer:
	free(buf);
	close(src_fd);
	close(dst_fd);
	return result;
#endif
}

bool nob_rename(const char *old_path, const char *new_path)
{
	nob_log(NOB_INFO, "Renaming %s -> %s", old_path, new_path);
	if (rename(old_path, new_path) < 0) {
		nob_log(NOB_ERROR, "Could not rename file: %s", strerror(errno));
		return false;
	}
	return true;
}

int nob_is_file_newer(const char *file_a, const char *file_b)
{
	struct stat stat_a;
	struct stat stat_b;

	if (stat(file_a, &stat_a) != 0) {
		nob_log(NOB_ERROR, "Could not stat file %s: %s",
			file_a, strerror(errno));
		return -2;
	}

	if (stat(file_b, &stat_b) != 0) {
		nob_log(NOB_ERROR, "Could not stat file %s: %s",
			file_b, strerror(errno));
		return -2;
	}

	if (stat_a.st_mtim.tv_sec > stat_b.st_mtim.tv_sec) return 1;
	else if (stat_a.st_mtim.tv_sec < stat_b.st_mtim.tv_sec) return -1;
	else return 0;
}

bool nob_read_entire_file(const char *path, Nob_String *str)
{
	bool result = true;

	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		nob_log(NOB_ERROR, "Could not open file %s for reading: %s",
			path, strerror(errno));
		nob_return_defer(false);
	}

	assert(0 && "Not implemented yet");

defer:
	if (f) fclose(f);
	return result;
}

bool nob_write_entire_file(const char *path, void *data, size_t size)
{
	bool result = true;

	FILE *f = fopen(path, "wb");
	if (f == NULL) {
		nob_log(NOB_ERROR, "Could not open file %s for writing: %s",
			path, strerror(errno));
		nob_return_defer(false);
	}

	char *buf = data;
	while (size > 0) {
		size_t n = fwrite(buf, 1, size, f);
		if (ferror(f)) {
			nob_log(NOB_ERROR, "Could not write into file %s: %s",
				path, strerror(errno));
			nob_return_defer(false);
		}
		size -= n;
		buf += n;
	}

defer:
	if (f) fclose(f);
	return result;
}

bool nob_proc_wait(Nob_Proc p)
{
	for (;;) {
		int wstatus = 0;
		if (waitpid(p, &wstatus, 0) < 0) {
			nob_log(NOB_ERROR, "Could not wait on command (pid %d): %s", p, strerror(errno));
			return false;
		}

		if (WIFEXITED(wstatus)) {
			int exit_status = WEXITSTATUS(wstatus);
			if (exit_status) {
				nob_log(NOB_ERROR, "command exited with exit code %d", exit_status);
				return false;
			}

			break;
		}

		if (WIFSIGNALED(wstatus)) {
			nob_log(NOB_ERROR, "command process was terminated by %s", strsignal(WTERMSIG(wstatus)));
			return false;
		}
	}

	return true;
}

void nob_cmd_render(Nob_Cmd cmd, Nob_String *render)
{
	nob_da_foreach(const char *, arg, &cmd) {
		nob_str_append_cstr(render, *arg);
		nob_str_append_cstr(render, " ");
	}
}

void nob_cmd_append_null(Nob_Cmd *cmd, ...)
{
	va_list args;
	va_start(args, cmd);

	const char *arg = va_arg(args, const char*);
	while (arg) {
		nob_da_append(cmd, arg);
		arg = va_arg(args, const char*);
	}

	va_end(args);
}

Nob_Proc nob_cmd_run_async(Nob_Cmd cmd)
{
	Nob_String str = {0};
	nob_cmd_render(cmd, &str);
	nob_str_append_null(&str);
	printf("[CMD] %s\n", str.items);

	pid_t cpid = fork();
	if (cpid < 0) {
		nob_log(NOB_ERROR, "Could not fork child process: %s", strerror(errno));
		free(str.items);
		return NOB_INVALID_PROC;
	}

	if (cpid == 0) {
		if (execvp(cmd.items[0], (char * const*)cmd.items)) {
			nob_log(NOB_ERROR, "Could not exec child process: %s", strerror(errno));
			exit(1);
		}
		assert(0 && "unreachable");
	}

	return cpid;
}

bool nob_cmd_run_sync(Nob_Cmd cmd)
{
	Nob_Proc p = nob_cmd_run_async(cmd);
	if (p == NOB_INVALID_PROC) return false;
	return nob_proc_wait(p);
}

bool nob_cmd(const char *arg0, ...)
{
	Nob_Cmd cmd = {0};
	nob_da_append(&cmd, arg0);

	va_list args;
	va_start(args, arg0);

	const char *arg = va_arg(args, const char*);
	while (arg) {
		nob_da_append(&cmd, arg);
		arg = va_arg(args, const char *);
	}

	va_end(args);

	bool result = nob_cmd_run_sync(cmd);
	free(cmd.items);

	return result;
}

#endif //NOB_IMPLEMENTATION

#endif //__NOB_H__
