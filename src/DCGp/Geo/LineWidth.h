#ifndef MPGP_LINEWIDTH_H
#define MPGP_LINEWIDTH_H

#include "DCGp/DcGpDLL.h"

#include "StateAttribute.h"

namespace DCGp
{
	class State;
	class DCGP_API LineWidth : public StateAttribute
	{
		Q_OBJECT
	public :
		LineWidth();
		LineWidth(float width);
		virtual ~LineWidth();

		StateAttribute::Type GetType() const {return LINEWIDTH;}

		virtual void Apply(State& state) const;

		void SetWidth(float width);
		inline float GetWidth() const {return m_width; }
	protected:
		float m_width;
	};
}

#endif // MPGP_LINEWIDTH_H
