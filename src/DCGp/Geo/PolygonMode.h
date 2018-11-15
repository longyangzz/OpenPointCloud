#ifndef MPGP_POLYGONMODE_H
#define MPGP_POLYGONMODE_H

#include "DCGp/DcGpDLL.h"

#include "StateAttribute.h"

namespace DCGp
{
	class State;
	class DCGP_API PolygonMode : public StateAttribute
	{
		Q_OBJECT
	public :
		enum Mode {
			POINT = GL_POINT,
			LINE = GL_LINE,
			FILL = GL_FILL
		};

		enum Face {
			FRONT_AND_BACK,
			FRONT,
			BACK
		};

		PolygonMode();
		virtual ~PolygonMode();

		PolygonMode(Face face,Mode mode);

		StateAttribute::Type GetType() const {return POLYGONMODE;}
		void setMode(Face face,Mode mode);
		Mode getMode(Face face) const;

		inline bool getFrontAndBack() const { return _modeFront==_modeBack; }

		virtual void Apply(State& state) const;

	protected:

		Mode _modeFront;
		Mode _modeBack;

	};
}

#endif // MPGP_POLYGONMODE_H
