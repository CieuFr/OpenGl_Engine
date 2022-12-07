#ifndef __UTILS__
#define __UTILS__

#include <fstream>
#include <sstream>
#include <string>
#include "GL/gl3w.h"


static inline std::string readFile( const std::string & p_filePath )
{
	std::ifstream ifs( p_filePath, std::ifstream::in );
	if ( !ifs.is_open() )
		throw std::ios_base::failure( "Cannot open file: " + p_filePath );

	std::stringstream s;
	s << ifs.rdbuf();
	ifs.close();
	return s.str();
}


static float myLerp( float a, float b, float f ) { return a + f * ( b - a ); }

#endif