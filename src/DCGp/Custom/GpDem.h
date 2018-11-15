#ifndef DCGP_DEM_H
#define DCGP_DEM_H

#include "DCGp/DcGpDLL.h"
#include "QMap"

//C++
#include <memory>
#include <map>
#include <array>

//DCCore
#include "DCCore/DCDefs.h"

//common
#include "DCCore/BasicTypes.h"
#include "DCGp/Custom/GpBoundBox.h"

#include "DCGp/GpEntity.h"

//class QTableWidget;
class QImage;

//格网大小类型
typedef double GridSize_Type;
//格网数量类型
typedef unsigned GridCount_Type;
//DEM的顶点类型

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpDEM)
	DECLARE_SMARTPTR(DcGpDEMImpl)

	class DCGP_API DcGpDEM : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpDEM)
		DECLARE_IMPLEMENT(DcGpDEM)
		GET_CLASSNAME(DcGpDEM)

	public:
		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpDEM("Dem");
		}

		typedef std::array<unsigned, 2> Index;
		typedef std::map<Index, std::vector<double>> VertexHeight;

		DcGpDEM(QString name);
		virtual ~DcGpDEM();

		//设置左下角的坐标和格网数量
		void SetParameter(Point_3D startPoint, 
			GridCount_Type xNumber, 
			GridCount_Type yNumber,
			GridSize_Type xInterval,
			GridSize_Type yInterval,
			VertexHeight heights,
			bool holdBoder = true);

		//更新参数
		void UpdateParameter(Point_3D startPoint, 
							GridCount_Type xNumber, 
							GridCount_Type yNumber,
							GridSize_Type xInterval,
							GridSize_Type yInterval);

		//获取参数
		QMap<QString, double> GetParameter();

		//添加顶点
		//void AddVertex(DEMVertex vertex);

		//显示模式
		enum ShowStyle
		{
			eShowPoint      =   0   ,
			eShowGrid		=	1	,
			eShowFill		=	2	,
			eShowDefault	=	3
		};
		//设置显示模式
		inline void SetShowStyle(ShowStyle style);
		//显示属性
		void ShowProperties(QTableWidget* widget);
		//计算土方量
		void CalculateEarthVolumn(double height, double& excavation, double& fill);

		//设置法向量
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//获取法向量值
		virtual std::vector<Normal> GetNormals();
		//是否拥有向量场
		virtual bool HasNormals() const;

		//添加标量场
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//获取指定的标量场
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//获取指定名称的标量场对应的索引号
		int GetScalarFieldIndexByName(QString name);

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

		DcGpScalarFieldPtr GetCurrentScalarField();

		//是否拥有标量场
		virtual bool HasDisplayedScalarField() const;

		//获取顶点
		std::vector<PointCoord> GetVertexs();

		PointCoord GetVertex(unsigned index);

		PointCoord* GetVertex(unsigned index) const;

		//获取链接关系
		std::vector<std::array<unsigned, 4>> GetLinks();

		//获取边界盒
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//!\brief获取最大点
		PointCoord GetMaxPoint() const;
		//!\brief获取最小点
		PointCoord GetMinPoint() const;

		//添加点
		void AddPoint(DCVector3D vector);

		//添加点到指定的位置
		void AddPoint(DCVector3D vector, unsigned index);

		//为点容器分配空间
		void Resize(unsigned size);

		//点个数
		unsigned PSize() const;

		void InvalidBoundBox();

		//获取指定顶点的颜色
		PointColor GetPointColor(unsigned index) const;

		//添加一个链接关系
		void AddLink(std::array<unsigned, 4> link);
		std::array<unsigned, 4> GetLink(unsigned index);

		//link num
		unsigned LinkSize();

		//添加一个颜色值
		void AddColor(PointColor color, unsigned index);

		//初始化颜色空间
		bool ReserveTheRGBTable();

		//返回指定索引处颜色的引用
		PointColor& GetColor(const unsigned index);

		//设置关联的纹理图片
		void SetImage(QImage& image, const QString& tfwFile = "");

		void BindTargetTexture(QImage& image);

		//!\ 添加纹理坐标
		void AddTexCoord(DCVector2D texCoord);

		//!\ 纹理坐标
		bool HasTextures();

		void CancelTexture();

		//坐标转换
		void ApplyTransform(EigenMatrix4d& transMatrix);
	private:

		//绘制
		void DrawMyselfOnly(DcGpDrawContext& context);
		//根据网格顶点的索引生成序号
		unsigned GetNumberByIndex(Index index);
		//根据网格的序号生成索引
		Index GetIndexByNumber(unsigned number);

		//!\brief 更新context中sfColorScaleToDisplay变量值
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] 上下文绘制参数 */);
	protected:
		DcGpDEM();	
	private slots:

		void ChangeShowStyle(int style);
	};
}
#endif