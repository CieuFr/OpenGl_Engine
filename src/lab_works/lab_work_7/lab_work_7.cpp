#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_7.hpp"

#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include <string>

namespace M3D_ISICG
{

	const std::string LabWork7::_shaderFolder = "src/lab_works/lab_work_7/shaders/";

	LabWork7::~LabWork7() { glDeleteProgram( aProgram ); }

	float ourLerp( float a, float b, float f ) { return a + f * ( b - a ); }  

	
	// renderQuad() renders a 1x1 XY quad in NDC
	// -----------------------------------------
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void		 renderQuad()
	{
		if ( quadVAO == 0 )
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays( 1, &quadVAO );
			glGenBuffers( 1, &quadVBO );
			glBindVertexArray( quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)0 );
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
		}
		glBindVertexArray( quadVAO );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glBindVertexArray( 0 );
	}


	bool LabWork7::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable( GL_DEPTH_TEST );

		// Chemin des shaders
		const std::string vertexShaderStr = readFile( _shaderFolder + "ao1.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "ao1.frag" );

		// Création des shaders
		const GLuint aVertexShader	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Création des shaders
		glShaderSource( aVertexShader, 1, &vSrc, NULL );
		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader );
		glCompileShader( aFragmentShader );

		// Code Cf. Tp 1 pour vérifier si les shaders compilent
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

		// Initialisation du Program
		aProgram = glCreateProgram();
		aProgram2 = glCreateProgram();
		aProgram3 = glCreateProgram();
		aProgram4 = glCreateProgram();

		// Attache des shaders
		glAttachShader( aProgram, aVertexShader );
		glAttachShader( aProgram, aFragmentShader );

		// Link du programme
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

		// Deletion des shaders
		glDeleteShader( aVertexShader );
		glDeleteShader( aFragmentShader );

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "uMVPMatrix" );

		////=============AO2 ==============/

		const std::string vertexShaderStr2 = readFile( _shaderFolder + "ao2.vert" );
		const std::string fragShaderStr2	  = readFile( _shaderFolder + "ao2.frag" );

		// Création des shaders
		const GLuint aVertexShader2	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader2 = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
		const GLchar * vSrc2 = vertexShaderStr2.c_str();
		const GLchar * fSrc2 = fragShaderStr2.c_str();

		// Création des shaders
		glShaderSource( aVertexShader2, 1, &vSrc2, NULL );
		glShaderSource( aFragmentShader2, 1, &fSrc2, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader2 );
		glCompileShader( aFragmentShader2 );
		glAttachShader( aProgram2, aVertexShader2 );
		glAttachShader( aProgram2, aFragmentShader2 );
		glLinkProgram( aProgram2 );

		GLint linked2;
		glGetProgramiv( aProgram2, GL_LINK_STATUS, &linked2 );
		if ( !linked2 )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( aProgram2, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( aVertexShader2 );
		glDeleteShader( aFragmentShader2 );

		//==================3======================
		const std::string vertexShaderStr3 = readFile( _shaderFolder + "ao3.vert" );
		const std::string fragShaderStr3	  = readFile( _shaderFolder + "ao3.frag" );

		// Création des shaders
		const GLuint aVertexShader3	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader3 = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
		const GLchar * vSrc3 = vertexShaderStr3.c_str();
		const GLchar * fSrc3 = fragShaderStr3.c_str();

		// Création des shaders
		glShaderSource( aVertexShader3, 1, &vSrc3, NULL );
		glShaderSource( aFragmentShader3, 1, &fSrc3, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader3 );
		glCompileShader( aFragmentShader3 );

		glAttachShader( aProgram3, aVertexShader3 );
		glAttachShader( aProgram3, aFragmentShader3 );
		glLinkProgram( aProgram3 );
		GLint linked3;
		glGetProgramiv( aProgram3, GL_LINK_STATUS, &linked3 );
		if ( !linked3 )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( aProgram3, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}



		glDeleteShader( aVertexShader3 );
		glDeleteShader( aFragmentShader3 );


		//==================4======================

		const std::string vertexShaderStr4 = readFile( _shaderFolder + "ao4.vert" );
		const std::string fragShaderStr4	  = readFile( _shaderFolder + "ao4.frag" );
		// Création des shaders
		const GLuint aVertexShader4	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader4 = glCreateShader( GL_FRAGMENT_SHADER );
		// Récupération des locations des shaders
		const GLchar * vSrc4 = vertexShaderStr4.c_str();
		const GLchar * fSrc4 = fragShaderStr4.c_str();
		// Création des shaders
		glShaderSource( aVertexShader4, 1, &vSrc4, NULL );
		glShaderSource( aFragmentShader4, 1, &fSrc4, NULL );
		// Compilation des shaders
		glCompileShader( aVertexShader4 );
		glCompileShader( aFragmentShader4 );

				glAttachShader( aProgram4, aVertexShader4 );
		glAttachShader( aProgram4, aFragmentShader4 );
		glLinkProgram( aProgram4 );

		GLint linked4;
		glGetProgramiv( aProgram4, GL_LINK_STATUS, &linked4 );
		if ( !linked4 )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( aProgram4, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( aVertexShader4 );
		glDeleteShader( aFragmentShader );


		
		//===================AO ==========================

		/* Shader shaderGeometryPass( "ao1.vert", "ao1.frag" );
		Shader shaderLightingPass( "ao2.vert", "ao2.frag" );
		Shader shaderSSAO( "ao3.vert", "ao3.frag" );
		Shader shaderSSAOBlur( "ao4.vert", "ao4.frag" );*/


		const unsigned int SCR_WIDTH  = 800;
		const unsigned int SCR_HEIGHT = 600;

	
		glGenFramebuffers( 1, &gBuffer );
		glBindFramebuffer( GL_FRAMEBUFFER, gBuffer );
		
		// position color buffer
		glGenTextures( 1, &gPosition );
		glBindTexture( GL_TEXTURE_2D, gPosition );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0 );
		// normal color buffer
		glGenTextures( 1, &gNormal );
		glBindTexture( GL_TEXTURE_2D, gNormal );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0 );
		// color + specular color buffer
		glGenTextures( 1, &gAlbedo );
		glBindTexture( GL_TEXTURE_2D, gAlbedo );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0 );
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
		unsigned int attachments[ 3 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers( 3, attachments );
		// create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers( 1, &rboDepth );
		glBindRenderbuffer( GL_RENDERBUFFER, rboDepth );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth );
		// finally check if framebuffer is complete
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		
		
		glGenFramebuffers( 1, &ssaoFBO );
		glGenFramebuffers( 1, &ssaoBlurFBO );
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoFBO );
		
		// SSAO color buffer
		glGenTextures( 1, &ssaoColorBuffer );
		glBindTexture( GL_TEXTURE_2D, ssaoColorBuffer );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0 );
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cout << "SSAO Framebuffer not complete!" << std::endl;
		// and blur stage
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoBlurFBO );
		glGenTextures( 1, &ssaoColorBufferBlur );
		glBindTexture( GL_TEXTURE_2D, ssaoColorBufferBlur );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0 );
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		// generate sample kernel
		// ----------------------
		std::uniform_real_distribution<GLfloat> randomFloats( 0.0, 1.0 ); // generates random floats between 0.0 and 1.0
		std::default_random_engine				generator;
		
		for ( unsigned int i = 0; i < 64; ++i )
		{
			glm::vec3 sample( randomFloats( generator ) * 2.0 - 1.0,
							  randomFloats( generator ) * 2.0 - 1.0,
							  randomFloats( generator ) );
			sample = glm::normalize( sample );
			sample *= randomFloats( generator );
			float scale = float( i ) / 64.0f;

			// scale samples s.t. they're more aligned to center of kernel
			scale = ourLerp( 0.1f, 1.0f, scale * scale );
			sample *= scale;
			ssaoKernel.push_back( sample );
		}

		// generate noise texture
		// ----------------------
		std::vector<glm::vec3> ssaoNoise;
		for ( unsigned int i = 0; i < 16; i++ )
		{
			glm::vec3 noise( randomFloats( generator ) * 2.0 - 1.0,
							 randomFloats( generator ) * 2.0 - 1.0,
							 0.0f ); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back( noise );
		}
		
		glGenTextures( 1, &noiseTexture );
		glBindTexture( GL_TEXTURE_2D, noiseTexture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[ 0 ] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		// lighting info
		// -------------
		
		lightColor = glm::vec3( 0.2, 0.2, 0.7 );


		glUseProgram( aProgram2 );
		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram, "gPosition" ), 0 );
		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram, "gNormal" ), 1 );
		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram, "gAlbedo" ), 2 );
		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram, "ssao" ), 3 );

		glUseProgram( aProgram3 );
		glProgramUniform1f( aProgram3, glGetUniformLocation( aProgram, "gPosition" ), 0 );
		glProgramUniform1f( aProgram3, glGetUniformLocation( aProgram, "gNormal" ), 1 );
		glProgramUniform1f( aProgram3, glGetUniformLocation( aProgram, "texNoise" ), 2 );
	
		glUseProgram( aProgram4 );
		glProgramUniform1f( aProgram4, glGetUniformLocation( aProgram, "ssaoInput" ), 0 );


		//=================== FIN AO  ==========================


		//=============TP 4 ==============/

		_tmm.load( "bunny", FilePath( "./data/models/bunny.obj" ) );

		//_tmm._transformation = glm::scale( _tmm._transformation, Vec3f( 0.003, 0.003, 0.003 ) );
		//=============FIN ==============/

		// INIT du program
		glUseProgram( aProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork7::animate( const float p_deltaTime ) {}

	void LabWork7::render()
	{
		
		// glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if ( luminosityNeedsUpdating )
		{
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera.setFovy( _fovy );
			// fovyNeedsUpdating = false;
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

		glProgramUniformMatrix3fv(
			aProgram, glGetUniformLocation( aProgram, "normalMatrix" ), 1, GL_FALSE, glm::value_ptr( normalMatrix ) );

		glProgramUniformMatrix4fv( aProgram,
								   glGetUniformLocation( aProgram, "MVMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _matrixWtoV * _tmm._transformation ) );

		lightPos = Vec4f( 1.42, 1.72, -0.5, 1.0f );
		glProgramUniform3fv( aProgram,
							 glGetUniformLocation( aProgram, "lightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f(lightPos,1.0) ) );

		glProgramUniform3fv(
			aProgram, glGetUniformLocation( aProgram, "cameraPos" ), 1, glm::value_ptr( _camera._position ) );

		




		////////////////////////AOOO/////////////////////////////////

		  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glBindFramebuffer( GL_FRAMEBUFFER, gBuffer );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_tmm.render( aProgram );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		// 2. generate SSAO texture
		// ------------------------
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoFBO );
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( aProgram3 ); 
		// Send kernel + rotation
		for (unsigned int i = 0; i < 64; ++i) {
			std::string			iChain = "samples[" + std::to_string( i ) + "]";
			glProgramUniform3fv( aProgram3, glGetUniformLocation( aProgram3, iChain.c_str() ),
								 1,
								 glm::value_ptr( ssaoKernel[ i ] ) );
		}
	

		glProgramUniformMatrix4fv( aProgram3,
								   glGetUniformLocation( aProgram3, "projection" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _camera.getProjectionMatrix() ) );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, gPosition );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, gNormal );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, noiseTexture );
		renderQuad();
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		// 3. blur SSAO texture to remove noise
		// ------------------------------------
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoBlurFBO );
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( aProgram4 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, ssaoColorBuffer );
		renderQuad();
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
		// -----------------------------------------------------------------------------------------------------
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( aProgram2 ); 
		// send light relevant uniforms
		glm::vec3 lightPosView = glm::vec3( _camera.getViewMatrix() * glm::vec4( lightPos, 1.0 ) );
		glProgramUniform3fv( aProgram2,
							 glGetUniformLocation( aProgram2, "light.Position" ),
							 1, glm::value_ptr( lightPosView ) );
		glProgramUniform3fv(
			aProgram2, glGetUniformLocation( aProgram2, "light.Color" ), 1, glm::value_ptr( lightColor ) );

		// Update attenuation parameters
		const float linear	  = 0.09f;
		const float quadratic = 0.032f;

		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram2, "light.Linear" ), linear );
		glProgramUniform1f( aProgram2, glGetUniformLocation( aProgram2, "light.Quadratic" ), quadratic );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, gPosition );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, gNormal );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, gAlbedo );
		glActiveTexture( GL_TEXTURE3 ); // add extra SSAO texture to lighting pass
		glBindTexture( GL_TEXTURE_2D, ssaoColorBufferBlur );
		renderQuad();

		//============================FIN AO ==============================

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
		if ( ImGui::ColorEdit3( "BackGround Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		ImGui::Checkbox( "Trackball", &trackballSwitch );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrtho );

		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork7::_updateViewMatrix()
	{
		_matrixWtoV			  = _camera.getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork7::_updateProjectionMatrix()
	{
		_matrixVtoC			  = _camera.getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork7::_initCamera()
	{
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 0.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

} // namespace M3D_ISICG
