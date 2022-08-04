#include <iostream>
#include <cstdarg>
#include <cassert>
#include <time.h>
#include "imadethis.h"
#include "AudioFile.h"
#include "OggFile.h"
#include "SoundTester.h"
#include "textdata.h"

using namespace std;

void print_help()
{
	printf("Available Flags:\n");
	printf("-lfe := Logs if sound file exits\n");
	printf("-lisl := Logs invalid sound length for sound data\n");
	printf("-lsd := Logs information about WAV and OGG sound data\n");
	printf("-lall := Turns on all the flags above\n");
	printf("-fsl := Applies correct sound lenghts to sound data entries\n");
	printf("-cms := Comments invalid sound data entries\n");
	printf("-file [SND_FILE_PATH]:= Selects relative path to the Sound Data file(Requeired!)\n");
}

int main(int argc, char *argv[])
{

	i32 ret = 0;

	if (argc < 2)
	{
		print_help();
		return ret;
	}
	else
	{
		Flags = 0;

		i8 sdpath[_MAX_PATH];
		i8 nsdpath[_MAX_PATH];
		i8 logpath[_MAX_PATH];

		for (i32 i = 1; i < argc; i++)
		{
			if (!strcmp("-lfe", argv[i]))
			{
				Flags |= FLAG_LOG_FILE_EXIST;
			}
			else if (!strcmp("-lisl", argv[i]))
			{
				Flags |= FLAG_LOG_INVALID_SOUND_LENGTH;
			}
			else if (!strcmp("-lsd", argv[i]))
			{
				Flags |= FLAG_LOG_SOUND_DATA;
			}
			else if (!strcmp("-lall", argv[i]))
			{
				Flags |= FLAG_LOG_ALL;
			}
			else if (!strcmp("-fsl", argv[i]))
			{
				Flags |= FLAG_FIX_SOUND_LENGTH;
			}
			else if (!strcmp("-cms", argv[i]))
			{
				Flags |= FLAG_COMMENT_MISSING_SOUNDS;
			}
			else if (!strcmp("-file", argv[i]))
			{
				if ((i + 1) < argc)
				{
					strcpy(sdpath, argv[i + 1]);
					if (Flags & FLAG_FIX_SOUND_LENGTH || FLAG_COMMENT_MISSING_SOUNDS)
					{
						strcpy(nsdpath, sdpath);
						strcat(nsdpath, ".new");
						SdFile = new ProgLog(nsdpath);
					}
					//no more flags
					break; 
				}
				else
				{
					printf("Invalid -file argument!\n");
					print_help();
					return ret;
				}
			}
			else
			{
				printf("Invalid flag:= %s\n", argv[i]);
				print_help();
				return ret;
			}
		}

		strcpy(logpath, sdpath);
		strcat(logpath, ".log");

		Log = new ProgLog(logpath);
		Log->printf("This is an AUTO-GENERATED file by SoundTester");
		Log->printf("Resource generated from file: %s", sdpath);
	
		time_t now;
		time(&now);
		Log->printf(ctime(&now));

		ret = load_sd(sdpath);

		delete SdFile;
		delete Log;
	}
	system("pause");
	return ret;
}