#include "lab_work_3.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"


namespace M3D_ISICG
{

	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() {
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( VAO, 0 );
		glDeleteVertexArrays( 1, &VAO );
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

		for (int i = 0; i < 4; i++) {		
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


		glCreateBuffers( 1, &cube.VBOVertices );
		glCreateBuffers( 1, &cube.EBO );
		glCreateBuffers( 1, &cube.VBOColors );

		
		glNamedBufferData(
			cube.VBOVertices,
			cube.vectorPositions.size() * sizeof( Vec3f ),
			cube.vectorPositions.data(),
			GL_STATIC_DRAW );

		glNamedBufferData(
			cube.VBOColors, 
			cube.vectorColors.size() * sizeof( Vec3f ),
			cube.vectorColors.data(), 
			GL_STATIC_DRAW );

		glNamedBufferData(
			cube.EBO,
			cube.vectorIndices.size() * sizeof( int ),
			cube.vectorIndices.data(),
			GL_STATIC_DRAW );
		
		//cube.transformationMatrice = Mat4f( 1.0f );
		cube.transformationMatrice = glm::scale( cube.transformationMatrice, Vec3f( 0.8f, 0.8f, 0.8f ));

		return cube;
		
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

	
		glEnable(GL_DEPTH_TEST);  

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw3.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "lw3.frag" );

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


		_cube = _createCube();

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// MATRIX L TO W 
		matrixLtoW = glGetUniformLocation( aProgram, "matrixLtoW" );
		glProgramUniformMatrix4fv( aProgram, matrixLtoW, 1, GL_FALSE, glm::value_ptr(_cube.transformationMatrice) );

		matrixWtoVGluint = glGetUniformLocation( aProgram, "matrixWtoV" );
		glProgramUniformMatrix4fv( aProgram, matrixWtoVGluint, 1, GL_FALSE, glm::value_ptr( _matrixWtoV ) );

		matrixVtoCGluint = glGetUniformLocation( aProgram, "matrixVtoC" );
		glProgramUniformMatrix4fv( aProgram, matrixVtoCGluint, 1, GL_FALSE, glm::value_ptr( _matrixVtoC ) );

	


		glCreateVertexArrays( 1, &_cube.VAO );
		

		glEnableVertexArrayAttrib( _cube.VAO, 0 );
		glVertexArrayAttribFormat( _cube.VAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _cube.VAO, 0, _cube.VBOVertices, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube.VAO, 0, 0 );


		glEnableVertexArrayAttrib( _cube.VAO, 1 );
		glVertexArrayAttribFormat( _cube.VAO, 1, 3, GL_FLOAT, GL_TRUE, 0 );
		glVertexArrayVertexBuffer( _cube.VAO, 1, _cube.VBOColors, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube.VAO, 1, 1 );



		glVertexArrayElementBuffer( _cube.VAO, _cube.EBO );

	


		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork3::animate( const float p_deltaTime ) {
		
		_cube.transformationMatrice = glm::rotate( _cube.transformationMatrice, p_deltaTime, Vec3f( 1, 1, 0 ) );
		glProgramUniformMatrix4fv( aProgram, matrixLtoW, 1, GL_FALSE, glm::value_ptr( _cube.transformationMatrice ) );
	
	}

	void LabWork3::render() { 
		//glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glBindVertexArray( _cube.VAO );
		glDrawElements( GL_TRIANGLES, _cube.vectorIndices.size(), GL_UNSIGNED_INT, 0 );
		//glDrawArrays(GL_TRIANGLES ,0,triangleVertexes.size());
		glBindVertexArray(0);

		if (luminosityNeedsUpdating) {
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera.setFovy(_fovy);
		}

	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		switch ( p_event.key.keysym.sym)
		{
		case SDLK_LEFT:
		case SDLK_q: _camera.moveRight( -0.05 ); break;

		
		}
		_updateViewMatrix();
	}

	void LabWork3::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork3::_updateViewMatrix() {

		_matrixWtoV = _camera.getViewMatrix();
		glProgramUniformMatrix4fv( aProgram, matrixWtoVGluint, 1, GL_FALSE, glm::value_ptr( _matrixWtoV ) );
		
	}

	void LabWork3::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera.getProjectionMatrix();
		glProgramUniformMatrix4fv( aProgram, matrixVtoCGluint, 1, GL_FALSE, glm::value_ptr( _matrixVtoC ) );
		
	}

	void LabWork3::_initCamera() { 
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
	}

} // namespace M3D_ISICG
