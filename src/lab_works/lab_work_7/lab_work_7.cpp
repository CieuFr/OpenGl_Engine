#include "lab_work_7.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"


namespace M3D_ISICG
{

	const std::string LabWork7::_shaderFolder = "src/lab_works/lab_work_7/shaders/";

	LabWork7::~LabWork7() {
		glDeleteProgram( aProgram );

	}

	float lerp( float a, float b, float f ) { return a + f * ( b - a ); }  

	bool LabWork7::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

	
		glEnable(GL_DEPTH_TEST);  

		//Chemin des shaders 
		const std::string vertexShaderStr = readFile( _shaderFolder + "ao.vert" );
		const std::string fragShaderStr = readFile( _shaderFolder + "ao.frag" );

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
		transformationMatrix = glGetUniformLocation( aProgram, "uMVPMatrix" );

		

		////=============AO ==============/

	


			//===================AO ==========================

		

		//#define checkImageWidth 64
		//#define checkImageHeight 64


		//	glGenTextures( 1, &gPosition );
		//glBindTexture( GL_TEXTURE_2D, gPosition );
		//	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_FLOAT, NULL );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );  


		//std::uniform_real_distribution<float> randomFloats( 0.0, 1.0 ); // random floats between [0.0, 1.0]
		//std::default_random_engine			  generator;
		//std::vector<glm::vec3>				  ssaoKernel;
		//for ( unsigned int i = 0; i < 64; ++i )
		//{
		//	glm::vec3 sample( randomFloats( generator ) * 2.0 - 1.0,
		//					  randomFloats( generator ) * 2.0 - 1.0,
		//					  randomFloats( generator ) );
		//	sample = glm::normalize( sample );
		//	sample *= randomFloats( generator );

		//	float scale = (float)i / 64.0;
		//	scale		= lerp( 0.1f, 1.0f, scale * scale );
		//	sample *= scale;
		//	ssaoKernel.push_back( sample );
		//}


		//std::vector<glm::vec3> ssaoNoise;
		//for ( unsigned int i = 0; i < 16; i++ )
		//{
		//	glm::vec3 noise( randomFloats( generator ) * 2.0 - 1.0, randomFloats( generator ) * 2.0 - 1.0, 0.0f );
		//	ssaoNoise.push_back( noise );
		//}

		//unsigned int noiseTexture;
		//glGenTextures( 1, &noiseTexture );
		//glBindTexture( GL_TEXTURE_2D, noiseTexture );
		//glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[ 0 ] );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


		//unsigned int ssaoFBO;
		//glGenFramebuffers( 1, &ssaoFBO );
		//glBindFramebuffer( GL_FRAMEBUFFER, ssaoFBO );

		//unsigned int ssaoColorBuffer;
		//glGenTextures( 1, &ssaoColorBuffer );
		//glBindTexture( GL_TEXTURE_2D, ssaoColorBuffer );
		//glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, checkImageWidth, checkImageHeight, 0, GL_RED, GL_FLOAT, NULL );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0 );  

		//=================== FIN AO  ==========================



		//=============FIN AO ==============/


		//=============TP 4 ==============/

		_tmm.load("bunny", FilePath("./data/models/bunny.obj" ));

		
		//_tmm._transformation = glm::scale( _tmm._transformation, Vec3f(0.003, 0.003, 0.003) );
		//=============FIN ==============/


		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork7::animate( const float p_deltaTime ) {

			
	}



	void LabWork7::render() { 

	






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

		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
		
		Mat3f normalMatrix = glm::transpose( glm::inverse( _matrixWtoV * _tmm._transformation ) );


		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );


		glProgramUniformMatrix3fv( aProgram,
								   glGetUniformLocation( aProgram, "normalMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( normalMatrix ) );


		glProgramUniformMatrix4fv( aProgram,
								   glGetUniformLocation( aProgram, "MVMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _matrixWtoV *_tmm._transformation ) );


		glProgramUniform3fv( aProgram,
								   glGetUniformLocation( aProgram, "lightPos" ),
								   1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera._position,1.0)
							 
							 /* Vec4f( 1.42, 1.72, -0.5, 1.0f )*/ ) );

			glProgramUniform3fv(
			aProgram, glGetUniformLocation( aProgram, "cameraPos" ), 1, glm::value_ptr( _camera._position ) );


		_tmm.render( aProgram );




	}

	void LabWork7::handleEvents( const SDL_Event & p_event )
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

	void LabWork7::displayUI()
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

	void LabWork7::_updateViewMatrix() {

		_matrixWtoV = _camera.getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork7::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera.getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
		
	}

	void LabWork7::_initCamera() { 
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 0.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	
	}

} // namespace M3D_ISICG
