#ifndef MPGP_POINTSIZE_H
#define MPGP_POINTSIZE_H

#include "DCGp/DcGpDLL.h"

#include "StateAttribute.h"

namespace DCGp
{
	class State;
	class DCGP_API PointSize : public StateAttribute
	{
		Q_OBJECT
	public :
		PointSize();
		PointSize(float size);
		virtual ~PointSize();

		StateAttribute::Type GetType() const {return POINTSIZE;}

		virtual void Apply(State& state) const;


		void SetSize(float size);
		inline float GetSize() const {return m_size; }
	protected:
		float m_size;
	};
}

#endif // MPGP_POINTSIZE_H
