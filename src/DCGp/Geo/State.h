//----------------------------------------------------------------------
//author:wly
//time:
//dsc:渲染状态，主要用来管理mode和 Attribute
//
//----------------------------------------------------------------------


#ifndef MPGP_STATE_H
#define MPGP_STATE_H

#include "DCCore/DCGL.h"
#include "DCCore/BasicTypes.h"

#include "DCGp/DcGpDLL.h"

#include "StateAttribute.h"

namespace DCGp
{
	class Texture2D;
	class DCGP_API State
	{
	public:
		State();
		~State();

		//! 属性列表，记录所有的属性
		//属性和flag
		typedef std::pair<StateAttribute* ,StateAttribute::OverrideValue>    PtrAttributePair;

		typedef std::map<StateAttribute::TypeMemberPair,PtrAttributePair>    AttributeList;
		typedef std::map<StateAttribute::GLMode,StateAttribute::GLModeValue>  ModeList;

		//! 接口
		//void SetPolygonMode(PolygonMode* polygonmode);
		//PolygonMode* GetPolygonMode();

		void SetMode(StateAttribute::GLMode mode, StateAttribute::GLModeValue value);
		void SetMode(ModeList& modeList,StateAttribute::GLMode mode, StateAttribute::GLModeValue value);

		StateAttribute::GLModeValue GetMode(StateAttribute::GLMode mode) const;
		StateAttribute::GLModeValue GetMode(const ModeList& modeList,StateAttribute::GLMode mode) const;

		void SetAttribute(StateAttribute *attribute, StateAttribute::OverrideValue value=StateAttribute::OFF);
		void SetAttribute(AttributeList& attributeList,StateAttribute *attribute, const StateAttribute::OverrideValue value=StateAttribute::OFF);

		StateAttribute* GetAttribute(StateAttribute::Type type, unsigned int member = 0);
		StateAttribute* GetAttribute(AttributeList& attributeList,const StateAttribute::Type type, unsigned int member);
		
		void Apply();

		inline bool ApplyMode(StateAttribute::GLMode mode,bool enabled);

		void SetTextureAttribute(Texture2D* textureAttribute, StateAttribute::OverrideValue value=StateAttribute::ON);
		Texture2D* GetTextureAttribute() {return m_texture2D;}

		inline bool ApplyModeOnTexUnit();

		//------------------------------------------顶点数组-------------------------------------------------------
		inline void SetVertexPointer(std::vector<DCVector3D >& points);
		
		inline void SetVertexPointer( GLint size, GLenum type,
			GLsizei stride, const GLvoid *ptr );

		inline void SetNormalPointer( GLenum type, GLsizei stride,
			const GLvoid *ptr );

		inline void SetTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
		
		inline void DisableVertexPointer();

		inline void DisableColorPointer();

		inline void DisableNormalPointer();

		inline void DisableTexCoordPointer();

		void State::ApplyDisablingOfVertexAttributes();
	private:
		
		//! 常规属性及mode
		ModeList m_modeList;
		AttributeList  m_attributeList;

		//! 纹理属性及mode
		Texture2D* m_texture2D;

		//! 顶点数组
		struct EnabledArrayPair
		{
			EnabledArrayPair():_lazy_disable(false),_dirty(true),_enabled(false),_normalized(0),_pointer(0) {}
			EnabledArrayPair(const EnabledArrayPair& eap):_lazy_disable(eap._lazy_disable),_dirty(eap._dirty), _enabled(eap._enabled),_normalized(eap._normalized),_pointer(eap._pointer) {}
			EnabledArrayPair& operator = (const EnabledArrayPair& eap) { _lazy_disable = eap._lazy_disable;_dirty=eap._dirty; _enabled=eap._enabled; _normalized=eap._normalized;_pointer=eap._pointer; return *this; }

			bool            _lazy_disable;
			bool            _dirty;
			bool            _enabled;
			GLboolean       _normalized;
			const GLvoid*   _pointer;
		};

		typedef std::vector<EnabledArrayPair>                   EnabledTexCoordArrayList;
		typedef std::vector<EnabledArrayPair>                   EnabledVertexAttribArrayList;

		EnabledArrayPair                _vertexArray;
		EnabledArrayPair                _normalArray;
		EnabledArrayPair                _colorArray;
		EnabledArrayPair                _secondaryColorArray;
		EnabledArrayPair                _fogArray;
		EnabledArrayPair				_texCoordArray;
	};

}

#endif // STATE_H
