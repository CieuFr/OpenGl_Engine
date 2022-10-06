#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1() {
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( VAO, 0 );
		glDeleteVertexArrays( 1, &VAO );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "lw1.frag" );

		//Création des shaders
		const GLuint aVertexShader = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		//Récupération des locations des shaders 
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Création des shaders 
		glShaderSource( aVertexShader, 1, &vSrc, NULL );
		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader );
		glCompileShader( aFragmentShader );

		//Code Cf. Tp 1 pour vérifier si les shaders compilent
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

		//Init des positions du triangle
		triangleVertexes.push_back( Vec2f( -0.5f, 0.5f ) );
		triangleVertexes.push_back( Vec2f( 0.5f, 0.5f ) );
		triangleVertexes.push_back( Vec2f( 0.5f, -0.5f ) );

		// Init VBO  Vertex Buffer Object 
		glCreateBuffers( 1, &VBO );

		// Remplissage VBO 
		glNamedBufferData( VBO, triangleVertexes.size() * sizeof(Vec2f), triangleVertexes.data(), GL_STATIC_DRAW );

		// Delete VBO 
		glDeleteBuffers( 1, &VBO );

		// Création VAO  Vertex Array Object
		glCreateVertexArrays( 1, &VAO );
		
		//Activez l’attribut 0 du VAO
		glEnableVertexArrayAttrib( VAO, 0 );

		//Definissez le format de l’attribut 
		glVertexArrayAttribFormat( VAO, 0, 2, GL_FLOAT_VEC2,GL_TRUE,0);

		//Liez le VAO et le VBO
		glVertexArrayVertexBuffer( VAO, 0, VBO, 0, sizeof( Vec2f ) );

		//Connectez le VAO avec le Vertex shader
		glVertexArrayAttribBinding( VAO, 0, 0 );

		//INIT du program 
		glUseProgram( aProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() { 
		//glClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray( VAO );
		glDrawArrays( GL_POINTS / GL_LINES / GL_TRIANGLES ,0,3);
		glBindVertexArray(0);
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
