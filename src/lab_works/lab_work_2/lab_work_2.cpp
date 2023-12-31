#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2() {
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( VAO, 0 );
		glDeleteVertexArrays( 1, &VAO );
		// Delete VBO
		glDeleteBuffers( 1, &VBO );
		// Delete VBO
		glDeleteBuffers( 1, &VBO2 );

			// Delete VBO
		glDeleteBuffers( 1, &EBO );
	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glDisable( GL_DEPTH_TEST );
		

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "lw1.frag" );

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

		// Get Uniform uTranslation
		aTranslationX = glGetUniformLocation( aProgram, "uTranslationX" );

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		//Init des positions du triangle
		/* # REMOVE FOR QUAD 
		 triangleVertices.push_back( Vec2f( -0.5f, 0.5f ) );
		triangleVertices.push_back( Vec2f( 0.5f, 0.5f ) );
		triangleVertices.push_back( Vec2f( 0.5f, -0.5f ) );
		triangleVertices.push_back( Vec2f( -0.5f, -0.5f ) );

		triangleColors.push_back( Vec4f(1.f,0.f,0.f,1.f ));
		triangleColors.push_back( Vec4f( 0.f, 1.f, 0.f, 1.f ) );
		triangleColors.push_back( Vec4f( 0.f, 0.f, 1.f, 1.f ) );
		triangleColors.push_back( Vec4f( 1.f, 0.f, 1.f, 1.f ) );
		*/


		// Init VBO  Vertex Buffer Object Sommet
		glCreateBuffers( 1, &VBO );

		// Creation EBO Sommet
		glCreateBuffers( 1, &EBO );

        // Init VBO  Vertex Buffer Object COULEUR
		glCreateBuffers( 1, &VBO2 );

	

		// Remplissage EBO

		/*#REMOVE FOR QUAD
		* 
		*/
		/* eboPositions = { 0, 1, 2, 0, 3, 2 };*/


		//#CIRCLE

		Vec2f	center = { 0.3f, -0.2f };
		GLfloat radius = 0.5f;
		triangleVertices.push_back( center );
		triangleVertices.push_back( Vec2f( center.x + radius, center.y ) );
		Vec3f tempVec3 = getRandomVec3f();
		triangleColors.push_back( Vec4f( tempVec3.x, tempVec3.y, tempVec3.z, 1.f ) );
		tempVec3 = getRandomVec3f();
		triangleColors.push_back( Vec4f( tempVec3.x, tempVec3.y, tempVec3.z, 1.f ) );
		float numberOfVertices = 64;

		for ( float i = 1; i <= numberOfVertices; i++ )
		{
			GLfloat angle = ( i * 2.f * glm::pi<float>() ) / numberOfVertices;
			triangleVertices.push_back(
				Vec2f( center.x + radius * glm::cos( angle ), center.y + radius * glm::sin( angle ) ) );
			tempVec3 = getRandomVec3f();
			triangleColors.push_back( Vec4f( tempVec3.x, tempVec3.y, tempVec3.z, 1.f ) );
			eboPositions.push_back( 0 );
			eboPositions.push_back( (int)i );
			if (i + 1 <= numberOfVertices) {
				eboPositions.push_back( (int)i + 1 );
			}
			else
			{
				eboPositions.push_back( 1 );
					
			}
		}

			// Remplissage VBO SOMMET
		glNamedBufferData( VBO, triangleVertices.size() * sizeof( Vec2f ), triangleVertices.data(), GL_STATIC_DRAW );

		// Remplissage VBO COULEUR
		glNamedBufferData( VBO2, triangleColors.size() * sizeof( Vec4f ), triangleColors.data(), GL_STATIC_DRAW );

		glNamedBufferData( EBO, eboPositions.size() * sizeof( int ), eboPositions.data(), GL_STATIC_DRAW );
		
		// Cr�ation VAO  Vertex Array Object
		glCreateVertexArrays( 1, &VAO );
		
		//Activez l�attribut 0 du VAO
		glEnableVertexArrayAttrib( VAO, 0 );
		// Activez l�attribut 0 du VAO COULEUR
		glEnableVertexArrayAttrib( VAO, 1 );

		//Definissez le format de l�attribut 
		glVertexArrayAttribFormat( VAO, 0, 2, GL_FLOAT,GL_FALSE,0);

		// Definissez le format de l�attribut
		glVertexArrayAttribFormat( VAO, 1, 4, GL_FLOAT, GL_TRUE, 0 );

		//Liez le VAO et le VBO SOMMET
		glVertexArrayVertexBuffer( VAO, 0, VBO, 0, sizeof( Vec2f ) );

		// Liez le VAO et le VBO COULEUR
		glVertexArrayVertexBuffer( VAO, 1, VBO2, 0, sizeof( Vec4f ) );

		//Connectez le VAO avec le Vertex shader
		glVertexArrayAttribBinding( VAO, 0, 0 );

			// Connectez le VAO avec le Vertex shader
		glVertexArrayAttribBinding( VAO, 1, 1 );

		

		//Link EBO VAO
		glVertexArrayElementBuffer(VAO,EBO);

	


		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {
		_time += p_deltaTime;
		glProgramUniform1f( aProgram, aTranslationX, glm::sin(_time) );
		
	
	}

	void LabWork2::render() { 
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

	void LabWork2::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork2::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void circleDrawing( Vec2f center, GLuint numberOfVertices, GLfloat radius )
	{ 
		if (numberOfVertices < 3) {
			numberOfVertices = 3;
		}
		




	}

} // namespace M3D_ISICG
