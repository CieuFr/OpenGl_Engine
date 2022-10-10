#include "lab_work_3.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	struct Mesh
	{
		Vec3f					  vectorPositions;
		Vec3f					  vectorColors;
		std::vector<unsigned int> vectorIndices;
		Mat4f					  transformationMatrice;
		GLuint					  VBO;
		GLuint					  VAO;
		GLuint					  EBO;
	};

	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() {
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( _cube.VAO, 0 );
		glDeleteVertexArrays( 1, &_cube.VAO );
		// Delete VBO
		glDeleteBuffers( 1, &_cube.VBOVertices );
		// Delete VBO
		glDeleteBuffers( 1, &_cube.VBOColors );

			// Delete VBO
		glDeleteBuffers( 1, &_cube.EBO );
	}

	LabWork3::Mesh LabWork3::_createCube()
	{ 
		LabWork3::Mesh cube;
		cube.vectorPositions.push_back( Vec3f( 0.5f, 0.5f, -0.5f ) );
		cube.vectorPositions.push_back( Vec3f( 0.5f, -0.5f, -0.5f ) );
		cube.vectorPositions.push_back( Vec3f( -0.5f, -0.5f, -0.5f ) );
		cube.vectorPositions.push_back( Vec3f( -0.5f, 0.5f, -0.5f ) );
		cube.vectorPositions.push_back( Vec3f( 0.5f, 0.5f, 0.5f ) );
		cube.vectorPositions.push_back( Vec3f( 0.5f, -0.5f, 0.5f ) );
		cube.vectorPositions.push_back( Vec3f( -0.5f, -0.5f, 0.5f ) );
		cube.vectorPositions.push_back( Vec3f( -0.5f, 0.5f, 0.5f ) );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );
		cube.vectorColors.push_back( getRandomVec3f() );

		for (int i = 0; i < 5; i++) {		
				cube.vectorIndices.push_back( i );
				cube.vectorIndices.push_back( (i+1)%4 );
				cube.vectorIndices.push_back( i + 4 );
				cube.vectorIndices.push_back(( ( i + 1 ) % 4 ) +4 );
				cube.vectorIndices.push_back( ( i + 1 ) % 4 );
				cube.vectorIndices.push_back( i + 4 );
		}

		//BAS
		cube.vectorIndices.push_back( 0 );
		cube.vectorIndices.push_back( 1 );
		cube.vectorIndices.push_back( 2 );
		cube.vectorIndices.push_back( 0 );
		cube.vectorIndices.push_back( 2 );
		cube.vectorIndices.push_back( 3 );

		//HAUT
		cube.vectorIndices.push_back( 4 );
		cube.vectorIndices.push_back( 5 );
		cube.vectorIndices.push_back( 6 );
		cube.vectorIndices.push_back( 4 );
		cube.vectorIndices.push_back( 6 );
		cube.vectorIndices.push_back( 7 );

		
		return cube;
		
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

	
		

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw3.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "lw3.frag" );

		//Cr�ation des shaders
		const GLuint aVertexShader = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		//R�cup�ration des locations des shaders 
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Cr�ation des shaders 
		glShaderSource( aVertexShader, 1, &vSrc, NULL );
		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader );
		glCompileShader( aFragmentShader );

		//Code Cf. Tp 1 pour v�rifier si les shaders compilent
		GLint compiled;
		glGetShaderiv( aVertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( aVertexShader, sizeof( log ), NULL, log );
			glDeleteShader( aVertexShader );
			glDeleteShader( aFragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		//Initialisation du Program
		aProgram = glCreateProgram();

		//Attache des shaders
		glAttachShader( aProgram, aVertexShader );
		glAttachShader( aProgram, aFragmentShader );

		//Link du programme
		glLinkProgram( aProgram );
		GLint linked;
		glGetProgramiv( aProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( aProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		//Deletion des shaders 
		glDeleteShader( aVertexShader );
		glDeleteShader( aFragmentShader );



		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );



		_cube = _createCube();

		// Init VBO  Vertex Buffer Object Sommet
		glCreateBuffers( 1, &_cube.VBOVertices );

		// Creation EBO Sommet
		glCreateBuffers( 1, &_cube.EBO );

        // Init VBO  Vertex Buffer Object COULEUR
		glCreateBuffers( 1, &_cube.VBOColors );


		

		glNamedBufferData( _cube.VBOVertices,
						   _cube.vectorPositions.size() * sizeof( Vec3f ),
						   _cube.vectorPositions.data(),
						   GL_STATIC_DRAW );

		glNamedBufferData(
			_cube.VBOColors, _cube.vectorColors.size() * sizeof( Vec3f ), _cube.vectorColors.data(), GL_STATIC_DRAW );

		glNamedBufferData(
			_cube.EBO, _cube.vectorIndices.size() * sizeof( int ), _cube.vectorIndices.data(), GL_STATIC_DRAW );
		

		// Cr�ation VAO  Vertex Array Object
		glCreateVertexArrays( 1, &_cube.VAO );
		
		//Activez l�attribut 0 du VAO
		glEnableVertexArrayAttrib( _cube.VAO, 0 );
		// Activez l�attribut 0 du VAO COULEUR
		glEnableVertexArrayAttrib( _cube.VAO, 1 );

		//Definissez le format de l�attribut 
		glVertexArrayAttribFormat( _cube.VAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		// Definissez le format de l�attribut
		glVertexArrayAttribFormat( _cube.VAO, 1, 4, GL_FLOAT, GL_TRUE, 0 );

		//Liez le VAO et le VBO SOMMET
		glVertexArrayVertexBuffer( _cube.VAO, 0, VBO, 0, sizeof( Vec2f ) );

		// Liez le VAO et le VBO COULEUR
		glVertexArrayVertexBuffer( _cube.VAO, 1, VBO2, 0, sizeof( Vec4f ) );

		//Connectez le VAO avec le Vertex shader
		glVertexArrayAttribBinding( _cube.VAO, 0, 0 );

			// Connectez le VAO avec le Vertex shader
		glVertexArrayAttribBinding( _cube.VAO, 1, 1 );

		

		//Link EBO VAO
		glVertexArrayElementBuffer( _cube.VAO, _cube.EBO );

	


		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork3::animate( const float p_deltaTime ) {
		
		
	
	}

	void LabWork3::render() { 
		//glClearColor
		glClear(GL_COLOR_BUFFER_BIT );
		glBindVertexArray( VAO );
		glDrawElements( GL_TRIANGLES, eboPositions.size(), GL_UNSIGNED_INT, 0 );
		//glDrawArrays(GL_TRIANGLES ,0,triangleVertexes.size());
		glBindVertexArray(0);

		if (luminosityNeedsUpdating) {
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork3::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG