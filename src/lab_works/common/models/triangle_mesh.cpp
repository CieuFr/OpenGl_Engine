#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL()
	{		
		glCreateBuffers( 1, &_vbo );
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _vbo,
						   _vertices.size() * sizeof( Vec3f ),
						   _vertices.data(),
						   GL_STATIC_DRAW );

			glNamedBufferData(
			_ebo, _indices.size() * sizeof( int ), _indices.data(), GL_STATIC_DRAW );


		glCreateVertexArrays( 1, &_vao );

		glEnableVertexArrayAttrib( _vao, 0 );
		glEnableVertexArrayAttrib( _vao, 1 );
		glEnableVertexArrayAttrib( _vao, 2 );
		glEnableVertexArrayAttrib( _vao, 3 );
		glEnableVertexArrayAttrib( _vao, 4 );

		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 2, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, offsetof( Vertex, _position ), sizeof( Vec3f ) );
		glVertexArrayVertexBuffer( _vao, 1, _vbo, offsetof( Vertex, _normal ), sizeof( Vec3f ) );
		glVertexArrayVertexBuffer( _vao, 2, _vbo, offsetof( Vertex, _texCoords ), sizeof( Vec3f ) );
		glVertexArrayVertexBuffer( _vao, 3, _vbo, offsetof( Vertex, _tangent ), sizeof( Vec3f ) );
		glVertexArrayVertexBuffer( _vao, 4, _vbo, offsetof( Vertex, _bitangent ), sizeof( Vec3f ) );

		glVertexArrayAttribBinding( _vao, 0, 0 );
		glVertexArrayAttribBinding( _vao, 1, 1 );
		glVertexArrayAttribBinding( _vao, 2, 2 );
		glVertexArrayAttribBinding( _vao, 3, 3 );
		glVertexArrayAttribBinding( _vao, 4, 4 );

		glVertexArrayElementBuffer( _vao,_ebo );

	}
} // namespace M3D_ISICG
