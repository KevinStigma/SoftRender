#include "RenderView.h"
#include <QMouseEvent>
#include <QPicture>
#include <iostream>
#include <windows.h>
#include <WinBase.h>
#include <cstring>

extern GlobalSys* global_sys;
RenderView::RenderView(QWidget *parent):QWidget(parent),viewport_image(global_sys->buffer_width,global_sys->buffer_height,QImage::Format_RGB888),
	backface_check(true),render_buffer(NULL),output_polygon(false),draw_line(false),m_arcball(global_sys->buffer_width,global_sys->buffer_height),
	flat_shading(true),z_buffer(NULL),main_buffer(NULL),back_buffer(NULL),time_label(NULL),draw_vertex(false),draw_texture(false)
{
	main_buffer=new zyk::UCHAR3[global_sys->pixel_num];
	back_buffer=new zyk::UCHAR3[global_sys->pixel_num];
	z_buffer=new float[global_sys->pixel_num];
	memset(main_buffer,0,sizeof(zyk::UCHAR3)*global_sys->pixel_num);
	memset(back_buffer,0,sizeof(zyk::UCHAR3)*global_sys->pixel_num);
	render_buffer=main_buffer;
	setFixedSize(global_sys->buffer_width,global_sys->buffer_height);
	generateObj(3);
	min_clip_y=min_clip_x=0.0f;
	max_clip_x=global_sys->buffer_width-1;
	max_clip_y=global_sys->buffer_height-1;
	global_sys->m_cam.update_arcball_matrix(m_arcball.Transform.M);
	cur_id=-1;
	reset_Zbuffer();
}

RenderView::~RenderView()
{
	SAFE_DELETE_ARRAY(main_buffer);
	SAFE_DELETE_ARRAY(back_buffer);
	SAFE_DELETE_ARRAY(z_buffer);
}

void RenderView::render2viewport(zyk::RenderList&render_list)
{
	zyk::UCHAR3 color[3];
	if(cur_id==-1)
	{
		for(int i=0;i<render_list.num_polys;i++)
			Draw_Triangle_2D_ZB(render_list.plist[i],render_buffer,z_buffer,width());
	}
	else
		Draw_Triangle_2D_ZB(render_list.plist[cur_id],render_buffer,z_buffer,width());


	for(int i=0;i<height();i++)
	{
		int w_ind=i*width();
		for(int j=0;j<width();j++)
		{
			int index=w_ind+j;
			uint color=0;
			zyk::UCHAR3 tmp_color=render_buffer[index];
			viewport_image.setPixel(j,i,qRgb((int)tmp_color.x,(int)tmp_color.y,(int)tmp_color.z));
		}
	}
}

inline void RenderView::local_2_world(zyk::ObjectMesh& obj)
{
	//we use direct computation instead of matrix computation for saving more time
	for(int i=0;i<obj.num_vertices;i++)
	{
		for(int j=0;j<3;j++)
			obj.vert[i].vtrans[j]=obj.vert[i].vlocal[j]+obj.world_pos[j];
		obj.vert[i].vtrans[3]=1.0f;
		obj.vert[i].vtrans=global_sys->m_cam.arcball_trans_cam*obj.vert[i].vtrans;
		obj.vert[i].trans_nor=global_sys->m_cam.arcball_trans_cam*obj.vert[i].local_nor;
	}
	for(int i=0;i<obj.num_polys;i++)
		obj.plist[i].trans_normal=global_sys->m_cam.arcball_trans_cam*obj.plist[i].local_normal;
}

inline void RenderView::object_cull(zyk::ObjectMesh& obj)
{
	if(obj.cull_object(global_sys->m_cam,CULL_OBJECT_XYZ_PLANE))
	{
		obj.state=1;
	}
}

int RenderView::plus_id()
{
	if(cur_id==num_polys-1)
		cur_id=-1;
	else
		cur_id++;
	return cur_id;
}
int RenderView::minus_id()
{
	if(cur_id==-1)
		cur_id=num_polys-1;
	else
		cur_id--;
	return cur_id;
}

void RenderView::outputArcball(const std::string& filename)
{
	FILE*fp=fopen(filename.c_str(),"wb");
	if(!fp)
		return;
	float buffer[16];
	//store row major order
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			buffer[i*4+j]=global_sys->m_cam.arcball_trans_cam(i,j);

	fwrite(buffer,sizeof(float),16,fp);
	fwrite(&m_arcball.mRadius,sizeof(float),1,fp);
	fclose(fp);
}

void RenderView::inputArcball(const std::string& filename)
{
	FILE*fp=fopen(filename.c_str(),"rb");
	if(!fp)
		return;
	float buffer[16];
	fread(buffer,sizeof(float),16,fp);
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			global_sys->m_cam.arcball_trans_cam(i,j)=buffer[i*4+j];

	fread(&m_arcball.mRadius,sizeof(float),1,fp);
	fclose(fp);
}

void RenderView::backface_cull(zyk::ObjectMesh&obj)
{
	if(obj.state||!backface_check)
		return; // the object has been culled
	Vec4 view_dir=zyk::V3toV4(global_sys->m_cam.target-global_sys->m_cam.pos,0);//world coordinate
	for(int i=0;i<obj.num_polys;i++)
	{
		if(!(obj.plist[i].trans_normal.dot(view_dir)<0.f))
			obj.plist[i].state=1;
		else
			obj.plist[i].state=0;
	}
}

inline void color_clip(Vec4& color)
{
	for(int ci=0;ci<4;ci++)
	{
		if(color[ci]<0)
			color[ci]=0.f;
		else if(color[ci]>1.f)
			color[ci]=1.f;
	}
}

