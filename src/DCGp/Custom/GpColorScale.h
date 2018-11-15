#ifndef DCGP_COLOR_SCALE_H
#define DCGP_COLOR_SCALE_H

#include "DCGp/DcGpDLL.h"

//C++标准库
#include <assert.h>
#include <array>
#include <map>

//Qt标准库
#include "QList"
class QString;
class QColor;

#include "DCCore/DCDefs.h"

//Local
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{

//颜色刻度尺的元素（位置和颜色）
	DECLARE_SMARTPTR(DcGpColorScaleElement)
	DECLARE_SMARTPTR(DcGpColorScaleElementImpl)

class DCGP_API DcGpColorScaleElement
{
	DECLARE_PHONY_CONSTRUCTOR(DcGpColorScaleElement)
	DECLARE_IMPLEMENT(DcGpColorScaleElement)

public:
	//根据位置设置颜色
	DcGpColorScaleElement(double position, QColor color);

	//关联位置
	inline double Position() const;
	inline double& Position();

	//获取颜色
	inline QColor Color() const;
	inline QColor& Color();

	//比较两个颜色元素
	inline static bool IsSmaller(const DcGpColorScaleElement& e1, const DcGpColorScaleElement& e2);

protected:
	//默认构造函数
	DcGpColorScaleElement();
};

//颜色刻度尺
//根据指定的几种颜色和步长生成颜色刻度尺(颜色渐变)
//步长不小于给定的颜色数目
DECLARE_SMARTPTR(DcGpColorScale)
DECLARE_SMARTPTR(DcGpColorScaleImpl)

class DCGP_API DcGpColorScale
{
	DECLARE_PHONY_CONSTRUCTOR(DcGpColorScale)

	DECLARE_IMPLEMENT(DcGpColorScale)

public:
	//默认构造函数
	//参数一：刻度尺的名字
	//参数二：Unique ID
	DcGpColorScale(QString name);

	//使用颜色元素进行构造
	DcGpColorScale(QString name, QList<DcGpColorScaleElement> elements);

	//析构函数
	virtual ~DcGpColorScale();

	//获取默认的原色表
	static std::map<QString, DcGpColorScalePtr> DefualtColorScales();

	//获取名字
	QString GetName() const;

	//设置名字
	void SetName(const QString& name);

	//返回唯一标识
	QString GetUniqueID() const;

	//设置唯一标记
	void SetUniqueID(unsigned uniqueID);
	inline bool IsRelative() const;

	inline void SetRelative();

	void SetAbsolute(double minVal, double maxVal);

	void GetAbsoluteBoundaries(double& minVal, double& maxVal) const;

	//刻度尺是否锁定，锁定则不可更改
	inline bool IsLocked() const;

	//设置刻度尺是否锁定
	inline void SetLocked(bool state);

	//返回当前的步长
	inline int Count() const;

	//获取指定位置的元素
	inline DcGpColorScaleElement& Element(int index);

	//获取指定位置的元素(const)
	inline const DcGpColorScaleElement& Element(int index) const;

	//添加元素，刻度尺可操作状态下
	void Insert(const DcGpColorScaleElement& step, bool autoUpdate = true);

	//删除指定的元素
	void Remove(int index, bool autoUpdate = true);

	//清除所有元素
	void Clear();

	//更新
	void Update();

	inline double GetRelativePosition(double value) const;

	inline const RGBAColor GetColorByValue(double value, const RGBAColor outOfRangeColor) const;

	inline const RGBAColor GetColorByRelativePos(double relativePos) const;

	inline const RGBAColor GetColorByRelativePos(double relativePos, unsigned steps, const RGBAColor outOfRangeColor) const;

	//通过索引获取颜色
	inline const RGBAColor GetColorByIndex(unsigned index) const;

protected:
	DcGpColorScale();
	//对元素进行排序
	void Sort();
};

}
#endif //CC_COLOR_SCALE_HEADER
