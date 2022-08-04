#pragma once

//defines
#define SD_FILE_LINE_LENGTH 512
#define SD_FILE_MAX_TAG_LENGTH 128
#define SD_FILE_MAX_SND_LENGTH 32

enum SDFileEntry
{
	PARAM_SOUND_TAG,
	PARAM_SOUND_DIMENSIONS,
	PARAM_SOUND_WAV,
	PARAM_SOUND_QUALITY,
	PARAM_SOUND_STREAM,
	PARAM_FILE_LENGTH,
	PARAM_LAST
};

//IMPLEMENTATION
i32 load_sd(const i8 *filename);
i32 load_sd_list(const i8 *filename, const i8 *pathname);
i32Bool read_bool(i32Bool *pVal, i8 *lineStr);
i32Bool read_token(i8 *paramStr, i8 *lineStr, u32 max_len);
i32Bool read_text(i8 *paramStr, i8 *lineStr, u32 max_len);

// flags used for the program to determine what tests to run
static u32 Flags = 0;

//DEFINITIONS
i32 load_sd(const i8 *sndfilepath)
{
	i8 fpathname[_MAX_PATH];
	i8 ffilename[_MAX_PATH];

	strcpy(fpathname, sndfilepath);
	strcpy(ffilename, sndfilepath);

	for (i32 i = strlen(fpathname);;i--)
	{
		if (fpathname[i] == '\\')
		{
			fpathname[i + 1] = '\0';
			break;
		}
	}

	return load_sd_list(ffilename, fpathname);
}


static i8 srcCopy[SD_FILE_LINE_LENGTH];
static i32 currParam;

i32 load_sd_list(const i8 *filename, const i8 *pathname)
{
	FILE *pFile = fopen(filename, "rt");
	if (!pFile)
	{
		Log->printf("Error! Failed to open sound dictionary file:= %s", filename);
		return FALSE;
	}

	//for logs
	i8 fpathname[_MAX_PATH];
	FILE *soundFile = NULL;
	i32 temp_sound_length;
	OggVorbis_File OggData;
	i32 currline = 0;
	i8 currlineTXT[SD_FILE_LINE_LENGTH];
	u32 nSndLen = 0;

	//strings for each parameter
	i8 tag[SD_FILE_MAX_TAG_LENGTH + 1];
	i8 wav[SD_FILE_MAX_TAG_LENGTH + 1];
	i8 length_str[SD_FILE_MAX_SND_LENGTH];
	i8 dimension[3];
	i32Bool snd_2D;
	u32 sndLen;
	i32Bool snd_quality;
	i32Bool snd_stream;
	u32 sound_length;

	while (fgets(currlineTXT, SD_FILE_LINE_LENGTH, pFile))
	{
		// if it's not blank, a comment or new line then
		if (!((strlen(currlineTXT) == 0) || (currlineTXT[0] == '#') || (currlineTXT[0] == '\n')))
		{
			currParam = PARAM_SOUND_TAG;
			sndLen = 0;

			while (currParam <= PARAM_LAST)
			{
				switch (currParam)
				{
				case PARAM_SOUND_TAG:
					if (!read_token(tag, currlineTXT, SD_FILE_MAX_TAG_LENGTH))
						Log->printf("Error! Incorrect SOUND_TAG PARAMETER at line: %d", currline);
					currParam++;
					break;
				case PARAM_SOUND_DIMENSIONS:
					if (!read_token(dimension, currlineTXT, 2))
						Log->printf("Error! Incorrect SOUND_DIMENSIONS PARAMETER at line: %d", currline);
					if (!strcmp(dimension, "2D"))
						snd_2D = TRUE;
					else if (!strcmp(dimension, "3D"))
						snd_2D = FALSE;
					else
						Log->printf("Error! Invalid SOUND_DIMENSIONS PARAMETER at line: %d", currline);
					currParam++;
					break;

				case PARAM_SOUND_WAV:
					if (!read_text(wav, currlineTXT, SD_FILE_MAX_TAG_LENGTH))
						Log->printf("Error! Incorrect SOUND_WAV PARAMETER at line: %d", currline);
					currParam++;
					break;
				case PARAM_SOUND_QUALITY:
					snd_quality = FALSE;
					if (!read_bool(&snd_quality, currlineTXT))
						Log->printf("Error! Incorrect SOUND_QUALITY PARAMETER at line: %d", currline);
					currParam++;
					break;
				case PARAM_SOUND_STREAM:
					snd_stream = FALSE;
					if (!read_bool(&snd_stream, currlineTXT))
						Log->printf("Error! Incorrect SOUND_STREAM PARAMETER at line: %d", currline);
					currParam++;
					break;
				case PARAM_FILE_LENGTH:
					if (read_token(length_str, currlineTXT, SD_FILE_MAX_SND_LENGTH))
						sound_length = atoi(length_str);
					else
						Log->printf("Error! Incorrect FILE_LENGTH PARAMETER at line: %d", currline);
					currParam++;
					break;
				case PARAM_LAST:
					strcpy(fpathname, pathname);
					strcat(fpathname, wav);
					if (fexist(fpathname))
					{
						nSndLen = 0;
						soundFile = fopen(fpathname, "rt");
						if (!soundFile)
						{
							Log->printf("Error! Failed to open sound file:= %s", fpathname);
						}
						else
						{
							//if file exists then determine data type!
							i8 header[5];
							fgets(header, sizeof(header), soundFile);
							fclose(soundFile);
							if (strncmp(header, "RIFF", 4) == 0)
							{
								AudioFile<float> a;
								a.load(fpathname);
								nSndLen = u32(a.getLengthInSeconds() * 1000);

								if (Flags && FLAG_LOG_WAV)
								{
									Log->printf("WAV: %s,%s,\"%s\",%d,%d,%d", tag, dimension, wav, snd_quality, snd_stream, nSndLen);
								}

								if (Flags & FLAG_LOG_INVALID_SOUND_LENGTH)
								{
									if (nSndLen != sound_length)
									{
										Log->printf("WAV FILE: Incorrect duration of the file: %s Excepted value:= %d, Received Value:= %d", wav, sound_length, nSndLen);
									}
								}
							}
							else if (strncmp(header, "Ogg", 3) == 0)
							{
								if (load_ogg(OggData, fpathname))
								{
									nSndLen = u32(ov_time_total(&OggData, -1) * 1000);

									if (Flags && FLAG_LOG_OGG)
									{
										Log->printf("OGG: %s,%s,\"%s\",%d,%d,%d", tag, dimension, wav, snd_quality, snd_stream, nSndLen);
									}

									if (Flags & FLAG_LOG_INVALID_SOUND_LENGTH)
									{
										if (nSndLen != sound_length)
										{

											Log->printf("OGG FILE: Incorrect duration of the file: %s Excepted value:= %d, Received Value:= %d", wav, sound_length, nSndLen);
										}
									}
								}
							}
							else
							{
								Log->printf("Error! Unknown sound data type. file:= %s", wav);
							}
						}

						if (Flags & FLAG_FIX_SOUND_LENGTH)
							SdFile->printf("%s,%s,\"%s\",%d,%d,%d", tag, dimension, wav, snd_quality, snd_stream, nSndLen);
					}
					else
					{
						if (Flags & FLAG_LOG_FILE_EXIST)
							Log->printf("Error! File:= %s was not found!", wav);

						if (Flags & FLAG_COMMENT_MISSING_SOUNDS)
							SdFile->printf("# %s,%s,\"%s\",%d,%d,0", tag, dimension, wav, snd_quality, snd_stream);
					}
					currParam++;
					break;
				}
			}
		}
		else
		{
			//print uncommented line
			if (Flags & FLAG_FIX_SOUND_LENGTH || Flags & FLAG_COMMENT_MISSING_SOUNDS)
				SdFile->printf(remove_new_line(currlineTXT));
		}
		currline++;
	}
	fclose(pFile);
	return TRUE;
}

