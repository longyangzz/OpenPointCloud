#include "Impl_Parameters.hxx"
using namespace DCView;

//构造函数
GlobalParameters::GlobalParametersImpl
	::GlobalParametersImpl(GlobalParameters* pInterface)
	: m_pInterface(pInterface)
{
	//创建默认操作组合键
	CompositeKey key;
	{
		//平移
		key.buttons = Qt::MidButton;
		m_compositeKeys.insert(eTranslateOperation, key);

		//旋转
		key.buttons = Qt::LeftButton;
		m_compositeKeys.insert(eRotateOperation, key);

		//区域缩放
		key.buttons = Qt::MidButton;
		key.keys = Qt::ShiftModifier;
		m_compositeKeys.insert(eZoomOnRegion, key);
	}

	//创建默认背景
	m_background.role = eBckFill;
	m_background.color = Qt::black;
}

//设置组合键
void GlobalParameters::GlobalParametersImpl
	::SetCompositeKey(OperationFlag flag, CompositeKey keys)
{
	m_compositeKeys[flag] = keys;
}

//获取组合键
CompositeKey GlobalParameters::GlobalParametersImpl
	::GetCompositeKey(OperationFlag flag) const
{
	return m_compositeKeys[flag];
}

//设置背景色
void GlobalParameters::GlobalParametersImpl
	::SetBackground(const QColor& rBckFill)
{
	m_background.role = eBckFill;
	m_background.color = rBckFill;
}
void GlobalParameters::GlobalParametersImpl
	::SetBackground(const QLinearGradient& rBckGradient)
{
	m_background.role = eBckGradient;
	m_background.gradient = rBckGradient;
}

//获取背景规则
BackgroundRole GlobalParameters::GlobalParametersImpl
	::GetBackgroundRole() const
{
	return m_background.role;
}
//获取背景
QBrush GlobalParameters::GlobalParametersImpl
	::GetBackground() const
{
	if (m_background.role == eBckFill)
	{
		return QBrush(m_background.color);
	}
	else 
	{
		return QBrush(m_background.gradient);
	}
}