void RenderView::lighting_compute(zyk::RenderList&render_list)
{
	using zyk::dot_multV4;
	const zyk::Light & light=global_sys->light;
	zyk::Material *material=NULL;
	Vec3 light_dir_cam=zyk::transformNormal(light.dir,global_sys->m_cam.mcam);
	light_dir_cam.normalize();
	if(flat_shading)
	{
		for(int i=0;i<render_list.num_polys;i++)
		{
			zyk::PolyFace& poly=render_list.plist[i];
			if(poly.state)
				continue;
			material=poly.mat_ptr;
			assert(material);
			Vec4 color(0,0,0,0);
			float val=poly.normal_camera.dot(light_dir_cam);
			if(draw_texture&&poly.mat_ptr->texture.data)
			{
				Vec4 ambient(0.2f,0.2f,0.2f,1.0f);
				Vec4 diffuse(0.8f,0.8f,0.8f,1.0f);
				color=dot_multV4(ambient,light.c_ambient)+
					dot_multV4(max(val,0.0f)*light.c_diffuse,diffuse);
			}
			else
			{
				color=dot_multV4(material->ka*material->ra,light.c_ambient)+
					dot_multV4(max(val,0.0f)*light.c_diffuse,material->kd*material->rd);
			}
			color_clip(color);
			poly.color=color;
		}
	}
	else  //Gouraud shading
	{
		for(int i=0;i<render_list.num_polys;i++)
		{
			zyk::PolyFace& poly=render_list.plist[i];
			if(poly.state)
				continue;
			material=poly.mat_ptr;
			assert(material);
			for(int j=0;j<3;j++)
			{
				Vec4 color(0,0,0,0);
				float diff_val=poly.v_normal[j].dot(light_dir_cam);
				Vec3 r_vec=-light_dir_cam+2*light_dir_cam.dot(poly.v_normal[j])*poly.v_normal[j];
				Vec3 view_vec=Vec3(-poly.tlist[j](0),-poly.tlist[j](1),-poly.tlist[j](2));
				view_vec.normalize();
				float spec_val=pow(max(view_vec.dot(r_vec),0.0f),1.0f);

				if(draw_texture&&poly.mat_ptr->texture.data)
				{
					Vec4 ambient(0.2f,0.2f,0.2f,1.0f);
					Vec4 diffuse(0.6f,0.6f,0.6f,1.0f);
					Vec4 specu(0.2f,0.2f,0.2f,1.0f);
					color=dot_multV4(ambient,light.c_ambient)+
						dot_multV4(max(diff_val,0.0f)*light.c_diffuse,diffuse)
						+dot_multV4(spec_val*light.c_specular,specu);
				}
				else
				{
					color=dot_multV4(material->ka*material->ra,light.c_ambient)+
						dot_multV4(max(diff_val,0.0f)*light.c_diffuse,material->kd*material->rd)+
						dot_multV4(spec_val*light.c_specular,material->ks*material->rs);
				}
				color_clip(color);
				poly.v_color[j]=color;
			}
		}
	}
}

void RenderView::insert_obj_to_renderlist(zyk::ObjectMesh&obj,zyk::RenderList&render_list)
{
	if(obj.state)
		return;
	if(render_list.num_polys>=RENDERLIST_MAX_POLYS)
	{
		std::cout<<"refuse to insert new polygon!"<<std::endl;
		return;
	}
	int count=render_list.num_polys;
	for(int i=0;i<obj.num_polys;i++)
	{
		if(obj.plist[i].state)
			continue;
		zyk::Polygon& polygon=obj.plist[i];
		if(global_sys->m_cam.Out_of_Frustum(polygon.vlist[polygon.vert[0]].vtrans)&&
			global_sys->m_cam.Out_of_Frustum(polygon.vlist[polygon.vert[1]].vtrans)&&
			global_sys->m_cam.Out_of_Frustum(polygon.vlist[polygon.vert[2]].vtrans))
			continue;

		zyk::PolyFace& poly_face=render_list.plist[count];
		poly_face.color=obj.plist[i].color;
		for(int j=0;j<3;j++)
		{
			//the initial vertex is in camera coordinate
			int index=obj.plist[i].vert[j];
			poly_face.vlist[j]=obj.plist[i].vlist[index].vtrans;
			poly_face.v_normal[j]=zyk::V4toV3(obj.plist[i].vlist[index].trans_nor);
			poly_face.tlist[j]=poly_face.vlist[j];
			poly_face.texc[j]=obj.plist[i].vlist[index].texc;
		}
		poly_face.mat_ptr=obj.material;
		poly_face.normal_camera=zyk::transformNormal(zyk::V4toV3(obj.plist[i].trans_normal),global_sys->m_cam.mcam);
		render_list.poly_ptrs[count]=&poly_face;
		count++;
		if(count>=RENDERLIST_MAX_POLYS)
		{
			std::cout<<"refuse to insert new polygon!"<<std::endl;
			render_list.num_polys=count;
			return;
		}
	}
	render_list.num_polys=count;
	num_polys=count;
}

void RenderView::world_2_camera(zyk::ObjectMesh& obj)
{
	if(obj.state)
		return;
	const zyk::Camera& pCam=global_sys->m_cam;
	for(int i=0;i<obj.num_vertices;i++)
	{
		Vec4 &vert=obj.vert[i].vtrans;
		vert=pCam.mcam*vert;
		obj.vert[i].trans_nor=pCam.mcam*obj.vert[i].trans_nor;
	}
}

void RenderView::clip_in_z_coord(zyk::RenderList& render_list)
{
	float near_z=-global_sys->m_cam.near_clip_z;
	float far_z=-global_sys->m_cam.far_clip_z;
	int out_index[3];
	int in_index[3];
	int out_count=0,in_count=0;
	int num_poly=render_list.num_polys;
	for(int i=0;i<num_poly;i++)
	{
		zyk::PolyFace& poly_face=render_list.plist[i];
		in_count=out_count=0;
		for(int j=0;j<3;j++)
		{
			if(poly_face.tlist[j](2)<=near_z)
			{
				in_index[in_count]=j;
				in_count++;
			}
			else
			{
				out_index[out_count]=j;
				out_count++;
			}
		}
		assert(in_count+out_count==3);
		assert(out_count<3);

		if(out_count==0)
			continue;
		else if(out_count==1)
		{
			Vec4 edge_point[2],in_point[2];
			Vec3 edge_normal[2],in_normal[2];
			for(int j=0;j<2;j++)
			{
				Vec3 p0=zyk::V4toV3(poly_face.tlist[in_index[j]]);
				in_point[j]=zyk::V3toV4(p0,1);
				in_normal[j]=poly_face.v_normal[in_index[j]];
				Vec3 dir=zyk::V4toV3(poly_face.tlist[out_index[0]]-poly_face.tlist[in_index[j]]);
				float t=(near_z-p0(2))/dir(2);
				assert(t>=0.f&&t<=1.f);
				edge_point[j]=zyk::V3toV4(p0+t*dir,1);
				//interpolate the normal
				edge_normal[j]=(1-t)*poly_face.v_normal[in_index[j]]+t*poly_face.v_normal[out_index[0]];
			}

			poly_face.vlist[0]=poly_face.tlist[0]=in_point[0];
			poly_face.vlist[1]=poly_face.tlist[1]=edge_point[0];
			poly_face.vlist[2]=poly_face.tlist[2]=edge_point[1];

			poly_face.v_normal[0]=in_normal[0];
			poly_face.v_normal[1]=edge_normal[0];
			poly_face.v_normal[2]=edge_normal[1];

			if(render_list.num_polys==RENDERLIST_MAX_POLYS)
			{
				std::cout<<"The number of polygons is out of range"<<std::endl;
				return;
			}

			zyk::PolyFace& poly2=render_list.plist[render_list.num_polys];
			poly2=poly_face;
			poly2.vlist[0]=poly2.tlist[0]=in_point[0];
			poly2.vlist[1]=poly2.tlist[1]=in_point[1];
			poly2.vlist[2]=poly2.tlist[2]=edge_point[1];

			poly2.v_normal[0]=in_normal[0];
			poly2.v_normal[1]=in_normal[1];
			poly2.v_normal[2]=edge_normal[1];
			render_list.num_polys++;
		}
		else
		{
			Vec3 p0=zyk::V4toV3(poly_face.tlist[in_index[0]]);
			for(int j=0;j<2;j++)
			{
				Vec3 dir=zyk::V4toV3(poly_face.tlist[out_index[j]]-poly_face.tlist[in_index[0]]);
				float t=(near_z-poly_face.tlist[in_index[0]](2))/dir(2);
				assert(t>=0.f&&t<=1.f);
				Vec3 p=p0+t*dir;
				Vec3 normal=(1-t)*poly_face.v_normal[in_index[0]]+t*poly_face.v_normal[out_index[j]];
				poly_face.tlist[out_index[j]]=poly_face.vlist[out_index[j]]=zyk::V3toV4(p,1);
				poly_face.v_normal[out_index[j]]=normal;
			}
		}
	}
	for(int i=0;i<render_list.num_polys;i++)
		render_list.poly_ptrs[i]=&render_list.plist[i];
}

