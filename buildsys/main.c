#include <stdio.h>
#define NOB_IMPLEMENTATION
#include "../nob.h"
#include "../config.h"
// TODO: Consider moving to ../config.h
#define CFLAGS "-nostdlib", "-march=x86-64", "-ffreestanding", "-static", "-Wall", "-fomit-frame-pointer", "-fno-builtin", "-fno-stack-protector", "-mno-red-zone", "-mno-mmx", "-mno-sse", "-mno-sse2", "-mno-3dnow", "-fPIC"


const char* get_ext(const char* path) {
    const char* end = path;
    while(*end) end++;
    while(end >= path) {
        if(*end == '.') return end+1;
        if(*end == '/' || *end == '\\') break;
        end--;
    }
    return NULL;
}
const char* get_base(const char* path) {
    const char* end = path;
    while(*end) end++;
    while(end >= path) {
        if(*end == '/' || *end == '\\') return end+1;
        end--;
    }
    return end;
}
char* shift_args(int *argc, char ***argv) {
    if((*argc) <= 0) return NULL;
    char* arg = **argv;
    (*argc)--;
    (*argv)++;
    return arg;
}
const char* strip_prefix(const char* str, const char* prefix) {
     size_t len = strlen(prefix);
     if(strncmp(str, prefix, strlen(prefix))==0) return str+len;
     return NULL;
}

bool nob_mkdir_if_not_exists_silent(const char *path) {
     if(nob_file_exists(path)) return true;
     return nob_mkdir_if_not_exists(path);
}

