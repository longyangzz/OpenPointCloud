#ifndef DCCORE_IOINTERFACE_H
#define DCCORE_IOINTERFACE_H

//qt
#include "QString"
#include "QDir"

//io
#include "IoPlugin.h"

#include "DCCoreDLL.h"

namespace DCCore
{
	
	DCCORE_EXPORT QString GetIOPluginPath();

	DCCORE_EXPORT bool GetIOPlugin(QDir dir, QString extension, IoPlugin*& io);
}

#endif // DCCORE_IOINTERFACE_H
