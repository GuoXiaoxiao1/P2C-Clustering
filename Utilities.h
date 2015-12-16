////////////////////////////////////////////////////////////////////////////////////
// Copyright © Charalambos "Charis" Poullis, charalambos@poullis.org    	  //
// This work can only be used under an exclusive license of the author.           //
////////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include "Vector.h"
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#define _USE_MATH_DEFINES
#include <math.h>

///To convert from radians to degrees and back
#define RADIANS_TO_DEGREES	57.295779513082320876798
#define DEGREES_TO_RADIANS	0.0174532925199432957692

#define INVALID_VALUE	666

#define EPSILON   1e-06

#define _MAX_PATH	256

///Helper function for _format(...)
static std::string vformat(const char *fmt, va_list argPtr) {
    /// We draw the line at a 1MB string.
    const int maxSize = 1000000;

    /// If the string is less than 161 characters,
    /// allocate it on the stack because this saves
    /// the malloc/free time.
    const int bufSize = 512;
    char stackBuffer[bufSize];

    int attemptedSize = bufSize - 1;

    int numChars = vsnprintf(stackBuffer, attemptedSize, fmt, argPtr);

    if (numChars >= 0) {
        /// Got it on the first try.
		//printf("%s\n",stackBuffer);
		return std::string(stackBuffer);
    }

    /// Now use the heap.
    char* heapBuffer = NULL;

	while ((numChars == -1 || numChars >= attemptedSize) && (attemptedSize < maxSize)) {
        /// Try a bigger size
        attemptedSize *= 2;
        heapBuffer = (char*)realloc(heapBuffer, attemptedSize + 1);
        numChars = vsnprintf(heapBuffer, attemptedSize, fmt, argPtr);
    }

	//printf("%s\n",heapBuffer);
    std::string result = std::string(heapBuffer);

    free(heapBuffer);

    return result;
}

///Prints out a string given a set of parameters. Like printf but for strings.
static std::string _format(const char* fmt ...) {
    va_list argList;
    va_start(argList,fmt);
    std::string result = vformat(fmt, argList);
    va_end(argList);
    return result;
}

///Function for timestamp. Returns the time in the format YMDHMS
static std::string timestamp()	{
  ///TIMESTAMP prints the current YMDHMS date as a time stamp.
  #define TIME_SIZE	40

  static char time_buffer[TIME_SIZE];
  time_t now = time ( NULL );
  const struct tm *tm = localtime ( &now );

  size_t len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  #undef TIME_SIZE

  return _format("%s",time_buffer);
}

///Clamps a value between the range [a,b]
inline float clamp(float x, float a, float b)
{
    return x < a ? a : (x > b ? b : x);
}

///Copies the string and returns a pointer to the copied string
inline void copy_string(char *input, char *&output)	{
	output = new char[strlen(input)+1];
	for (unsigned int i=0;i<strlen(input)+1;i++)	{
		output[i] = input[i];
	}
	return;
}

inline void copy_strings(char **input, int number_of_strings, char **&output)	{
	output = new char*[number_of_strings];
	for (unsigned int i=0;i<number_of_strings;i++)	{
		copy_string(input[i],output[i]);
	}
	return;
}

/// Function to return the current working directory
/// this is generally the application path
static void getCurrentPath(char* buffer)	{
	getcwd(buffer, _MAX_PATH);
	return;
}

static std::string fullPath(const char *relative_path)	{
	// _MAX_PATH is the maximum length allowed for a path
	char working_dir[_MAX_PATH];
	// use the function to get the path
	getCurrentPath(working_dir);

	//std::cout << "Working directory: " << working_dir << std::endl;

	std::string full_path = std::string(working_dir) + "/" + std::string(relative_path);

	return full_path;
}

inline static unsigned short int float2short(float f)	{
  unsigned int bits = * (unsigned int *) &f;
  return (bits >> 15);
}

inline static unsigned short int signedfloat2short(float f)	{
  unsigned int bits = * (unsigned int *) &f;
  return (bits >> 16);
}

inline static float short2float(unsigned short int s)	{
	unsigned int bits = s << 15;
	return * (float *) &bits;
}

inline static float short2signedfloat(unsigned short int s)	{
	unsigned int bits = s << 16;
	return * (float *) &bits;
}

inline int _round(float num)	{
	if (ceil(num)-num > 0.5f)	{
		return int(floor(num));
	}
	else	{
		return int(ceil(num));
	}
}

///Comparison function for two ints using the std::sort
template<class T>
static bool compare_func(const T &v1, const T &v2)	{
	return (v1 < v2);
}

template<class T>
static void remove(std::vector<T *> &vec, int args, T *v1, ...)	{
 	va_list a_list;
	//Initialize the va_list i.e T variable 'list' by the address
	//of first unknown variable argument by a call of va_start() macro
	va_start(a_list, v1);
	//In loop, retrieve each argument
	//second argument to va_arg is datatype
	//of expected argument
	typename std::vector<T *>::iterator it = vec.begin();
	for (int i=0;i<vec.size();i++,it++)	{
		if (vec[i] == (T *) v1)	{
			delete vec[i];
			vec.erase(it);
			break;
		}
	}
	for (int a=1;a<args;a++)	{
		T *v = va_arg(a_list, T *);
		typename std::vector<T *>::iterator it = vec.begin();
		for (int i=0;i<vec.size();i++,it++)	{
			if (vec[i] == v)	{
				delete vec[i];
				vec.erase(it);
				break;
			}
		}
	}
	va_end(a_list);
	return;
}

template<class T>
static void remove(std::vector<T *> &vec, std::vector<T *> const &to_be_removed)	{

	for (int i=0;i<to_be_removed.size();i++)	{
		typename std::vector<T *>::iterator it = vec.begin();
		bool not_found = false;
		for (int j=0;it!=vec.end();j++,it++)	{
			if (to_be_removed[i] == vec[j])	{
				delete vec[j];
				vec.erase(it);
				not_found = true;
				break;
			}
		}
		if (!not_found)	{
			printf("here 4\n");
			exit(0);
		}
	}
	return;
}

template<class T>
static void remove(std::vector<T> &vec, std::vector<T> const &to_be_removed)	{

	for (int i=0;i<to_be_removed.size();i++)	{
		typename std::vector<T>::iterator it = vec.begin();
		bool not_found = false;
		for (int j=0;it!=vec.end();j++,it++)	{
			if (to_be_removed[i] == vec[j])	{
				vec.erase(it);
				not_found = true;
				break;
			}
		}
		if (!not_found)	{
			printf("here 5\n");
			exit(0);
		}
	}
	return;
}

///to_be_removed MUST be in ascending order because the function reads it in reverse
template<class T>
static void remove(std::vector<T *> &vec, std::vector<int> const &to_be_removed)	{

	for (int i=to_be_removed.size()-1;i>=0;i--)	{
		typename std::vector<T *>::iterator it = vec.begin() + to_be_removed[i];
		delete vec[to_be_removed[i]];
		vec.erase(it);
	}
	return;
}

///to_be_removed MUST be in ascending order because the function reads it in reverse
template<class T>
static void remove(std::vector<T> &vec, std::vector<int> const &to_be_removed)	{

	for (int i=to_be_removed.size()-1;i>=0;i--)	{
		typename std::vector<T>::iterator it = vec.begin() + to_be_removed[i];
		vec.erase(it);
	}
	return;
}



#endif
