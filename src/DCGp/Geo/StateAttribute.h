#ifndef MPGP_STATEATTRIBUTE_H
#define MPGP_STATEATTRIBUTE_H

#include "DCCore/DCGL.h"
#include <QObject>

namespace DCGp
{
	class State;
	class StateAttribute : public QObject
	{
		Q_OBJECT

	public:
		StateAttribute();
		virtual ~StateAttribute();

		/** GLMode is the value used in glEnable/glDisable(mode) */
		typedef GLenum          GLMode;
		/** GLModeValue is used to specify whether a mode is enabled (ON) or disabled (OFF).
			* GLMoveValue is also used to specify the override behavior of modes from parent to children. 
			* See enum Value description for more details.*/
		typedef unsigned int    GLModeValue;
		/** Override is used to specify the override behavior of StateAttributes
			* from parent to children. 
			* See enum Value description for more details.*/
		typedef unsigned int    OverrideValue;

		/** list values which can be used to set either GLModeValues or OverrideValues.
			* When using in conjunction with GLModeValues, all Values have meaning. 
			* When using in conjunction with StateAttribute OverrideValue only 
			* OFF,OVERRIDE and INHERIT are meaningful.
			* However, they are useful when using GLModeValue 
			* and OverrideValue in conjunction with each other as when using
			* StateSet::setAttributeAndModes(..).*/
		enum Values
		{
			/** means that associated GLMode and Override is disabled.*/
			OFF          = 0x0,
			/** means that associated GLMode is enabled and Override is disabled.*/
			ON           = 0x1,
			/** Overriding of GLMode's or StateAttributes is enabled, so that state below it is overridden.*/
			OVERRIDE     = 0x2,
			/** Protecting of GLMode's or StateAttributes is enabled, so that state from above cannot override this and below state.*/
			PROTECTED    = 0x4,            
			/** means that GLMode or StateAttribute should be inherited from above.*/
			INHERIT      = 0x8
		};
        
		/** Type identifier to differentiate between different state types. */
		// typedef unsigned int Type;

		/** Values of StateAttribute::Type used to aid identification
			* of different StateAttribute subclasses. Each subclass defines
			* its own value in the virtual Type getType() method.  When 
			* extending the osg's StateAttribute's simply define your
			* own Type value which is unique, using the StateAttribute::Type
			* enum as a guide of what values to use.  If your new subclass
			* needs to override a standard StateAttriubte then simply use 
			* that type's value. */
		enum Type
		{
			TEXTURE,
            
			POLYGONMODE,
			POLYGONOFFSET,
			MATERIAL,
			ALPHAFUNC,
			ANTIALIAS,
			COLORTABLE,
			CULLFACE,
			FOG,
			FRONTFACE,
            
			LIGHT,

			POINTSIZE,
			LINEWIDTH,
			LINESTIPPLE,
			POLYGONSTIPPLE,
			SHADEMODEL,
			TEXENV,
			TEXENVFILTER,
			TEXGEN,
			TEXMAT,
			LIGHTMODEL,
			BLENDFUNC,
			BLENDEQUATION,
			LOGICOP,
			STENCIL,
			COLORMASK,
			DEPTH,
			VIEWPORT,
			SCISSOR,
			BLENDCOLOR,
			MULTISAMPLE,
			CLIPPLANE,
			COLORMATRIX,
			VERTEXPROGRAM,
			FRAGMENTPROGRAM,
			POINTSPRITE,
			PROGRAM,
			CLAMPCOLOR,
			HINT,
		};

		typedef std::pair<Type,unsigned int> TypeMemberPair;

		virtual void Apply(State&) const {}

		virtual Type GetType() const = 0;

		virtual unsigned int GetMember() const { return 0; }

		inline TypeMemberPair GetTypeMemberPair() const { return TypeMemberPair(GetType(),GetMember()); }
	private:
	
	};
}

#endif // MPGP_STATEATTRIBUTE_H
