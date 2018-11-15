/*########################################################################
								DcDs
					版权所属	北京鼎创力维科技有限公司
							2014年05月28日

						辅助Openggl窗口，实现辅助显示

########################################################################*/


#ifndef DCDS_RENDERING_TOOL_H
#define DCDS_RENDERING_TOOL_H

//C++
#include <list>

#include "DCGp/GpDrawContext.h"

namespace MPViewer
{
	//!\brief 结构体用来存在标签的放置位置信息
	struct Vlabel
	{
		int yPos; 		/**< label center pos **/
		int yMin; 		/**< label 'ROI' min **/
		int yMax; 		/**< label 'ROI' max **/
		double val; 	/**< label value **/

		//!\ 默认构造函数
		Vlabel(int y, int y1, int y2, double v) : yPos(y), yMin(y1), yMax(y2),val(v) { assert(y2>=y1); }
	};

	//!\brief 标签容器
	typedef std::list<Vlabel> VlabelSet;

	//!\brief DcDs命名空间

	//!\brief 辅助渲染工具
	class ViewRenderingTool
	{
	public:

		//!\brief 绘制颜色条
		static void DrawColorBar(const DcGp::DcGpDrawContext& context /*<[in] 上下文绘制参数*/);
	};
}

#endif
