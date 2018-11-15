#include "Material.h"

//Qt
#include "QDebug"

using namespace DCGp;

Material::Material()
{
    _colorMode = OFF;

    _ambientFrontAndBack = true;
    _ambientFront.set(0.2f, 0.2f, 0.2f, 1.0f);
    _ambientBack.set(0.2f, 0.2f, 0.2f, 1.0f);

    _diffuseFrontAndBack = true;
    _diffuseFront.set(0.8f, 0.8f, 0.8f, 1.0f);
    _diffuseBack.set(0.8f, 0.8f, 0.8f, 1.0f);

    _specularFrontAndBack = true;
    _specularFront.set(0.0f, 0.0f, 0.0f, 1.0f);
    _specularBack.set(0.0f, 0.0f, 0.0f, 1.0f);

    _emissionFrontAndBack = true;
    _emissionFront.set(0.0f, 0.0f, 0.0f, 1.0f);
    _emissionBack.set(0.0f, 0.0f, 0.0f, 1.0f);

    _shininessFrontAndBack = true;
    _shininessFront = 0.0f;
    _shininessBack = 0.0f;
}


Material::~Material()
{
}

Material& Material:: operator = (const Material& rhs)
{
    if (&rhs==this) return *this;
    
    _colorMode= rhs._colorMode;
    _ambientFrontAndBack= rhs._ambientFrontAndBack;
    _ambientFront= rhs._ambientFront;
    _ambientBack= rhs._ambientBack;
    _diffuseFrontAndBack= rhs._diffuseFrontAndBack;
    _diffuseFront= rhs._diffuseFront;
    _diffuseBack= rhs._diffuseBack;
    _specularFrontAndBack= rhs._specularFrontAndBack;
    _specularFront= rhs._specularFront;
    _specularBack= rhs._specularBack;
    _emissionFrontAndBack= rhs._emissionFrontAndBack;
    _emissionFront= rhs._emissionFront;
    _emissionBack= rhs._emissionBack;
    _shininessFrontAndBack= rhs._shininessFrontAndBack;
    _shininessFront= rhs._shininessFront;
    _shininessBack= rhs._shininessBack;
    
    return *this;
}


void Material::SetAmbient(Face face, const fvec4& ambient )
{
    switch(face)
    {
        case(FRONT):
            _ambientFrontAndBack = false;
            _ambientFront = ambient;
            //clampArray4BetweenRange(_ambientFront,0.0f,1.0f,"osg::Material::setAmbient(..)");
            break;
        case(BACK):
            _ambientFrontAndBack = false;
            _ambientBack = ambient;
            //clampArray4BetweenRange(_ambientBack,0.0f,1.0f,"Material::setAmbient(..)");
            break;
        case(FRONT_AND_BACK):
            _ambientFrontAndBack = true;
            _ambientFront = ambient;
            //clampArray4BetweenRange(_ambientFront,0.0f,1.0f,"Material::setAmbient(..)");
            _ambientBack = _ambientFront;
            break;
        default:
            qDebug() <<"Notice: invalid Face passed to Material::setAmbient()."<<endl;
    }
}


const fvec4& Material::GetAmbient(Face face) const
{
    switch(face)
    {
        case(FRONT):
            return _ambientFront;
        case(BACK):
            return _ambientBack;
        case(FRONT_AND_BACK):
            if (!_ambientFrontAndBack)
            {
                qDebug()<<"Notice: Material::getAmbient(FRONT_AND_BACK) called on material "<< endl;
                qDebug()<<"        with separate FRONT and BACK ambient colors."<< endl;
            }
            return _ambientFront;
    }
    qDebug() <<"Notice: invalid Face passed to Material::getAmbient()."<< endl;
    return _ambientFront;
}


void Material::SetDiffuse(Face face, const fvec4& diffuse )
{
    switch(face)
    {
        case(FRONT):
            _diffuseFrontAndBack = false;
            _diffuseFront = diffuse;
            //clampArray4BetweenRange(_diffuseFront,0.0f,1.0f,"Material::setDiffuse(..)");
            break;
        case(BACK):
            _diffuseFrontAndBack = false;
            _diffuseBack = diffuse;
            //clampArray4BetweenRange(_diffuseBack,0.0f,1.0f,"Material::setDiffuse(..)");
            break;
        case(FRONT_AND_BACK):
            _diffuseFrontAndBack = true;
            _diffuseFront = diffuse;
            //clampArray4BetweenRange(_diffuseFront,0.0f,1.0f,"Material::setDiffuse(..)");
            _diffuseBack = _diffuseFront;
            break;
        default:
            qDebug() <<"Notice: invalid Face passed to Material::setDiffuse()."<< endl;
            break;
    }
}


