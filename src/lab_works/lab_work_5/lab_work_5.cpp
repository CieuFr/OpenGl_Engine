#include "lab_work_5.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"



namespace M3D_ISICG
{

	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_5/shaders/";

	LabWork5::~LabWork5() {
		glDeleteProgram( aProgram );
		glDeleteProgram( _programSkyBox.getProgramId() );

	}

	bool LabWork5::initSkyBox()
	{
		const std::string vertexShaderStr2 = _shaderFolder + "sky_box.vert";
		const std::string fragShaderStr2   = _shaderFolder + "sky_box.frag";
		std::string		  paths2[ 2 ]	   = { vertexShaderStr2, fragShaderStr2 };

		_programSkyBox.createProgram( paths2 );

		glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &skyboxTexture );

		glTextureParameteri( skyboxTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTextureParameteri( skyboxTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( skyboxTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTextureParameteri( skyboxTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTextureParameteri( skyboxTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		std::string faces[ 6 ] { "./data/models/right.png",	 "./data/models/left.png",	"./data/models/top.png",
								 "./data/models/bottom.png", "./data/models/front.png", "./data/models/back.png" };

		bool  notInit = true;
		Image image;

		for ( size_t i = 0; i < 6; i++ )
		{
			image.load( faces[ i ] );

			if ( notInit )
			{
				glTextureStorage2D( skyboxTexture, 1, GL_RGB32F, image._width, image._height );
				notInit = false;
			}
			glTextureSubImage3D(
				skyboxTexture, 0, 0, 0, i, image._width, image._height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image._pixels );
		}

		return true;
	}


	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

	
		glEnable(GL_DEPTH_TEST);  
		//glEnable( GL_BLEND );
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//Chemin des shaders 
		const std::string vertexShaderStr =  _shaderFolder + "mesh_texture.vert" ;
		const std::string fragShaderStr = _shaderFolder + "mesh_texture.frag" ;

		std::string paths[ 2 ] = { vertexShaderStr, fragShaderStr };
		
		program1.createProgram(paths);
	
		aProgram = program1.getProgramId();
		_initCamera();

		// Get Uniform luminosity


		program1.setUniform( "luminosity", _luminosity );

		// Get Uniform transformationMatrix
		transformationMatrix = glGetUniformLocation( aProgram, "uMVPMatrix" );



		//=============TP 5 ==============/

		_tmm.load("sponza", FilePath("./data/models/sponza.obj" ));

		_tmm._transformation = glm::scale( _tmm._transformation, Vec3f( 0.003, 0.003, 0.003 ) );
		_skyboxMesh			 = drawer.createCube();
		
		initSkyBox();
		
			// INIT du program
		glUseProgram( aProgram );


		std::cout << "Done!" << std::endl;
		return true;
	}


	void LabWork5::renderSkyBox()
	{
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glDepthFunc( GL_LEQUAL );
		glUseProgram( _programSkyBox.getProgramId() );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glm::mat4 view		 = glm::mat4( 1.0f );
		glm::mat4 projection = glm::mat4( 1.0f );
		view				 = glm::mat4(
			glm::mat3( glm::lookAt( _camera->_position, _camera->_position + _camera->_invDirection, _camera->_up ) ) );
		projection = glm::perspective( glm::radians( 45.0f ), (float)_windowWidth / _windowHeight, 0.1f, 100.0f );

		glProgramUniformMatrix4fv( _programSkyBox.getProgramId(),
								   glGetUniformLocation( _programSkyBox.getProgramId(), "projection" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( projection ) );

		glProgramUniformMatrix4fv( _programSkyBox.getProgramId(),
								   glGetUniformLocation( _programSkyBox.getProgramId(), "view" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( view ) );

		glBindVertexArray( _skyboxMesh.VAO );

		glBindTextureUnit( 0, skyboxTexture );

		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

		glBindTextureUnit( 0, 0 );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		glDepthFunc( GL_LESS );
	}


	void LabWork5::animate( const float p_deltaTime ) {

			
	}

	void LabWork5::render() { 
		//glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if (luminosityNeedsUpdating) {
			program1.setUniform( "luminosity", _luminosity );
		}


		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera->setFovy(_fovy);
			//fovyNeedsUpdating = false;
		}

		if ( perspecNeedsUpdating )
		{
			_camera->switchPerspect();
			_updateProjectionMatrix();
			
			perspecNeedsUpdating = false;
		}

		if ( trackBallNeedsUpdating )
		{
			if (trackBallEnabled) {
				_trackBallCamera.switchCamera( _camera->_position );
				_camera = &_trackBallCamera;
			}
			else
			{
				_baseCamera.switchCamera( _camera->_position );
				_camera = &_baseCamera;
			}
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
								   glm::value_ptr( _matrixWtoV * _tmm._transformation ) );

				glProgramUniformMatrix4fv( aProgram,
								   glGetUniformLocation( aProgram, "modelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _tmm._transformation ) );


		glProgramUniform3fv( aProgram,
								   glGetUniformLocation( aProgram, "lightPos" ),
								   1,
							 glm::value_ptr( _matrixWtoV * Vec4f( _camera->_position,1) ) );

		glProgramUniform3fv(
			aProgram, glGetUniformLocation( aProgram, "cameraPos" ), 1, glm::value_ptr( _camera->_position ) );


		_tmm.render( aProgram );

	
		renderSkyBox();	
	
	
	}

	void LabWork5::handleEvents( const SDL_Event & p_event )
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

	void LabWork5::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		trackBallNeedsUpdating = ImGui::Checkbox( "Trackball", &trackBallEnabled );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrthoEnabled );


		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork5::_updateViewMatrix() {

		_matrixWtoV = _camera->getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork5::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera->getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	}

	void LabWork5::_initCamera() { 
		_camera->setScreenSize( 1280, 720 );
		_camera->setPosition( Vec3f( 0.f, 0.f, 0.2f ) );
		_camera->setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _tmm._transformation;
	
	}

} // namespace M3D_ISICG
