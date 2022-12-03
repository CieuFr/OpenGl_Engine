
#ifndef __BASE_CAMERA_HPP__
#define __BASE_CAMERA_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include "GL/gl3w.h"
#include "define.hpp"

namespace M3D_ISICG
{
	class BaseCamera
	{

		 public:
		BaseCamera() {};
		BaseCamera(Vec3f position) { _position = position;
		};

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }
	
		Vec3f _position = VEC3F_ZERO;
	  protected:
		bool _orthoPerspec = false;

		
		Vec3f _invDirection = Vec3f( 0.f, 0.f, 1.f );  // Dw dans le cours.
		Vec3f _right		= Vec3f( -1.f, 0.f, 0.f ); // Rw dans le cours.
		Vec3f _up			= Vec3f( 0.f, 1.f, 0.f );  // Uw dans le cours.
		// Angles defining the orientation in degrees
		float _yaw	 = 90.f;
		float _pitch = 0.f;

		int	  _screenWidth	= 1280;
		int	  _screenHeight = 720;
		float _aspectRatio	= float( _screenWidth ) / _screenHeight;
		float _fovy			= 60.f;
		float _zNear		= 0.1f;
		float _zFar			= 1000.f;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		public:

		void setScreenSize( const int p_width, const int p_height )
		{
			_screenWidth  = p_width;
			_screenHeight = p_height;
			_aspectRatio  = float( _screenWidth ) / _screenHeight;
			_updateVectors();
			_computeViewMatrix();
			_computeProjectionMatrix();
		}

		virtual void moveFront( const float p_delta )
		{
			_position -= _invDirection * p_delta;
			_computeViewMatrix();
		}

		virtual void moveRight( const float p_delta )
		{
			_position += _right * p_delta;
			_computeViewMatrix();
		}

		virtual void moveUp( const float p_delta )
		{
			_position += _up * p_delta;
			_computeViewMatrix();
		}

		virtual void rotate( const float p_yaw, const float p_pitch )
		{
			_yaw   = glm::mod( _yaw + p_yaw, 360.f );
			_pitch = glm::clamp( _pitch + p_pitch, -89.f, 89.f );
			_updateVectors();
		}

		virtual void switchCamera( Vec3f position ) { setPosition(position); }


		void print() const
		{
			std::cout << "======== Camera ========" << std::endl;
			std::cout << "Position: " << glm::to_string( _position ) << std::endl;
			std::cout << "View direction: " << glm::to_string( -_invDirection ) << std::endl;
			std::cout << "Right: " << glm::to_string( _right ) << std::endl;
			std::cout << "Up: " << glm::to_string( _up ) << std::endl;
			std::cout << "Yaw: " << _yaw << std::endl;
			std::cout << "Pitch: " << _pitch << std::endl;
			std::cout << "========================" << std::endl;
		}

		 void setPosition( const Vec3f & p_position )
		{
			_position = p_position;
			_computeViewMatrix();
		}

		void setLookAt( const Vec3f & p_lookAt )
		{
			_invDirection = _position - p_lookAt;
			_computeViewMatrix();
		}

		void setFovy( const float p_fovy ) // fovy en radians
		{
			_fovy = p_fovy;
			_computeProjectionMatrix();
		}

		void _computeViewMatrix() { _viewMatrix = glm::lookAt( _position, _position - _invDirection, _up ); }

		void _computeProjectionMatrix()
		{
			if ( !_orthoPerspec )
			{
				_projectionMatrix = glm::perspective( glm::radians( _fovy ), _aspectRatio, _zNear, _zFar );
			
			}
			else
			{
				_projectionMatrix = glm::ortho( -2.f, 2.f, -1.111f, 1.111f, _zNear, _zFar );
			}

		}

		void _updateVectors()
		{
			const float yaw	  = glm::radians( _yaw );
			const float pitch = glm::radians( _pitch );
			_invDirection	  = glm::normalize(
				Vec3f( glm::cos( yaw ) * glm::cos( pitch ), glm::sin( pitch ), glm::sin( yaw ) * glm::cos( pitch ) ) );
			_right = glm::normalize( glm::cross( Vec3f( 0.f, 1.f, 0.f ), _invDirection ) ); // We suppose 'y' as up.
			_up	   = glm::normalize( glm::cross( _invDirection, _right ) );

			_computeViewMatrix();
		}

		void switchPerspect()
		{
			_orthoPerspec = !_orthoPerspec;
			_computeProjectionMatrix();
		}

	

	}; // namespace M3D_ISICG
} // namespace M3D_ISICG
#endif // __BASE_LAB_WORK_HPP__