void RenderView::camera_2_perspective(zyk::RenderList& render_list)
{
	const zyk::Camera& pCam=global_sys->m_cam;
	for(int i=0;i<render_list.num_polys;i++)
	{
		zyk::PolyFace&poly_face=render_list.plist[i];
		for(int j=0;j<3;j++)
		{
			//now the 3rd coordinate value of the vector is -(a*z+b)/z, in [-1,1]. 
			Vec4& vert=poly_face.tlist[j];
			vert=pCam.mper*vert;
			float inv=1.f/vert(3);
			for(int k=0;k<4;k++)
				vert(k)*=inv;
			if(!(vert(2)<=1&&vert(2)>=-1))
				int z=0;
		}
	}
}

void RenderView::per_2_viewport(zyk::RenderList& render_list)
{
	const zyk::Camera& pCam=global_sys->m_cam;
	float alpha=0.5f*pCam.viewport_width;
	float beta=0.5f*pCam.viewport_height;

	//we use direct computation instead of matrix computation for saving more time
	for(int i=0;i<render_list.num_polys;i++)
	{
		zyk::PolyFace& poly_face=render_list.plist[i];
		for(int j=0;j<3;j++)
		{
			//poly_face.tlist[j]=global_sys->m_cam.mscr*poly_face.tlist[j];
			Vec4& vert=poly_face.tlist[j];
			vert(0)=(vert(0)+1)*alpha-0.5f;
			vert(1)=pCam.viewport_height-1-((vert(1)+1)*beta-0.5f);
		}
	}
}

#define RECORD_TIME
//#define DRAW_POLYGON
void RenderView::paintEvent(QPaintEvent* paint_event)
{
#ifdef RECORD_TIME
	DWORD start_time=GetTickCount();
#endif
	global_sys->m_cam.target=global_sys->m_cam.backup_target;
	global_sys->m_cam.pos=global_sys->m_cam.backup_pos;
	Vec3 normal_dir=(global_sys->m_cam.target-global_sys->m_cam.pos).normalized();
	global_sys->m_cam.pos+=normal_dir*m_arcball.mRadius;
	global_sys->m_cam.reset_camera_matrix();

	local_2_world(obj_mesh);
	object_cull(obj_mesh);
	backface_cull(obj_mesh);
	world_2_camera(obj_mesh);
	insert_obj_to_renderlist(obj_mesh,m_render_list);
	clip_in_z_coord(m_render_list);
	lighting_compute(m_render_list);
	camera_2_perspective(m_render_list);
	per_2_viewport(m_render_list);

#ifndef DRAW_POLYGON
	render2viewport(m_render_list);
#endif
	if(output_polygon)
	{
		m_render_list.outputPolygon(filename);
		output_polygon=false;
	}
	QPainter painter(this);
	 
#ifdef DRAW_POLYGON
	QPen pen;
	QBrush brush;
	pen.setColor(QColor(0,0,0));
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0,0,0,255));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0,0,width()-1,height()-1);

	//pen.setColor(QColor(255,255,255));
	//painter.setPen(pen);
	QPointF points[3];
	for(int i=0;i<m_render_list.num_polys;i++)
	{
		const zyk::PolyFace& poly_face=(*m_render_list.poly_ptrs[i]);
		for(int j=0;j<3;j++)
		{
			points[j].setX(poly_face.tlist[j][0]);
			points[j].setY(poly_face.tlist[j][1]);
		}
		QColor color(poly_face.color(0)*255,poly_face.color(1)*255,poly_face.color(2)*255);
		pen.setColor(color);
		brush.setColor(color);
		painter.setPen(pen);
		painter.setBrush(brush);
		painter.drawPolygon(points,3);
	}
#endif

#ifndef DRAW_POLYGON
	painter.drawPixmap(0,0,QPixmap::fromImage(viewport_image));
#endif
	QPen pen;
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	if(draw_line)
	{
		pen.setWidth(1.0f);
		pen.setColor(QColor(255,255,255));
		brush.setColor(QColor(255,255,255));
		painter.setPen(pen);
		for(int i=0;i<m_render_list.num_polys;i++)
			m_render_list.plist[i].draw_lines(painter);
	}

	if(draw_vertex)
	{
		pen.setColor(QColor(255,0,0));
		painter.setPen(pen);
		for(int i=0;i<m_render_list.num_polys;i++)
			m_render_list.plist[i].draw_vertices(painter);
	}

	obj_mesh.reset_object();
	m_render_list.reset();
	buffer_switch();
	memset(render_buffer,0,sizeof(zyk::UCHAR3)*global_sys->pixel_num);
	reset_Zbuffer();
#ifdef RECORD_TIME
	DWORD end_time=GetTickCount();
	QString time_str("ms per frame:"+QString::number(end_time-start_time,10));
	time_label->setText(time_str);
	//std::cout<<end_time-start_time<<std::endl;
#endif
	update();
}

