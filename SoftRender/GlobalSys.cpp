#include "GlobalSys.h"

GlobalSys::GlobalSys()
{
	buffer_width=800;
	buffer_height=600;
	pixel_num=buffer_width*buffer_height;
	m_cam.init((float)buffer_width,(float)buffer_height);
	init_Light();
	init_Material();
}
GlobalSys::~GlobalSys()
{
}

void GlobalSys::reset_materials()
{
	for(int i=0;i<10;i++)
		materials[i].texture.free_data();
}

void GlobalSys::init_Light()
{
	light.attr=1; //direction light
	light.c_ambient=Vec4::Ones();
	light.c_diffuse=Vec4::Ones();
	light.c_specular=Vec4::Ones();
	light.pos=Vec3::Zero();
	light.dir=Vec3(0,1,1).normalized();

}

void GlobalSys::init_Material()
{
	materials[0].name="gray";
	materials[0].attr=2; //flat
	materials[0].color.x=200;
	materials[0].color.y=200;
	materials[0].color.z=200;

	materials[0].power=materials[0].ks=materials[0].kd=materials[0].ka=1.0f;
	materials[0].ra=Vec4(0.12108f,0.13282f,0.15078f,1.0f);
	//materials[0].ra=Vec4(1.0f,0.0f,0.0f,1.0f);
	materials[0].rd=Vec4(0.48432f,0.53128f,0.60312f,1.0f);
	materials[0].rs=Vec4(1.0f,1.0f,1.0f,1.0f);
}

GlobalSys* global_sys=NULL;

