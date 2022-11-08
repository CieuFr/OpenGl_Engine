#include "lab_work_4.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"


namespace M3D_ISICG
{

	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4() {
		glDeleteProgram( aProgram );

	}

	

	bool LabWork4::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

	
		glEnable(GL_DEPTH_TEST);  

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "mesh.frag" );

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

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "transformationMatrix" );



		//=============TP 4 ==============/

		_tmm.load("bunny", FilePath("./data/models/bunny.obj" ));

		

		//=============FIN ==============/


		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork4::animate( const float p_deltaTime ) {

			
	}

	void LabWork4::render() { 
		//glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		
	
		if (luminosityNeedsUpdating) {
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera.setFovy(_fovy);
			//fovyNeedsUpdating = false;
		}

		if ( perspecNeedsUpdating )
		{
			_camera.switchPerspect();
			_updateProjectionMatrix();
			perspecNeedsUpdating = false;
		}

		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		_tmm.render( aProgram );

	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
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

	void LabWork4::displayUI()
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

	void LabWork4::_updateViewMatrix() {

		_matrixWtoV = _camera.getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork4::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera.getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
		
	}

	void LabWork4::_initCamera() { 
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	
	}

} // namespace M3D_ISICG