//mouse event
void RenderView::mousePressEvent(QMouseEvent* mouse_event)
{
	QPoint qpoint=mapFromGlobal(QCursor::pos());
	int posx=qpoint.x();
	int posy=qpoint.y();
	if(mouse_event->button()==Qt::LeftButton)
	{
		m_arcball.MousePt.s.X = posx;
		m_arcball.MousePt.s.Y = posy;

		m_arcball.LastRot = m_arcball.ThisRot;                  
		m_arcball.ArcBall.click(&m_arcball.MousePt);
		m_arcball.button_status=1;
	}
	else if(mouse_event->button()==Qt::RightButton)
		m_arcball.button_status=2;
	m_arcball.mLastMousePos.s.X=static_cast<LONG>(qpoint.x());
	m_arcball.mLastMousePos.s.Y=static_cast<LONG>(qpoint.y());
	setMouseTracking(true);
	grabMouse();
}
void RenderView::mouseReleaseEvent(QMouseEvent* mouse_event)
{
	releaseMouse();
	setMouseTracking(false);
}
void RenderView::mouseMoveEvent(QMouseEvent* mouse_event)
{
	QPoint qpoint=mapFromGlobal(QCursor::pos());
	int posx=qpoint.x();
	int posy=qpoint.y();
	if(m_arcball.button_status==1)
	{
		m_arcball.MousePt.s.X = posx;
		m_arcball.MousePt.s.Y = posy;
		Quat4fT     ThisQuat;
		m_arcball.ArcBall.drag(&m_arcball.MousePt, &ThisQuat);                       
		Matrix3fSetRotationFromQuat4f(&m_arcball.ThisRot, &ThisQuat);  
		Matrix3fMulMatrix3f(&m_arcball.ThisRot, &m_arcball.LastRot);   
		Matrix4fSetRotationFromMatrix3f(&m_arcball.Transform, &m_arcball.ThisRot); 
		global_sys->m_cam.update_arcball_matrix(m_arcball.Transform.M);
	}
	else if(m_arcball.button_status==2)
	{
		float dx = 0.002f*static_cast<float>(posx - m_arcball.mLastMousePos.s.X);
		float dy = 0.002f*static_cast<float>(posy - m_arcball.mLastMousePos.s.Y);

		m_arcball.mRadius -= (dx - dy);
	}
	m_arcball.mLastMousePos.s.X = posx;
	m_arcball.mLastMousePos.s.Y = posy;
}

void RenderView::initArcball()
{
	m_arcball.init();
	global_sys->m_cam.update_arcball_matrix(m_arcball.Transform.M);
}

void RenderView::generateObj(int id)
{
	zyk::ObjectMesh &obj=obj_mesh;
	obj.num_polys=0;
	obj.num_vertices=0;
	m_render_list.num_polys=0;
	global_sys->reset_materials();
	switch(id)
	{
	case 0:
		{
			zyk::Load_OBJECT4DV1_COB(obj,"data\\sphere02.cob",Vec3(0.5,0.5,0.5),Vec3(0,0,0),Vec4(60.0f,0,1,0),1);
			break;
		}
	case 1:
		{
			zyk::Load_OBJECT4DV1_PLG(obj,"data\\tankg3.plg",Vec3(0.01,0.01,0.01),Vec3(0,0,0),Vec4(45.0f,1,1,0));
			break;
		}
	case 2:
		{
			zyk::Load_OBJECT4DV1_OBJ(obj,"data\\teapot.obj",Vec3(0.2,0.2,0.2),Vec3(0,0,0),Vec4(0.0f,-1,0,0),0);
			break;
		}
	case 3:
		{
			zyk::Load_OBJECT4DV1_OBJ(obj,"data\\cube.obj",Vec3(0.2,0.2,0.2),Vec3(0,0,0),Vec4(0.0f,-1,0,0),0);
			break;
		}
	case 4:
		{
			zyk::Load_OBJECT4DV1_OBJ(obj,"data\\violin.obj",Vec3(0.2,0.2,0.2),Vec3(0,0,0),Vec4(0.0f,-1,0,0),0);
			break;
		}
	case 5:
		{
			generateRectangle(obj);
			break;
		}
	}
	updateObj(obj);
	obj.material=&global_sys->materials[0];
}

void RenderView::generateRectangle(zyk::ObjectMesh&obj)
{
	obj.name="test mesh";
	obj.num_polys=2;
	obj.num_vertices=4;
	obj.vert[0].vlocal=Vec4(-1,1,-1,1);
	obj.vert[1].vlocal=Vec4(-1,-1,-1,1);
	obj.vert[2].vlocal=Vec4(1,-1,-1,1);
	obj.vert[3].vlocal=Vec4(1,1,-1,1);

	obj.vert[0].texc.u=0;
	obj.vert[0].texc.v=0;
	obj.vert[1].texc.u=0;
	obj.vert[1].texc.v=1;
	obj.vert[2].texc.u=1;
	obj.vert[2].texc.v=1;
	obj.vert[3].texc.u=1;
	obj.vert[3].texc.v=0;

	obj.world_pos=Vec3(0,0,0);
	for(int i=0;i<obj.num_polys;i++)
		obj.plist[i].vlist=obj.vert;

	obj.plist[0].vert[0]=0;
	obj.plist[0].vert[1]=2;
	obj.plist[0].vert[2]=1;

	obj.plist[1].vert[0]=0;
	obj.plist[1].vert[1]=3;
	obj.plist[1].vert[2]=2;
	zyk::load_texture(global_sys->materials[0],"data\\black.bmp");
}

void RenderView::updateObj(zyk::ObjectMesh&obj)
{
	obj.centralize();
	obj.computeRadius();
	obj.computeNormal();
}

inline void RenderView::buffer_switch()
{
	(render_buffer==main_buffer)?render_buffer=back_buffer:render_buffer=main_buffer;
}

inline bool checkZ(float z)
{
	if(!(z>=-1&&z<=1))
		return false;
	return true;
}

Vec4 getColorFromTexture(const Vec2& texcoord,const zyk::Image& texture)
{
	assert(texture.data);
	static float inv255=1.0f/255.0f;
	int x=texture.width*texcoord(0),y=texture.height*texcoord(1);
	if(x==texture.width)
		x=texture.width-1;
	if(y==texture.height)
		y=texture.height-1;

	int index=(y*texture.width+x)*texture.nbytes;
	return Vec4(texture.data[index]*inv255,
		texture.data[index+1]*inv255,
				texture.data[index+2]*inv255,1.0f);
}

