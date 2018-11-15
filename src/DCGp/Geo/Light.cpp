#include "Light.h"

using namespace DCGp;

Light::Light( void )
{
    init();
}

Light::Light(unsigned int lightnum)
{
    init();
    _lightnum = lightnum;
}

Light::~Light( void )
{
}


void Light::init( void )
{
    _lightnum = 0;
    _ambient.set(0.05f,0.05f,0.05f,1.0f);
    _diffuse.set(0.8f,0.8f,0.8f,1.0f);
    _specular.set(0.05f,0.05f,0.05f,1.0f);
    _position.set(0.0f,0.0f,1.0f,0.0f);
    _direction.Set(0.0f,0.0f,-1.0f);
    _spot_exponent = 0.0f;
    _spot_cutoff = 180.0f;
    _constant_attenuation = 1.0f;
    _linear_attenuation = 0.0f;
    _quadratic_attenuation = 0.0f;

    //     OSG_DEBUG << "_ambient "<<_ambient<<std::endl;
    //     OSG_DEBUG << "_diffuse "<<_diffuse<<std::endl;
    //     OSG_DEBUG << "_specular "<<_specular<<std::endl;
    //     OSG_DEBUG << "_position "<<_position<<std::endl;
    //     OSG_DEBUG << "_direction "<<_direction<<std::endl;
    //     OSG_DEBUG << "_spot_exponent "<<_spot_exponent<<std::endl;
    //     OSG_DEBUG << "_spot_cutoff "<<_spot_cutoff<<std::endl;
    //     OSG_DEBUG << "_constant_attenuation "<<_constant_attenuation<<std::endl;
    //     OSG_DEBUG << "_linear_attenuation "<<_linear_attenuation<<std::endl;
    //     OSG_DEBUG << "_quadratic_attenuation "<<_quadratic_attenuation<<std::endl;
}

void Light::setLightNum(int num) 
{
    if (_lightnum==num) return;

    _lightnum = num;
}

void Light::captureLightState()
{
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_AMBIENT, _ambient.ptr() );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_DIFFUSE, _diffuse.ptr() );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPECULAR, _specular.ptr() );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_POSITION, _position.ptr() );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_DIRECTION, &_direction[0] );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_EXPONENT, &_spot_exponent );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_CUTOFF,   &_spot_cutoff );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_CONSTANT_ATTENUATION,   &_constant_attenuation );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_LINEAR_ATTENUATION,   &_linear_attenuation );
    glGetLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_QUADRATIC_ATTENUATION,   &_quadratic_attenuation );
}

void Light::Apply(State&) const
{
    glLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_AMBIENT,               _ambient.ptr() );
    glLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_DIFFUSE,               _diffuse.ptr() );
    glLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPECULAR,              _specular.ptr() );
    glLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_POSITION,              _position.ptr() );
    glLightfv( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_DIRECTION,        &_direction[0] );
    glLightf ( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_EXPONENT,         _spot_exponent );
    glLightf ( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_SPOT_CUTOFF,           _spot_cutoff );
    glLightf ( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_CONSTANT_ATTENUATION,  _constant_attenuation );
    glLightf ( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_LINEAR_ATTENUATION,    _linear_attenuation );
    glLightf ( (GLenum)((int)GL_LIGHT0 + _lightnum), GL_QUADRATIC_ATTENUATION, _quadratic_attenuation );
}
