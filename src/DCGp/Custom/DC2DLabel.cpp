//Qt
#include <QSharedPointer>

//System
#include <string.h>
#include <assert.h>

#include "DCCore/DCGL.h"

//DcGp
#include "DCGp/GpBasicDevice.h"
#include "DCGp/GpDrawContext.h"

//自定义
#include "DC2DLabel.h"

using DcGp::DC2DLabel;


DC2DLabel::DC2DLabel(QString name/*=QString()*/)
	: DcGp::DcGpEntity(name.isEmpty() ? "label" : name)
	, m_showFullBody(true)
	, m_dispIn3D(true)
	, m_dispIn2D(true)
{
	m_screenPos[0] = m_screenPos[1] = 0.05f;

	clear(false);

	LockVisibility(false);
	SetEnabled(true);
}

QString DC2DLabel::GetName()
{
	QString processedName = DcGpObject::GetName();

	size_t count = m_points.size();
	if (count>0)
	{
		processedName.replace(QString("pt_0_idx"),QString::number(m_points[0].index));
		if (count>1)
		{
			processedName.replace(QString("pt_1_idx"),QString::number(m_points[1].index));
			if (m_points[0].cloud)
				processedName.replace(QString("pt_0_cloud_id"),QString::number(m_points[0].cloud->GetUniqueID()));
			if (m_points[1].cloud)
				processedName.replace(QString("pt_1_cloud_id"),QString::number(m_points[1].cloud->GetUniqueID()));
			if (count>2)
			{
				processedName.replace(QString("pt_2_idx"),QString::number(m_points[2].index));
				if (m_points[2].cloud)
					processedName.replace(QString("pt_2_cloud_id"),QString::number(m_points[2].cloud->GetUniqueID()));
			}
			if (count>3)
			{
				processedName.replace(QString("pt_3_idx"),QString::number(m_points[3].index));
				if (m_points[3].cloud)
					processedName.replace(QString("pt_3_cloud_id"),QString::number(m_points[3].cloud->GetUniqueID()));
			}
		}
	}

	SetName(processedName);
	return processedName;
}
//*********************************************************************************************
void RadToDegree(const double& rad, int& degree, int& minute, int& secend )
{
	double dfm  = rad * 180.0/3.14159265358979;
	degree = floor(dfm);
	minute = floor((dfm - degree)* 60.0) ;
	secend = ((dfm -degree)-minute/60.0)*3600;

}


//计算倾角
double CalculateAngleDIP(const DCVector3D& vEigen)
{
	double  dip      = 0;
	double  sumSqrt  = sqrt(vEigen.x * vEigen.x + vEigen.y * vEigen.y + vEigen.z * vEigen.z);
	dip       = abs(vEigen.z) / sumSqrt;
	dip = acos(dip);
	return dip;
}

//计算倾向
double CalculateAngleTendency(const DCVector3D& vEigen)
{
	double dd=0;
	double cosKapa =  vEigen.y / (sqrt(vEigen.x * vEigen.x + vEigen.y * vEigen.y ) + 1e-10);
	double fm = sqrt(vEigen.x * vEigen.x + vEigen.y * vEigen.y + vEigen.z * vEigen.z);
	double cosalph = vEigen.x  / fm;
	double cosgama = vEigen.z  / fm;
	double tempDD= std::acos(cosKapa);
	if (cosalph >=0 && cosgama >0)
	{
		dd = tempDD;
	}
	else if (cosalph > 0 && cosgama <= 0)
	{
		dd = tempDD + M_PI;
	}
	else if (cosalph <0 && cosgama >= 0)
	{
		dd = 2*M_PI - tempDD;
	}
	else if (cosalph <= 0 && cosgama <0)
	{
		dd = M_PI - tempDD;
	}
	return dd;
}

//计算三角形的法向量 
DCVector3D CalculateNormals(DCVector3D p1, DCVector3D p2, DCVector3D p3)
{
	DCVector3D P1P2 = p2-p1;
	DCVector3D P1P3 = p3-p1;
	DCVector3D N = P1P2.Cross(P1P3); //N=ABxAC
	N.Normalize();
	return N;
}

