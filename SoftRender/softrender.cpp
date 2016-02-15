#include "softrender.h"


extern GlobalSys* global_sys;
SoftRender::SoftRender(QWidget *parent)
	: QMainWindow(parent)
{
	global_sys=new GlobalSys();
	ui.setupUi(this);
	connect(ui.DefineButton,SIGNAL(clicked()),this,SLOT(setCameraAttrib()));
	connect(ui.BackFaceCheck,SIGNAL(clicked()),this,SLOT(setBackface()));
	connect(ui.plusButton,SIGNAL(clicked()),this,SLOT(plusButton()));
	connect(ui.minusButton,SIGNAL(clicked()),this,SLOT(minusButton()));
	connect(ui.FlatshadingCheck,SIGNAL(clicked()),this,SLOT(FlatshadingCheck()));
	connect(ui.GroundshadingCheck,SIGNAL(clicked()),this,SLOT(GouraudshadingCheck()));
	connect(ui.WireFrameCheck,SIGNAL(clicked()),this,SLOT(WireFrameCheck()));
	connect(ui.VertexCheck,SIGNAL(clicked()),this,SLOT(VertexCheck()));
	connect(ui.TextureCheck,SIGNAL(clicked()),this,SLOT(TextureCheck()));
	connect(ui.MeshCube,SIGNAL(clicked()),this,SLOT(loadCube()));
	connect(ui.MeshRec,SIGNAL(clicked()),this,SLOT(loadRec()));
	connect(ui.MeshSphere,SIGNAL(clicked()),this,SLOT(loadSphere()));
	connect(ui.MeshTank,SIGNAL(clicked()),this,SLOT(loadTank()));
	connect(ui.MeshViolin,SIGNAL(clicked()),this,SLOT(loadViolin()));
	connect(ui.MeshTeapot,SIGNAL(clicked()),this,SLOT(loadTeapot()));
	ui.render_widget->setTimeLabel(ui.TimeRecord);
	ui.plusButton->setVisible(false);
	ui.minusButton->setVisible(false);
	ui.label_14->setVisible(false);
}

SoftRender::~SoftRender()
{
	SAFE_DELETE(global_sys);
}


void SoftRender::setCameraAttrib()
{
	zyk::Camera& pCam=global_sys->m_cam;
	QString num[3];
	num[0]=ui.posEditx->text();
	num[1]=ui.posEdity->text();
	num[2]=ui.posEditz->text();
	for(int i=0;i<3;i++)
		pCam.pos[i]=num[i].toFloat();
	pCam.backup_pos=pCam.pos;

	num[0]=ui.tarEditx->text();
	num[1]=ui.tarEdity->text();
	num[2]=ui.tarEditz->text();
	for(int i=0;i<3;i++)
		pCam.target[i]=num[i].toFloat();
	pCam.backup_target=pCam.target;

	pCam.fov=ui.fovEdit->text().toFloat();
	pCam.near_clip_z=ui.nearEdit->text().toFloat();
	pCam.far_clip_z=ui.farEdit->text().toFloat();

	pCam.n=-(pCam.target-pCam.pos).normalized();
	pCam.v=Vec3(0,1,0);
	pCam.u=pCam.v.cross(pCam.n);
	pCam.v=pCam.n.cross(pCam.u);

	pCam.view_height=pCam.near_clip_z*tan(pCam.fov*DEG_TO_RAD*0.5f)*2.0f;
	pCam.view_width=pCam.view_height*pCam.aspect_ratio;

	pCam.reset_camera_matrix();
	pCam.reset_perspective_proj_matrix();
	pCam.reset_viewport_matrix();
	//TODO:calcualte the six faces of the view frustum

	std::cout<<"New camera attrib set!"<<std::endl;
}

void SoftRender::setBackface()
{
	bool is=ui.BackFaceCheck->isChecked();
	ui.render_widget->set_backfade_cull(is);
}

void SoftRender::plusButton()
{
	int id=ui.render_widget->plus_id();
	ui.label_14->setText(QString::number(id));
}
void SoftRender::minusButton()
{
	int id=ui.render_widget->minus_id();
	ui.label_14->setText(QString::number(id));
}

void SoftRender::FlatshadingCheck()
{
	bool is=ui.FlatshadingCheck->isChecked();
	ui.render_widget->setFlatShading();
}
void SoftRender::GouraudshadingCheck()
{
	bool is=ui.GroundshadingCheck->isChecked();
	ui.render_widget->setGouraudShading();
}

void SoftRender::WireFrameCheck()
{
	bool is=ui.WireFrameCheck->isChecked();
	ui.render_widget->setDrawWireFrame(is);
}

void SoftRender::VertexCheck()
{
	bool is=ui.VertexCheck->isChecked();
	ui.render_widget->setDrawVertex(is);
}

void SoftRender::TextureCheck()
{
	bool is=ui.TextureCheck->isChecked();
	ui.render_widget->setTextureDraw(is);
}

void SoftRender::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
	case Qt::Key_A:
		{
			std::cout<<"Input filename:"<<std::endl;
			std::string filename;
			std::cin>>filename;
			ui.render_widget->set_output(true,filename);
			break;
		}
	case Qt::Key_O:
		{
			outputArcballParameter();
			break;
		}
	case Qt::Key_I:
		{
			inputArcballPara();
			break;
		}
	case Qt::Key_R:
		{
			initView();
			break;
		}
	}
}

void SoftRender::outputArcballParameter()
{
	std::string filename;
	std::cout<<"File name:";
	std::cin>>filename;
	ui.render_widget->outputArcball(filename);
}

void SoftRender::loadCube()
{
	ui.render_widget->generateObj(3);
}
void SoftRender::loadViolin()
{
	ui.render_widget->generateObj(4);
}
void SoftRender::loadSphere()
{
	ui.render_widget->generateObj(0);
}
void SoftRender::loadRec()
{
	ui.render_widget->generateObj(5);
}
void SoftRender::loadTank()
{
	ui.render_widget->generateObj(1);
}

void SoftRender::loadTeapot()
{
	ui.render_widget->generateObj(2);
}

void SoftRender::inputArcballPara()
{
	ui.render_widget->inputArcball("arcball");
}

void SoftRender::initView()
{
	ui.posEditx->setText("0");
	ui.posEdity->setText("0");
	ui.posEditz->setText("3");
	ui.tarEditx->setText("0");
	ui.tarEdity->setText("0");
	ui.tarEditz->setText("0");
	ui.fovEdit->setText("45");
	ui.nearEdit->setText("0.1");
	ui.farEdit->setText("1000");
	setCameraAttrib();
	ui.render_widget->initArcball();
}