void RenderView::Draw_Bottom_Tri_ZB(Vec3& v1,Vec3& v2,Vec3& v3,const Vec4 pColor[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,bool is_gouraud)
{
	float x_left,x_right,dxdy_left,dxdy_right,xs,xe,x_left_init,x_right_init,temp;
	float dzdy_left,dzdy_right,dzdx,z_start,z_end;
	int y_start,y_end,y_id;
	Vec4 temp_c;
	Vec4 start_c,end_c,dcdy_left,dcdy_right,dcdx;
	
	float x1,x2,x3,y1,y2,y3,z1,z2,z3;
	x1=v1(0);y1=v1(1);z1=v1(2);
	x2=v2(0);y2=v2(1);z2=v2(2);
	x3=v3(0);y3=v3(1);z3=v3(2);

	Vec4 color1=pColor[0],color2=pColor[1],color3=pColor[2];
	Vec4 f_color=pColor[0];//flat shading color
	if(x2>x3)
	{
		SWAP(x2,x3,temp);
		SWAP(y2,y3,temp);
		SWAP(z2,z3,temp);
		if(is_gouraud)
			SWAP(color2,color3,temp_c);
	}

	dxdy_left=(x1-x2)/(y2-y1);
	dxdy_right=(x3-x1)/(y2-y1);
	dzdy_left=(z2-z1)/(y2-y1);
	dzdy_right=(z3-z1)/(y3-y1);

	if(y1<min_clip_y)
	{
		y_start=min_clip_y;
		xs=x1-dxdy_left*(y_start-y1);
		xe=x1+dxdy_right*(y_start-y1);
	}
	else
	{
		y_start=ceil(y1);
		xs=x1-dxdy_left*(y_start-y1);
		xe=x1+dxdy_right*(y_start-y1);
	}

	float zl,zr;
	zl=z1+dzdy_left*(y_start-y1);
	zr=z1+dzdy_right*(y_start-y1);

	if(y3>max_clip_y)
		y_end=max_clip_y;		
	else
		y_end=ceil(y3)-1;

	Vec4 cl,cr;
	if(is_gouraud)
	{
		cl=color1;
		cr=color2;
		dcdy_left=(cr-cl)/(y2-y1);
		cr=color3;
		dcdy_right=(cr-cl)/(y2-y1);
		cl=color1+dcdy_left*(y_start-y1);
		cr=color1+dcdy_right*(y_start-y1);
	}

	dest_buffer=dest_buffer+(unsigned int)y_start*mempitch;
	zBuffer=zBuffer+(unsigned int)y_start*mempitch;
	for(y_id=y_start;y_id<=y_end;y_id++,dest_buffer+=mempitch,zBuffer+=mempitch)
	{
		x_left_init=x_left=xs;
		x_right_init=x_right=xe;		
		z_start=zl;
		z_end=zr;
		start_c=cl;
		end_c=cr;
		if(is_gouraud)
		{
			if(FCMP(x_right_init,x_left_init))
				dcdx=Vec4::Zero();
			else
				dcdx=(end_c-start_c)/(x_right_init-x_left_init);
			cl+=dcdy_left;
			cr+=dcdy_right;
		}

		if(FCMP(x_right_init,x_left_init))
			dzdx=0;
		else
			dzdx=(z_end-z_start)/(x_right_init-x_left_init);

		xs-=dxdy_left;
		xe+=dxdy_right;
		zl+=dzdy_left;
		zr+=dzdy_right;

		if(x_left<min_clip_x)
		{
			if(x_right<min_clip_x)
				continue;
			x_left=min_clip_x;
		}

		if(x_right>max_clip_x)
		{
			if(x_left>max_clip_x)
				continue;
			x_right=max_clip_x;
		}

		size_t index1,index2;
		index1=x_left;
		index2=x_right;
		for(size_t x=index1;x<=index2;x++)
		{
			float val=max((float)x-x_left_init,0);
			float z=z_start+dzdx*val;
			if(z<zBuffer[x])
				continue;
			zBuffer[x]=z;
			if(is_gouraud)
			{
				Vec4 color=(start_c+dcdx*val);
				zyk::clip_0_to_1(color);
				dest_buffer[x].x=color(0)*255.0f+0.5f;
				dest_buffer[x].y=color(1)*255.0f+0.5f;
				dest_buffer[x].z=color(2)*255.0f+0.5f;
			}
			else
			{
				zyk::clip_0_to_1(f_color);
				Vec4 color=f_color*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=color(0);
				dest_buffer[x].y=color(1);
				dest_buffer[x].z=color(2);
			}
		}
	}
}

void RenderView::Draw_Top_Tri_ZB(Vec3& v1,Vec3&v2,Vec3& v3,const Vec4 pColor[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,bool is_gouraud)
{
	float x_left,x_right,x_left_init,x_right_init,dxdy_left,dxdy_right,temp;
	int y_start,y_end,y_id;
	float dzdy_left,dzdy_right,dzdx,z_start,z_end;
	Vec4 temp_c;
	Vec4 start_c,end_c,dcdy_left,dcdy_right,dcdx;

	float x1,x2,x3,y1,y2,y3,z1,z2,z3;
	x1=v1(0);y1=v1(1);z1=v1(2);
	x2=v2(0);y2=v2(1);z2=v2(2);
	x3=v3(0);y3=v3(1);z3=v3(2);

	Vec4 color1=pColor[0],color2=pColor[1],color3=pColor[2];
	Vec4 f_color=pColor[0];
	if(x1>x2)
	{
		SWAP(x1,x2,temp);
		SWAP(y1,y2,temp);
		SWAP(z1,z2,temp);
		if(is_gouraud)
			SWAP(color1,color2,temp_c);
	}

	dxdy_left=(x3-x1)/(y3-y1);
	dxdy_right=(x2-x3)/(y3-y1);

	dzdy_left=(z3-z1)/(y3-y1);
	dzdy_right=(z3-z2)/(y3-y1);

	float xs,xe;
	if(y1<min_clip_y)
	{
		y_start=min_clip_y;
		xs=x1+dxdy_left*(y_start-y1);
		xe=x2-dxdy_right*(y_start-y1);
	}
	else
	{
		y_start=ceil(y1);
		xs=x1+dxdy_left*(y_start-y1);
		xe=x2-dxdy_right*(y_start-y1);
	}

	if(y3>max_clip_y)
		y_end=max_clip_y;		
	else
		y_end=ceil(y3)-1;

	float zl,zr;
	zl=z1+dzdy_left*(y_start-y1);
	zr=z2+dzdy_right*(y_start-y1);

	Vec4 cl,cr;
	if(is_gouraud)
	{
		cl=color1;
		cr=color3;
		dcdy_left=(cr-cl)/(y3-y1);
		cl=color2;
		dcdy_right=(cr-cl)/(y3-y2);
		cl=color1+dcdy_left*(y_start-y1);
		cr=color2+dcdy_right*(y_start-y1);
	}
	
	dest_buffer=dest_buffer+(unsigned int)y_start*mempitch;
	zBuffer=zBuffer+(unsigned int)y_start*mempitch;

	for(y_id=y_start;y_id<=y_end;y_id++,dest_buffer+=mempitch,zBuffer+=mempitch)
	{
		x_left_init=x_left=xs;
		x_right_init=x_right=xe;
		z_start=zl;
		z_end=zr;
		start_c=cl;
		end_c=cr;

		if(is_gouraud)
		{
			if(FCMP(x_right_init,x_left_init))
				dcdx=Vec4::Zero();
			else
				dcdx=(end_c-start_c)/(x_right_init-x_left_init);
			cl+=dcdy_left;
			cr+=dcdy_right;
		}
		
		if(FCMP(x_right_init,x_left_init))
			dzdx=0;
		else
			dzdx=(z_end-z_start)/(x_right_init-x_left_init);

		zl+=dzdy_left;
		zr+=dzdy_right;
		xs+=dxdy_left;
		xe-=dxdy_right;

		if(x_left<min_clip_x)
		{
			if(x_right<min_clip_x)
				continue;
			x_left=min_clip_x;
		}

		if(x_right>max_clip_x)
		{
			if(x_left>max_clip_x)
				continue;
			x_right=max_clip_x;
		}

		size_t index1,index2;
		index1=x_left;
		index2=x_right;
		for(size_t x=index1;x<=index2;x++)
		{
			float val=max((float)x-x_left_init,0);
			float z=z_start+dzdx*val;
			
			if(z<zBuffer[x])
				continue;
			zBuffer[x]=z;
			if(is_gouraud)
			{
				Vec4 color=(start_c+dcdx*val);
				zyk::clip_0_to_1(color);
				dest_buffer[x].x=color(0)*255.0f+0.5f;
				dest_buffer[x].y=color(1)*255.0f+0.5f;
				dest_buffer[x].z=color(2)*255.0f+0.5f;
			}
			else
			{
				zyk::clip_0_to_1(f_color);
				Vec4 color=f_color*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=color(0);
				dest_buffer[x].y=color(1);
				dest_buffer[x].z=color(2);
			}
		}
	}
}


void RenderView::Draw_Triangle_2D_ZB(const zyk::PolyFace& poly,zyk::UCHAR3 *dest_buffer,float* zBuffer, int mempitch)
{
	float temp;
	Vec4 temp_c;
	Vec4 color1=poly.v_color[0],color2=poly.v_color[1],color3=poly.v_color[2];
	float x1,x2,x3,y1,y2,y3,z1,z2,z3;
	float u1,v1,u2,v2,u3,v3;

	x1=poly.tlist[0](0);y1=poly.tlist[0](1);z1=1/(-poly.vlist[0](2));
	x2=poly.tlist[1](0);y2=poly.tlist[1](1);z2=1/(-poly.vlist[1](2));
	x3=poly.tlist[2](0);y3=poly.tlist[2](1);z3=1/(-poly.vlist[2](2));
	
	if(draw_texture)
	{
		u1=poly.texc[0].u;v1=poly.texc[0].v;
		u2=poly.texc[1].u;v2=poly.texc[1].v;
		u3=poly.texc[2].u;v3=poly.texc[2].v;
	}

	if((FCMP(y1,y2)&&FCMP(y2,y3))||(FCMP(x1,x2)&&FCMP(x2,x3)))//the triangle has been degradated into a line
		return;
	if(y1>y2)
	{
		SWAP(y1,y2,temp);
		SWAP(x1,x2,temp);
		SWAP(z1,z2,temp);
		if(!flat_shading)
			SWAP(color1,color2,temp_c);
		if(draw_texture)
		{
			SWAP(u1,u2,temp);
			SWAP(v1,v2,temp);
		}
	}
	if(y1>y3)
	{
		SWAP(y1,y3,temp);
		SWAP(x1,x3,temp);
		SWAP(z1,z3,temp);
		if(!flat_shading)
			SWAP(color1,color3,temp_c);
		if(draw_texture)
		{
			SWAP(u1,u3,temp);
			SWAP(v1,v3,temp);
		}
	}
	if(y2>y3)
	{
		SWAP(y2,y3,temp);
		SWAP(x2,x3,temp);
		SWAP(z2,z3,temp);
		if(!flat_shading)
			SWAP(color2,color3,temp_c);
		if(draw_texture)
		{
			SWAP(u2,u3,temp);
			SWAP(v2,v3,temp);
		}
	}

	if(y3<min_clip_y||y1>max_clip_y||(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)
		||(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
		return;

	float new_x;
	Vec4 adj_color[3];
	if(FCMP(y2,y3))   //float bottom triangle
	{
		if(flat_shading)
			adj_color[0]=poly.color;
		else
			{adj_color[0]=color1;adj_color[1]=color2;adj_color[2]=color3;}
		if(!draw_texture||!poly.mat_ptr->texture.data)
			Draw_Bottom_Tri_ZB(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(x3,y3,z3),adj_color,dest_buffer,zBuffer,mempitch,!flat_shading);
		else
			Draw_Bottom_Tri_ZB_Tex(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(x3,y3,z3),Vec2(u1,v1),Vec2(u2,v2),Vec2(u3,v3),
			adj_color,dest_buffer,zBuffer,mempitch,poly.mat_ptr->texture,!flat_shading);
	}
	else if(FCMP(y1,y2))  //flat top triangle
	{
		if(flat_shading)
			adj_color[0]=poly.color;
		else
			{adj_color[0]=color1;adj_color[1]=color2;adj_color[2]=color3;}
		if(!draw_texture||!poly.mat_ptr->texture.data)
			Draw_Top_Tri_ZB(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(x3,y3,z3),adj_color,dest_buffer,zBuffer,mempitch,!flat_shading);
		else
			Draw_Top_Tri_ZB_Tex(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(x3,y3,z3),Vec2(u1,v1),Vec2(u2,v2),Vec2(u3,v3),
			adj_color,dest_buffer,zBuffer,mempitch,poly.mat_ptr->texture,!flat_shading);
	}
	else
	{
		float tmp=(y2-y1)/(y3-y1);
		new_x=x1+tmp*(x3-x1);
		Vec4 new_color=color1+(color3-color1)*tmp;
		float new_z=z1+tmp*(z3-z1);
		float new_u=0,new_v=0;

		if(draw_texture)
		{
			float u1_z=u1*z1,u3_z=u3*z3,v1_z=v1*z1,v3_z=v3*z3;
			new_u=(u1_z+(u3_z-u1_z)*tmp)/new_z;
			new_v=(v1_z+(v3_z-v1_z)*tmp)/new_z;
		}

		if(flat_shading)
			adj_color[0]=poly.color;
		else
			{adj_color[0]=color1;adj_color[1]=color2;adj_color[2]=new_color;}
		if(!draw_texture||!poly.mat_ptr->texture.data)
			Draw_Bottom_Tri_ZB(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(new_x,y2,new_z),adj_color,dest_buffer,zBuffer,mempitch,!flat_shading);
		else
		{
			Draw_Bottom_Tri_ZB_Tex(Vec3(x1,y1,z1),Vec3(x2,y2,z2),Vec3(new_x,y2,new_z),Vec2(u1,v1),Vec2(u2,v2),Vec2(new_u,new_v),
			adj_color,dest_buffer,zBuffer,mempitch,poly.mat_ptr->texture,!flat_shading);
		}

		if(!flat_shading)
			{adj_color[0]=color2;adj_color[1]=new_color;adj_color[2]=color3;}
		if(!draw_texture||!poly.mat_ptr->texture.data)
			Draw_Top_Tri_ZB(Vec3(x2,y2,z2),Vec3(new_x,y2,new_z),Vec3(x3,y3,z3),adj_color,dest_buffer,zBuffer,mempitch,!flat_shading);
		else
		{
			Draw_Top_Tri_ZB_Tex(Vec3(x2,y2,z2),Vec3(new_x,y2,new_z),Vec3(x3,y3,z3),Vec2(u2,v2),Vec2(new_u,new_v),Vec2(u3,v3),
			adj_color,dest_buffer,zBuffer,mempitch,poly.mat_ptr->texture,!flat_shading);
		}
	}
}

void RenderView::Draw_Top_Tri_ZB_Tex(Vec3& v1,Vec3&v2,Vec3& v3,Vec2& tex1,Vec2& tex2,Vec2& tex3,const Vec4 pColor[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,zyk::Image&texture,bool is_gouraud)
{
	float x_left,x_right,x_left_init,x_right_init,dxdy_left,dxdy_right,temp;
	int y_start,y_end,y_id;
	float dzdy_left,dzdy_right,dzdx,z_start,z_end;
	Vec4 temp_c;
	Vec4 start_c,end_c,dcdy_left,dcdy_right,dcdx;
	Vec4 color1=pColor[0],color2=pColor[1],color3=pColor[2];
	Vec2 texuv[3],tempt,dtdy_left,dtdy_right,dtdx;

	float x1,x2,x3,y1,y2,y3,z1,z2,z3;
	x1=v1(0);y1=v1(1);z1=v1(2);
	x2=v2(0);y2=v2(1);z2=v2(2);
	x3=v3(0);y3=v3(1);z3=v3(2);
	texuv[0]=Vec2(tex1(0)*z1,tex1(1)*z1);
	texuv[1]=Vec2(tex2(0)*z2,tex2(1)*z2);
	texuv[2]=Vec2(tex3(0)*z3,tex3(1)*z3);

	if(x1>x2)
	{
		SWAP(x1,x2,temp);
		SWAP(y1,y2,temp);
		SWAP(z1,z2,temp);
		SWAP(texuv[0],texuv[1],tempt);
		if(is_gouraud)
			SWAP(color1,color2,temp_c);
	}

	dxdy_left=(x3-x1)/(y3-y1);
	dxdy_right=(x2-x3)/(y3-y1);
	dzdy_left=(z3-z1)/(y3-y1);
	dzdy_right=(z3-z2)/(y3-y1);
	dtdy_left=(texuv[2]-texuv[0])/(y3-y1);
	dtdy_right=(texuv[2]-texuv[1])/(y3-y1);

	float xs,xe;
	if(y1<min_clip_y)
	{
		y_start=min_clip_y;
		xs=x1+dxdy_left*(y_start-y1);
		xe=x2-dxdy_right*(y_start-y1);
	}
	else
	{
		y_start=ceil(y1);
		xs=x1+dxdy_left*(y_start-y1);
		xe=x2-dxdy_right*(y_start-y1);
	}

	if(y3>max_clip_y)
		y_end=max_clip_y;		
	else
		y_end=ceil(y3)-1;

	float zl,zr;
	zl=z1+dzdy_left*(y_start-y1);
	zr=z2+dzdy_right*(y_start-y1);

	Vec2 tl,tr;
	tl=texuv[0]+dtdy_left*(y_start-y1);
	tr=texuv[1]+dtdy_right*(y_start-y1);

	Vec4 cl,cr;
	if(is_gouraud)
	{
		cl=color1;
		cr=color3;
		dcdy_left=(cr-cl)/(y3-y1);
		cl=color2;
		dcdy_right=(cr-cl)/(y3-y2);
		cl=color1+dcdy_left*(y_start-y1);
		cr=color2+dcdy_right*(y_start-y1);
	}

	Vec2 t_start,t_end;
	dest_buffer=dest_buffer+(unsigned int)y_start*mempitch;
	zBuffer=zBuffer+(unsigned int)y_start*mempitch;

	for(y_id=y_start;y_id<=y_end;y_id++,dest_buffer+=mempitch,zBuffer+=mempitch)
	{
		x_left_init=x_left=xs;
		x_right_init=x_right=xe;
		z_start=zl;
		z_end=zr;
		start_c=cl;
		end_c=cr;
		t_start=tl;
		t_end=tr;

		if(is_gouraud)
		{
			if(FCMP(x_right_init,x_left_init))
				dcdx=Vec4::Zero();
			else
				dcdx=(end_c-start_c)/(x_right_init-x_left_init);
			cl+=dcdy_left;
			cr+=dcdy_right;
		}

		if(FCMP(x_right_init,x_left_init))
		{
			dzdx=0;
			dtdx.setZero();
		}
		else
		{
			dzdx=(z_end-z_start)/(x_right_init-x_left_init);
			dtdx=(t_end-t_start)/(x_right_init-x_left_init);
		}
		zl+=dzdy_left;
		zr+=dzdy_right;
		xs+=dxdy_left;
		xe-=dxdy_right;
		tl+=dtdy_left;
		tr+=dtdy_right;

		if(x_left<min_clip_x)
		{
			if(x_right<min_clip_x)
				continue;
			x_left=min_clip_x;
		}

		if(x_right>max_clip_x)
		{
			if(x_left>max_clip_x)
				continue;
			x_right=max_clip_x;
		}

		size_t index1,index2;
		index1=x_left;
		index2=x_right;
		for(size_t x=index1;x<=index2;x++)
		{
			float val=max((float)x-x_left_init,0);
			float z=z_start+dzdx*val;

			if(z<zBuffer[x])
				continue;
			zBuffer[x]=z;
			Vec2 tex=(t_start+dtdx*val)/z;
			Vec4 tex_color=getColorFromTexture(tex,texture);
			if(is_gouraud)
			{
				Vec4 color=(start_c+dcdx*val);
				zyk::clip_0_to_1(color);
				Vec4 fcolor=zyk::dot_multV4(color,tex_color)*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				//Vec4 color=color1*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=fcolor(0);
				dest_buffer[x].y=fcolor(1);
				dest_buffer[x].z=fcolor(2);
			}
			else
			{
				zyk::clip_0_to_1(color1);
				Vec4 color=zyk::dot_multV4(color1,tex_color)*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				//Vec4 color=color1*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=color(0);
				dest_buffer[x].y=color(1);
				dest_buffer[x].z=color(2);
			}
		}
	}
}

void RenderView::Draw_Bottom_Tri_ZB_Tex(Vec3& ve1,Vec3&ve2,Vec3& ve3,Vec2& tex1,Vec2& tex2,Vec2& tex3,const Vec4 pColor[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,zyk::Image&texture,bool is_gouraud)
{
	float x_left,x_right,dxdy_left,dxdy_right,xs,xe,x_left_init,x_right_init,temp;
	float dzdy_left,dzdy_right,dzdx,z_start,z_end;
	int y_start,y_end,y_id;
	Vec4 temp_c;
	Vec4 start_c,end_c,dcdy_left,dcdy_right,dcdx;
	Vec4 color1=pColor[0],color2=pColor[1],color3=pColor[2];

	float x1,x2,x3,y1,y2,y3,z1,z2,z3;
	Vec2 texuv[3],tempt,dtdy_left,dtdy_right,dtdx;
	x1=ve1(0);y1=ve1(1);z1=ve1(2);
	x2=ve2(0);y2=ve2(1);z2=ve2(2);
	x3=ve3(0);y3=ve3(1);z3=ve3(2);
	texuv[0]=Vec2(tex1(0)*z1,tex1(1)*z1);
	texuv[1]=Vec2(tex2(0)*z2,tex2(1)*z2);
	texuv[2]=Vec2(tex3(0)*z3,tex3(1)*z3);

	if(x2>x3)
	{
		SWAP(x2,x3,temp);
		SWAP(y2,y3,temp);
		SWAP(z2,z3,temp);
		SWAP(texuv[1],texuv[2],tempt);
		if(is_gouraud)
			SWAP(color2,color3,temp_c);
	}

	dxdy_left=(x1-x2)/(y2-y1);
	dxdy_right=(x3-x1)/(y2-y1);
	dzdy_left=(z2-z1)/(y2-y1);
	dzdy_right=(z3-z1)/(y3-y1);
	dtdy_left=(texuv[1]-texuv[0])/(y2-y1);
	dtdy_right=(texuv[2]-texuv[0])/(y3-y1);

	if(y1<min_clip_y)
	{
		y_start=min_clip_y;
		xs=x1-dxdy_left*(y_start-y1);
		xe=x1+dxdy_right*(y_start-y1);
	}
	else
	{
		y_start=ceil(y1);
		xs=x1-dxdy_left*(y_start-y1);
		xe=x1+dxdy_right*(y_start-y1);
	}

	float zl,zr;
	zl=z1+dzdy_left*(y_start-y1);
	zr=z1+dzdy_right*(y_start-y1);

	Vec2 tl,tr;
	tl=texuv[0]+dtdy_left*(y_start-y1);
	tr=texuv[0]+dtdy_right*(y_start-y1);

	if(y3>max_clip_y)
		y_end=max_clip_y;		
	else
		y_end=ceil(y3)-1;

	Vec4 cl,cr;
	if(is_gouraud)
	{
		cl=color1;
		cr=color2;
		dcdy_left=(cr-cl)/(y2-y1);
		cr=color3;
		dcdy_right=(cr-cl)/(y2-y1);
		cl=color1+dcdy_left*(y_start-y1);
		cr=color1+dcdy_right*(y_start-y1);
	}

	Vec2 t_start,t_end;
	dest_buffer=dest_buffer+(unsigned int)y_start*mempitch;
	zBuffer=zBuffer+(unsigned int)y_start*mempitch;
	for(y_id=y_start;y_id<=y_end;y_id++,dest_buffer+=mempitch,zBuffer+=mempitch)
	{
		x_left_init=x_left=xs;
		x_right_init=x_right=xe;		
		z_start=zl;
		z_end=zr;
		t_start=tl;
		t_end=tr;
		start_c=cl;
		end_c=cr;
		if(is_gouraud)
		{
			if(FCMP(x_right_init,x_left_init))
				dcdx=Vec4::Zero();
			else
				dcdx=(end_c-start_c)/(x_right_init-x_left_init);
			cl+=dcdy_left;
			cr+=dcdy_right;
		}

		if(FCMP(x_right_init,x_left_init))
		{
			dzdx=0;
			dtdx.setZero();
		}
		else
		{
			dzdx=(z_end-z_start)/(x_right_init-x_left_init);
			dtdx=(t_end-t_start)/(x_right_init-x_left_init);
		}

		xs-=dxdy_left;
		xe+=dxdy_right;
		zl+=dzdy_left;
		zr+=dzdy_right;
		tl+=dtdy_left;
		tr+=dtdy_right;

		if(x_left<min_clip_x)
		{
			if(x_right<min_clip_x)
				continue;
			x_left=min_clip_x;
		}

		if(x_right>max_clip_x)
		{
			if(x_left>max_clip_x)
				continue;
			x_right=max_clip_x;
		}

		size_t index1,index2;
		index1=x_left;
		index2=x_right;
		for(size_t x=index1;x<=index2;x++)
		{
			float val=max((float)x-x_left_init,0);
			float z=z_start+dzdx*val;
			if(z<zBuffer[x])
				continue;
			zBuffer[x]=z;

			Vec2 tex=(t_start+dtdx*val)/z;
			Vec4 tex_color=getColorFromTexture(tex,texture);
			//std::cout<<tex_color*255.0f<<std::endl;
			if(is_gouraud)
			{
				Vec4 color=(start_c+dcdx*val);
				zyk::clip_0_to_1(color);
				Vec4 fcolor=zyk::dot_multV4(color,tex_color)*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				//Vec4 color=color1*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=fcolor(0);
				dest_buffer[x].y=fcolor(1);
				dest_buffer[x].z=fcolor(2);
			}
			else
			{
				zyk::clip_0_to_1(color1);
				Vec4 color=zyk::dot_multV4(color1,tex_color)*255.0f+Vec4(0.5f,0.5f,0.5f,0.0f);
				dest_buffer[x].x=color(0);
				dest_buffer[x].y=color(1);
				dest_buffer[x].z=color(2);
			}
		}
	}
}

inline void RenderView::reset_Zbuffer()
{
	for(int i=0;i<global_sys->pixel_num;i++)
		z_buffer[i]=0.f;
}