bool make_build_dirs() {
    if(!nob_mkdir_if_not_exists_silent("./bin")) return false;
    if(!nob_mkdir_if_not_exists_silent("./bin/kernel")) return false;
    if(!nob_mkdir_if_not_exists_silent("./bin/iso")) return false;
    return true;
}
bool remove_objs(const char* dirpath) {
   DIR *dir = opendir(dirpath);
   if (dir == NULL) {
       nob_log(NOB_ERROR, "Could not open directory %s: %s",dirpath,strerror(errno));
       return false;
   }
   errno = 0;
   struct dirent *ent = readdir(dir);
   while(ent != NULL) {
        const char* fext = get_ext(ent->d_name);
        const char* path = nob_temp_sprintf("%s/%s",dirpath,ent->d_name); 
        Nob_File_Type type = nob_get_file_type(path);
        if(strcmp(fext, "o")==0) {
            if(type == NOB_FILE_REGULAR) {
               if(!nob_delete_file(path)) {
                  closedir(dir);
                  return false;
               }
            }
        }
        if (type == NOB_FILE_DIRECTORY) {
            Nob_String_Builder sb = {0};
            nob_sb_append_cstr(&sb, path);
            nob_sb_append_null(&sb);
            if(!remove_objs(sb.items)) {
                nob_sb_free(sb);
                return false;
            }
            nob_sb_free(sb);
        }
        ent = readdir(dir);
   }
   if (dir) closedir(dir);
   return true;
}
bool clean() {
    if(nob_file_exists("./bin/kernel")) {
        if (!remove_objs("./bin/kernel")) return false;
    }
    return true;
}
// TODO: cc but async
bool cc(const char* ipath, const char* opath) {
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, GCC);
    nob_cmd_append(&cmd, CFLAGS);
    nob_cmd_append(&cmd, "-I", "./kernel/vendor/limine");
    nob_cmd_append(&cmd, "-c", ipath, "-o", opath);
    if(!nob_cmd_run_sync(cmd)) {
       nob_cmd_free(cmd);
       return false;
    }
    nob_cmd_free(cmd);
    return true;
}
#define SRC_DIR "./kernel/src"
#define BUILD_DIR "./bin/kernel"
bool build_kernel_dir(const char* srcdir, bool forced) {
   bool result = true;
   Nob_String_Builder opath = {0};
   DIR *dir = opendir(srcdir);
   if (dir == NULL) {
       nob_log(NOB_ERROR, "Could not open directory %s: %s",srcdir,strerror(errno));
       return false;
   }
   errno = 0;
   struct dirent *ent = readdir(dir);
   while(ent != NULL) {
        const char* fext = get_ext(ent->d_name);
        const char* path = nob_temp_sprintf("%s/%s",srcdir,ent->d_name); 
        Nob_File_Type type = nob_get_file_type(path);
        if(strcmp(fext, "c")==0) {
            if(type == NOB_FILE_REGULAR) {
               opath.count = 0;
               nob_sb_append_cstr(&opath, BUILD_DIR);
               nob_sb_append_cstr(&opath, "/");
               const char* file = strip_prefix(path, SRC_DIR)+1;
               Nob_String_View sv = nob_sv_from_cstr(file);
               sv.count-=2; // Remove .c
               nob_sb_append_buf(&opath,sv.data,sv.count);
               nob_sb_append_cstr(&opath, ".o");
               nob_sb_append_null(&opath);
               if((!nob_file_exists(opath.items)) || nob_needs_rebuild1(opath.items,path) || forced) {
                   if(!cc(path,opath.items)) nob_return_defer(false);
               }
            }
        }
        if (type == NOB_FILE_DIRECTORY && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
         
           opath.count = 0;
           nob_sb_append_cstr(&opath, BUILD_DIR);
           nob_sb_append_cstr(&opath, strip_prefix(path, SRC_DIR)+1);
           nob_sb_append_null(&opath);
           if(!nob_mkdir_if_not_exists_silent(opath.items)) nob_return_defer(false);
           opath.count = 0;
           nob_sb_append_cstr(&opath, path);
           nob_sb_append_null(&opath);
           if(!build_kernel_dir(opath.items, forced)) nob_return_defer(false);
        }
        ent = readdir(dir);
   }
defer:
   if (dir) closedir(dir);
   if (opath.items) nob_sb_free(opath);
   return result;
}
bool build_kernel(bool forced) {
    if(!build_kernel_dir(SRC_DIR, forced)) return false;
    nob_log(NOB_INFO, "Built kernel successfully");
    return true;
}
bool make_iso() {
    Nob_Cmd cmd = {0};
    nob_cmd_append(
        &cmd,
        "xorriso",
        "-as", "mkisofs",
        "-b", "limine-bios-cd.bin",
        "-no-emul-boot",
        "-boot-load-size", "4",
        "-boot-info-table",
        "--efi-boot", "limine-uefi-cd.bin",
        "-efi-boot-part",
        "--efi-boot-image",
        "./bin/iso",
        "-o",
        "./bin/OS.iso"
    );
    if (!nob_cmd_run_sync(cmd)) {
        nob_cmd_free(cmd);
        return false;
    }
    nob_cmd_free(cmd);
    return true;
}
bool find_objs(const char* dirpath, Nob_File_Paths *paths) {
    Nob_String_Builder sb={0};
    bool result = true;
    DIR *dir = NULL;

    dir = opendir(dirpath);
    if (dir == NULL) {
        nob_log(NOB_ERROR, "Could not open directory %s: %s", dirpath, strerror(errno));
        nob_return_defer(false);
    }

    errno = 0;
    struct dirent *ent = readdir(dir);
    while (ent != NULL) {
        const char* fext = get_ext(ent->d_name);
        const char* path = nob_temp_sprintf("%s/%s",dirpath,ent->d_name);
        Nob_File_Type type = nob_get_file_type(path);
        if(strcmp(fext, "o") == 0) {
            if(type == NOB_FILE_REGULAR) {
                nob_da_append(paths,path);
            }
        }
        if (type == NOB_FILE_DIRECTORY && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            sb.count = 0;
            nob_sb_append_cstr(&sb,nob_temp_sprintf("%s/%s",dir,ent->d_name));
            nob_sb_append_null(&sb);
            if(!find_objs(sb.items, paths)) nob_return_defer(false);
        }
        ent = readdir(dir);
    }

    if (errno != 0) {
        nob_log(NOB_ERROR, "Could not read directory %s: %s", dirpath, strerror(errno));
        nob_return_defer(false);
    }

defer:
    if (dir) closedir(dir);
    nob_sb_free(sb);
    return result;
}
bool link_kernel() {
    nob_log(NOB_INFO, "Linking kernel");
    Nob_File_Paths paths = {0};
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, LD, "-T", "./linker/link.ld", "-o", "./bin/iso/kernel");
    if(!find_objs(BUILD_DIR,&paths)) {
        nob_cmd_free(cmd);
        return false;
    }
    for(size_t i = 0; i < paths.count; ++i) {
        nob_cmd_append(&cmd, paths.items[i]);
    }
    nob_da_free(paths);
    if(!nob_cmd_run_sync(cmd)) {
        nob_cmd_free(cmd);
        return false;
    }
    nob_cmd_free(cmd);
    nob_log(NOB_INFO, "Linked kernel successfully");
    return true;
}
bool _copy_all_to(const char* to, const char** paths, size_t paths_count) {
    for(size_t i = 0; i < paths_count; ++i) {
        const char* path = nob_temp_sprintf("%s/%s",to,get_base(paths[i]));
        if(!nob_file_exists(path) || nob_needs_rebuild1(path,paths[i])) {
            if(!nob_copy_file(paths[i],path)) return false;
        }
    }
    return true;
}
#define copy_all_to(to, ...) \
    _copy_all_to((to), \
                 ((const char*[]){__VA_ARGS__}), \
                 (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)))