//计算倾向倾角+1
void CalculateDipAndDD(const DCVector3D& p1, const DCVector3D& p2, const DCVector3D& p3, double& dip, double& dd)
{

	DCVector3D vEigen = CalculateNormals(p1,p2,p3);
	dip = CalculateAngleDIP(vEigen);
	dd = CalculateAngleTendency(vEigen);
}
//*********************************************************************************************


void DC2DLabel::setPosition(float x, float y)
{
	m_screenPos[0] = x;
	m_screenPos[1] = y;
}

bool DC2DLabel::Move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight)
{
	assert(screenHeight > 0 && screenWidth > 0);
	
	m_screenPos[0] += static_cast<float>(dx)/static_cast<float>(screenWidth);
	m_screenPos[1] += static_cast<float>(dy)/static_cast<float>(screenHeight);

	return true;
}

void DC2DLabel::clear(bool ignoreDependencies)
{
	if (ignoreDependencies)
	{
		m_points.clear();
	}
	else
	{
		//remove all dependencies first!
		while (!m_points.empty())
		{
			m_points.back().cloud->RemoveChild(this);
			m_points.pop_back();
		}
	}

	m_lastScreenPos[0] = m_lastScreenPos[1] = -1;
	memset(m_labelROI,0,sizeof(int)*4);
	SetVisible(false);
	SetName("Label");
}

void DC2DLabel::updateName()
{
	switch(m_points.size())
	{
	case 0:
		SetName("Label");
		break;
	case 1:
		SetName("Point pt_0_idx");
		break;
	case 2:
		if (m_points[0].cloud == m_points[1].cloud)
			SetName("Vector pt_0_idx - pt_1_idx");
		else
			SetName("Vector pt_0_idx(@pt_0_cloud_id) - pt_1_idx@(@pt_1_cloud_id)");
		break;
	case 3:
		if (m_points[0].cloud == m_points[2].cloud && m_points[1].cloud == m_points[2].cloud)
			SetName(QString(tr("Triplet")).append("pt_0_idx - pt_1_idx - pt_2_idx"));
		else
			SetName(QString(tr("Triplet")).append("pt_0_idx(@pt_0_cloud_id) - pt_1_idx(@pt_1_cloud_id) - pt_2_idx(@pt_2_cloud_id)"));
		break;
	case 4:
		if (m_points[0].cloud == m_points[2].cloud && m_points[1].cloud == m_points[2].cloud)
			SetName(QString(tr("QUADS")).append("pt_0_idx - pt_1_idx - pt_2_idx - pt_3_idx"));
		else
			SetName(QString(tr("QUADS")).append("pt_0_idx(@pt_0_cloud_id) - pt_1_idx(@pt_1_cloud_id) - pt_2_idx(@pt_2_cloud_id) - pt_3_idx(@pt_3_cloud_id)"));
		break;
	}
}

bool DC2DLabel::addPoint(DcGp::DcGpPointCloud* cloud, unsigned pointIndex)
{
	if (m_points.size() == 4)
		return false;

	try
	{
		m_points.resize(m_points.size()+1);
	}
	catch(std::bad_alloc)
	{
		//not enough memory
		return false;
	}

	m_points.back().cloud = cloud;
	m_points.back().index = pointIndex;

	updateName();

	//we want to be notified whenever an associated cloud is deleted (in which case
	//we'll automatically clear the label)
	//cloud->RemoveChild(this,DP_NOTIFY_OTHER_ON_DELETE);
	//we must also warn the cloud whenever we delete this label
	//addDependency(cloud,DP_NOTIFY_OTHER_ON_DELETE); //DGM: automatically done by the previous call to addDependency!

	return true;
}

//return angle between two vectors (in degrees)
//warning: vectors will be normalized by default
double GetAngle_deg(DCVector3D& AB, DCVector3D& AC)
{
	AB.Normalize();
	AC.Normalize();
	double dotprod = AB.Dot(AC);
	if (dotprod<=-1.0)
		return 180.0;
	else if (dotprod>1.0)
		return 0.0;
	return 180.0*acos(dotprod)/M_PI;
}

