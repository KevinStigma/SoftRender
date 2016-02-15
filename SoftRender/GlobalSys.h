#ifndef SOFTRENDER_GLOBALSYS_H
#define SOFTRENDER_GLOBALSYS_H
#include "Data_structure.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if(p){delete (p);(p)=NULL;}}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) {if(p){delete[](p);(p)=NULL;}}
#endif

#define MAX_MATERIAL 10

class GlobalSys
{
public:
	GlobalSys();
	~GlobalSys();
	void reset_materials();
	int buffer_width,buffer_height,pixel_num;
	zyk::Camera m_cam;
	zyk::Material materials[10];
	zyk::Light light;
	void init_Light();
	void init_Material();
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif