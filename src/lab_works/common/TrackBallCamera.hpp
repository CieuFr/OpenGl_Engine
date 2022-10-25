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
		TrackBallCamera() {};

		 void moveRight( const float p_delta ) override;

		 void moveUp( const float p_delta ) override;
		
		 void rotate( const float p_yaw, const float p_pitch ) override;

		 void moveFront( const float p_delta );
		 

		 private: 

		 void updateTrackBallCameraAfterMove();
		 float _radius;
		 float _azimuthAngle; 

		 float _polarAngle;
		 Vec3f _center = Vec3f(0,0,0);
		   
		
	};
} // namespace M3D_ISICG

#endif // __CAMERA_HPP__
#pragma once
