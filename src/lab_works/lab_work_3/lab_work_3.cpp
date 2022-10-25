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
		

		glCreateVertexArrays( 1, &cube.VAO );

		glEnableVertexArrayAttrib( cube.VAO, 0 );
		glVertexArrayAttribFormat( cube.VAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( cube.VAO, 0, cube.VBOVertices, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( cube.VAO, 0, 0 );

		glEnableVertexArrayAttrib( cube.VAO, 1 );
		glVertexArrayAttribFormat( cube.VAO, 1, 3, GL_FLOAT, GL_TRUE, 0 );
		glVertexArrayVertexBuffer( cube.VAO, 1, cube.VBOColors, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( cube.VAO, 1, 1 );

		glVertexArrayElementBuffer( cube.VAO, cube.EBO );


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

		_cube2 = _createCube();

		_cube2.transformationMatrice = glm::translate( _cube2.transformationMatrice, Vec3f( 3.0f, .0f, .0f ) );

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "transformationMatrix" );

		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork3::animate( const float p_deltaTime ) {

		_cube.transformationMatrice = glm::rotate( _cube.transformationMatrice, p_deltaTime, Vec3f( 1, 1, 0 ) );

		_cube2.transformationMatrice = glm::translate( _cube2.transformationMatrice, Vec3f( -3.0f, .0f, .0f ) );
		_cube2.transformationMatrice = glm::rotate( _cube2.transformationMatrice, p_deltaTime, Vec3f( 0, 1, 0 ) );
		_cube2.transformationMatrice = glm::translate( _cube2.transformationMatrice, Vec3f( 3.0f, .0f, .0f ) );

		
	}

	void LabWork3::render() { 
		//glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		glBindVertexArray( _cube.VAO );
		glDrawElements( GL_TRIANGLES, _cube.vectorIndices.size(), GL_UNSIGNED_INT, 0 );

		
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube2.transformationMatrice;
		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		glBindVertexArray( _cube2.VAO );
		glDrawElements( GL_TRIANGLES, _cube2.vectorIndices.size(), GL_UNSIGNED_INT, 0 );


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

		if ( perspecNeedsUpdating )
		{
			_camera.switchPerspect();
			_updateProjectionMatrix();
			perspecNeedsUpdating = false;
		}

	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		ImGui::Checkbox( "Trackball", &trackballSwitch );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrtho );


		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork3::_updateViewMatrix() {

		_matrixWtoV = _camera.getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
	}

	void LabWork3::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera.getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
		
	}

	void LabWork3::_initCamera() { 
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
	
	}

} // namespace M3D_ISICG
