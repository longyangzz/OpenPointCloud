#ifndef DC_PYRA_QUADTREE_H
#define DC_PYRA_QUADTREE_H

#include "pyramidDLL.h"
#include "QString"
#include <array>

namespace DC
{
	namespace Pyra
	{
		class Quadnode;
		class BoundBox;
		class NodeDebug;

		class PYRAMID_API Quadtree
		{
		public:
			Quadtree(int depth, BoundBox* box, QString fileName);

			//根据传进的文件名，解析出basename，然后从对应的box辅助文件中，初始化box
			Quadtree(QString fileName);
			~Quadtree();

			//根据传入的深度值和边界盒，进行四叉树结构分割建立树形结构
			void QuadCreateBranch(Quadnode* parent);

			Quadnode* GetRoot() const;

			QString GetBaseFileName() const;

			//重绘
			void Draw(const float& actualPsize, std::array<std::array<float, 4> ,6> frustum, const float& inter);

			BoundBox* GetBoundBox() const;

			uint CaluLoadedLevel(float pixelSize, uint depth, const float& inter);
		private:
			//根据传入的文件名，初始化解析出basename
			void InitGenBaseNameFromOther(QString name);

			//根据basename，初始化box
			void InitBoxAndDepthFromFile();
		private: 
			//初始距离
			float m_initDis;
			//是否初始化
			bool m_isDisInited;
			
			//深度
			unsigned m_depth;
			
			//根节点
			Quadnode* m_root;

			//basename
			QString m_baseFileName;

			QString m_suffix;

			//dirPath
			QString m_dir;

			BoundBox* m_boundBox;

			NodeDebug* m_nodeDebug;
		};
	}
}

#endif // DC_PYRA_QUADTREE_H
