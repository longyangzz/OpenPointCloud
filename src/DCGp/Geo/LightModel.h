#ifndef MPGP_LIGHTMODEL_H
#define MPGP_LIGHTMODEL_H

//DCCore
#include "DCCore/Vector.h"
#include "StateAttribute.h"

#include "DCGp/DcGpDLL.h"

namespace DCGp 
{

class DCGP_API LightModel : public StateAttribute
{
	Q_OBJECT
    public :

        LightModel();

		virtual ~LightModel();
		StateAttribute::Type GetType() const {return LIGHTMODEL;}

        void setAmbientIntensity(const fvec4& ambient) { _ambient = ambient; }
        const fvec4& getAmbientIntensity() const { return _ambient; }


        enum ColorControl
        {
            SEPARATE_SPECULAR_COLOR,
            SINGLE_COLOR
        };
        
        void setColorControl(ColorControl cc) { _colorControl = cc; }
        inline ColorControl getColorControl() const { return _colorControl; }


        void setLocalViewer(bool localViewer) { _localViewer=localViewer; }
        inline bool getLocalViewer() const { return _localViewer; }


        void setTwoSided(bool twoSided) { _twoSided = twoSided; }
        inline bool getTwoSided() const { return _twoSided; }



        virtual void Apply(State& state) const;


    protected :
        fvec4 _ambient;
        ColorControl _colorControl;
        bool _localViewer;
        bool _twoSided;

};

}

#endif