const fvec4& Material::GetDiffuse(Face face) const
{
    switch(face)
    {
        case(FRONT):
            return _diffuseFront;
        case(BACK):
            return _diffuseBack;
        case(FRONT_AND_BACK):
            if (!_diffuseFrontAndBack)
            {
                qDebug()<<"Notice: Material::getDiffuse(FRONT_AND_BACK) called on material "<< endl;
                qDebug()<<"        with separate FRONT and BACK diffuse colors."<< endl;
            }
            return _diffuseFront;
    }
    qDebug()<<"Notice: invalid Face passed to Material::getDiffuse()."<< endl;
    return _diffuseFront;
}


void Material::SetSpecular(Face face, const fvec4& specular )
{
    switch(face)
    {
        case(FRONT):
            _specularFrontAndBack = false;
            _specularFront = specular;
            //clampArray4BetweenRange(_specularFront,0.0f,1.0f,"Material::setSpecular(..)");
            break;
        case(BACK):
            _specularFrontAndBack = false;
            _specularBack = specular;
            //clampArray4BetweenRange(_specularBack,0.0f,1.0f,"Material::setSpecular(..)");
            break;
        case(FRONT_AND_BACK):
            _specularFrontAndBack = true;
            _specularFront = specular;
            //clampArray4BetweenRange(_specularFront,0.0f,1.0f,"Material::setSpecular(..)");
            _specularBack = _specularFront;
            break;
        default:
            qDebug() <<"Notice: invalid Face passed to Material::setSpecular()."<< endl;
            break;
    }
}


const fvec4& Material::GetSpecular(Face face) const
{
    switch(face)
    {
        case(FRONT):
            return _specularFront;
        case(BACK):
            return _specularBack;
        case(FRONT_AND_BACK):
            if (!_specularFrontAndBack)
            {
                qDebug()<<"Notice: Material::getSpecular(FRONT_AND_BACK) called on material "<< endl;
                qDebug()<<"        with separate FRONT and BACK specular colors."<< endl;
            }
            return _specularFront;
    }
    qDebug()<<"Notice: invalid Face passed to Material::getSpecular()."<< endl;
    return _specularFront;
}


void Material::SetEmission(Face face, const fvec4& emission )
{
    switch(face)
    {
        case(FRONT):
            _emissionFrontAndBack = false;
            _emissionFront = emission;
            //clampArray4BetweenRange(_emissionFront,0.0f,1.0f,"Material::setEmission(..)");
            break;
        case(BACK):
            _emissionFrontAndBack = false;
            _emissionBack = emission;
            //clampArray4BetweenRange(_emissionBack,0.0f,1.0f,"Material::setEmission(..)");
            break;
        case(FRONT_AND_BACK):
            _emissionFrontAndBack = true;
            _emissionFront = emission;
            //clampArray4BetweenRange(_emissionFront,0.0f,1.0f,"Material::setEmission(..)");
            _emissionBack = _emissionFront;
            break;
        default:
            qDebug() <<"Notice: invalid Face passed to Material::setEmission()."<< endl;
            break;
    }
}


const fvec4& Material::GetEmission(Face face) const
{
    switch(face)
    {
        case(FRONT):
            return _emissionFront;
        case(BACK):
            return _emissionBack;
        case(FRONT_AND_BACK):
            if (!_emissionFrontAndBack)
            {
                qDebug()<<"Notice: Material::getEmission(FRONT_AND_BACK) called on material "<< endl;
                qDebug()<<"        with separate FRONT and BACK emission colors."<< endl;
            }
            return _emissionFront;
    }
    qDebug() <<"Notice: invalid Face passed to Material::getEmission()."<< endl;
    return _emissionFront;
}


void Material::SetShininess(Face face, float shininess )
{
    //clampBetweenRange(shininess,0.0f,128.0f,"Material::setShininess()");
    
    switch(face)
    {
        case(FRONT):
            _shininessFrontAndBack = false;
            _shininessFront = shininess;
            break;
        case(BACK):
            _shininessFrontAndBack = false;
            _shininessBack = shininess;
            break;
        case(FRONT_AND_BACK):
            _shininessFrontAndBack = true;
            _shininessFront = shininess;
            _shininessBack = shininess;
            break;
        default:
            qDebug() <<"Notice: invalid Face passed to Material::setShininess()."<< endl;
            break;
    }
}