i32Bool read_bool(i32Bool *pVal, i8 *lineStr)
{
	_ASSERT(pVal);
	_ASSERT(lineStr);

	i8 seps[] = " ,\t\n";

	if (currParam == PARAM_SOUND_TAG)
		strcpy(srcCopy, lineStr);

	i8* token = strtok((currParam == PARAM_SOUND_TAG) ? srcCopy : NULL, seps);

	if (token == NULL)
		return FALSE;

	*pVal = (atoi(token) == 1);

	return TRUE;
}

i32Bool read_token(i8 *paramStr, i8 *lineStr, u32 max_len)
{
	_ASSERT(paramStr);
	_ASSERT(lineStr);

	i8 seps[] = " ,\t\n";

	if (currParam == PARAM_SOUND_TAG)
		strcpy(srcCopy, lineStr);

	i8* token = strtok((currParam == PARAM_SOUND_TAG) ? srcCopy : NULL, seps);

	if ((token == NULL) || (strlen(token) > max_len))
		return FALSE;

	strcpy(paramStr, token);

	return TRUE;
}
i32Bool read_text(i8 *paramStr, i8 *lineStr, u32 max_len)
{
	if (!read_token(paramStr, lineStr, max_len + 2))
		return FALSE;

	//remove "'s from the string
	if ((strlen(paramStr) <= 2) || (paramStr[0] != '"') || (paramStr[strlen(paramStr) - 1] != '"'))
		return FALSE;

	i8 temp[SD_FILE_LINE_LENGTH];
	u32 orig_len = strlen(paramStr);
	strcpy(temp, paramStr);
	strncpy(paramStr, temp + 1, orig_len - 2);
	paramStr[orig_len - 2] = NULL;

	return TRUE;
}