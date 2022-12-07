#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_8.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "victor_toolkit/utils.hpp"


namespace M3D_ISICG
{

	const std::string LabWork8::_shaderFolder = "src/lab_works/lab_work_8/shaders/";

	LabWork8::~LabWork8()
	{
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( quadVAO, 0 );
		glDeleteVertexArrays( 1, &quadVAO );
		// Delete VBO
		glDeleteBuffers( 1, &quadVBO );

		// Delete VBO
		glDeleteBuffers( 1, &quadEBO );
	}

	bool LabWork8::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable( GL_DEPTH_TEST );
		// glEnable( GL_BLEND );
		// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		// Chemin des shaders
		const std::string vertexShaderStr = _shaderFolder + "geometry_pass.vert";
		const std::string fragShaderStr	  = _shaderFolder + "geometry_pass.frag";
		std::string		  paths[ 2 ]	  = { vertexShaderStr, fragShaderStr };
		programWrapper.createProgram( paths );
		aProgram = programWrapper.getProgramId();

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "uMVPMatrix" );

		//=============TP 5 ==============/

		_tmm.load( "sponza", FilePath( "./data/models/sponza.obj" ) );

		// REMOVE COMMENT FOR SPONZA
		_tmm._transformation = glm::scale( _tmm._transformation, Vec3f( 0.003, 0.003, 0.003 ) );

		//=============TP 6 ==============/

		initLightingPassProgram();
		initGBuffer();
		initAOPasses();

		// INIT du program
		glUseProgram( aProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork8::animate( const float p_deltaTime ) {}

	bool LabWork8::initLightingPassProgram()
	{
		const std::string vertexShaderStr = _shaderFolder + "lighting_pass.vert";
		const std::string fragShaderStr	  = _shaderFolder + "lighting_pass.frag";
		std::string		  paths[ 2 ]	  = { vertexShaderStr, fragShaderStr };

		programWrapper2.createProgramOnlyFS( fragShaderStr );
		_lightingPassProgram = programWrapper2.getProgramId();

		quadVAO = drawer.drawQuad();

		return true;
	}


	bool LabWork8::initAOPasses()
	{
		const std::string fragShaderStr	  = _shaderFolder + "ssao_blur.frag";
		const std::string fragShaderStr2 = _shaderFolder + "ssao.frag";

		programWrapper3.createProgramOnlyFS( fragShaderStr );
		programWrapper4.createProgramOnlyFS( fragShaderStr2 );

		glGenFramebuffers( 1, &ssaoFBO );
		glGenFramebuffers( 1, &ssaoBlurFBO );

		computeAO();

		glCreateTextures( GL_TEXTURE_2D, 1, &noiseTexture );
		glTextureStorage2D( noiseTexture, 1, GL_RGBA32F, _windowWidth, _windowHeight );
		glTextureParameteri( noiseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( noiseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTextureParameteri( noiseTexture, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTextureParameteri( noiseTexture, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTextureSubImage2D( ssaoColorBuffer, 0, 0, 0, _windowWidth, _windowHeight, GL_RED, GL_FLOAT, &ssaoNoise[ 0 ] );
		
		glCreateTextures( GL_TEXTURE_2D, 1, &ssaoColorBuffer );
		glTextureStorage2D( ssaoColorBuffer, 1, GL_RGBA32F, _windowWidth, _windowHeight );
		glTextureParameteri( ssaoColorBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( ssaoColorBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( ssaoFBO, GL_COLOR_ATTACHMENT0, ssaoColorBuffer, 0 );

		glTextureSubImage2D( ssaoColorBuffer, 0, 0, 0, _windowWidth, _windowHeight, GL_RED, GL_FLOAT, NULL );

		if ( GL_FRAMEBUFFER_COMPLETE != glCheckNamedFramebufferStatus( _fboId, GL_DRAW_FRAMEBUFFER ) )
		{
			std::cout << "FRAMEBUFFER SSAOFBO ERROR \n";
		}

		glCreateTextures( GL_TEXTURE_2D, 1, &ssaoColorBufferBlur );
		glTextureStorage2D( ssaoColorBufferBlur, 1, GL_RGBA32F, _windowWidth, _windowHeight );
		glTextureParameteri( ssaoColorBufferBlur, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( ssaoColorBufferBlur, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( ssaoBlurFBO, GL_COLOR_ATTACHMENT0, ssaoColorBufferBlur, 0 );
		glTextureSubImage2D( ssaoColorBufferBlur, 0, 0, 0, _windowWidth, _windowHeight, GL_RED, GL_FLOAT, NULL );


		if ( GL_FRAMEBUFFER_COMPLETE != glCheckNamedFramebufferStatus( ssaoBlurFBO, GL_DRAW_FRAMEBUFFER ) )
		{
			std::cout << "FRAMEBUFFER SSAOFBO ERROR \n";
		}


		//glNamedFramebufferDrawBuffers( _fboId, 6, _drawBuffers );



		return true;
	}


	// TP6

	void LabWork8::initGBuffer()
	{
		glCreateFramebuffers( 1, &_fboId );

		glCreateTextures( GL_TEXTURE_2D, 6, _gBufferTextures );

		for ( size_t i = 0; i < 5; i++ )
		{
			glTextureStorage2D( _gBufferTextures[ i ], 1, GL_RGBA32F, _windowWidth, _windowHeight );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glNamedFramebufferTexture( _fboId, _drawBuffers[ i ], _gBufferTextures[ i ], 0 );
		}

		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32F, _windowWidth, _windowHeight );
		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_DEPTH_ATTACHMENT, _gBufferTextures[ 5 ], 0 );

		glNamedFramebufferDrawBuffers( _fboId, 5, _drawBuffers );

		if ( GL_FRAMEBUFFER_COMPLETE != glCheckNamedFramebufferStatus( _fboId, GL_DRAW_FRAMEBUFFER ) )
		{
			std::cout << "FRAMEBUFFER ERROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOR \n";
		}
	}


	void LabWork8::render()
	{
		glUseProgram( aProgram );
		glEnable( GL_DEPTH_TEST );
		// glClearColor

		if ( luminosityNeedsUpdating )
		{
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera->setFovy( _fovy );
			// fovyNeedsUpdating = false;
		}

		if ( perspecNeedsUpdating )
		{
			_camera->switchPerspect();
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

		glProgramUniformMatrix4fv( aProgram,
								   glGetUniformLocation( aProgram, "modelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _tmm._transformation ) );

		glProgramUniform3fv( aProgram,
							 glGetUniformLocation( aProgram, "lightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera->_position, 1 ) ) );

		glProgramUniform3fv(
			aProgram, glGetUniformLocation( aProgram, "cameraPos" ), 1, glm::value_ptr( _camera->_position ) );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fboId );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_tmm.render( aProgram );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		if ( lightPassEnabled )
		{
			renderAOPasses();
			renderLightingPass();
		}
		else
		{
			glNamedFramebufferReadBuffer( _fboId, _drawBuffers[ _listBoxSelectedValue ] );

			glBlitNamedFramebuffer( _fboId,
									0,
									0,
									0,
									_windowWidth,
									_windowHeight,
									0,
									0,
									_windowWidth,
									_windowHeight,
									GL_COLOR_BUFFER_BIT,
									GL_NEAREST );
		}

		// TODO CLEAN , UNIFORM, TEXTURE, DRAW
	}

	void LabWork8::renderLightingPass()
	{
		glUseProgram( _lightingPassProgram );
		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		for ( size_t i = 0; i < 6; i++ )
		{
			glBindTextureUnit( i, _gBufferTextures[ i ] );
		}
		glBindTextureUnit( 6,ssaoColorBufferBlur );


		// ATTENTION A PASSER EN TANGENT SPACE
		glProgramUniform3fv( _lightingPassProgram,
							 glGetUniformLocation( aProgram, "TlightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera->_position, 1 ) ) );

		glBindVertexArray( quadVAO );

		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		for ( size_t i = 0; i < 6; i++ )
		{
			glBindTextureUnit( i, 0 );
		}
	}


	void LabWork8::renderAOPasses() {
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoFBO );
		programWrapper3.useProgram();
		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindTextureUnit( 0, _gBufferTextures[ 0 ] );
		glBindTextureUnit( 1, _gBufferTextures[ 1 ] );
		glBindTextureUnit( 2, noiseTexture );

		glBindVertexArray( quadVAO );

		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		glBindTextureUnit( 1, 0 );
		glBindTextureUnit( 2, 0 );
		glBindTextureUnit( 0, 0 );

		
		glBindFramebuffer( GL_FRAMEBUFFER, ssaoBlurFBO );
		programWrapper4.useProgram();
		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindTextureUnit( 0, ssaoColorBuffer );
	
		glBindVertexArray( quadVAO );

		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		glBindTextureUnit( 1, 0 );

	}

	void LabWork8::computeAO() {
		std::uniform_real_distribution<float> randomFloats( 0.f, 1.f ); // generates random floats between 0.0 and 1.0
		std::default_random_engine			  generator;
		
		for ( unsigned int i = 0; i < 64; ++i )
		{
			glm::vec3 sample( randomFloats( generator ) * 2.0 - 1.0,
							  randomFloats( generator ) * 2.0 - 1.0,
							  randomFloats( generator ) );
			sample = glm::normalize( sample );
			sample *= randomFloats( generator );
			float scale = float( i ) / 64.0f;

			// scale samples s.t. they're more aligned to center of kernel
			scale = myLerp( 0.1f, 1.0f, scale * scale );
			sample *= scale;
			ssaoKernel.push_back( sample );
		}

		for ( unsigned int i = 0; i < 16; i++ )
		{
			glm::vec3 noise( randomFloats( generator ) * 2.0 - 1.0,
							 randomFloats( generator ) * 2.0 - 1.0,
							 0.0f ); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back( noise );
		}


	}


	void LabWork8::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera->moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera->moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera->moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera->moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera->moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera->moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera->rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork8::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if ( ImGui::ColorEdit3( "BackGround Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		ImGui::Checkbox( "Trackball", &trackballSwitch );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrtho );

		ImGui::Checkbox( "LightingPass", &lightPassEnabled );

		// source imgui

		if ( ImGui::BeginListBox( "ATTACHMENT TO DISPLAY" ) )
		{
			for ( int n = 0; n < 5; n++ )
			{
				const bool is_selected = ( _listBoxSelectedValue == n );
				if ( ImGui::Selectable( _listBox[ n ], is_selected ) )
					_listBoxSelectedValue = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if ( is_selected )
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork8::_updateViewMatrix()
	{
		_matrixWtoV			  = _camera->getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork8::_updateProjectionMatrix()
	{
		_matrixVtoC			  = _camera->getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork8::_initCamera()
	{
		_camera->setScreenSize( 1280, 720 );
		_camera->setPosition( Vec3f( 0.f, 0.f, 0.2f ) );
		_camera->setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

} // namespace M3D_ISICG