float Material::GetShininess(Face face) const
{
    switch(face)
    {
        case(FRONT):
            return _shininessFront;
        case(BACK):
            return _shininessBack;
        case(FRONT_AND_BACK):
            if (!_shininessFrontAndBack)
            {
                qDebug()<<"Notice: Material::getShininess(FRONT_AND_BACK) called on material "<< endl;
                qDebug()<<"        with separate FRONT and BACK shininess colors."<< endl;
            }
            return _shininessFront;
    }
    qDebug()<<"Notice: invalid Face passed to Material::getShininess()."<< endl;
    return _shininessFront;
}

void Material::SetTransparency(Face face,float transparency)
{
   //clampBetweenRange(transparency,0.0f,1.0f,"Material::setTransparency()");

   if (face==FRONT || face==FRONT_AND_BACK)
   {
        _ambientFront[3] = 1.0f-transparency;
        _diffuseFront[3] = 1.0f-transparency;
        _specularFront[3] = 1.0f-transparency;
        _emissionFront[3] = 1.0f-transparency;
    }

    if (face==BACK || face==FRONT_AND_BACK)
    {
        _ambientBack[3] = 1.0f-transparency;
        _diffuseBack[3] = 1.0f-transparency;
        _specularBack[3] = 1.0f-transparency;
        _emissionBack[3] = 1.0f-transparency;
    }
}        

void Material::SetAlpha(Face face,float alpha)
{
   //clampBetweenRange(alpha,0.0f,1.0f,"Material::setAlpha()");

   if (face==FRONT || face==FRONT_AND_BACK)
   {
        _ambientFront[3] = alpha;
        _diffuseFront[3] = alpha;
        _specularFront[3] = alpha;
        _emissionFront[3] = alpha;
    }

    if (face==BACK || face==FRONT_AND_BACK)
    {
        _ambientBack[3] = alpha;
        _diffuseBack[3] = alpha;
        _specularBack[3] = alpha;
        _emissionBack[3] = alpha;
    }
}

void Material::Apply(State&) const
{
#ifdef DC_GL_FIXED_FUNCTION_AVAILABLE

#ifdef DC_GL1_AVAILABLE
    if (_colorMode==OFF)
    {
        glDisable(GL_COLOR_MATERIAL);

        glColor4fv(_diffuseFront.ptr());
    }
    else
    {
        glColorMaterial(GL_FRONT_AND_BACK,(GLenum)_colorMode);
        glEnable(GL_COLOR_MATERIAL);
        switch(_colorMode)
        {
            case(AMBIENT): glColor4fv(_ambientFront.ptr()); break;
            case(DIFFUSE): glColor4fv(_diffuseFront.ptr()); break;
            case(SPECULAR): glColor4fv(_specularFront.ptr()); break;
            case(EMISSION): glColor4fv(_emissionFront.ptr()); break;
            case(AMBIENT_AND_DIFFUSE): glColor4fv(_diffuseFront.ptr()); break;
            case(OFF): break;
        }
    }
#endif

    if (_colorMode!=AMBIENT && _colorMode!=AMBIENT_AND_DIFFUSE)
    {
        if (_ambientFrontAndBack)
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, _ambientFront.ptr() );
        }
        else
        {
            glMaterialfv( GL_FRONT, GL_AMBIENT, _ambientFront.ptr() );
            glMaterialfv( GL_BACK, GL_AMBIENT, _ambientBack.ptr() );
        }
    }

    if (_colorMode!=DIFFUSE && _colorMode!=AMBIENT_AND_DIFFUSE)
    {
        if (_diffuseFrontAndBack)
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, _diffuseFront.ptr() );
        }
        else
        {
            glMaterialfv( GL_FRONT, GL_DIFFUSE, _diffuseFront.ptr() );
            glMaterialfv( GL_BACK, GL_DIFFUSE, _diffuseBack.ptr() );
        }
    }

    if (_colorMode!=SPECULAR)
    {
        if (_specularFrontAndBack)
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, _specularFront.ptr() );
        }
        else
        {
            glMaterialfv( GL_FRONT, GL_SPECULAR, _specularFront.ptr() );
            glMaterialfv( GL_BACK, GL_SPECULAR, _specularBack.ptr() );
        }
    }

    if (_colorMode!=EMISSION)
    {
        if (_emissionFrontAndBack)
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, _emissionFront.ptr() );
        }
        else
        {
            glMaterialfv( GL_FRONT, GL_EMISSION, _emissionFront.ptr() );
            glMaterialfv( GL_BACK, GL_EMISSION, _emissionBack.ptr() );
        }
    }

    if (_shininessFrontAndBack)
    {
        glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, _shininessFront );
    }
    else
    {
        glMaterialf( GL_FRONT, GL_SHININESS, _shininessFront );
        glMaterialf( GL_BACK, GL_SHININESS, _shininessBack );
    }
#else
    qDebug()<<"Warning: Material::apply(State&) - not supported."<<endl;
#endif
}
