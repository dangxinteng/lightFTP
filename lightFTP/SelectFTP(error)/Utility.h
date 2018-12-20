#pragma once
#include<string>
void strupper(char *str)
{
	while (*str != '\0')
	{
		if (*str >= 'a' && *str <= 'z')
			*str ^= 32;
		str++;
	}	
}
void replace(char * p, char cOld, char cNew)
{
	while (*p)
	{
		if (*p == cOld)
			*p = cNew;
		++p;
	}
}

const char *monthStr[] =
{
	"Jan"
	,"Feb"
	,"Mar"
	,"Apr"
	,"May"
	,"Jun"
	,"Jul"
	,"Aug"
	,"Sep"
	,"Oct"
	,"Dec"
	,"Nov"
	,NULL
};
#ifndef _WIN32
#define localtime_s(x, y) localtime_r(y, x);
#endif
std::string get_file_time(time_t tt)
{
	char s[32];
	time_t now = time(NULL);
	tm t1, t2;
	localtime_s(&t1, &tt);

	localtime_s(&t2, &now);

	if (t1.tm_year == t2.tm_year)
		sprintf(s, "%s %d %d:%d", monthStr[t1.tm_mon], t1.tm_mday, t1.tm_hour, t1.tm_min);
	else
		sprintf(s, "%s %d %d", monthStr[t1.tm_mon], t1.tm_mday, 1900 + t1.tm_year);

	return s;
}