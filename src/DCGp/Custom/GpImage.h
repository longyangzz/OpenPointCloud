#ifndef GPIMAGE_H
#define GPIMAGE_H

#include "DCGp/DcGpDLL.h"

#include "DCCore/DCDefs.h"

#include "DCGp/GpEntity.h"

class QImage;
namespace DcGp
{
	DECLARE_SMARTPTR(DcGpImage)
	DECLARE_SMARTPTR(DcGpImageImpl)

	class DCGP_API DcGpImage : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpImage)
		DECLARE_IMPLEMENT(DcGpImage)
		GET_CLASSNAME(DcGpImage)

	public:

		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpImage("Image");
		}
		DcGpImage();
		DcGpImage(QString name);
		~DcGpImage();

		//!\brief 绘制图形
		virtual void DrawMyselfOnly(DcGpDrawContext& context);

		//!\brief 从文件中加载，传入是否添加偏移量以及偏移量值
		virtual bool LoadFromFile(const QString& fileName,
			                      bool coordinatesShiftEnabled,
			                      double* coordinatesShift = 0);

	protected:
		void SetTarget(QImage& image);
	private:

	};
}

#endif // GPIMAGE_H
