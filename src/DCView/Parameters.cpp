#include "DCView/Parameters.h"
using namespace DCView;

//Qt
#include "QtCore/QCoreApplication"

//Impl
#include "Impl/Impl_Parameters.hxx"

//静态实体
GlobalParameters GlobalParameters::s_globalParams;

//构造函数
GlobalParameters::GlobalParameters()
{
	m_pImpl = new GlobalParametersImpl(this);
}

GlobalParameters::~GlobalParameters()
{
	delete m_pImpl;
	m_pImpl = nullptr;
}

//设置组合键
void GlobalParameters::SetCompositeKey(OperationFlag flag, 
	Qt::MouseButtons buttons, 
	Qt::KeyboardModifiers keys)
{
	CompositeKey key;
	key.buttons = buttons;
	key.keys = keys;

	s_globalParams.m_pImpl->SetCompositeKey(flag, key);
}

//获取组合键
CompositeKey GlobalParameters::GetCompositeKey(OperationFlag flag)
{
	return s_globalParams.m_pImpl->GetCompositeKey(flag);
}

//设置背景色
void GlobalParameters::SetBackground(const QColor& rBckFill)
{
	s_globalParams.m_pImpl->SetBackground(rBckFill);
}
void GlobalParameters::SetBackground(const QLinearGradient& rBckGradient)
{
	s_globalParams.m_pImpl->SetBackground(rBckGradient);
}

//获取背景规则
GlobalParameters::BackgroundRole GlobalParameters::GetBackgroundRole()
{
	return s_globalParams.m_pImpl->GetBackgroundRole();
}
//获取背景
QBrush GlobalParameters::GetBackground()
{
	return s_globalParams.m_pImpl->GetBackground();
}

//Shaders路径
QString GlobalParameters::ShadersPath()
{
	return QCoreApplication::applicationDirPath() + "/Support/Shaders";
}

unsigned char g_bbColor[3] = {255,0,0};
float g_difuseLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float g_specularLight[4] = {0.17f, 0.17f, 0.17f, 1.0f};
//float g_ambientLight[4] = {0.079f, 0.079f, 0.079f, 1.0f};
float g_ambientLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float* GlobalParameters::DiffuseLight()
{
	return g_difuseLight;
}
float* GlobalParameters::AmbientLight()
{
	return g_ambientLight;
}

void GlobalParameters::SetAmbientLight(const QColor& ambient)
{
	g_ambientLight[0] = ambient.red() / 255.0;
	g_ambientLight[1] = ambient.green() / 255.0;
	g_ambientLight[2] = ambient.blue()  / 255.0;
	g_ambientLight[3] = ambient.alpha()  / 255.0;
}

float* GlobalParameters::SpecularLight()
{
	return g_specularLight;
}

unsigned char* GlobalParameters::BoundBoxColor()
{
	return g_bbColor;
}