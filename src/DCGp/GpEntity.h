#ifndef DCGP_ENTITY_H
#define DCGP_ENTITY_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>

//Qt
#include "QVector"

#include "DCCore/DCDefs.h"
#include "GpDrawableObject.h"
//#include "DCUtil/AbstractEntityVisitor.h"

namespace DCUtil
{
	class AbstractEntityVisitor;
}

namespace DcGp
{
	
	class DcGpBasicDevice;
	class DcGpBoundBox;

	DECLARE_SMARTPTR(DcGpEntity)
	DECLARE_SMARTPTR(DcGpEntityImpl)

	typedef QVector<DcGpEntity*> Container;
	typedef std::vector<DcGpEntityPtr> ContainerPtr;

	class DCGP_API DcGpEntity : public DcGpDrawableObject
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpEntity)
		DECLARE_IMPLEMENT(DcGpEntity)
		GET_CLASSNAME(DcGpEntity)

		Q_OBJECT
		public:
			DcGpEntity(QString name);

			~DcGpEntity();

			void AddChildPtr(DcGpEntityPtr pChild, bool dependant = true);
			void AddItemPtr(DcGpDrawableObjectPtr pItem);
			DcGpEntityPtr GetChildPtr(unsigned index);
			DcGpEntityPtr GetParentPtr();
			void SetParentPtr(DcGpEntityPtr pParent);
			void RemoveChildPtr(const DcGpEntityPtr pChild);
			void RemoveChildPtr(int pos);
			void DeleteAllChildrenPtr();
			DcGpEntityPtr FindPtr(unsigned uniqueID);
			int GetChildIndexPtr(const DcGpEntityPtr child) const;
			ContainerPtr GetChildrenPtr() const;
			unsigned GetChildrenNumberPtr() const;
			//添加子图形容器
			void AddChild(DcGpEntity* child, bool dependant = true);
			//添加图元对象
			void AddItem(DcGpDrawableObject* item);
			//获取父对象
			DcGpEntity* GetParent() const;
			//设置父对象
			void SetParent(DcGpEntity* parent);
			//移除指定的子对象
			void RemoveChild(const DcGpEntity* object, bool isDelete = true);
			//移除指定索引处的子对象，并不执行delete
			void RemoveChild(int pos, bool isDelete = true);
			//移除并删除所有的子对象
			void DeleteAllChildren();
			//
			DcGpEntity* Find(unsigned uniqueID);
			//获取子对象的序号
			int GetChildIndex(const DcGpEntity* child) const;
			//获取子对象
			DcGpEntity* GetChild(unsigned index);
			//
			Container GetChildren() const;
			//获取子对象的数量
			unsigned GetChildrenNumber() const;

			//! 判断当前对象是不是传入对象的父类
			bool IsAncestorOf(const DcGpEntity* anObject) const;


			//绘制
			virtual void Draw(DcGpDrawContext& context);
			virtual void FastDraw3D(DcGpDrawContext& context);
			virtual void DrawWithNames(DcGpDrawContext& context, bool drawSample = false);
			//仅绘制实体，不绘制子对象
			virtual void DrawMyselfOnly(DcGpDrawContext& context);
			virtual void FastDrawMyselfOnly(DcGpDrawContext& context);
			virtual void DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample = false);

			//获取索引
			int GetIndex() const;

			//继承自DcGpDrawableObject
			virtual DcGpBoundBox GetBoundBox(bool relative=true, bool withGLfeatures=false, const DcGpBasicDevice* window=0);
			virtual DcGpBoundBox GetFitBoundBox(DcGpMatrix& trans);
			virtual DcGpBoundBox GetMyOwnBoundBox();
			virtual DcGpBoundBox GetDisplayBoundBox();

			//连接窗体
			virtual void LinkWindow(DcGpBasicDevice* window);

			//! Behavior when selected
			enum SelectionBehavior 
			{ 
				eSelectionAABoundBox		=	0	,
				eSelectionFitBoundBox				,
				eSelectionIgnored
			};
	
			//
			void SetSelectionBehavior(SelectionBehavior mode);

			//!返回选中状态
			SelectionBehavior GetSelectionBehavior() const;

			//是否偏移
			bool HasShifted() const;
			//设置坐标偏移
			void SetShift(Point_3D shift);
			//获取偏移
			Point_3D GetShift() const;

			//DoubleClicked
			virtual void DoubleClicked(unsigned selectedID, unsigned subSelectedID);

			//保存到文件
			virtual bool SaveToFile(QFile& file) const;

			//!brief 设置提示信息
			void SetToolTip(const QString& strToolTip);
			//!brief 获取提示信息
			QString GetToolTip() const;

			//计算法向量
			virtual void ComputerNormal() {};

			virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

			virtual void Ascend(DCUtil::AbstractEntityVisitor& nv);

			virtual void Traverse(DCUtil::AbstractEntityVisitor& nv);

		private:
			void Draw3DName(DcGpDrawContext& context);

		protected:
			DcGpEntity();
		};

}
#endif // DCGRAPHICCONTAINER_H
