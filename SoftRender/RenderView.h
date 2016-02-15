#ifndef SOFTRENDER_RENDERVIEW_H
#define SOFTRENDER_RENDERVIEW_H
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include "GlobalSys.h"
#include "MyArcball.h"
class RenderView:public QWidget
{
	Q_OBJECT
public:
	RenderView(QWidget *parent = 0);
	~RenderView();
	void outputArcball(const std::string& filename);
	void inputArcball(const std::string& filename);
	int plus_id();
	int minus_id();
	inline void set_backfade_cull(bool is){backface_check=is;}
	inline void set_output(bool is,std::string fname){output_polygon=is;filename=fname;}
	inline void setFlatShading(){flat_shading=true;}
	inline void setGouraudShading(){flat_shading=false;}
	inline void setTimeLabel(QLabel*label){time_label=label;}
	inline void setDrawWireFrame(bool is){draw_line=is;}
	inline void setDrawVertex(bool is){draw_vertex=is;}
	inline void setTextureDraw(bool is){draw_texture=is;}
	void initArcball();
	void generateObj(int id=0);
protected:
	void paintEvent(QPaintEvent *);
	//mouse event
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);

	void generateRectangle(zyk::ObjectMesh&obj);
	void updateObj(zyk::ObjectMesh&obj);
	void render2viewport(zyk::RenderList&render_list);
	void local_2_world(zyk::ObjectMesh& obj);
	void object_cull(zyk::ObjectMesh& obj);
	void backface_cull(zyk::ObjectMesh&obj);
	void clip_in_z_coord(zyk::RenderList& render_list);
	void world_2_camera(zyk::ObjectMesh&obj);
	void camera_2_perspective(zyk::RenderList& render_list);
	void per_2_viewport(zyk::RenderList& render_list);
	void lighting_compute(zyk::RenderList&render_list);
	void insert_obj_to_renderlist(zyk::ObjectMesh&obj,zyk::RenderList&render_list);
	void painter_sort(zyk::RenderList& render_list); //we have updated to the z-buffer algorithm, so this function doesn't be used now

	/* this is new function for drawing triangles, the functino includs flat shading,goruaud shading and z buffer */
	void Draw_Triangle_2D_ZB(const zyk::PolyFace& poly,zyk::UCHAR3 *dest_buffer,float* zBuffer,int mempitch);
	void Draw_Bottom_Tri_ZB(Vec3& v1,Vec3&v2,Vec3& v3,const Vec4 color[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,bool is_gouraud);
	void Draw_Top_Tri_ZB(Vec3& v1,Vec3&v2,Vec3& v3,const Vec4 color[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,bool is_gouraud);

	void Draw_Bottom_Tri_ZB_Tex(Vec3& v1,Vec3&v2,Vec3& v3,Vec2& tex1,Vec2& tex2,Vec2& tex3,const Vec4 color[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,zyk::Image& texture,bool is_gouraud);
	void Draw_Top_Tri_ZB_Tex(Vec3& v1,Vec3&v2,Vec3& v3,Vec2& tex1,Vec2& tex2,Vec2& tex3,const Vec4 color[],zyk::UCHAR3*dest_buffer,float* zBuffer,int mempitch,zyk::Image&texture,bool is_gouraud);

	void reset_Zbuffer();
	void buffer_switch();
	zyk::UCHAR3* main_buffer,*back_buffer,*render_buffer;
	float* z_buffer;
	
	QImage viewport_image;
	zyk::ObjectMesh obj_mesh;
	zyk::RenderList m_render_list;
	bool backface_check,draw_line,draw_vertex,draw_texture;
	bool output_polygon;
	bool flat_shading; //if flat_shaidng is false, run Gouraud shading
	std::string filename;
	float min_clip_x,min_clip_y,max_clip_x,max_clip_y;
	MyArcball m_arcball;
	int cur_id,num_polys;
	QLabel* time_label;
	
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
#endif