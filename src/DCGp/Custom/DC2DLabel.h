#ifndef DC2DLABEL_H
#define DC2DLABEL_H

#include "DCGp/DcGpDLL.h"

//Qt
#include <QString>
#include <QStringList>
#include <QtGui/QFontMetrics>

#include "DCCore/DCDefs.h"
#include "DCCore/Const.h"

//×Ô¶¨Òå
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "GpPointCloud.h"
#include "GpInteractor.h"

namespace DcGp
{

class DCGP_API DC2DLabel : public DcGpEntity , public DcGpInteractor
{
public:

	GET_CLASSNAME(DC2DLabel);

	//! Default constructor
	DC2DLabel(QString name = QString("label"));

	//inherited from ccObject
	QString GetName();
	//inherited from ccHObject
	//virtual CC_CLASS_ENUM getClassID() const { return CC_TYPES::LABEL_2D; }
	//virtual bool isSerializable() const { return true; }

	//! Returns 'raw' name (no replacement of default keywords)
	QString GetRawName() const { return DcGpObject::GetName(); }

	//! Adds a point to label
	/** Adding a point to a label will automatcillay make it 'mute'.
		1 point  = 'point' label (point position, normal, color, etc.)
		2 points = 'vector' label (vertices position, distance)
		3 points = "triangle/plane' label (vertices position, area, normal)
		\return false if 'full'
	**/
	bool addPoint(DcGpPointCloud* cloud, unsigned pointIndex);

	//! Gets label content (as it will be displayed)
	/** \param precision displayed numbers precision
		\return label body (one string per line)
	**/
	QStringList getLabelContent(int precision);

	//inherited from ccInteractor
	virtual bool acceptClick(int x, int y, Qt::MouseButton button);
	virtual bool Move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight);

	//! Sets relative position
	void setPosition(float x, float y);

	//! Returns relative position
	const float* getPosition() const { return m_screenPos; }

	//! Clears label
	void clear(bool ignoreDependencies = false);

	//! Returns current size
	unsigned size() const { return (unsigned)m_points.size(); }

	//! Whether to collapse label or not
	void setCollapsed(bool state) { m_showFullBody = !state; }

	//! Returns Whether the label is collapsed or not
	bool isCollapsed() const { return !m_showFullBody; }

	//! Whether to display the label in 3D (title only)
	void setDisplayedIn3D(bool state) { m_dispIn3D = state; }

	//! Returns whether the label is displayed in 3D (title only)
	bool isDisplayedIn3D() const { return m_dispIn3D; }

	//! Whether to display the label in 2D
	void setDisplayedIn2D(bool state) { m_dispIn2D = state; }

	//! Returns whether the label is displayed in 2D
	bool isDisplayedIn2D() const { return m_dispIn2D; }

	//! Picked point descriptor
	/** Label 'points' can be shared between multiple entities
	**/
	struct PickedPoint
	{
		//! cloud
		DcGpPointCloud* cloud;
		//! index
		unsigned index;

		//! Default constructor
		PickedPoint()
			: cloud(0)
			, index(0)
		{}

		//! Constructor from a point and its index
		PickedPoint(DcGpPointCloud* _cloud, unsigned _index)
			: cloud(_cloud)
			, index(_index)
		{}
	};

	//! Returns a given point
	const PickedPoint& getPoint(unsigned index) const { return m_points[index]; }

protected:

	virtual void DrawMyselfOnly(DcGpDrawContext& context);

	//! Draws the entity only (not its children) - 2D version
	virtual void drawMeOnly2D(DcGpDrawContext& context);
	//! Draws the entity only (not its children) - 3D version
	virtual void drawMeOnly3D(DcGpDrawContext& context);

	//! Picked points
	std::vector<PickedPoint> m_points;

	//! Updates the label 'name'
	void updateName();

	//! Whether to show full label body or not
	bool m_showFullBody;

	//! label ROI
	/** ROI is displayed relatively to m_screenPos
		(projected in displayed screen space). m_screenPos
		corresponds to its top-left corner. So ROI[1] is
		the distance to the upper border and ROI[3] is the
		distance to the lower border (relatively to m_screenPos).
	**/
	int m_labelROI[4];

	//! close button ROI
	//int m_closeButtonROI[4];

	//! Label position (percentage of screen size)
	float m_screenPos[2];

	//! Label position at last display (absolute)
	int m_lastScreenPos[2];

	//! Whether to display the label in 3D (title only)
	bool m_dispIn3D;

	//! Whether to display the label in 2D
	bool m_dispIn2D;

};

}

#endif // DC2DLABEL_H
