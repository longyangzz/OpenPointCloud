#include "SplitFactory.h"

#include "AsciiSpliter.h"
#include "LasSpliter.h"

using namespace DC::Pyra;

SplitFactory::SplitFactory(const QString& suffix)
{
	m_suffix = suffix;
}

SplitFactory::~SplitFactory()
{

}

SPliter* SplitFactory::CreateSpliter()
{
	if (m_suffix.toUpper() == "TXT")
	{
		return new AsciiSpliter(m_suffix);
	}
	else if (m_suffix.toUpper() == "LAS")
	{
		return new LasSpliter(m_suffix);
	}
	return nullptr;
}