bool make_limine() {
    if(!copy_all_to(
        "./bin/iso",
        "./kernel/vendor/limine/limine-bios.sys", "./kernel/vendor/limine/limine-bios-cd.bin",
        "./kernel/vendor/limine/limine-uefi-cd.bin",
        "./kernel/limine.cfg"
    )) return false;
    nob_log(NOB_INFO, "Copied limine");
    return true;
}
typedef struct {
    char* exe;
    int argc;
    char** argv;
} Build;


typedef struct {
   const char* name;
   bool (*run)(Build*);
   const char* desc;
} Cmd;
bool help(Build* build);
bool build(Build* build);
bool run(Build* build);
bool bruh(Build* build);
Cmd commands[] = {
   { .name = "build", .run=build, .desc="Build the kernel and make iso" },
   { .name = "run"  , .run=run  , .desc="Run iso using qemu" },
   { .name = "bruh" , .run=bruh , .desc="Build+Run iso using qemu" },
   { .name = "help" , .run=build, .desc="Help command that explains either what a specific subcommand does or lists all subcommands" }
};

bool help(Build* build) {
    const char* what = shift_args(&build->argc, &build->argv);
    if(what) {
        for(size_t i = 0; i < NOB_ARRAY_LEN(commands); ++i) {
             if(strcmp(commands[i].name, what) == 0) {
                 nob_log(NOB_INFO, "%s: %s",what,commands[i].desc);
                 return true; 
             }
        }
        nob_log(NOB_ERROR, "Unknown subcommand: %s",what);
        return false;
    }
    nob_log(NOB_INFO, "%s <subcommand>", build->exe);
    nob_log(NOB_INFO, "List of subcommands:");
    for(size_t i = 0; i < NOB_ARRAY_LEN(commands); ++i) {
        nob_log(NOB_INFO, "  %s: %s",commands[i].name,commands[i].desc);
    }
    return true;
}
// TODO Separate these out maybe? Idk
bool build(Build* build) {
    if(!make_build_dirs()) return false;
    bool forced = false;
    if(build->argc > 0 && strcmp(build->argv[0], "-f")==0) forced = true;
    if(!build_kernel(forced)) return false;
    if(!link_kernel()) return false;
    if(!make_limine()) return false;
    if(!make_iso()) return false;
    return true;
}
bool run(Build* build) {
    Nob_Cmd cmd = {0};
    nob_cmd_append(
        &cmd,
        "qemu-system-x86_64",
        "-serial", "file:kernel_logs.txt",
        "-cpu", "max",
        "-smp", "2",
        "-m", "128",
        "-cdrom", "./bin/OS.iso"
    );
    if (!nob_cmd_run_sync(cmd)) {
        nob_cmd_free(cmd);
        return false;
    }
    nob_cmd_free(cmd);
    return true;
}
bool bruh(Build* b) {
    if(!build(b)) return false;
    if(!run(b)) return false;
    return true;
}
int main(int argc, char** argv) {
#if defined(__MAH__) || defined (__APPLE__)
    nob_log(NOB_WARNING, "NOB_GO_REBUILD_URSELF has some issues building on Mac and its temporarily disabled whilst I figure out why it doesn't work.");
    nob_log(NOB_INFO   , "You'll have to rebuild buildsys yourself every time if you're on MacOS.");
    nob_log(NOB_INFO   , "Sorry for the inconvenience ;-;");
#else
    NOB_GO_REBUILD_URSELF(argc,argv);
#endif
    Build build = {0};
    build.exe = shift_args(&argc, &argv);
    assert(build.exe && "First argument should be program itself");
    const char* cmd = shift_args(&argc, &argv);
    build.argc = argc;
    build.argv = argv;
    if(cmd == NULL) {
        nob_log(NOB_ERROR, "Expected subcommand but found nothing!");
        help(&build);
        return 1;
    }
    for(size_t i = 0; i < NOB_ARRAY_LEN(commands); ++i) {
        if(strcmp(commands[i].name,cmd) == 0) {
            if(!commands[i].run(&build)) return 1;
            return 0;
        }
    }
    nob_log(NOB_ERROR, "Unknown subcommand %s", cmd);
    return 1;
}
