#ifndef VERSION_H
#define VERSION_H
/******************************************
 ________________VERSION API_______________
*******************************************/
#define STRINGIZE_DO_192F(x) #x
#define STRINGIZE_192F(x) STRINGIZE_DO_192F(x)

#define APP_VERSION_MAJOR_192F 0
#define APP_VERSION_MINOR_192F 0
#define APP_VERSION_PATCH_192F 3
#define APP_VERSION_STRING_192F STRINGIZE_192F(APP_VERSION_MAJOR_192F) "." STRINGIZE_192F(APP_VERSION_MINOR_192F) "." STRINGIZE_192F(APP_VERSION_PATCH_192F)

static const char APP_NAME[] = "Green SCHIP8 Emulator";
static const char APP_AUTHER[] = "Pavel314";
static const char APP_BUILD_DATE[] = __DATE__; // "Mmm dd yyyy"
static const char APP_BUILD_TIME[] = __TIME__; // "hh:mm:ss"
static const char APP_DEV_DATE[] = "2026.01.27"; // DEV_DATE - Represents the date range during which the application was actively developed (for nostalgic purposes in the future).
/*END VERSION API*/
#endif