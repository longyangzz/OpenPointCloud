#ifndef MPGP_LIGHT_H
#define MPGP_LIGHT_H

//DCCore
#include "DCCore/Vector.h"
#include "StateAttribute.h"

#include "DCGp/DcGpDLL.h"

#ifndef GL_LIGHT0
    #define GL_LIGHT0 0x4000
    #define GL_LIGHT1 0x4001
    #define GL_LIGHT2 0x4002
    #define GL_LIGHT3 0x4003
    #define GL_LIGHT4 0x4004
    #define GL_LIGHT5 0x4005
    #define GL_LIGHT6 0x4006
    #define GL_LIGHT7 0x4007
#endif

#ifndef GL_LIGHTING
    #define GL_LIGHTING 0x0B50
#endif

namespace DCGp
{

class DCGP_API Light : public StateAttribute
{
	Q_OBJECT
    public :

        Light();

        Light(unsigned int lightnum);

		virtual ~Light();

        virtual Type GetType() const { return LIGHT; }

		int GetMode()
		{
			return (GL_LIGHT0+_lightnum);
		}

        virtual unsigned int getMember() const { return _lightnum; }

        /** Set which OpenGL light to operate on. */
        void setLightNum(int num);
        
        /** Get which OpenGL light this osg::Light operates on. */
        int getLightNum() const { return _lightnum; }

        /** Set the ambient component of the light. */
        inline void setAmbient( const fvec4& ambient ) { _ambient = ambient; }

        /** Get the ambient component of the light. */
        inline const fvec4& getAmbient() const { return _ambient; }

        /** Set the diffuse component of the light. */
        inline void setDiffuse( const fvec4& diffuse ) { _diffuse = diffuse; }

        /** Get the diffuse component of the light. */
        inline const fvec4& getDiffuse() const { return _diffuse; }

        /** Set the specular component of the light. */
        inline void setSpecular( const fvec4& specular ) { _specular = specular; }

        /** Get the specular component of the light. */
        inline const fvec4& getSpecular() const { return _specular; }

        /** Set the position of the light. */    
        inline void setPosition( const fvec4& position ) { _position = position; }

        /** Get the position of the light. */    
        inline const fvec4& getPosition() const { return _position; }

        /** Set the direction of the light. */    
        inline void setDirection( const fvec3& direction ) { _direction = direction; }

        /** Get the direction of the light. */    
        inline const fvec3& getDirection() const { return _direction; }

        /** Set the constant attenuation of the light. */
        inline void setConstantAttenuation( float constant_attenuation )     { _constant_attenuation = constant_attenuation; }

        /** Get the constant attenuation of the light. */
        inline float getConstantAttenuation() const { return _constant_attenuation; }

        /** Set the linear attenuation of the light. */
        inline void setLinearAttenuation ( float linear_attenuation )        { _linear_attenuation = linear_attenuation; }

        /** Get the linear attenuation of the light. */
        inline float getLinearAttenuation () const { return _linear_attenuation; }

        /** Set the quadratic attenuation of the light. */
        inline void setQuadraticAttenuation ( float quadratic_attenuation )  { _quadratic_attenuation = quadratic_attenuation; }

        /** Get the quadratic attenuation of the light. */
        inline float getQuadraticAttenuation()  const { return _quadratic_attenuation; }

        /** Set the spot exponent of the light. */
        inline void setSpotExponent( float spot_exponent )                   { _spot_exponent = spot_exponent; }

        /** Get the spot exponent of the light. */
        inline float getSpotExponent() const { return _spot_exponent; }

        /** Set the spot cutoff of the light. */
        inline void setSpotCutoff( float spot_cutoff )                       { _spot_cutoff = spot_cutoff; }

        /** Get the spot cutoff of the light. */
        inline float getSpotCutoff() const                                   { return _spot_cutoff; }

        /** Capture the lighting settings of the current OpenGL state
          * and store them in this object.
        */
        void captureLightState();

        /** Apply the light's state to the OpenGL state machine. */
        virtual void Apply(State& state) const;

    protected :
        /** Initialize the light's settings with some decent defaults. */
        void init();

        int _lightnum;                           // OpenGL light number

        fvec4 _ambient;                           // r, g, b, w
        fvec4 _diffuse;                           // r, g, b, w
        fvec4 _specular;                          // r, g, b, w
        fvec4 _position;                          // x, y, z, w
        fvec3 _direction;                         // x, y, z
        float _constant_attenuation;             // constant
        float _linear_attenuation;               // linear
        float _quadratic_attenuation;            // quadratic
        float _spot_exponent;                    // exponent
        float _spot_cutoff;                      // spread
};

}

#endif