void AddPointCoordinates(QStringList& body, unsigned pointIndex, DcGp::DcGpPointCloud* cloud, int precision)
{
	assert(cloud);
	Point_3D P = Point_3D::FromArray(cloud->GetPoint(pointIndex).u);
	const Point_3D& shift = cloud->GetShift();
	bool isShifted = (shift.Normal2() != 0);

	QString coordStr = QString("P#%0:").arg(pointIndex);
// 	if (isShifted)
// 	{
// 		body << coordStr;
// 		coordStr = QString("  [shifted]");
// 	}
	
	coordStr += QString(" (%1;%2;%3)").arg(P.x - shift.x,0,'f',precision).arg(P.y - shift.y,0,'f',precision).arg(P.z - shift.z,0,'f',precision);
	body << coordStr;
	
// 	if (isShifted)
// 	{
// 		DCVector3D Pg = *P + shift;
// 		QString globCoordStr = QString("  [original] (%1;%2;%3)").arg(Pg.x,0,'f',precision).arg(Pg.y,0,'f',precision).arg(Pg.z,0,'f',precision);
// 		body << globCoordStr;
// 	}
}

QStringList DC2DLabel::getLabelContent(int precision)
{
	QStringList body;

	switch(m_points.size())
	{
	case 0:
		//can happen if the associated cloud(s) has(ve) been deleted!
		body << "Deprecated";
		break;

	case 1: //point
		{
			//init title
			/*title = m_title;
			//automatically elide the title
			title = titleFontMetrics.elidedText(title,Qt::ElideRight,dx);
			//*/

			//coordinates
			DcGpPointCloud* cloud = m_points[0].cloud;
			const unsigned& pointIndex = m_points[0].index;
			AddPointCoordinates(body,pointIndex,cloud,precision);

			//normal
			if (cloud->HasNormals())
			{
				const DCVector3D N = (cloud->GetNormals())[pointIndex];
				QString normStr = QString("Normal: (%1;%2;%3)").arg(N.x,0,'f',precision).arg(N.y,0,'f',precision).arg(N.z,0,'f',precision);
				body << normStr;
			}
			//color
			if (cloud->HasColors())
			{
			    ColorType* C = &cloud->GetPointColor(pointIndex).at(0);
				assert(C);
				QString colorStr = QString("Color: (%1;%2;%3)").arg(C[0]).arg(C[1]).arg(C[2]);
				body << colorStr;
			}
			//scalar field
			if (cloud->GetCurrentScalarField())
			{
				ScalarType value = cloud->GetCurrentScalarField()->GetPointScalarValue(pointIndex);
				QString sfStr = QString("Scalar Value: %1").arg(value, 0, 'f',precision);
				body << sfStr;
			}
			
		}
		break;

	case 2: //vector
		{
			//1st point
			DcGpPointCloud* cloud1 = m_points[0].cloud;
			const unsigned& pointIndex1 = m_points[0].index;
			const DCVector3D P1 = cloud1->GetPoint(pointIndex1);
			//2nd point
			DcGpPointCloud* cloud2 = m_points[1].cloud;
			const unsigned& pointIndex2 = m_points[1].index;
			const DCVector3D P2 = cloud2->GetPoint(pointIndex2);

			PointCoordinateType d = (P1-P2).Normal();
			QString distStr = QString("Distance = %1").arg(d,0,'f',precision);
			body << distStr;

			AddPointCoordinates(body,pointIndex1,cloud1,precision);
			AddPointCoordinates(body,pointIndex2,cloud2,precision);
		}
		break;

	case 3: //triangle/plane
		{
			//1st point
			DcGpPointCloud* cloud1 = m_points[0].cloud;
			const unsigned& pointIndex1 = m_points[0].index;
			const DCVector3D P1 = cloud1->GetPoint(pointIndex1);
			//2nd point
			DcGpPointCloud* cloud2 = m_points[1].cloud;
			const unsigned& pointIndex2 = m_points[1].index;
			const DCVector3D P2 = cloud2->GetPoint(pointIndex2);
			//3rd point
			DcGpPointCloud* cloud3 = m_points[2].cloud;
			const unsigned& pointIndex3 = m_points[2].index;
			const DCVector3D P3 = cloud3->GetPoint(pointIndex3);

			//area
			DCVector3D P1P2 = P2-P1;
			DCVector3D P1P3 = P3-P1;
			DCVector3D N = P1P2.Cross(P1P3); //N=ABxAC
			PointCoordinateType area = N.Normal()*(PointCoordinateType)0.5;
			QString areaStr = QString("Area = %1").arg(area,0,'f',9);
			body << areaStr;

			//coordinates
			AddPointCoordinates(body,pointIndex1,cloud1,precision);
			AddPointCoordinates(body,pointIndex2,cloud2,precision);
			AddPointCoordinates(body,pointIndex3,cloud3,precision);

			//normal
			N.Normalize();
			QString normStr = QString("Normal: (%1;%2;%3)").arg(N.x,0,'f',precision).arg(N.y,0,'f',precision).arg(N.z,0,'f',precision);
			body << normStr;

			//angle
			DCVector3D P2P3 = P3-P2;

			//negatives
			DCVector3D _P1P2 = -P1P2;
			DCVector3D _P1P3 = -P1P3;
			DCVector3D _P2P3 = -P2P3;

			double angleAtP1 = GetAngle_deg(P1P2,P1P3);
			double angleAtP2 = GetAngle_deg(P2P3,_P1P2);
			double angleAtP3 = GetAngle_deg(_P1P3,_P2P3); //should be equal to 180-a1-a2!
			QString angleStr = QString("Angles: A=%1, B=%3, C=%5 deg.").arg(angleAtP1,0,'f',precision).arg(angleAtP2,0,'f',precision).arg(angleAtP3,0,'f',precision);
			body << angleStr;

			//倾角倾向
			double dip = 0;
			double dd = 0;
			CalculateDipAndDD(P1, P2, P3, dip, dd);

			//度分秒
			int degree1, minute1, second1;
			int degree2, minute2, second2;
			RadToDegree(dip, degree1, minute1, second1);
			RadToDegree(dd, degree2, minute2, second2);
			QString ddStr = QString("DD = %1 Deg %2 Min %3 Sec").arg(degree2).arg(minute2).arg(second2);
			body << ddStr;

			QString dipStr = QString("Dip = %1 Deg %2 Min %3 Sec").arg(degree1).arg(minute1).arg(second1);
			body << dipStr;
		}
		break;
	case 4: //dem/plane
		{
			//1st point
			DcGpPointCloud* cloud1 = m_points[0].cloud;
			const unsigned& pointIndex1 = m_points[0].index;
			const DCVector3D P1 = cloud1->GetPoint(pointIndex1);
			//2nd point
			DcGpPointCloud* cloud2 = m_points[1].cloud;
			const unsigned& pointIndex2 = m_points[1].index;
			const DCVector3D P2 = cloud2->GetPoint(pointIndex2);
			//3rd point
			DcGpPointCloud* cloud3 = m_points[2].cloud;
			const unsigned& pointIndex3 = m_points[2].index;
			const DCVector3D P3 = cloud3->GetPoint(pointIndex3);
			//4rd point
			DcGpPointCloud* cloud4 = m_points[3].cloud;
			const unsigned& pointIndex4 = m_points[3].index;
			const DCVector3D P4 = cloud4->GetPoint(pointIndex4);

			//area
			DCVector3D P1P2 = P2-P1;
			DCVector3D P1P3 = P3-P1;
			DCVector3D N = P1P2.Cross(P1P3); //N=ABxAC
			PointCoordinateType area1 = N.Normal()*(PointCoordinateType)0.5;

			DCVector3D P1P4 = P4-P1;
			N = P1P4.Cross(P1P3); //N=ABxAC
			PointCoordinateType area2 = N.Normal()*(PointCoordinateType)0.5;

			PointCoordinateType area = area1 + area2;
			QString areaStr = QString("Area = %1").arg(area,0,'f',9);
			body << areaStr;

			//在label框中添加一条显示记录（点坐标）
			AddPointCoordinates(body,pointIndex1,cloud1,precision);
			AddPointCoordinates(body,pointIndex2,cloud2,precision);
			AddPointCoordinates(body,pointIndex3,cloud3,precision);
			AddPointCoordinates(body,pointIndex4,cloud4,precision);

			//normal
			N.Normalize();
			QString normStr = QString("Normal: (%1;%2;%3)").arg(N.x,0,'f',precision).arg(N.y,0,'f',precision).arg(N.z,0,'f',precision);
			body << normStr;
		}
		break;
	default:
		assert(false);
		break;
	}

	return body;
}

