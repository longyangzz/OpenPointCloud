#ifndef DCGP_POINTCLOUD_H
#define DCGP_POINTCLOUD_H

#include "DCGp/DcGpDLL.h"

//c++标准库
#include <vector>
#include <array>

//Qt
#include "QtGui/QMatrix4x4"

//Dc
#include "DCCore/DCDefs.h"

//Common
#include "DCCore/BasicTypes.h"

#include "DCGp/GpBasicTypes.h"
#include "GpScalarField.h"
#include "DCGp/GpEntity.h"

#include "DCUtil/AbstractEntityVisitor.h"

namespace DcFilter
{
	class BlockedGridToolkit;
}

namespace DcGp
{
	class EntityVisitor;
	class DcGpBoundBox;
	
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpPointCloud)
	DECLARE_SMARTPTR(DcGpPointCloudImpl)
	/*
		* 点云图元
	*/
	class DCGP_API DcGpPointCloud : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpPointCloud)
		DECLARE_IMPLEMENT(DcGpPointCloud)
		GET_CLASSNAME(DcGpPointCloud)

	public:

		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpPointCloud("PointCloud");
		}

		DcGpPointCloud(QString name);
		DcGpPointCloud(QString name, long num);
		DcGpPointCloud(DcGpPointCloud* associatedCloud);
		virtual ~DcGpPointCloud();

		virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

		//绘制图形
		virtual void DrawMyselfOnly(DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGpDrawContext& context);
		virtual void DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample = false);
		//添加点
		void AddPoint(DCVector3D vector);

		//设置法向量
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//获取法向量值
		virtual	std::vector<Normal> GetNormals();
		//增加颜色
		void AddColor(PointColor color);

		//添加标量场
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//获取指定的标量场
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//显示标量场
		virtual void ShowScalarField(bool state);

		//设置当前标量场的颜色表
		virtual void ChangeScalarFieldColorScale(QString name);

		//标量场的数量
		virtual unsigned ScalarFieldCount() const;

		//标量场的名字
		virtual QStringList ScalarFieldNames() const;

		//设置当前标量场
		void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField);

		virtual int GetCurrentScalarFieldIndex();
		DcGpScalarFieldPtr GetCurrentScalarField();

		//是否拥有颜色
		virtual bool HasColors() const;

		//是否拥有标量场
		virtual bool HasScalarFields() const;

		//是否拥有向量场
		virtual bool HasNormals() const;

		//获取指定点
		DCVector3D GetPoint(unsigned index) const;

		//获取最大点
		PointCoord GetMaxPoint() const;
		//获取最小点
		PointCoord GetMinPoint() const;

		//获取指定点的可见性
		uchar GetPointVisibility(unsigned index) const;

		//获取指定顶点的颜色
		PointColor GetPointColor(unsigned index) const;

		//获取指定点的选中状态
		bool GetPointChoosed(unsigned index) const;

		//设置指定点的选中状态
		void SetPointChoosed(unsigned index, bool status);

		//启用选取颜色
		void EnableChoosedColors();

		//停用选取颜色
		void DisableChoosedColors();

		//启用部分显示功能
		//参数一：是否启用
		//参数二：显示的内容（如果是true则显示显示状态为true的，否则显示状态为false的）
		void EnableVisibiltyTable(bool status, bool flag = true);

		//获取所有点
		std::vector<DCVector3D> GetPoints();
		std::vector<DCVector3D>& GetPointsRef();
		//设置所有点
		void SetPoints(std::vector<DCVector3D>& points);

		//返回指定索引处的数值
		DCVector3D GetDataByIndex(unsigned index);
		//获取点数量
		unsigned long Size() const;
		unsigned long GetChunk();

		//坐标转换
		void ApplyTransform(EigenMatrix4d& transMatrix);

		//获取绘制参数
		virtual void GetDrawingParameters(glDrawParameters& params) const;

		//刷新选取颜色
		virtual void RefreshChoosedColor();

		//取消选取
		void CancelChoosed();

		//删除所选的部分
		void DeleteChoosed();

		bool IsChoosed();

		//
		virtual DcGpBoundBox GetDisplayBoundBox();
		//获取边界盒
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//保存到文件
		bool SaveToFile(QFile& file) const;

		//释放各成员变量中多余的内存
		void AutoAdjust();

		//设置颜色容器的大小
		void SetColorSize(long num);

		int GetPointSize();

		//获取rgb值
		std::vector<PointColor> GetRGBColor();
		std::vector<PointColor>& GetRGBColorRef();

		//设置RGB的值
		void SetRGBColor(std::vector<PointColor> rgbColor);

		//!brief 设置更新颜色条的显示状态
		void SetSfColorBarVisibility(bool status /*!<[in] 传入的状态值 */);

		//!\brief 更新context中sfColorScaleToDisplay变量值
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] 上下文绘制参数 */);

		//!\brief 设置pop
		void SetPop(QMatrix4x4 matrix);
		//!\brief 获取pop
		QMatrix4x4 GetPop();

		//!\brief 获取选择的点云
		void GetChoosedPoints(std::vector<DCVector3D>& points);

		//DcFilter::BlockedGridToolkit* GetBlocked();


		//更新指定点的可见性
		void SetPointVisibility(unsigned index, uchar visibility);

		//!\brief 判断是否有点云被选取
		bool HasChoosed();

		//获取指定顶点的法向量
		DCVector3D GetPointNormal(unsigned index) const;

		//添加指定定点的法向量
		void AddNormal(DCVector3D normal);

		//设置法向量容器的大小
		void SetNormalSize(long num);

		//获取指定名称的标量场对应的索引号
		int GetScalarFieldIndexByName(QString name);

	protected:
		DcGpPointCloud();

	public slots:
		void SetPointSize(int size);
	};

}
#endif // POINTCLOUD_H
