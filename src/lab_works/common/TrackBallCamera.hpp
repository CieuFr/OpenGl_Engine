#ifndef __TRACKCAMERA_HPP__
#define __TRACKCAMERA_HPP__


#include "BaseCamera.hpp"
#include "GL/gl3w.h"
#include "define.hpp"

namespace M3D_ISICG
{
	
	class TrackBallCamera : public BaseCamera
	{
	  public:
		TrackBallCamera() {;
			TrackBallCamera( Vec3f( 0.f, 0.f, 0.f ), 1.f );
		};

		 TrackBallCamera(Vec3f center, float radius)
		{
			 std::cout << "radius INIT : : " << radius << "\n";
			 _radius		  = radius;
		    	_center		  = center;
			 _azimuthAngle	 = 0;
			 _polarAngle = 0;
			 setPosition( Vec3f( _center.x + radius, _center.y, _center.z ) );
			 setLookAt( _center );
			 std::cout << "radius 2 : : " << _radius << "\n";
		}

		void moveRight( const float p_delta ) override;

		 void moveUp( const float p_delta ) override;
		
		 void rotate( const float p_yaw, const float p_pitch ) override;

		 void moveFront( const float p_delta ) override;
		 
		 void switchCamera( Vec3f position ) override;
		

		 private: 

		 void updateTrackBallCameraAfterMove();
		  float pi = 3.141592653589793238462643383279502884f;
		 float _radius;
		 float _azimuthAngle; 

		 float _polarAngle;
		 Vec3f _center = Vec3f(0,0,0);
   
		
	};
} // namespace M3D_ISICG

#endif // __CAMERA_HPP__
#pragma once