bool DC2DLabel::acceptClick(int x, int y, Qt::MouseButton button)
{
	if (button == Qt::RightButton)
	{
		if (x >= m_lastScreenPos[0]+m_labelROI[0] && x <= m_lastScreenPos[0]+m_labelROI[2]
			&& y >= m_lastScreenPos[1]-m_labelROI[3] && y <= m_lastScreenPos[1]-m_labelROI[1])
			{
				//toggle collapse state
				m_showFullBody = !m_showFullBody;
				return true;
			}
	}

	return false;
}

void DC2DLabel::DrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	if (m_points.empty())
		return;

	//2D foreground only
	if (!MACRO_Foreground(context))
		return;

	//Not compatible with virtual transformation (see ccDrawableObject::enableGLTransformation)
	if (MACRO_VirtualTransEnabled(context))
		return;

	if (MACRO_Draw3D(context))
		drawMeOnly3D(context);
	else if (MACRO_Draw2D(context))
		drawMeOnly2D(context);
}

//unit point marker
//static QSharedPointer<ccSphere> c_unitPointMarker(0);

void DC2DLabel::drawMeOnly3D(DcGp::DcGpDrawContext& context)
{
	assert(!m_points.empty());

	//standard case: list names pushing
	bool pushName = MACRO_DrawEntityNames(context);
	if (pushName)
	{
		//not particularily fast
		if (MACRO_DrawFastNamesOnly(context))
			return;
		glPushName(GetUniqueID());
	}

	const float c_sizeFactor = 4.0f;
	bool loop = false;

	size_t count = m_points.size();
	switch (count)
	{
	case 3:
		{
			glPushAttrib(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);

			//we draw the triangle
			glColor4ub(255,255,0,128);
			glBegin(GL_TRIANGLES);
			for (unsigned i=0; i<count; ++i)
				glVertex3f(m_points[i].cloud->GetPoint(m_points[i].index).x, m_points[i].cloud->GetPoint(m_points[i].index).y, m_points[i].cloud->GetPoint(m_points[i].index).z);
			glEnd();

			glPopAttrib();
			loop=true;
		}
	case 2:
		{
			//segment width
			glPushAttrib(GL_LINE_BIT);
			glLineWidth(c_sizeFactor);

			//we draw the segments
			if (IsSelected())
				glColor3ub(255,0,0);
			else
				glColor3ub(0,255,0);
			glBegin(GL_LINES);
			for (unsigned i=0; i<count; i++)
			{
				if (i+1<count || loop)
				{
					glVertex3f(m_points[i].cloud->GetPoint(m_points[i].index).x, m_points[i].cloud->GetPoint(m_points[i].index).y, m_points[i].cloud->GetPoint(m_points[i].index).z);
					glVertex3f(m_points[(i+1)%count].cloud->GetPoint(m_points[(i+1)%count].index).x, m_points[(i+1)%count].cloud->GetPoint(m_points[(i+1)%count].index).y, m_points[(i+1)%count].cloud->GetPoint(m_points[(i+1)%count].index).z);
				}
			}
			glEnd();
			glPopAttrib();
		}

	case 1:
		{
			//display point marker as spheres
		//	{
		//		if (!c_unitPointMarker)
		//		{
		//			c_unitPointMarker = QSharedPointer<ccSphere>(new ccSphere(1.0f,0,"PointMarker",12));
		//			c_unitPointMarker->showColors(true);
		//			c_unitPointMarker->setVisible(true);
		//			c_unitPointMarker->setEnabled(true);
		//		}
		//	
		//		//build-up point maker own 'context'
		//		CC_DRAW_CONTEXT markerContext = context;
		//		markerContext.flags &= (~CC_DRAW_ENTITY_NAMES); //we must remove the 'push name flag' so that the sphere doesn't push its own!
		//		markerContext._win = 0;

		//		if (isSelected() && !pushName)
		//			c_unitPointMarker->setTempColor(ccColor::red);
		//		else
		//			c_unitPointMarker->setTempColor(ccColor::magenta);

		//		for (unsigned i=0; i<count; i++)
		//		{
		//			glMatrixMode(GL_MODELVIEW);
		//			glPushMatrix();
		//			const DCVector3D* P = m_points[i].cloud->getPoint(m_points[i].index);
		//			ccGL::Translate(P->x,P->y,P->z);
		//			glScalef(context.pickedPointsRadius,context.pickedPointsRadius,context.pickedPointsRadius);
		//			c_unitPointMarker->draw(markerContext);
		//			glPopMatrix();
		//		}
		//	}

		//	if (m_dispIn3D && !pushName) //no need to display label in point picking mode
		//	{
		//		QFont font(context._win->getTextDisplayFont()); //takes rendering zoom into account!
		//		//font.setPointSize(font.pointSize()+2);
		//		font.setBold(true);

		//		//draw their name
		//		glPushAttrib(GL_DEPTH_BUFFER_BIT);
		//		glDisable(GL_DEPTH_TEST);
		//		for (unsigned j=0; j<count; j++)
		//		{
		//			const DCVector3D* P = m_points[j].cloud->getPoint(m_points[j].index);
		//			QString title = (count == 1 ? getName() : QString("P#%0").arg(m_points[j].index));
		//			context._win->display3DLabel(	title,
		//											*P + DCVector3D(	context.pickedPointsTextShift,
		//															context.pickedPointsTextShift,
		//															context.pickedPointsTextShift),
		//											ccColor::magenta,
		//											font );
		//		}
		//		glPopAttrib();
		//	}
		}
	}

	if (pushName)
		glPopName();
}

