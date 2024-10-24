#include <linux/limits.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define SRC "src/main.c"
#define BUILD_FOLDER "build"
#define OUTPUT BUILD_FOLDER"/voc"
#define CMPCMDJSON "compile_commands.json"
#define RAYLIB_SRC "include/raylib/include"
#define RAYGUI_SRC "include/raygui/src"
#define RAYLIB_LIB "include/raylib/lib"

void compile_command_entry(Nob_String_Builder *sb, const char* cwd, bool need_command, const char* command, const char* file_path, const char* file_name) {
    sb_append_cstr(sb, "  {\n");

    sb_append_cstr(sb, "    \"directory\": \"");
    sb_append_cstr(sb, cwd);
    sb_append_cstr(sb, "\",\n");

    if (need_command) {
        sb_append_cstr(sb, "    \"command\": \"");
        sb_append_cstr(sb, command);
        sb_append_cstr(sb, "\",\n");
    }

    sb_append_cstr(sb, "    \"file\": \"");
    sb_append_cstr(sb, cwd);
    if (file_path) {
        sb_append_cstr(sb, "/");
        sb_append_cstr(sb, file_path);
    }
    sb_append_cstr(sb, "/");
    sb_append_cstr(sb, file_name);
    sb_append_cstr(sb, "\"\n");

    sb_append_cstr(sb, "  }");
}

bool generate_compile_commands(const char* command) {
    bool result = true;
    Nob_File_Paths rlsrc = {0};
    Nob_File_Paths rgsrc = {0};
    Nob_String_Builder sb = {0};
    char cwd[PATH_MAX] = {0};

    if (getcwd(cwd, PATH_MAX) == NULL) {
        nob_log(NOB_ERROR, "could not get current directory: %s", strerror(errno));
        nob_return_defer(false);
    }

    sb_append_cstr(&sb, "[\n");
    compile_command_entry(&sb, cwd, true, command, NULL, SRC);
    sb_append_cstr(&sb, "\n]\n");
    sb_append_null(&sb);

    if(!nob_write_entire_file(CMPCMDJSON, sb.items, sb.count))  nob_return_defer(false);
defer:
    nob_da_free(rlsrc);
    nob_da_free(rgsrc);
    nob_da_free(sb);
    return result;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};

    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra");
    nob_cmd_append(&cmd, "-I./"RAYLIB_SRC);
    nob_cmd_append(&cmd, "-I./"RAYGUI_SRC);
    nob_cmd_append(&cmd, "-o", OUTPUT, SRC);
    nob_cmd_append(&cmd, "-L./"RAYLIB_LIB);
    nob_cmd_append(&cmd, "-l:libraylib.a", "-lm");

    if (!nob_cmd_run_sync(cmd)) return 1;

    Nob_String_Builder sb = {0};
    nob_cmd_render(cmd, &sb);
    nob_sb_append_null(&sb);

    if (!file_exists(CMPCMDJSON)) generate_compile_commands(sb.items);

    return 0;
}
