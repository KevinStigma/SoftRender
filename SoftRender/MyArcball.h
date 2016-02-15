#ifndef ZYK_ARCBALL_H
#define ZYK_ARCBALL_H
#include "ArcBall.h"
class MyArcball
{
public:
	MyArcball(float w,float h):ArcBall(w,h),mRadius(0.0),button_status(0)
	{
		Matrix3fSetIdentity(&LastRot); 
		Matrix3fSetIdentity(&ThisRot); 
		Transform.M[0]=1.0f;Transform.M[1]=0.0f;Transform.M[2]=0.0f;Transform.M[3]=0.0f;
		Transform.M[4]=0.0f;Transform.M[5]=1.0f;Transform.M[6]=0.0f;Transform.M[7]=0.0f;
		Transform.M[8]=0.0f;Transform.M[9]=0.0f;Transform.M[10]=1.0f;Transform.M[11]=0.0f;
		Transform.M[12]=0.0f;Transform.M[13]=0.0f;Transform.M[14]=0.0f;Transform.M[15]=1.0f;
	}
	void init();
	ArcBallT    ArcBall;
	Matrix4fT   Transform;
	Matrix3fT   LastRot;
	Matrix3fT   ThisRot;
	Point2fT    MousePt;  
	float		mRadius;
	Point2fT	mLastMousePos;
	int			button_status;
};
#endif