//display parameters
static const int c_margin = 5;
static const int c_arrowBaseSize = 3;
//static const int c_buttonSize = 10;

void DC2DLabel::drawMeOnly2D(DcGp::DcGpDrawContext& context)
{
	if (!m_dispIn2D)
		return;

	assert(!m_points.empty());

	//standard case: list names pushing
	bool pushName = MACRO_DrawEntityNames(context);
	if (pushName)
		glPushName(GetUniqueID());

	//we should already be in orthoprojective & centered omde
	//glOrtho(-halfW,halfW,-halfH,halfH,-maxS,maxS);

	int strHeight = 0;
	int titleHeight = 0;
	QString title(GetName());
	QStringList body;
	GLdouble arrowDestX=-1.0,arrowDestY=-1.0;
	QFont bodyFont,titleFont;
	if (!pushName)
	{
		/*** line from 2D point to label ***/

		//compute arrow head position
		DCVector3D arrowDest = m_points[0].cloud->GetPoint(m_points[0].index);
		
		for (unsigned i=1;i<m_points.size();++i)
			arrowDest += m_points[i].cloud->GetPoint(m_points[i].index);
		arrowDest /= (PointCoordinateType)m_points.size();

		//project it in 2D screen coordinates
		int VP[4];
		context._win->GetViewportArray(VP);
		const double* MM = context._win->GetModelViewMatrixd(); //viewMat
		const double* MP = context._win->GetProjectionMatrixd(); //projMat
		GLdouble zp;
		gluProject(arrowDest.x,arrowDest.y,arrowDest.z,MM,MP,VP,&arrowDestX,&arrowDestY,&zp);

		/*** label border ***/
		bodyFont = context._win->GetDisplayedTextFont(); //takes rendering zoom into account!
		titleFont = QFont(context._win->GetDisplayedTextFont()); //takes rendering zoom into account!
		titleFont.setBold(true);
		QFontMetrics titleFontMetrics(titleFont);
		QFontMetrics bodyFontMetrics(bodyFont);

		strHeight = bodyFontMetrics.height();
		titleHeight = titleFontMetrics.height();

		if (m_showFullBody)
			body = getLabelContent(context.dispNumberPrecision);

		//确定box大小
		int dx = 150;
		dx = std::max(dx,titleFontMetrics.width(title));   //宽度

		int dy = c_margin;	//顶部页边的空白
		dy += titleHeight;	//标题的高度
		if (!body.empty())
		{
			dy += c_margin;	//标题与空白页边的间隔
			for (int j=0; j<body.size(); ++j)
			{
				dx = std::max(dx,bodyFontMetrics.width(body[j]));
				dy += strHeight; //文本内容高度
			}
			dy += c_margin;	//文本内容到下边边缘高度
		}
		else
		{
			dy += c_margin;	// vertical margin (purely for aesthetics)
		}
		dy += c_margin;		// 底部页边的空白
		dx += c_margin*2;	// 水平方向页边空白为*2

		//根据长宽确定矩形范围，存储在成员变量中
		m_labelROI[0]=0;
		m_labelROI[1]=0;
		m_labelROI[2]=dx;
		m_labelROI[3]=dy;

		//close button
		/*m_closeButtonROI[2]=dx-c_margin;
		m_closeButtonROI[0]=m_closeButtonROI[2]-c_buttonSize;
		m_closeButtonROI[3]=c_margin;
		m_closeButtonROI[1]=m_closeButtonROI[3]+c_buttonSize;
		//*/

		//automatically elide the title
		//title = titleFontMetrics.elidedText(title,Qt::ElideRight,m_closeButtonROI[0]-2*c_margin);
	}
	//窗口的宽和高
	int halfW = (context.glW>>1);
	int halfH = (context.glH>>1);

	//绘制label矩形
	int xStart = m_lastScreenPos[0] = (int)((float)context.glW * m_screenPos[0]);
	int yStart = m_lastScreenPos[1] = (int)((float)context.glH * (1.0f-m_screenPos[1]));

	//颜色
	bool highlighted = (!pushName && IsSelected());
	//默认背景色
	ColorType defaultBkgColor[4];
	memcpy(defaultBkgColor,context.labelDefaultCol,sizeof(ColorType)*3);
	defaultBkgColor[3] = (ColorType)((float)context.labelsTransparency*(float)MAX_COLOR_COMP/100.0f);
	//边界颜色，不能完全透明，否则看不到
	ColorType defaultBorderColor[4];
	if (highlighted)
		memcpy(defaultBorderColor,&QColor(255,0,0),sizeof(ColorType)*3);
	else
		memcpy(defaultBorderColor,context.labelDefaultCol,sizeof(ColorType)*3);
	defaultBorderColor[3] = (ColorType)((float)(50+context.labelsTransparency/2)*(float)MAX_COLOR_COMP/100.0f);

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(static_cast<GLfloat>(-halfW+xStart),static_cast<GLfloat>(-halfH+yStart),0);

	if (!pushName)
	{
		//计算指示箭头相对于label矩形的位置 (for 0 to 8)
		int arrowBaseConfig = 0;
		int iArrowDestX = (int)arrowDestX-xStart;
		int iArrowDestY = (int)arrowDestY-yStart;
		{
			if (iArrowDestX < m_labelROI[0]) //left
				arrowBaseConfig += 0;
			else if (iArrowDestX > m_labelROI[2]) //Right
				arrowBaseConfig += 2;
			else  //Middle
				arrowBaseConfig += 1;

			if (iArrowDestY > -m_labelROI[1]) //Top
				arrowBaseConfig += 0;
			else if (iArrowDestY < -m_labelROI[3]) //Bottom
				arrowBaseConfig += 6;
			else  //Middle
				arrowBaseConfig += 3;
		}

		//确定箭头从最近的角开始
		if (arrowBaseConfig != 4) //4 = label刚好在点上!
		{
			glColor4ubv(defaultBorderColor);
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(arrowDestX-xStart,arrowDestY-yStart);
			switch(arrowBaseConfig)
			{
			case 0: //top-left corner
				glVertex2i(m_labelROI[0], -m_labelROI[1]-2*c_arrowBaseSize);
				glVertex2i(m_labelROI[0], -m_labelROI[1]);
				glVertex2i(m_labelROI[0]+2*c_arrowBaseSize, -m_labelROI[1]);
				break;
			case 1: //top-middle edge
				glVertex2i(std::max(m_labelROI[0],iArrowDestX-c_arrowBaseSize), -m_labelROI[1]);
				glVertex2i(std::min(m_labelROI[2],iArrowDestX+c_arrowBaseSize), -m_labelROI[1]);
				break;
			case 2: //top-right corner
				glVertex2i(m_labelROI[2], -m_labelROI[1]-2*c_arrowBaseSize);
				glVertex2i(m_labelROI[2], -m_labelROI[1]);
				glVertex2i(m_labelROI[2]-2*c_arrowBaseSize, -m_labelROI[1]);
				break;
			case 3: //middle-left edge
				glVertex2i(m_labelROI[0], std::min(-m_labelROI[1],iArrowDestY+c_arrowBaseSize));
				glVertex2i(m_labelROI[0], std::max(-m_labelROI[3],iArrowDestY-c_arrowBaseSize));
				break;
			case 4: //middle of rectangle!
				break;
			case 5: //middle-right edge
				glVertex2i(m_labelROI[2], std::min(-m_labelROI[1],iArrowDestY+c_arrowBaseSize));
				glVertex2i(m_labelROI[2], std::max(-m_labelROI[3],iArrowDestY-c_arrowBaseSize));
				break;
			case 6: //bottom-left corner
				glVertex2i(m_labelROI[0], -m_labelROI[3]+2*c_arrowBaseSize);
				glVertex2i(m_labelROI[0], -m_labelROI[3]);
				glVertex2i(m_labelROI[0]+2*c_arrowBaseSize, -m_labelROI[3]);
				break;
			case 7: //bottom-middle edge
				glVertex2i(std::max(m_labelROI[0],iArrowDestX-c_arrowBaseSize), -m_labelROI[3]);
				glVertex2i(std::min(m_labelROI[2],iArrowDestX+c_arrowBaseSize), -m_labelROI[3]);
				break;
			case 8: //bottom-right corner
				glVertex2i(m_labelROI[2], -m_labelROI[3]+2*c_arrowBaseSize);
				glVertex2i(m_labelROI[2], -m_labelROI[3]);
				glVertex2i(m_labelROI[2]-2*c_arrowBaseSize, -m_labelROI[3]);
				break;
			}
			glEnd();
		}
	}

	//主矩形边框
	glColor4ubv(defaultBkgColor);
	glBegin(GL_QUADS);
	glVertex2i(m_labelROI[0], -m_labelROI[1]);
	glVertex2i(m_labelROI[0], -m_labelROI[3]);
	glVertex2i(m_labelROI[2], -m_labelROI[3]);
	glVertex2i(m_labelROI[2], -m_labelROI[1]);
	glEnd();

	//if (highlighted)
	{
		glPushAttrib(GL_LINE_BIT);
		glLineWidth(3.0f);
		glColor4ubv(defaultBorderColor);
		glBegin(GL_LINE_LOOP);
		glVertex2i(m_labelROI[0], -m_labelROI[1]);
		glVertex2i(m_labelROI[0], -m_labelROI[3]);
		glVertex2i(m_labelROI[2], -m_labelROI[3]);
		glVertex2i(m_labelROI[2], -m_labelROI[1]);
		glEnd();
		glPopAttrib();
	}

	//draw close button
	/*glColor3ubv(ccColor::black);
	glBegin(GL_LINE_LOOP);
	glVertex2i(m_closeButtonROI[0],-m_closeButtonROI[1]);
	glVertex2i(m_closeButtonROI[0],-m_closeButtonROI[3]);
	glVertex2i(m_closeButtonROI[2],-m_closeButtonROI[3]);
	glVertex2i(m_closeButtonROI[2],-m_closeButtonROI[1]);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(m_closeButtonROI[0]+2,-m_closeButtonROI[1]+2);
	glVertex2i(m_closeButtonROI[2]-2,-m_closeButtonROI[3]-2);
	glVertex2i(m_closeButtonROI[2]-2,-m_closeButtonROI[1]+2);
	glVertex2i(m_closeButtonROI[0]+2,-m_closeButtonROI[3]-2);
	glEnd();
	//*/

	//绘制文本
	if (!pushName)
	{
		int xStartRel = c_margin;
		int yStartRel = -c_margin;
		yStartRel -= titleHeight;

		const ColorType* defaultTextColor = (context.labelsTransparency<40 ? context.textDefaultCol :  DcGp::Color::DARKBLUE);

		context._win->Display2DLabel(title,xStart+xStartRel,yStart+yStartRel,DcGpBasicDevice::eDefaultAlign,0,defaultTextColor,&titleFont);
		yStartRel -= c_margin;

		if (!body.empty())
		{
			//line separation
			glColor4ubv(defaultBorderColor);
			glBegin(GL_LINES);
			glVertex2i(xStartRel,yStartRel);
			glVertex2i(xStartRel+m_labelROI[2]-m_labelROI[0]-2*c_margin,yStartRel);
			glEnd();

			//display body
			yStartRel -= c_margin;
			for (int i=0; i<body.size(); ++i)
			{
				yStartRel -= strHeight;
				context._win->Display2DLabel(body[i],xStart+xStartRel,yStart+yStartRel,DcGp::DcGpBasicDevice::eDefaultAlign,0,defaultTextColor,&bodyFont);
			}
		}
	}

	glPopAttrib();

	glPopMatrix();

	if (pushName)
		glPopName();
}