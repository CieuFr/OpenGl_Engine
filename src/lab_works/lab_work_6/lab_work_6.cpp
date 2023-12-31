#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_6.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{

	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

	LabWork6::~LabWork6() { 
		glDeleteProgram( aProgram );
		glDisableVertexArrayAttrib( quadVAO, 0 );
		glDeleteVertexArrays( 1, &quadVAO );
		// Delete VBO
		glDeleteBuffers( 1, &quadVBO );


		// Delete VBO
		glDeleteBuffers( 1, &quadEBO );
	
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable( GL_DEPTH_TEST );
		// glEnable( GL_BLEND );
		// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		// Chemin des shaders
		const std::string vertexShaderStr =  _shaderFolder + "geometry_pass.vert" ;
		const std::string fragShaderStr	  =  _shaderFolder + "geometry_pass.frag" ;
		std::string paths[ 2 ] = { vertexShaderStr, fragShaderStr };
		programWrapper.createProgram(paths);
		aProgram = programWrapper.getProgramId();

		_initCamera();

		// Get Uniform luminosity
		luminosityUint = glGetUniformLocation( aProgram, "luminosity" );
		glProgramUniform1f( aProgram, luminosityUint, _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "uMVPMatrix" );

		//=============TP 5 ==============/

		_tmm.load( "sponza", FilePath( "./data/models/sponza.obj" ) );

		//REMOVE COMMENT FOR SPONZA
		_tmm._transformation = glm::scale( _tmm._transformation, Vec3f( 0.003, 0.003, 0.003 ) );


		//=============TP 6 ==============/

		initLightingPassProgram();
		initGBuffer();

		// INIT du program
		glUseProgram( aProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::animate( const float p_deltaTime ) {}


	bool LabWork6::initLightingPassProgram()
	{
		const std::string vertexShaderStr =  _shaderFolder + "lighting_pass.vert" ;
		const std::string fragShaderStr	  =  _shaderFolder + "lighting_pass.frag" ;
		std::string		  paths[ 2 ]	  = { vertexShaderStr, fragShaderStr };
		
		programWrapper2.createProgramOnlyFS( fragShaderStr );
		_lightingPassProgram = programWrapper2.getProgramId();
		
		quadVAO = drawer.drawQuad();

		return true;

	}

	
// TP6
	
	void LabWork6::initGBuffer()
	{
		/*glGenRenderbuffers( 1, &rboDepth );
		glBindRenderbuffer( GL_RENDERBUFFER, rboDepth );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth );*/

		// INIT FBO
		glCreateFramebuffers( 1, &_fboId );

		glCreateTextures( GL_TEXTURE_2D, 6, _gBufferTextures );

		for ( size_t i = 0; i < 5; i++ )
		{
			glTextureStorage2D( _gBufferTextures[ i ], 1, GL_RGBA32F, _windowWidth, _windowHeight );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glNamedFramebufferTexture( _fboId, _drawBuffers[ i ], _gBufferTextures[ i ], 0 );
		}

		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32, _windowWidth, _windowHeight );
		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_DEPTH_ATTACHMENT, _gBufferTextures[ 5 ], 0 );

		glNamedFramebufferDrawBuffers( _fboId, 5, _drawBuffers );

		if ( GL_FRAMEBUFFER_COMPLETE != glCheckNamedFramebufferStatus( _fboId, GL_DRAW_FRAMEBUFFER ) )
		{
			std::cout << "FRAMEBUFFER ERROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOR \n";
		}
	}

	void LabWork6::initGBuffer2()
	{
		glCreateFramebuffers( 1, &_fboId );
		glBindFramebuffer( GL_FRAMEBUFFER, _fboId );
		// Cr�er un texture attachment pour stocker les positions des fragments

		glCreateTextures( GL_TEXTURE_2D, 1, &positionTexture );
		glTextureStorage2D( positionTexture, 1, GL_RGB16F, _windowWidth, _windowHeight );
		glTextureParameteri( positionTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( positionTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_COLOR_ATTACHMENT0, positionTexture, 0 );

		// Cr�er un texture attachment pour stocker les normales des fragments

		glCreateTextures( GL_TEXTURE_2D, 1, &normalTexture );
		glTextureStorage2D( normalTexture, 1, GL_RGB16F, _windowWidth, _windowHeight );
		glTextureParameteri( normalTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( normalTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_COLOR_ATTACHMENT1, normalTexture, 0 );

		// Cr�er un texture attachment pour stocker les ambient des fragments

		glCreateTextures( GL_TEXTURE_2D, 1, &ambientTexture );
		glTextureStorage2D( ambientTexture, 1, GL_RGBA16F, _windowWidth, _windowHeight );
		glTextureParameteri( ambientTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( ambientTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_COLOR_ATTACHMENT2, ambientTexture, 0 );

		// Cr�er un texture attachment pour stocker les diffuse des fragments

		glCreateTextures( GL_TEXTURE_2D, 1, &diffuseTexture );
		glTextureStorage2D( diffuseTexture, 1, GL_RGBA16F, _windowWidth, _windowHeight );
		glTextureParameteri( diffuseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( diffuseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_COLOR_ATTACHMENT3, diffuseTexture, 0 );

		// Cr�er un texture attachment pour stocker les specular des fragments

		glCreateTextures( GL_TEXTURE_2D, 1, &specularTexture );
		glTextureStorage2D( specularTexture, 1, GL_RGBA16F, _windowWidth, _windowHeight );
		glTextureParameteri( specularTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( specularTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_COLOR_ATTACHMENT4, specularTexture, 0 );

		// Cr�er un texture attachment pour stocker les profondeurs des fragments
		glCreateTextures( GL_TEXTURE_2D, 1, &depthTexture );
		glTextureStorage2D( depthTexture, 1, GL_DEPTH_COMPONENT32F, _windowWidth, _windowHeight );
		glTextureParameteri( depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glNamedFramebufferTexture( _fboId, GL_DEPTH_ATTACHMENT, depthTexture, 0 );

		// Cr�er un tableau des attachments pour le framebuffer
		GLuint attachments[ 5 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
									GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
		};
		glNamedFramebufferDrawBuffers( _fboId, 5, attachments );

		_gBufferTextures[ 0 ] = positionTexture;
		_gBufferTextures[ 1 ] = normalTexture;
		_gBufferTextures[ 2 ] = ambientTexture;
		_gBufferTextures[ 3 ] = diffuseTexture;
		_gBufferTextures[ 4 ] = specularTexture;
		_gBufferTextures[ 5 ] = depthTexture;
		
		// V�rifier que le framebuffer
		if ( glCheckNamedFramebufferStatus( _fboId, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cout << "Error: GBuffer framebuffer is incomplete!" << std::endl;
		}
		
	}
	
	void LabWork6::render()
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

		if (lightPassEnabled) {
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
									GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
									GL_NEAREST );
		}
	
		

		
		
		//TODO CLEAN , UNIFORM, TEXTURE, DRAW

	}

	void LabWork6::renderLightingPass() {

		glUseProgram( _lightingPassProgram );
		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		for ( size_t i = 0; i < 5; i++ ) {
			glBindTextureUnit( i, _gBufferTextures[ i ] );
		}
		
		//ATTENTION A PASSER EN TANGENT SPACE
		glProgramUniform3fv( _lightingPassProgram,
							 glGetUniformLocation( aProgram, "TlightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera->_position, 1 ) ) );

		glBindVertexArray( quadVAO );
		
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		for ( size_t i = 0; i < 5; i++ )
		{
			glBindTextureUnit( i, 0 );
		}
		

	}

	
	void LabWork6::drawQuad2()
		{
			int _indices[ 6 ] = { 0, 1, 2, 2, 1, 3 };
			// Les sommets du rectangle
			float _vertices[] = { 
					-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
			};

			unsigned int _vbo, _ebo;

			glCreateBuffers( 1, &_vbo );
			glCreateBuffers( 1, &_ebo );
			glNamedBufferData( _vbo, sizeof( _vertices ), _vertices, GL_STATIC_DRAW );

			glNamedBufferData( _ebo,  sizeof( _indices ), _indices, GL_STATIC_DRAW );

			glCreateVertexArrays( 1, &quadVAO );

			glEnableVertexArrayAttrib( quadVAO, 0 );
			glEnableVertexArrayAttrib( quadVAO, 1 );

			glVertexArrayAttribFormat( quadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayAttribFormat( quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float) );

			glVertexArrayAttribBinding( quadVAO, 0, 0 );
			glVertexArrayAttribBinding( quadVAO, 1, 0 );

			glVertexArrayVertexBuffer( quadVAO, 0, _vbo, 0, 5 * sizeof( float ) );

			glVertexArrayElementBuffer( quadVAO, _ebo );

		}
	

	void LabWork6::drawQuad() {
		if (quadVAO == 0) {
			Vec2f triangleVertices[ 4 ]
				= { Vec2f( -1.f, -1.f ), Vec2f( -1.f, 1.f ), Vec2f( 1.f, -1.f ), Vec2f( 1.f, 1.f ) };
			static float texCoords[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0 };

			int eboPositions[ 6 ] = { 0, 1, 2, 1, 2, 3 };

			 glCreateVertexArrays( 1, &quadVAO );

			glEnableVertexArrayAttrib( quadVAO, 0 );
			glEnableVertexArrayAttrib( quadVAO, 1 );

			// Init VBO  Vertex Buffer Object Sommet
			glCreateBuffers( 1, &quadVBO );
			glCreateBuffers( 1, &quadVBO2 );
			// Creation EBO Sommet
			glCreateBuffers( 1, &quadEBO );

			glVertexArrayAttribFormat( quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayVertexBuffer( quadVAO, 0, quadVBO, 0, sizeof( Vec2f ) );
			glVertexArrayAttribBinding( quadVAO, 0, 0 );
			glVertexArrayElementBuffer( quadVAO, quadEBO );

			glVertexArrayAttribFormat( quadVAO, 1, 2, GL_FLOAT, GL_TRUE, 0 );
			glVertexArrayVertexBuffer( quadVAO, 1, quadVBO2, 0, 2* sizeof( float ) );
			glVertexArrayAttribBinding( quadVAO, 1, 1 );

			glNamedBufferData( quadVBO, 4 * sizeof( Vec2f ), &triangleVertices, GL_STATIC_DRAW );
			glNamedBufferData( quadVBO2, 8 * sizeof( float ), &texCoords, GL_STATIC_DRAW );
			glNamedBufferData( quadEBO, 6 * sizeof( int ), &eboPositions, GL_STATIC_DRAW );
		
		}
		

	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
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

	void LabWork6::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if ( ImGui::ColorEdit3( "BackGround Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		ImGui::Checkbox( "Trackball", &trackballSwitch );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrtho );

		ImGui::Checkbox( "LightingPass", &lightPassEnabled );
		
		//source imgui
		
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

	void LabWork6::_updateViewMatrix()
	{
		_matrixWtoV			  = _camera->getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork6::_updateProjectionMatrix()
	{
		_matrixVtoC			  = _camera->getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork6::_initCamera()
	{
		_camera->setScreenSize( 1280, 720 );
		_camera->setPosition( Vec3f( 0.f, 0.f, 0.2f ) );
		_camera->setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

} // namespace M3D_ISICG
