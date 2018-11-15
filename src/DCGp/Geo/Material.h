#ifndef MPGP_MATERIAL_H
#define MPGP_MATERIAL_H

//DCCore
#include "DCCore/Vector.h"
#include "StateAttribute.h"

#include "DCGp/DcGpDLL.h"

#ifndef DC_GL_FIXED_FUNCTION_AVAILABLE
    #define GL_AMBIENT                  0x1200
    #define GL_DIFFUSE                  0x1201
    #define GL_SPECULAR                 0x1202
    #define GL_EMISSION                 0x1600
    #define GL_AMBIENT_AND_DIFFUSE      0x1602
    #define GL_COLOR_MATERIAL           0x0B57
#endif

namespace DCGp 
{
/*OpenGL glMaterial state.*/
class DCGP_API Material : public StateAttribute
{
	Q_OBJECT
    public :

        Material();

		virtual ~Material();
       
		Material& operator = (const Material& rhs);

		StateAttribute::Type GetType() const {return MATERIAL;}

        virtual void Apply(State& state) const;

        enum Face 
		{
            FRONT           = GL_FRONT,
            BACK            = GL_BACK,
            FRONT_AND_BACK  = GL_FRONT_AND_BACK
        };

        enum ColorMode 
		{
            AMBIENT = GL_AMBIENT,
            DIFFUSE = GL_DIFFUSE,
            SPECULAR = GL_SPECULAR,
            EMISSION = GL_EMISSION,
            AMBIENT_AND_DIFFUSE = GL_AMBIENT_AND_DIFFUSE,
            OFF            
        };

        inline void SetColorMode(ColorMode mode) { _colorMode = mode; }
        inline ColorMode GetColorMode() const { return _colorMode; }

        void SetAmbient( Face face, const fvec4& ambient );
        const fvec4& GetAmbient(Face face) const;
        inline bool GetAmbientFrontAndBack() const { return _ambientFrontAndBack; }

        void SetDiffuse( Face face, const fvec4& diffuse );
        const fvec4& GetDiffuse(Face face) const;
        inline bool GetDiffuseFrontAndBack() const { return _diffuseFrontAndBack; }

        /** Set specular value of specified face(s) of the material, 
          * valid specular[0..3] range is 0.0 to 1.0.
        */
        void SetSpecular( Face face, const fvec4& specular );
        
        /** Get the specular value for specified face. */
        const fvec4& GetSpecular(Face face) const;
        
        /** Return whether specular values are equal for front and back faces
          * or not.
        */
        inline bool GetSpecularFrontAndBack() const { return _specularFrontAndBack; }

        /** Set emission value of specified face(s) of the material, 
          * valid emission[0..3] range is 0.0 to 1.0.
        */
        void SetEmission( Face face, const fvec4& emission );
        
        /** Get the emission value for specified face. */
        const fvec4& GetEmission(Face face) const;
        
        /** Return whether emission values are equal for front and back faces
          * or not.
        */
        inline bool GetEmissionFrontAndBack() const { return _emissionFrontAndBack; }

        /** Set shininess of specified face(s) of the material.
          * valid shininess range is 0.0 to 128.0.
        */
        void SetShininess(Face face, float shininess );
        
        /** Get the shininess value for specified face. */
        float GetShininess(Face face) const;
        
        /** Return whether shininess values are equal for front and back faces
          * or not.
        */
        inline bool GetShininessFrontAndBack() const { return _shininessFrontAndBack; }
        
        /** Set the alpha value of ambient, diffuse, specular and emission
          * colors of specified face, to 1-transparency.
          * Valid transparency range is 0.0 to 1.0.
        */
        void SetTransparency(Face face,float trans);

        /** Set the alpha value of ambient, diffuse, specular and emission
          * colors. Valid transparency range is 0.0 to 1.0.
        */
        void SetAlpha(Face face,float alpha);

    protected :

        ColorMode _colorMode;

        bool _ambientFrontAndBack;
        fvec4 _ambientFront;                          // r, g, b, w
        fvec4 _ambientBack;                           // r, g, b, w

        bool _diffuseFrontAndBack;
        fvec4 _diffuseFront;                          // r, g, b, w
        fvec4 _diffuseBack;                           // r, g, b, w

        bool _specularFrontAndBack;
        fvec4 _specularFront;                         // r, g, b, w
        fvec4 _specularBack;                          // r, g, b, w

        bool _emissionFrontAndBack;
        fvec4 _emissionFront;                         // r, g, b, w
        fvec4 _emissionBack;                          // r, g, b, w

        bool   _shininessFrontAndBack;
        float  _shininessFront;                      // values 0 - 128.0
        float  _shininessBack;                       // values 0 - 128.0

};

}

#endif
