#pragma once

#include "DCGp/DcGpDLL.h"

#include <QObject>
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpPointCloud.h"

namespace DCUtil
{
	class DatabaseRequest;
	
}

namespace DcGp
{
	class DcGpDrawContext;
	class DcGpBoundBox;

	struct DCGP_API PerRangeData
	{
		PerRangeData();

		QString                     _filename;
		float                           _priorityOffset;
		float                           _priorityScale;
		double                          _timeStamp;
		unsigned int                    _frameNumber;

		//需要保存一个数据请求节点，默认为nullptr，加载后有值，用来区分加载请求是否已添加，并且是否已读完。
		boost::shared_ptr<DCUtil::DatabaseRequest > m_databaseRequest;
	};

	typedef std::vector<PerRangeData> PerRangeDataList;
	typedef std::pair<float, float>  MinMaxPair;
	typedef MinMaxPair Range;

	class DCGP_API PageLodEntity : public DcGp::DcGpEntity
	{
		Q_OBJECT
			GET_CLASSNAME(PageLodEntity)
	public:
		PageLodEntity(QString name);
		~PageLodEntity();

		virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

		virtual void Traverse(DCUtil::AbstractEntityVisitor& nv);

		//绘制图形
		virtual void DrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGpDrawContext& context);

		void SetTopLevelEntity(DcGp::DcGpPointCloud* topLevelEntity);

		//添加点
		void AddPoint(DCVector3D vector);

		//增加颜色
		void AddColor(PointColor color);

		unsigned Size();

		//
		virtual DcGpBoundBox GetDisplayBoundBox();
		//获取边界盒
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//std::vector<QString > GetPageQuadtree();
		void SetFilename(unsigned int childNo, const QString& filename);

		/** Set the database path to prepend to children's filenames.*/
		void SetDatabasePath(const QString& path)
		{
			m_databasePath = path;
		}

		/** Get the database path used to prepend to children's filenames.*/
		inline const QString& GetDatabasePath() const { return m_databasePath; }
		PerRangeDataList GetPerRangeDataList() { return m_perRangeDataList;  }

		void SetRange(Range rg)
		{
			m_range = rg;
		}

		void SetCenter(DCVector3D center)
		{
			m_pageBoxCenter = center;
		}
	private:
		DcGp::DcGpPointCloud* m_topLevelEntity;

		//!关联的子级文件名称
		PerRangeDataList m_perRangeDataList;
		QString         m_databasePath;

		//! 分区边界盒中心，不同于实际的数据边界盒
		DCVector3D m_pageBoxCenter;

		//! 支持的距离值
		Range m_range;

		//std::vector<QString > m_pageQuadtree;
	};
}

