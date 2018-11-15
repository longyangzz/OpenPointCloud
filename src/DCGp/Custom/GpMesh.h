#ifndef DCGP_MESH_H
#define DCGP_MESH_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>
#include <map>

//Qt
class QString;

//Common
#include "DCCore/Const.h"


#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "GpScalarField.h"
#include "DCGp/GpDrawContext.h"

#include "DCGp/GpBasicTypes.h"

class QImage;

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpMesh)
	DECLARE_SMARTPTR(DcGpMeshImpl)

	class DCGP_API DcGpMesh : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpMesh)
		DECLARE_IMPLEMENT(DcGpMesh)
		GET_CLASSNAME(DcGpMesh)

	public:
		struct FilterPara
		{
			float maxEdge;
			double maxAngle;
			double minAngel;

			FilterPara(float _maxEdge = 5.0, double _maxAngle = 90.0, double _minAngle = 5.0)
			{
				maxEdge = _maxEdge;
				maxAngle = _maxAngle;
				minAngel = _minAngle;
			}
		};

		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpMesh("Mesh");
		}

		//构造函数
		DcGpMesh(QString name); //传入顶点数据对象指针
		DcGpMesh(QString name, std::vector<DCVector3D> vertices); //传入顶点数据对象指针
		virtual ~DcGpMesh();

		//成员函数
		void AddPoint(DCVector3D point);
		void AddNormal(DCVector3D normal);

		//! 添加顶点颜色
		void AddVexColor(PointColor color);
		void AddTriangle(unsigned index1, unsigned index2, unsigned index3); //传入三角形的3个点索引号

		//绘制mesh
		virtual void DrawMyselfOnly(DcGpDrawContext& context);
		
		virtual DcGpBoundBox GetDisplayBoundBox();
		//获取边界盒
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//获取所有顶点
		std::vector<PointCoord> GetVertexs();
		LinkContainer GetLinks();

		//设置链接关系
		void SetLinks(LinkContainer links);
		//设置顶点数据
		void SetPoints(std::vector<DCVector3D> points);
		//设置拓扑关系
		void SetTopologys(std::vector<std::array<long, 3>> topologys);

		//获取指定点
		DCVector3D GetPoint(unsigned index) const;
		//获取法向量值
		virtual	std::vector<Normal> GetNormals();
		//设置法向量
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//是否拥有向量场
		virtual bool HasNormals() const;

		//设置当前标量场
		void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField);
		//更改当前标量场的颜色表
		virtual void ChangeScalarFieldColorScale(QString name);
		//是否拥有标量场
		virtual bool HasScalarFields() const;
		//显示标量场
		virtual void ShowScalarField(bool state);

		//标量场的数量
		virtual unsigned ScalarFieldCount() const;

		//标量场的名字
		virtual QStringList ScalarFieldNames() const;

		//显示属性信息
		void ShowProperties(QTableWidget* widget);

		//!\brief 计算德罗尼三角网
		//bool BuildDelaunay(std::vector<DCVector2D> &point2ds);

		//!\brief 添加拓扑关系
		void AddTopology(long neighbor1, long neighbor2, long neighbor3);

		//!\brief 获取拓扑关系
		std::vector<std::array<long, 3>> GetTopologys();

		//!\brief 从文件中加载，传入是否添加偏移量以及偏移量值
		virtual bool LoadFromFile(const QString& fileName /*!< [in] 文件名*/,
								  bool coordinatesShiftEnabled  /*!< [in] 是否添加偏移量*/,
			                      double* coordinatesShift = 0  /*!< [in] 偏移量数组指针*/);

		//!\brief 加载txt格式
		bool LoadWrl(const QString& fileName /*!< [in] 文件名*/,
			              bool coordinatesShiftEnabled = false /*!< [in] 是否添加偏移量*/,
			              double* coordinatesShift = 0 /*!< [in] 偏移量数组指针*/);

		//!\brief获取最大点
		PointCoord GetMaxPoint() const;
		//!\brief获取最小点
		PointCoord GetMinPoint() const;

	    void SetMaxPoint(PointCoord pt);
		//!\brief设置最小点
		void SetMinPoint(PointCoord pt);

		//! Sets a particular point compressed normal
		/** WARNING: normals must be enabled.
		**/
		void SetPointNormalIndex(unsigned pointIndex, NormalType norm);

		//! Sets a particular point normal (shortcut)
		/** WARNING: normals must be enabled.
			Normal is automatically compressed before storage.
		**/
		void SetPointNormal(unsigned pointIndex, const DCVector3D& N);

		LinkContainer& GetLinks() const;

		void GetVertexs(std::vector<DCVector3D>& points);

		//获取指定点的选中状态
		//bool GetPointSelected(unsigned index) const;

		//设置指定点的选中状态
		//void SetPointSelected(unsigned index, bool status);

		//启用选取颜色
		void EnableSelectedColors();

		//停用选取颜色
		void DisableSelectedColors();

		//点size
		unsigned PSize();

		//取消选取
		void CancelSelected();

		//删除所选的部分
		void DeleteSelected();

		Link GetLink(unsigned index);

		void Filter(FilterPara para);

		DCVector3D& GetPoint(unsigned index);

		void AddTriangle(Link tri); //传入索引号数组

		//!\brief 根据指定的索引号获取拓扑关系
		std::array<long, 3> GetTopology(unsigned index);

		unsigned LinkSize();

		bool HasSelected();

		//获取指定mesh的选中状态
		bool GetMeshSelected(unsigned index) const;

		//设置指定mesh的选中状态
		void SetMeshSelected(unsigned index, bool status);

		//获取选中的三角网,返回网格号
		std::vector<unsigned> GetSelectMeshs();

		bool SaveToFile(QFile& file) const;

		//添加标量场
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//获取指定的标量场
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//获取指定顶点的颜色
		PointColor GetPointColor(unsigned index) const;

		//获取指定顶点的法向量
		DCVector3D GetPointNormal(unsigned index) const;

		//根据关联的纹理图片读取图片、绑定图片
		void MapTexBySetTextImgFile(QString& fileName, const QString& tfwFile = "", const QString& suffix = "jpg");

		void LoadGLTextureByQImage(QImage& image, const QString& suffix = "jpg");

		//!\ 添加纹理坐标
		void AddTexCoord(DCVector2D texCoord);

		//!\ 添加纹理坐标
		bool HasTextures();

		void CancelTexture();

		ShowStyle GetShowStyle();

		void SetShowStyle(ShowStyle showStyle);

		//坐标转换
		void ApplyTransform(EigenMatrix4d& transMatrix);

		//传入关联的纹理文件名,用来在读取模型文件的时候，将文件中存储的纹理路径保存起来
		void SetTexImgFile(const QString& texFileName);

		void SetMaterial(DcGp::DcGpMaterial* mat);
	private:
		//!\brief 更新context中sfColorScaleToDisplay变量值
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] 上下文绘制参数 */);


	protected:
		DcGpMesh();

	private slots:
		void ChangeShowStyle(int style);
	};
}

#endif // DCMESH_H
