#pragma once

#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"
#include "textdata.h"

i32 load_ogg(OggVorbis_File &OggData, const i8 *filename)
{

	FILE *fp;
	vorbis_info *vi;

	memset(&OggData, 0, sizeof(OggVorbis_File));

	if (!(fp = fopen(filename, "rb")))
	{
		Log->printf("load_ogg: Failed to open OGG file:= %s", filename);
		return FALSE;
	}

	if (ov_open(fp, &OggData, NULL, 0) < 0)
	{
		Log->printf("load_ogg: : Failed to open OGG file with ov_open:= := %s", filename);
		fclose(fp);
		return FALSE;
	}
	if (!(vi = ov_info(&OggData, -1)))
	{
		Log->printf("load_ogg:: Failed to get ov_info:= %s", filename);
		return FALSE;
	}
	if ((vi->channels = ov_pcm_total(&OggData, -1)) == OV_EINVAL)
	{
		Log->printf("load_ogg: The number oof channels is different:= %s", filename);
		return FALSE;
	}
	fclose(fp);
	return TRUE;

}