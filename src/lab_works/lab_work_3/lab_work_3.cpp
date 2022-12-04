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


		glDeleteBuffers( 1, &_cube.VBOVertices );
		glDeleteBuffers( 1, &_cube.VBOColors );
		glDeleteBuffers( 1, &_cube.EBO );
		glDeleteVertexArrays( 1, &_cube.VAO );

		glDeleteBuffers( 1, &_cube2.VBOVertices );
		glDeleteBuffers( 1, &_cube2.VBOColors );
		glDeleteBuffers( 1, &_cube2.EBO );
		glDeleteVertexArrays( 1, &_cube2.VAO );
	}


	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable(GL_DEPTH_TEST);  

		//Chemin des shaders 
		const std::string vertexShaderStr =  _shaderFolder + "lw3.vert" ;
		const std::string fragShaderStr =  _shaderFolder + "lw3.frag" ;

		std::string paths[ 2 ] = { vertexShaderStr, fragShaderStr };

		programWrapper.createProgram( paths );

		aProgram = programWrapper.getProgramId();


		_cube = _graphCube1._createCube();
		_graphCube1.SetMesh( _cube );
		
		_cube2 = _graphCube1._createCube();
		_graphCube2.SetMesh( _cube2 );

		_cube3 = _graphCube1._createCube();
		_graphCube3.SetMesh( _cube3 );
		
		_graphCube1.AddChild( &_graphCube2 );
		_graphCube2.AddChild( &_graphCube3 );

		_graphCube2.SetTransform( glm::translate( _graphCube2.GetTransform(), Vec3f( 3.0f, .0f, .0f ) ) );
		
		_graphCube3.SetTransform( glm::translate( _graphCube3.GetTransform(), Vec3f( 1.5f, .0f, .0f ) ) );

		_graphCube1.Update();

	//	std::cout << "cube 3 pos : " << _graphCube3.GetWorldTransform() << "\n";

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

		//_graphCube1.SetWorldTransform( glm::rotate( _graphCube1.GetWorldTransform(), p_deltaTime, Vec3f( 1, 1, 0 ) ) );
		
		_graphCube2.SetTransform( glm::translate( _graphCube2.GetTransform(), Vec3f( -3.0f, .0f, .0f ) ) );
		_graphCube2.SetTransform( glm::rotate( _graphCube2.GetTransform(), p_deltaTime, Vec3f( 0, 1, 0 ) ) );
		_graphCube2.SetTransform( glm::translate( _graphCube2.GetTransform(), Vec3f( 3.0f, .0f, .0f ) ) );
		
		_graphCube3.SetTransform( glm::translate( _graphCube3.GetTransform(), Vec3f( -1.5f, .0f, .0f ) ) );
		_graphCube3.SetTransform( glm::rotate( _graphCube3.GetTransform(), p_deltaTime, Vec3f( 0, 1, 0 ) ) );
		_graphCube3.SetTransform( glm::translate( _graphCube3.GetTransform(), Vec3f( 1.5f, .0f, .0f ) ) );

		_graphCube1.Update();

		


	}


	void LabWork3::render() { 
		//glClearColor
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_transformationMatrix = _matrixVtoC * _matrixWtoV * _graphCube1.GetWorldTransform();
		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		glBindVertexArray( _cube.VAO );
		glDrawElements( GL_TRIANGLES, _cube.vectorIndices.size(), GL_UNSIGNED_INT, 0 );

		
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _graphCube2.GetWorldTransform();
		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		glBindVertexArray( _cube2.VAO );
		glDrawElements( GL_TRIANGLES, _cube2.vectorIndices.size(), GL_UNSIGNED_INT, 0 );


		_transformationMatrix = _matrixVtoC * _matrixWtoV * _graphCube3.GetWorldTransform();
		glProgramUniformMatrix4fv(
			aProgram, transformationMatrix, 1, GL_FALSE, glm::value_ptr( _transformationMatrix ) );

		glBindVertexArray( _cube3.VAO );
		glDrawElements( GL_TRIANGLES, _cube3.vectorIndices.size(), GL_UNSIGNED_INT, 0 );


		//glDrawArrays(GL_TRIANGLES ,0,triangleVertexes.size());
		glBindVertexArray(0);

		if (luminosityNeedsUpdating) {
			glProgramUniform1f( aProgram, luminosityUint, _luminosity );
		}
		if ( fovyNeedsUpdating )
		{
			_updateViewMatrix();
			_updateProjectionMatrix();
			_camera->setFovy(_fovy);
		}

		if ( perspecNeedsUpdating )
		{
			_camera->switchPerspect();
			_updateProjectionMatrix();
			perspecNeedsUpdating = false;
		}

			if ( trackBallNeedsUpdating )
		{
			if ( trackBallEnabled )
			{
				_camera->setPosition( Vec3f( 0 ) );
				_trackBallCamera.switchCamera( _camera->_position );
				_camera = &_trackBallCamera;
			}
			else
			{
				_baseCamera.switchCamera( _camera->_position );
				_camera = &_baseCamera;
			}
		}

	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
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

	void LabWork3::displayUI()
	{
		luminosityNeedsUpdating = ImGui::SliderFloat( "Luminosity", &_luminosity, 0, 1 );
		if (ImGui::ColorEdit3("BackGround Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		};

		trackBallNeedsUpdating = ImGui::Checkbox( "Trackball", &trackBallEnabled );
		perspecNeedsUpdating = ImGui::Checkbox( "Ortho", &perspecOrtho );


		fovyNeedsUpdating = ImGui::SliderFloat( "Fovy", &_fovy, 0, 180 );
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	void LabWork3::_updateViewMatrix() {

		_matrixWtoV = _camera->getViewMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
	}

	void LabWork3::_updateProjectionMatrix()
	{
		_matrixVtoC = _camera->getProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
		
	}

	void LabWork3::_initCamera() { 
		_camera->setScreenSize( 1280, 720 );
		_camera->setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera->setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
		_transformationMatrix = _matrixVtoC * _matrixWtoV * _cube.transformationMatrice;
	
	}

} // namespace M3D_ISICG
