#ifndef DCGP_SCALAR_FIELD_H
#define DCGP_SCALAR_FIELD_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>
#include <map>
#include <set>

//Qt
class QString;

#include "DCCore/DCDefs.h"

//
#include "DCGp/GpBasicTypes.h"
#include "GpColorScale.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpScalarFieldImpl)
	//标量场
	class DCGP_API DcGpScalarField
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpScalarField)
		DECLARE_IMPLEMENT(DcGpScalarField)

	public:

		class ScalarRange
		{
		public:
			//构造函数
			ScalarRange()
				: m_min(0)
				, m_max(0)
			{}

			inline ScalarType Range() const {return m_max - m_min;}

		private:
			//最大值
			ScalarType m_max;
			//最小值
			ScalarType m_min;
		};

		DcGpScalarField(QString name);


		//通过num值初始化m_data的大小
		DcGpScalarField(QString name, unsigned num);
		virtual ~DcGpScalarField();

		//添加数据
		void AddData(ScalarType data);

		//添加数据到指定的索引位置
		void AddData(unsigned index, ScalarType data);
		//传入数据
		/*void SetScalarData(SharedData data);*/

		//设置颜色表
		inline void SetColorScale(DcGpColorScalePtr colorScale);

		//获取颜色表
		DcGpColorScalePtr GetColorScale();

		//获取颜色表(const)
		DcGpColorScalePtr GetColorScale() const;

		//获取颜色
		std::vector<RGBColor> GetColors() const;

		//对数据进行预处理，算出其数值范围
		void Prepare();

		//改变颜色表
		void ChangeColorScale(QString name);
		//设置m_data的大小
		void SetScaleFieldSize(long num);

		//!\brief 获取显示范围
		//!\retval 显示范围
	    std::set<double> GetScaleRange() const;

		//!\brief 获取指定位置的颜色值
		const RGBAColor GetColor(ScalarType value);

		//!\brief 获取指定索引号的颜色值
		RGBColor GetColor(unsigned index);

		//!\brief 获取当前标量的名字
		//!\retval 名字
		QString GetName() const;

		void ComputeMinAndMax();

		//!\brief 计算均值和方差
		void ComputeMeanAndVariance(ScalarType &mean, ScalarType* variance=0) const;

		ScalarType GetMin();

		ScalarType GetMax();

		double GetPointScalarValue(unsigned index);

		unsigned Size();

		//***********************为las数据处理添加的方法**************************************************
		void SetDatas(const unsigned& size, const int& value);
		void SetDatas(std::vector<ScalarType> values);
		std::vector<ScalarType>& GetDatas();

		//设置更新颜色数
		void SetColorSteps(unsigned steps);
	protected:
		DcGpScalarField();

		//!\brief 显示范围
		ScalarRange m_displayRange;
	};
}
#endif