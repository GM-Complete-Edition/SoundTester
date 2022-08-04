#pragma once

#define PROG_LOG_BUFF_SIZE 1024

class ProgLog
{
public:
	FILE *fp;

	ProgLog();
	ProgLog(const i8 *filename);
	~ProgLog();

	void printf(const i8 *s, ...);

	void addstr(const i8 *str);
	i32Bool setlogfile(const i8 *filename);

	void close();
		
};

ProgLog::ProgLog()
{
	fp = NULL;
}

ProgLog::ProgLog(const i8 *filename)
{
	setlogfile(filename);
}

ProgLog::~ProgLog()
{
	close();
}

void ProgLog::printf(const i8 *s, ...)
{
	va_list args;
	i8 buf[PROG_LOG_BUFF_SIZE];

	va_start(args, s);
	vsprintf(buf, s, args);
	va_end(args);

	addstr(buf);
}

void ProgLog::addstr(const i8 *str)
{
	if (fp)
	{
		fprintf(fp, "%s\n", str);
		fflush(fp);
	}
}

i32Bool ProgLog::setlogfile(const i8 *filename)
{
	fp = fopen(filename, "wt");
	return (fp != NULL);
}

void ProgLog::close()
{
	if (fp)
		fclose(fp);
	fp = NULL;
}

ProgLog *Log = NULL;
ProgLog *SdFile = NULL;

