#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#endif

#include <config.h>

#include <fstream>
#include <iostream>

#include <bin/tpl/whereami/whereami.h>

#include <libasr/exception.h>
#include <lfortran/utils.h>
#include <libasr/string_utils.h>

namespace LCompilers::LFortran {

ExecutionMode execution_mode;
std::string lfortran_exec_path_dir;

void get_executable_path(std::string &executable_path, int &dirname_length)
{
#ifdef HAVE_WHEREAMI
    int length;

    length = wai_getExecutablePath(NULL, 0, &dirname_length);
    if (length > 0) {
        std::string path(length+1, '\0');
        wai_getExecutablePath(&path[0], length, &dirname_length);
        executable_path = path;
        if (executable_path[executable_path.size()-1] == '\0') {
            executable_path = executable_path.substr(0,executable_path.size()-1);
        }
    } else {
        throw LCompilersException("Cannot determine executable path.");
    }
#else
    executable_path = "src/bin/lfortran.js";
    dirname_length = 7;
#endif
}

void set_exec_path_and_mode(std::string &executable_path, int &dirname_length) {
    lfortran_exec_path_dir = executable_path.substr(0, dirname_length);

    if (   endswith(lfortran_exec_path_dir, "src/bin")
        || endswith(lfortran_exec_path_dir, "src\\bin")
        || endswith(lfortran_exec_path_dir, "SRC\\BIN")) {
        // Development version
        execution_mode = ExecutionMode::LFortranDevelopment;
    } else if (endswith(lfortran_exec_path_dir, "src/lfortran/tests") ||
               endswith(to_lower(lfortran_exec_path_dir), "src\\lfortran\\tests")) {
        // CTest Tests
        execution_mode = ExecutionMode::LFortranCtest;
    } else {
        // Installed version
        execution_mode = ExecutionMode::LFortranInstalled;
    }
}

std::string get_runtime_library_dir()
{
#ifdef HAVE_BUILD_TO_WASM
    return "asset_dir";
#endif
    char *env_p = std::getenv("LFORTRAN_RUNTIME_LIBRARY_DIR");
    if (env_p) return env_p;

    std::string path;
    int dirname_length;
    get_executable_path(path, dirname_length);
    std::string dirname = path.substr(0,dirname_length);
    if (   endswith(dirname, "src/bin")
        || endswith(dirname, "src\\bin")
        || endswith(dirname, "SRC\\BIN")) {
        // Development version
        return dirname + "/../runtime";
    } else if (endswith(dirname, "src/lfortran/tests") ||
               endswith(to_lower(dirname), "src\\lfortran\\tests")) {
        // CTest Tests
        return dirname + "/../../runtime";
    } else {
        // Installed version
        return dirname + "/" + CMAKE_INSTALL_LIBDIR_RELATIVE;
    }
}

std::string get_runtime_library_header_dir()
{
    char *env_p = std::getenv("LFORTRAN_RUNTIME_LIBRARY_HEADER_DIR");
    if (env_p) return env_p;

    std::string path;
    int dirname_length;
    get_executable_path(path, dirname_length);
    std::string dirname = path.substr(0,dirname_length);
    return dirname + "/" + CMAKE_INSTALL_INCLUDEDIR_RELATIVE + "/lfortran/impure";
}

std::string get_runtime_library_c_header_dir()
{
    char *env_p = std::getenv("LFORTRAN_RUNTIME_LIBRARY_HEADER_DIR");
    if (env_p) return env_p;

    // The header file is in src/libasr/runtime for development, but in impure
    // in installed version
    std::string path;
    int dirname_length;
    get_executable_path(path, dirname_length);
    std::string dirname = path.substr(0,dirname_length);
    if (   endswith(dirname, "src/bin")
        || endswith(dirname, "src\\bin")
        || endswith(dirname, "SRC\\BIN")) {
        // Development version
        return dirname + "/../libasr/runtime";
    } else if (endswith(dirname, "src/lpython/tests") ||
               endswith(to_lower(dirname), "src\\lpython\\tests")) {
        // CTest Tests
        return dirname + "/../../libasr/runtime";
    } else {
        // Installed version
        return dirname + "/" + CMAKE_INSTALL_INCLUDEDIR_RELATIVE + "/lfortran/impure";
    }

    return path;
}

std::string get_dwarf_scripts_dir()
{
    char *env_p = std::getenv("LFORTRAN_DWARF_SCRIPTS_DIR");
    if (env_p) return std::string(env_p);

    std::string path;
    int dirname_length;
    get_executable_path(path, dirname_length);
    std::string dirname = path.substr(0,dirname_length);
    if (   endswith(dirname, "src/bin")
        || endswith(dirname, "src\\bin")
        || endswith(dirname, "SRC\\BIN")) {
        // Development version
        return dirname + "/../libasr";
    } else if (endswith(dirname, "src/lpython/tests") ||
               endswith(to_lower(dirname), "src\\lpython\\tests")) {
        // CTest Tests
        return dirname + "/../../libasr";
    } else {
        // Installed version
        return dirname + "/../share/lfortran";
    }
    return path;
}

// Decodes the exit status code of the process (in Unix)
// See `WEXITSTATUS` for more information.
// https://stackoverflow.com/a/27117435/15913193
// https://linux.die.net/man/3/system
int32_t get_exit_status(int32_t err) {
    return (((err) >> 8) & 0x000000ff);
}

std::string get_kokkos_includedir()
{
    std::string include("/include");
    char *env_p = std::getenv("LFORTRAN_KOKKOS_DIR");
    if (env_p) return env_p + include;
#ifdef KOKKOS_INCLUDEDIR
    return KOKKOS_INCLUDEDIR;
#else
    std::cerr << "The code C++ generated by the C++ LFortran backend uses the Kokkos library" << std::endl;
    std::cerr << "(https://github.com/kokkos/kokkos). Please define the LFORTRAN_KOKKOS_DIR" << std::endl;
    std::cerr << "environment variable to point to the Kokkos installation." << std::endl;
    throw LCompilersException("LFORTRAN_KOKKOS_DIR is not defined");
#endif
}

std::string get_kokkos_libdir()
{
    std::string lib("/lib");
    char *env_p = std::getenv("LFORTRAN_KOKKOS_DIR");
    if (env_p) return env_p + lib;
#ifdef KOKKOS_LIBDIR
    return KOKKOS_LIBDIR;
#else
    throw LCompilersException("LFORTRAN_KOKKOS_DIR is not defined");
#endif
}

} // namespace LCompilers::LFortran
