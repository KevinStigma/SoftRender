#ifndef SOFTRENDER_H
#define SOFTRENDER_H

#include <QtWidgets/QMainWindow>
#include <QKeyEvent>
#include <QFileDialog>
#include "ui_softrender.h"
#include "GlobalSys.h"

class SoftRender : public QMainWindow
{
	Q_OBJECT

public:
	SoftRender(QWidget *parent = 0);
	~SoftRender();

public slots:
	void setCameraAttrib();		
	void setBackface();
	void plusButton();
	void minusButton();
	void FlatshadingCheck();
	void GouraudshadingCheck();
	void WireFrameCheck();
	void VertexCheck();
	void TextureCheck();
	void loadCube();
	void loadViolin();
	void loadSphere();
	void loadRec();
	void loadTank();
	void loadTeapot();
protected:
	void keyPressEvent(QKeyEvent *e);
	void outputArcballParameter();
	void inputArcballPara();
	void initView();
private:
	Ui::SoftRenderClass ui;
};

#endif // SOFTRENDER_H
