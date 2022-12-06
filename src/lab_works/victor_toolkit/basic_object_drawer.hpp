#pragma once


#include "GL/gl3w.h"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "glm/gtc/type_ptr.hpp"

class BasicObjectDrawer
{
  public: 
	GLuint drawQuad()
	{
		int _indices[ 6 ] = { 0, 1, 2, 2, 1, 3 };
		// Les sommets du rectangle
		float _vertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
		};

		unsigned int quadVAO, _vbo, _ebo;

		glCreateBuffers( 1, &_vbo );
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _vbo, sizeof( _vertices ), _vertices, GL_STATIC_DRAW );

		glNamedBufferData( _ebo, sizeof( _indices ), _indices, GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &quadVAO );

		glEnableVertexArrayAttrib( quadVAO, 0 );
		glEnableVertexArrayAttrib( quadVAO, 1 );

		glVertexArrayAttribFormat( quadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof( float ) );

		glVertexArrayAttribBinding( quadVAO, 0, 0 );
		glVertexArrayAttribBinding( quadVAO, 1, 0 );

		glVertexArrayVertexBuffer( quadVAO, 0, _vbo, 0, 5 * sizeof( float ) );

		glVertexArrayElementBuffer( quadVAO, _ebo );

		return quadVAO;
	}

};