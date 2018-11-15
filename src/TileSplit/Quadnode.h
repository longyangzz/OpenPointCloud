#ifndef DC_PYRA_QUADNODE_H
#define DC_PYRA_QUADNODE_H

//Qt
#include "QString"
#include "QVector"

#include "Common.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace DC
{
	namespace Pyra
	{
		class BoundBox;
		class Geometry;

		class Quadnode
		{
		public:
			Quadnode();
			~Quadnode();

			void SetBoundBox(BoundBox* box);
			BoundBox* GetBoundBox() const;

			void SetLevel(const LevelType& level);
			int GetLevel() const;

			void SetFullName(const QString& fullName);

			//添加一个子节点
			void AddChild(Quadnode* child, bool dependant = true);

			void SetParent(Quadnode* parent);
			Quadnode* GetParent() const;

			void Draw(const float& actualPsize, std::array<std::array<float, 4> ,6> frustum, unsigned depth);

			//安装具体的几何图元类
			void SetGeometry(Geometry* geometry);

		//视景体与边界盒裁剪接口
		bool PointInFrustum(float x, float y, float z, std::array<std::array<float, 4> ,6> frustum);
		bool CubeInFrustum( float x, float y, float z, float size, std::array<std::array<float, 4> ,6> frustum);
		bool BoxInFrustum(const Point3D& minCorner, const Point3D& maxCorner, std::array<std::array<float, 4> ,6> frustum);
		//数据获取接口
		public:

			//必须是返回引用
			std::vector<DCVector3D>& GetPoints();
			unsigned GetPointsSize();

			std::vector<PointColor>& GetRGBColors();
		private:
			void SetNodeLoadState(bool state);
		private:
			//边界盒
			BoundBox* m_box;

			//所在的层级号
			LevelType m_level;

			//关联的文件名
			QString m_fileFullname;

			//子节点容器
			QVector<Quadnode* > m_children;

			//图元对象
			Geometry* m_geometry;

			std::vector<DCVector3D> m_points;
			std::vector<PointColor> m_rgbColors;

			//节点是否已加载
			bool m_isLoaded;

			unsigned m_lastLevel;

			//父节点
			Quadnode* m_parent;
		};
	}
}



#endif // DC_PYRA_QUADNODE_H
