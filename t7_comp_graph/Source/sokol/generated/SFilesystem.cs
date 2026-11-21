// machine generated, do not edit
using System;
using System.Runtime.InteropServices;
using M = System.Runtime.InteropServices.MarshalAsAttribute;
using U = System.Runtime.InteropServices.UnmanagedType;

namespace Sokol
{
public static unsafe partial class SFilesystem
{
public enum sfs_result_t
{
    SFS_RESULT_OK = 0,
    SFS_RESULT_ERROR = -1,
    SFS_RESULT_NOT_FOUND = -2,
    SFS_RESULT_PERMISSION = -3,
    SFS_RESULT_INVALID_PARAM = -4,
}
public enum sfs_path_type_t
{
    SFS_PATHTYPE_NONE = 0,
    SFS_PATHTYPE_FILE = 1,
    SFS_PATHTYPE_DIRECTORY = 2,
    SFS_PATHTYPE_OTHER = 3,
}
[StructLayout(LayoutKind.Sequential)]
public struct sfs_path_info_t
{
    public sfs_path_type_t type;
    public long size;
    public long create_time;
    public long modify_time;
    public long access_time;
}
public enum sfs_folder_t
{
    SFS_FOLDER_HOME = 0,
    SFS_FOLDER_DESKTOP = 1,
    SFS_FOLDER_DOCUMENTS = 2,
    SFS_FOLDER_DOWNLOADS = 3,
    SFS_FOLDER_MUSIC = 4,
    SFS_FOLDER_PICTURES = 5,
    SFS_FOLDER_PUBLICSHARE = 6,
    SFS_FOLDER_SAVEDGAMES = 7,
    SFS_FOLDER_SCREENSHOTS = 8,
    SFS_FOLDER_TEMPLATES = 9,
    SFS_FOLDER_VIDEOS = 10,
    SFS_FOLDER_COUNT,
}
public enum sfs_enum_result_t
{
    SFS_ENUM_CONTINUE = 0,
    SFS_ENUM_SUCCESS = 1,
    SFS_ENUM_FAILURE = -1,
}
public enum sfs_glob_flags_t
{
    SFS_GLOB_NONE = 0,
    SFS_GLOB_CASE_INSENSITIVE = 1,
}
public enum sfs_whence_t
{
    SFS_WHENCE_SET = 0,
    SFS_WHENCE_CUR = 1,
    SFS_WHENCE_END = 2,
}
public enum sfs_open_mode_t
{
    SFS_OPEN_READ = 0,
    SFS_OPEN_WRITE = 1,
    SFS_OPEN_APPEND = 2,
    SFS_OPEN_READ_WRITE = 3,
    SFS_OPEN_CREATE_WRITE = 4,
    SFS_OPEN_APPEND_READ = 5,
}
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_set_android_internal_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_set_android_internal_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sfs_set_android_internal_path([M(U.LPUTF8Str)] string path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_base_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_base_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_base_path();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_pref_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_pref_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_pref_path([M(U.LPUTF8Str)] string org, [M(U.LPUTF8Str)] string app);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_user_folder", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_user_folder", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_user_folder(sfs_folder_t folder);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_current_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_current_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_current_directory();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_set_current_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_set_current_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_set_current_directory([M(U.LPUTF8Str)] string path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_temp_dir", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_temp_dir", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_temp_dir();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_assets_dir", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_assets_dir", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_get_assets_dir();

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_free_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_free_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sfs_free_path(IntPtr path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_create_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_create_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_create_directory([M(U.LPUTF8Str)] string path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_remove_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_remove_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_remove_path([M(U.LPUTF8Str)] string path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_rename_path", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_rename_path", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_rename_path([M(U.LPUTF8Str)] string oldpath, [M(U.LPUTF8Str)] string newpath);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_copy_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_copy_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_copy_file([M(U.LPUTF8Str)] string oldpath, [M(U.LPUTF8Str)] string newpath);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_path_info", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_path_info", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_get_path_info([M(U.LPUTF8Str)] string path, sfs_path_info_t* out_info);

#if WEB
[DllImport("sokol", EntryPoint = "sfs_path_exists", CallingConvention = CallingConvention.Cdecl)]
private static extern int sfs_path_exists_native([M(U.LPUTF8Str)] string path);
public static bool sfs_path_exists([M(U.LPUTF8Str)] string path) => sfs_path_exists_native(path) != 0;
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_path_exists", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_path_exists", CallingConvention = CallingConvention.Cdecl)]
#endif
[return: M(U.I1)]
public static extern bool sfs_path_exists([M(U.LPUTF8Str)] string path);
#endif

#if WEB
[DllImport("sokol", EntryPoint = "sfs_is_directory", CallingConvention = CallingConvention.Cdecl)]
private static extern int sfs_is_directory_native([M(U.LPUTF8Str)] string path);
public static bool sfs_is_directory([M(U.LPUTF8Str)] string path) => sfs_is_directory_native(path) != 0;
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_is_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_is_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
[return: M(U.I1)]
public static extern bool sfs_is_directory([M(U.LPUTF8Str)] string path);
#endif

#if WEB
[DllImport("sokol", EntryPoint = "sfs_is_file", CallingConvention = CallingConvention.Cdecl)]
private static extern int sfs_is_file_native([M(U.LPUTF8Str)] string path);
public static bool sfs_is_file([M(U.LPUTF8Str)] string path) => sfs_is_file_native(path) != 0;
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_is_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_is_file", CallingConvention = CallingConvention.Cdecl)]
#endif
[return: M(U.I1)]
public static extern bool sfs_is_file([M(U.LPUTF8Str)] string path);
#endif

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_last_modified_time", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_last_modified_time", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_get_last_modified_time([M(U.LPUTF8Str)] string path);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_enumerate_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_enumerate_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_enumerate_directory([M(U.LPUTF8Str)] string path, IntPtr callback, void* userdata);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_glob_directory", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_glob_directory", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_glob_directory([M(U.LPUTF8Str)] string path, [M(U.LPUTF8Str)] string pattern, sfs_glob_flags_t flags, ref int out_count);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_free_glob_results", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_free_glob_results", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern void sfs_free_glob_results(IntPtr results, int count);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_open_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_open_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern IntPtr sfs_open_file([M(U.LPUTF8Str)] string path, sfs_open_mode_t mode);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_close_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_close_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_close_file(IntPtr file);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_read_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_read_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_read_file(IntPtr file, void* buf, long count);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_write_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_write_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_write_file(IntPtr file, void* buf, long count);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_seek_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_seek_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_seek_file(IntPtr file, long offset, sfs_whence_t whence);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_tell_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_tell_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_tell_file(IntPtr file);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_file_size", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_file_size", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern long sfs_get_file_size(IntPtr file);

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_flush_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_flush_file", CallingConvention = CallingConvention.Cdecl)]
#endif
public static extern sfs_result_t sfs_flush_file(IntPtr file);

#if WEB
[DllImport("sokol", EntryPoint = "sfs_eof_file", CallingConvention = CallingConvention.Cdecl)]
private static extern int sfs_eof_file_native(IntPtr file);
public static bool sfs_eof_file(IntPtr file) => sfs_eof_file_native(file) != 0;
#else
#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_eof_file", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_eof_file", CallingConvention = CallingConvention.Cdecl)]
#endif
[return: M(U.I1)]
public static extern bool sfs_eof_file(IntPtr file);
#endif

#if __IOS__
[DllImport("@rpath/sokol.framework/sokol", EntryPoint = "sfs_get_error", CallingConvention = CallingConvention.Cdecl)]
#else
[DllImport("sokol", EntryPoint = "sfs_get_error", CallingConvention = CallingConvention.Cdecl)]
#endif
private static extern IntPtr sfs_get_error_native();

public static string sfs_get_error()
{
    IntPtr ptr = sfs_get_error_native();
    if (ptr == IntPtr.Zero)
        return "";

    // Manual UTF-8 to string conversion to avoid marshalling corruption
    try
    {
        return Marshal.PtrToStringUTF8(ptr) ?? "";
    }
    catch
    {
        // Fallback in case of any marshalling issues
        return "";
    }
}

}
}
