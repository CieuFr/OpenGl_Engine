#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

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
		
		glProgramUniform3fv( p_glProgram,
							 glGetUniformLocation( p_glProgram, "ambient" ),
							 1,
							 glm::value_ptr( _material._ambient) );

		glProgramUniform3fv(
			p_glProgram, glGetUniformLocation( p_glProgram, "diffuse" ), 1, glm::value_ptr( _material._diffuse ) );


		glProgramUniform3fv(
			p_glProgram, glGetUniformLocation( p_glProgram, "specular" ), 1, glm::value_ptr( _material._specular ) );

		glProgramUniform1f( p_glProgram, glGetUniformLocation( p_glProgram, "shininess" ), _material._shininess );


		// TEXTURE DECLARATION BOOL POUR LE FRAGMENT SHADER 

		glProgramUniform1f( p_glProgram, glGetUniformLocation( p_glProgram, "uHasDiffuseMap" ), _material._hasDiffuseMap );

		glProgramUniform1f(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasAmbientMap" ), _material._hasAmbientMap );

		glProgramUniform1f(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasSpecularMap" ), _material._hasSpecularMap );

		glProgramUniform1f(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasShininessMap" ), _material._hasShininessMap );

		glProgramUniform1f(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasNormalMap" ), _material._hasNormalMap );

		
		
		glBindVertexArray( _vao );

		// BIND DES TEXTURES POUR ENVOYER LES DONNEES
		if (_material._hasDiffuseMap) {
			glBindTextureUnit( 1, _material._diffuseMap._id );
		}
		if ( _material._hasAmbientMap )
		{
			glBindTextureUnit( 2, _material._ambientMap._id );
		}
		if ( _material._hasSpecularMap )
		{
			glBindTextureUnit( 3, _material._specularMap._id );
		}
		if ( _material._hasShininessMap )
		{
			glBindTextureUnit( 4, _material._shininessMap._id );
		}
		if ( _material._hasNormalMap )
		{
			glBindTextureUnit( 5, _material._normalMap._id );
		}


		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );


		// TEXTURE RESET DES BIND TEXTURES 

		glBindTextureUnit( 1, 0 );
		glBindTextureUnit( 2, 0 );
		glBindTextureUnit( 3, 0 );
		glBindTextureUnit( 4, 0 );
		glBindTextureUnit( 5, 0 );

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

		// TP 5 TEXTURE
		

	}

	void TriangleMesh::_setupGL()
	{		
			

		// TO STRING 
		std::cout << glm::to_string( _material._ambient ) << "\n";



		glCreateBuffers( 1, &_vbo );
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _vbo,
						   _vertices.size() * sizeof( Vertex ),
						   _vertices.data(),
						   GL_STATIC_DRAW );

		glNamedBufferData(_ebo,
			_indices.size() * sizeof( int ),
			_indices.data(),
			GL_STATIC_DRAW );


		glCreateVertexArrays( 1, &_vao );

		glEnableVertexArrayAttrib( _vao, 0 );
		glEnableVertexArrayAttrib( _vao, 1 );
		glEnableVertexArrayAttrib( _vao, 2 );
		glEnableVertexArrayAttrib( _vao, 3 );
		glEnableVertexArrayAttrib( _vao, 4 );

		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _position ) );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );


		glVertexArrayAttribBinding( _vao, 0, 0 );
		glVertexArrayAttribBinding( _vao, 1, 0 );
		glVertexArrayAttribBinding( _vao, 2, 0 );
		glVertexArrayAttribBinding( _vao, 3, 0 );
		glVertexArrayAttribBinding( _vao, 4, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

		glVertexArrayElementBuffer( _vao,_ebo );

		
	}
} // namespace M3D_ISICG
