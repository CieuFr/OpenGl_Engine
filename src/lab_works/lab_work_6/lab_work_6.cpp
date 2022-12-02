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
		const std::string vertexShaderStr = readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "geometry_pass.frag" );

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

		//=============TP 5 ==============/

		_tmm.load( "bunny", FilePath( "./data/models/bunny.obj" ) );

		//REMOVE COMMENT FOR SPONZA
		//_tmm._transformation = glm::scale( _tmm._transformation, Vec3f( 0.003, 0.003, 0.003 ) );


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
	
		//const std::string vertexShaderStr = readFile( _shaderFolder + "lighting_pass.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lighting_pass.frag" );

		// Création des shaders
		//const GLuint aVertexShader	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
		//const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Création des shaders
		//glShaderSource( aVertexShader, 1, &vSrc, NULL );
		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
	//	glCompileShader( aVertexShader );
		glCompileShader( aFragmentShader );

		// Code Cf. Tp 1 pour vérifier si les shaders compilent
		GLint compiled;
		glGetShaderiv( aFragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( aFragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( aFragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Initialisation du Program
		_lightingPassProgram = glCreateProgram();

		// Attache des shaders
		//glAttachShader( _lightingPassProgram, aVertexShader );
		glAttachShader( _lightingPassProgram, aFragmentShader );

		// Link du programme
		glLinkProgram( _lightingPassProgram );
		GLint linked;
		glGetProgramiv( _lightingPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _lightingPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Deletion des shaders
		//glDeleteShader( aVertexShader );
		glDeleteShader( aFragmentShader );

		drawQuad();

		return true;

	}

	void LabWork6::initGBuffer() {

		// INIT FBO
		glCreateFramebuffers( 1, &_fboId );

		glCreateTextures( GL_TEXTURE_2D, 6, _gBufferTextures );
		
		for ( size_t i = 0; i < 5; i++ ) {
			glTextureStorage2D( _gBufferTextures[ i ], 1, GL_RGBA32F, _windowWidth, _windowHeight );
			glNamedFramebufferTexture( _fboId, _drawBuffers[ i ], _gBufferTextures[ i ], 0 );
		}

		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32, _windowWidth, _windowHeight );
		glNamedFramebufferTexture( _fboId, GL_DEPTH_ATTACHMENT, _gBufferTextures[ 5 ], 0 );

		glNamedFramebufferDrawBuffers( _fboId, 6, _drawBuffers );
		
		if ( GL_FRAMEBUFFER_COMPLETE != glCheckNamedFramebufferStatus( _fboId, GL_DRAW_FRAMEBUFFER) )
		{
			std::cout << "FRAMEBUFFER ERROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOR \n";		 
		}
	}

	void LabWork6::render()
	{
		glUseProgram( aProgram );
		glEnable( GL_DEPTH_TEST );
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

		glProgramUniformMatrix4fv( aProgram,
								   glGetUniformLocation( aProgram, "modelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _tmm._transformation ) );

		glProgramUniform3fv( aProgram,
							 glGetUniformLocation( aProgram, "lightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera._position, 1 ) ) );

		glProgramUniform3fv(
			aProgram, glGetUniformLocation( aProgram, "cameraPos" ), 1, glm::value_ptr( _camera._position ) );


		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fboId );

		_tmm.render( aProgram );

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
									GL_COLOR_BUFFER_BIT,
									GL_NEAREST );
		}
	
		

		
		
		//TODO CLEAN , UNIFORM, TEXTURE, DRAW

	}

	void LabWork6::renderLightingPass() {
		glUseProgram( _lightingPassProgram );
		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		for ( size_t i = 0; i < 5; i++ ) {
			glBindTextureUnit( i, _gBufferTextures[ i ] );
		}
		
		//ATTENTION A PASSER EN TANGENT SPACE
		glProgramUniform3fv( _lightingPassProgram,
							 glGetUniformLocation( aProgram, "TlightPos" ),
							 1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera._position, 1 ) ) );

		glBindVertexArray( quadVAO );

		
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		for ( size_t i = 0; i < 5; i++ )
		{
			glBindTextureUnit( i, 0 );
		}
		

	}

	void LabWork6::drawQuad() {
		
		Vec2f triangleVertices[ 4 ] = { Vec2f( -1.f, -1.f ), Vec2f( -1.f, 1.f ), Vec2f( 1.f, -1.f ), Vec2f( 1.f, 1.f ) };
		static float texCoords[] = { 0, 0, 0, 1,1, 0, 1, 1 };


		int eboPositions[6] = { 0, 1, 2, 1, 2, 3 };
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

	void LabWork6::handleEvents( const SDL_Event & p_event )
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
		_matrixWtoV			  = _camera.getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork6::_updateProjectionMatrix()
	{
		_matrixVtoC			  = _camera.getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork6::_initCamera()
	{
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f( 0.f, 0.f, 0.2f ) );
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

} // namespace M3D_ISICG
