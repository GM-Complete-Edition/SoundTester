#pragma once

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32Bool;
typedef int i32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;
#define FALSE 0
#define TRUE 1
#define FALSE_TEXT "FALSE"
#define TRUE_TEXT "TRUE"

//flags
#define FLAG_LOG_FILE_EXIST 1 << 0
#define FLAG_LOG_INVALID_SOUND_LENGTH 1 << 1
#define FLAG_LOG_WAV 1 << 2
#define FLAG_LOG_OGG 1 << 3
#define FLAG_LOG_SOUND_DATA FLAG_LOG_WAV | FLAG_LOG_OGG
#define FLAG_LOG_ALL FLAG_LOG_FILE_EXIST | FLAG_LOG_INVALID_SOUND_LENGTH | FLAG_LOG_SOUND_DATA
#define FLAG_FIX_SOUND_LENGTH 1 << 4
#define FLAG_COMMENT_MISSING_SOUNDS 1 << 5

i32Bool fexist(const i8* name)
{
	FILE *fp = fopen(name, "r");

	if (fp)
	{
		fclose(fp);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

_inline const i8 *remove_new_line(i8 *txt)
{
	txt[strcspn(txt, "\n")] = NULL;
	return txt;
}