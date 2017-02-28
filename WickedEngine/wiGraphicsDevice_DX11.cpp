#include "wiGraphicsDevice_DX11.h"
#include "Include_DX11.h"
#include "wiHelper.h"
#include "ResourceMapping.h"

#include "Utility/WicTextureLoader.h"
#include "Utility/DDSTextureLoader.h"
#include "Utility/ScreenGrab.h"

namespace wiGraphicsTypes
{
// Engine -> Native converters

inline UINT _ParseBindFlags(UINT value)
{
	UINT _flag = 0;

	if (value & BIND_VERTEX_BUFFER)
		_flag |= D3D11_BIND_VERTEX_BUFFER;
	if (value & BIND_INDEX_BUFFER)
		_flag |= D3D11_BIND_INDEX_BUFFER;
	if (value & BIND_CONSTANT_BUFFER)
		_flag |= D3D11_BIND_CONSTANT_BUFFER;
	if (value & BIND_SHADER_RESOURCE)
		_flag |= D3D11_BIND_SHADER_RESOURCE;
	if (value & BIND_STREAM_OUTPUT)
		_flag |= D3D11_BIND_STREAM_OUTPUT;
	if (value & BIND_RENDER_TARGET)
		_flag |= D3D11_BIND_RENDER_TARGET;
	if (value & BIND_DEPTH_STENCIL)
		_flag |= D3D11_BIND_DEPTH_STENCIL;
	if (value & BIND_UNORDERED_ACCESS)
		_flag |= D3D11_BIND_UNORDERED_ACCESS;

	return _flag;
}
inline UINT _ParseCPUAccessFlags(UINT value)
{
	UINT _flag = 0;

	if (value & CPU_ACCESS_WRITE)
		_flag |= D3D11_CPU_ACCESS_WRITE;
	if (value & CPU_ACCESS_READ)
		_flag |= D3D11_CPU_ACCESS_READ;

	return _flag;
}
inline UINT _ParseResourceMiscFlags(UINT value)
{
	UINT _flag = 0;

	if (value & RESOURCE_MISC_GENERATE_MIPS)
		_flag |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if (value & RESOURCE_MISC_SHARED)
		_flag |= D3D11_RESOURCE_MISC_SHARED;
	if (value & RESOURCE_MISC_TEXTURECUBE)
		_flag |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (value & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
		_flag |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	if (value & RESOURCE_MISC_BUFFER_STRUCTURED)
		_flag |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (value & RESOURCE_MISC_TILED)
		_flag |= D3D11_RESOURCE_MISC_TILED;

	return _flag;
}
inline UINT _ParseColorWriteMask(UINT value)
{
	UINT _flag = 0;

	if (value & COLOR_WRITE_ENABLE_RED)
		_flag |= D3D11_COLOR_WRITE_ENABLE_RED;
	if (value & COLOR_WRITE_ENABLE_GREEN)
		_flag |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	if (value & COLOR_WRITE_ENABLE_BLUE)
		_flag |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	if (value & COLOR_WRITE_ENABLE_ALPHA)
		_flag |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	if (value & COLOR_WRITE_ENABLE_ALL)
		_flag |= D3D11_COLOR_WRITE_ENABLE_ALL;

	return _flag;
}

inline D3D11_FILTER _ConvertFilter(FILTER value)
{
	switch (value)
	{
	case FILTER_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case FILTER_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case FILTER_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case FILTER_ANISOTROPIC:
		return D3D11_FILTER_ANISOTROPIC;
		break;
	case FILTER_COMPARISON_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		break;
	case FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	case FILTER_COMPARISON_ANISOTROPIC:
		return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		break;
	case FILTER_MINIMUM_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
		break;
	case FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
		break;
	case FILTER_MINIMUM_ANISOTROPIC:
		return D3D11_FILTER_MINIMUM_ANISOTROPIC;
		break;
	case FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
		break;
	case FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
		break;
	case FILTER_MAXIMUM_ANISOTROPIC:
		return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
		break;
	default:
		break;
	}
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}
inline D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(TEXTURE_ADDRESS_MODE value)
{
	switch (value)
	{
	case TEXTURE_ADDRESS_WRAP:
		return D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case TEXTURE_ADDRESS_MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case TEXTURE_ADDRESS_CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case TEXTURE_ADDRESS_BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case TEXTURE_ADDRESS_MIRROR_ONCE:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;
	default:
		break;
	}
	return D3D11_TEXTURE_ADDRESS_WRAP;
}
inline D3D11_COMPARISON_FUNC _ConvertComparisonFunc(COMPARISON_FUNC value)
{
	switch (value)
	{
	case COMPARISON_NEVER:
		return D3D11_COMPARISON_NEVER;
		break;
	case COMPARISON_LESS:
		return D3D11_COMPARISON_LESS;
		break;
	case COMPARISON_EQUAL:
		return D3D11_COMPARISON_EQUAL;
		break;
	case COMPARISON_LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
		break;
	case COMPARISON_GREATER:
		return D3D11_COMPARISON_GREATER;
		break;
	case COMPARISON_NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
		break;
	case COMPARISON_GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case COMPARISON_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
		break;
	default:
		break;
	}
	return D3D11_COMPARISON_NEVER;
}
inline D3D11_FILL_MODE _ConvertFillMode(FILL_MODE value)
{
	switch (value)
	{
	case FILL_WIREFRAME:
		return D3D11_FILL_WIREFRAME;
		break;
	case FILL_SOLID:
		return D3D11_FILL_SOLID;
		break;
	default:
		break;
	}
	return D3D11_FILL_WIREFRAME;
}
inline D3D11_CULL_MODE _ConvertCullMode(CULL_MODE value)
{
	switch (value)
	{
	case CULL_NONE:
		return D3D11_CULL_NONE;
		break;
	case CULL_FRONT:
		return D3D11_CULL_FRONT;
		break;
	case CULL_BACK:
		return D3D11_CULL_BACK;
		break;
	default:
		break;
	}
	return D3D11_CULL_NONE;
}
inline D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(DEPTH_WRITE_MASK value)
{
	switch (value)
	{
	case DEPTH_WRITE_MASK_ZERO:
		return D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_WRITE_MASK_ALL:
		return D3D11_DEPTH_WRITE_MASK_ALL;
		break;
	default:
		break;
	}
	return D3D11_DEPTH_WRITE_MASK_ZERO;
}
inline D3D11_STENCIL_OP _ConvertStencilOp(STENCIL_OP value)
{
	switch (value)
	{
	case STENCIL_OP_KEEP:
		return D3D11_STENCIL_OP_KEEP;
		break;
	case STENCIL_OP_ZERO:
		return D3D11_STENCIL_OP_ZERO;
		break;
	case STENCIL_OP_REPLACE:
		return D3D11_STENCIL_OP_REPLACE;
		break;
	case STENCIL_OP_INCR_SAT:
		return D3D11_STENCIL_OP_INCR_SAT;
		break;
	case STENCIL_OP_DECR_SAT:
		return D3D11_STENCIL_OP_DECR_SAT;
		break;
	case STENCIL_OP_INVERT:
		return D3D11_STENCIL_OP_INVERT;
		break;
	case STENCIL_OP_INCR:
		return D3D11_STENCIL_OP_INCR;
		break;
	case STENCIL_OP_DECR:
		return D3D11_STENCIL_OP_DECR;
		break;
	default:
		break;
	}
	return D3D11_STENCIL_OP_KEEP;
}
inline D3D11_BLEND _ConvertBlend(BLEND value)
{
	switch (value)
	{
	case BLEND_ZERO:
		return D3D11_BLEND_ZERO;
		break;
	case BLEND_ONE:
		return D3D11_BLEND_ONE;
		break;
	case BLEND_SRC_COLOR:
		return D3D11_BLEND_SRC_COLOR;
		break;
	case BLEND_INV_SRC_COLOR:
		return D3D11_BLEND_INV_SRC_COLOR;
		break;
	case BLEND_SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;
		break;
	case BLEND_INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case BLEND_DEST_ALPHA:
		return D3D11_BLEND_DEST_ALPHA;
		break;
	case BLEND_INV_DEST_ALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;
		break;
	case BLEND_DEST_COLOR:
		return D3D11_BLEND_DEST_COLOR;
		break;
	case BLEND_INV_DEST_COLOR:
		return D3D11_BLEND_INV_DEST_COLOR;
		break;
	case BLEND_SRC_ALPHA_SAT:
		return D3D11_BLEND_SRC_ALPHA_SAT;
		break;
	case BLEND_BLEND_FACTOR:
		return D3D11_BLEND_BLEND_FACTOR;
		break;
	case BLEND_INV_BLEND_FACTOR:
		return D3D11_BLEND_INV_BLEND_FACTOR;
		break;
	case BLEND_SRC1_COLOR:
		return D3D11_BLEND_SRC1_COLOR;
		break;
	case BLEND_INV_SRC1_COLOR:
		return D3D11_BLEND_INV_SRC1_COLOR;
		break;
	case BLEND_SRC1_ALPHA:
		return D3D11_BLEND_SRC1_ALPHA;
		break;
	case BLEND_INV_SRC1_ALPHA:
		return D3D11_BLEND_INV_SRC1_ALPHA;
		break;
	default:
		break;
	}
	return D3D11_BLEND_ZERO;
}
inline D3D11_BLEND_OP _ConvertBlendOp(BLEND_OP value)
{
	switch (value)
	{
	case BLEND_OP_ADD:
		return D3D11_BLEND_OP_ADD;
		break;
	case BLEND_OP_SUBTRACT:
		return D3D11_BLEND_OP_SUBTRACT;
		break;
	case BLEND_OP_REV_SUBTRACT:
		return D3D11_BLEND_OP_REV_SUBTRACT;
		break;
	case BLEND_OP_MIN:
		return D3D11_BLEND_OP_MIN;
		break;
	case BLEND_OP_MAX:
		return D3D11_BLEND_OP_MAX;
		break;
	default:
		break;
	}
	return D3D11_BLEND_OP_ADD;
}
inline D3D11_USAGE _ConvertUsage(USAGE value)
{
	switch (value)
	{
	case D3D11_USAGE_DEFAULT:
		return D3D11_USAGE_DEFAULT;
		break;
	case D3D11_USAGE_IMMUTABLE:
		return D3D11_USAGE_IMMUTABLE;
		break;
	case D3D11_USAGE_DYNAMIC:
		return D3D11_USAGE_DYNAMIC;
		break;
	case D3D11_USAGE_STAGING:
		return D3D11_USAGE_STAGING;
		break;
	default:
		break;
	}
	return D3D11_USAGE_DEFAULT;
}
inline D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(INPUT_CLASSIFICATION value)
{
	switch (value)
	{
	case INPUT_PER_VERTEX_DATA:
		return D3D11_INPUT_PER_VERTEX_DATA;
		break;
	case INPUT_PER_INSTANCE_DATA:
		return D3D11_INPUT_PER_INSTANCE_DATA;
		break;
	default:
		break;
	}
	return D3D11_INPUT_PER_VERTEX_DATA;
}
inline DXGI_FORMAT _ConvertFormat(FORMAT value)
{
	switch (value)
	{
	case FORMAT_UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
		break;
	case FORMAT_R32G32B32A32_TYPELESS:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case FORMAT_R32G32B32A32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case FORMAT_R32G32B32A32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
		break;
	case FORMAT_R32G32B32A32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
		break;
	case FORMAT_R32G32B32_TYPELESS:
		return DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case FORMAT_R32G32B32_FLOAT:
		return DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case FORMAT_R32G32B32_UINT:
		return DXGI_FORMAT_R32G32B32_UINT;
		break;
	case FORMAT_R32G32B32_SINT:
		return DXGI_FORMAT_R32G32B32_SINT;
		break;
	case FORMAT_R16G16B16A16_TYPELESS:
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		break;
	case FORMAT_R16G16B16A16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case FORMAT_R16G16B16A16_UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
		break;
	case FORMAT_R16G16B16A16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
		break;
	case FORMAT_R16G16B16A16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
		break;
	case FORMAT_R16G16B16A16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
		break;
	case FORMAT_R32G32_TYPELESS:
		return DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case FORMAT_R32G32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
		break;
	case FORMAT_R32G32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
		break;
	case FORMAT_R32G32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
		break;
	case FORMAT_R32G8X24_TYPELESS:
		return DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case FORMAT_D32_FLOAT_S8X24_UINT:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	case FORMAT_X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		break;
	case FORMAT_R10G10B10A2_TYPELESS:
		return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		break;
	case FORMAT_R10G10B10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
		break;
	case FORMAT_R10G10B10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_UINT;
		break;
	case FORMAT_R11G11B10_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
		break;
	case FORMAT_R8G8B8A8_TYPELESS:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		break;
	case FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		break;
	case FORMAT_R8G8B8A8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
		break;
	case FORMAT_R8G8B8A8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
		break;
	case FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
		break;
	case FORMAT_R16G16_TYPELESS:
		return DXGI_FORMAT_R16G16_TYPELESS;
		break;
	case FORMAT_R16G16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
		break;
	case FORMAT_R16G16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
		break;
	case FORMAT_R16G16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
		break;
	case FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
		break;
	case FORMAT_R16G16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
		break;
	case FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_R32_TYPELESS;
		break;
	case FORMAT_D32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
		break;
	case FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
		break;
	case FORMAT_R32_UINT:
		return DXGI_FORMAT_R32_UINT;
		break;
	case FORMAT_R32_SINT:
		return DXGI_FORMAT_R32_SINT;
		break;
	case FORMAT_R24G8_TYPELESS:
		return DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case FORMAT_D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case FORMAT_R24_UNORM_X8_TYPELESS:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case FORMAT_X24_TYPELESS_G8_UINT:
		return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		break;
	case FORMAT_R8G8_TYPELESS:
		return DXGI_FORMAT_R8G8_TYPELESS;
		break;
	case FORMAT_R8G8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
		break;
	case FORMAT_R8G8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
		break;
	case FORMAT_R8G8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
		break;
	case FORMAT_R8G8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
		break;
	case FORMAT_R16_TYPELESS:
		return DXGI_FORMAT_R16_TYPELESS;
		break;
	case FORMAT_R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
		break;
	case FORMAT_D16_UNORM:
		return DXGI_FORMAT_D16_UNORM;
		break;
	case FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
		break;
	case FORMAT_R16_UINT:
		return DXGI_FORMAT_R16_UINT;
		break;
	case FORMAT_R16_SNORM:
		return DXGI_FORMAT_R16_SNORM;
		break;
	case FORMAT_R16_SINT:
		return DXGI_FORMAT_R16_SINT;
		break;
	case FORMAT_R8_TYPELESS:
		return DXGI_FORMAT_R8_TYPELESS;
		break;
	case FORMAT_R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
		break;
	case FORMAT_R8_UINT:
		return DXGI_FORMAT_R8_UINT;
		break;
	case FORMAT_R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
		break;
	case FORMAT_R8_SINT:
		return DXGI_FORMAT_R8_SINT;
		break;
	case FORMAT_A8_UNORM:
		return DXGI_FORMAT_A8_UNORM;
		break;
	case FORMAT_R1_UNORM:
		return DXGI_FORMAT_R1_UNORM;
		break;
	case FORMAT_R9G9B9E5_SHAREDEXP:
		return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		break;
	case FORMAT_R8G8_B8G8_UNORM:
		return DXGI_FORMAT_R8G8_B8G8_UNORM;
		break;
	case FORMAT_G8R8_G8B8_UNORM:
		return DXGI_FORMAT_G8R8_G8B8_UNORM;
		break;
	case FORMAT_BC1_TYPELESS:
		return DXGI_FORMAT_BC1_TYPELESS;
		break;
	case FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM;
		break;
	case FORMAT_BC1_UNORM_SRGB:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
		break;
	case FORMAT_BC2_TYPELESS:
		return DXGI_FORMAT_BC2_TYPELESS;
		break;
	case FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM;
		break;
	case FORMAT_BC2_UNORM_SRGB:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
		break;
	case FORMAT_BC3_TYPELESS:
		return DXGI_FORMAT_BC3_TYPELESS;
		break;
	case FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM;
		break;
	case FORMAT_BC3_UNORM_SRGB:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
		break;
	case FORMAT_BC4_TYPELESS:
		return DXGI_FORMAT_BC4_TYPELESS;
		break;
	case FORMAT_BC4_UNORM:
		return DXGI_FORMAT_BC4_UNORM;
		break;
	case FORMAT_BC4_SNORM:
		return DXGI_FORMAT_BC4_SNORM;
		break;
	case FORMAT_BC5_TYPELESS:
		return DXGI_FORMAT_BC5_TYPELESS;
		break;
	case FORMAT_BC5_UNORM:
		return DXGI_FORMAT_BC5_UNORM;
		break;
	case FORMAT_BC5_SNORM:
		return DXGI_FORMAT_BC5_SNORM;
		break;
	case FORMAT_B5G6R5_UNORM:
		return DXGI_FORMAT_B5G6R5_UNORM;
		break;
	case FORMAT_B5G5R5A1_UNORM:
		return DXGI_FORMAT_B5G5R5A1_UNORM;
		break;
	case FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
		break;
	case FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM;
		break;
	case FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		break;
	case FORMAT_B8G8R8A8_TYPELESS:
		return DXGI_FORMAT_B8G8R8A8_TYPELESS;
		break;
	case FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		break;
	case FORMAT_B8G8R8X8_TYPELESS:
		return DXGI_FORMAT_B8G8R8X8_TYPELESS;
		break;
	case FORMAT_B8G8R8X8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		break;
	case FORMAT_BC6H_TYPELESS:
		return DXGI_FORMAT_BC6H_TYPELESS;
		break;
	case FORMAT_BC6H_UF16:
		return DXGI_FORMAT_BC6H_UF16;
		break;
	case FORMAT_BC6H_SF16:
		return DXGI_FORMAT_BC6H_SF16;
		break;
	case FORMAT_BC7_TYPELESS:
		return DXGI_FORMAT_BC7_TYPELESS;
		break;
	case FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM;
		break;
	case FORMAT_BC7_UNORM_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
		break;
	case FORMAT_AYUV:
		return DXGI_FORMAT_AYUV;
		break;
	case FORMAT_Y410:
		return DXGI_FORMAT_Y410;
		break;
	case FORMAT_Y416:
		return DXGI_FORMAT_Y416;
		break;
	case FORMAT_NV12:
		return DXGI_FORMAT_NV12;
		break;
	case FORMAT_P010:
		return DXGI_FORMAT_P010;
		break;
	case FORMAT_P016:
		return DXGI_FORMAT_P016;
		break;
	case FORMAT_420_OPAQUE:
		return DXGI_FORMAT_420_OPAQUE;
		break;
	case FORMAT_YUY2:
		return DXGI_FORMAT_YUY2;
		break;
	case FORMAT_Y210:
		return DXGI_FORMAT_Y210;
		break;
	case FORMAT_Y216:
		return DXGI_FORMAT_Y216;
		break;
	case FORMAT_NV11:
		return DXGI_FORMAT_NV11;
		break;
	case FORMAT_AI44:
		return DXGI_FORMAT_AI44;
		break;
	case FORMAT_IA44:
		return DXGI_FORMAT_IA44;
		break;
	case FORMAT_P8:
		return DXGI_FORMAT_P8;
		break;
	case FORMAT_A8P8:
		return DXGI_FORMAT_A8P8;
		break;
	case FORMAT_B4G4R4A4_UNORM:
		return DXGI_FORMAT_B4G4R4A4_UNORM;
		break;
	case FORMAT_FORCE_UINT:
		return DXGI_FORMAT_FORCE_UINT;
		break;
	default:
		break;
	}
	return DXGI_FORMAT_UNKNOWN;
}

inline D3D11_TEXTURE1D_DESC _ConvertTexture1DDesc(const Texture1DDesc* pDesc)
{
	D3D11_TEXTURE1D_DESC desc;
	desc.Width = pDesc->Width;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = pDesc->ArraySize;
	desc.Format = _ConvertFormat(pDesc->Format);
	desc.Usage = _ConvertUsage(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

	return desc;
}
inline D3D11_TEXTURE2D_DESC _ConvertTexture2DDesc(const Texture2DDesc* pDesc)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = pDesc->Width;
	desc.Height = pDesc->Height;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = pDesc->ArraySize;
	desc.Format = _ConvertFormat(pDesc->Format);
	desc.SampleDesc.Count = pDesc->SampleDesc.Count;
	desc.SampleDesc.Quality = pDesc->SampleDesc.Quality;
	desc.Usage = _ConvertUsage(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

	return desc;
}
inline D3D11_TEXTURE3D_DESC _ConvertTexture3DDesc(const Texture3DDesc* pDesc)
{
	D3D11_TEXTURE3D_DESC desc;
	desc.Width = pDesc->Width;
	desc.Height = pDesc->Height;
	desc.Depth = pDesc->Depth;
	desc.MipLevels = pDesc->MipLevels;
	desc.Format = _ConvertFormat(pDesc->Format);
	desc.Usage = _ConvertUsage(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);

	return desc;
}
inline D3D11_SUBRESOURCE_DATA* _ConvertSubresourceData(const SubresourceData* pInitialData)
{
	if (pInitialData == nullptr)
		return nullptr;

	D3D11_SUBRESOURCE_DATA* data = new D3D11_SUBRESOURCE_DATA;
	data->pSysMem = pInitialData->pSysMem;
	data->SysMemPitch = pInitialData->SysMemPitch;
	data->SysMemSlicePitch = pInitialData->SysMemSlicePitch;

	return data;
}


// Native -> Engine converters

inline UINT _ParseBindFlags_Inv(UINT value)
{
	UINT _flag = 0;

	if (value & D3D11_BIND_VERTEX_BUFFER)
		_flag |= BIND_VERTEX_BUFFER;
	if (value & D3D11_BIND_INDEX_BUFFER)
		_flag |= BIND_INDEX_BUFFER;
	if (value & D3D11_BIND_CONSTANT_BUFFER)
		_flag |= BIND_CONSTANT_BUFFER;
	if (value & D3D11_BIND_SHADER_RESOURCE)
		_flag |= BIND_SHADER_RESOURCE;
	if (value & D3D11_BIND_STREAM_OUTPUT)
		_flag |= BIND_STREAM_OUTPUT;
	if (value & D3D11_BIND_RENDER_TARGET)
		_flag |= BIND_RENDER_TARGET;
	if (value & D3D11_BIND_DEPTH_STENCIL)
		_flag |= BIND_DEPTH_STENCIL;
	if (value & D3D11_BIND_UNORDERED_ACCESS)
		_flag |= BIND_UNORDERED_ACCESS;

	return _flag;
}
inline UINT _ParseCPUAccessFlags_Inv(UINT value)
{
	UINT _flag = 0;

	if (value & D3D11_CPU_ACCESS_WRITE)
		_flag |= CPU_ACCESS_WRITE;
	if (value & D3D11_CPU_ACCESS_READ)
		_flag |= CPU_ACCESS_READ;

	return _flag;
}
inline UINT _ParseResourceMiscFlags_Inv(UINT value)
{
	UINT _flag = 0;

	if (value & D3D11_RESOURCE_MISC_GENERATE_MIPS)
		_flag |= RESOURCE_MISC_GENERATE_MIPS;
	if (value & D3D11_RESOURCE_MISC_SHARED)
		_flag |= RESOURCE_MISC_SHARED;
	if (value & D3D11_RESOURCE_MISC_TEXTURECUBE)
		_flag |= RESOURCE_MISC_TEXTURECUBE;
	if (value & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
		_flag |= RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	if (value & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		_flag |= RESOURCE_MISC_BUFFER_STRUCTURED;
	if (value & D3D11_RESOURCE_MISC_TILED)
		_flag |= RESOURCE_MISC_TILED;

	return _flag;
}

inline FORMAT _ConvertFormat_Inv(DXGI_FORMAT value)
{
	switch (value)
	{
	case DXGI_FORMAT_UNKNOWN:
		return FORMAT_UNKNOWN;
		break;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return FORMAT_R32G32B32A32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return FORMAT_R32G32B32A32_FLOAT;
		break;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return FORMAT_R32G32B32A32_UINT;
		break;
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return FORMAT_R32G32B32A32_SINT;
		break;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return FORMAT_R32G32B32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return FORMAT_R32G32B32_FLOAT;
		break;
	case DXGI_FORMAT_R32G32B32_UINT:
		return FORMAT_R32G32B32_UINT;
		break;
	case DXGI_FORMAT_R32G32B32_SINT:
		return FORMAT_R32G32B32_SINT;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return FORMAT_R16G16B16A16_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return FORMAT_R16G16B16A16_FLOAT;
		break;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return FORMAT_R16G16B16A16_UNORM;
		break;
	case DXGI_FORMAT_R16G16B16A16_UINT:
		return FORMAT_R16G16B16A16_UINT;
		break;
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return FORMAT_R16G16B16A16_SNORM;
		break;
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return FORMAT_R16G16B16A16_SINT;
		break;
	case DXGI_FORMAT_R32G32_TYPELESS:
		return FORMAT_R32G32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32_FLOAT:
		return FORMAT_R32G32_FLOAT;
		break;
	case DXGI_FORMAT_R32G32_UINT:
		return FORMAT_R32G32_UINT;
		break;
	case DXGI_FORMAT_R32G32_SINT:
		return FORMAT_R32G32_SINT;
		break;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		return FORMAT_R32G8X24_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return FORMAT_X32_TYPELESS_G8X24_UINT;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return FORMAT_R10G10B10A2_TYPELESS;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return FORMAT_R10G10B10A2_UNORM;
		break;
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return FORMAT_R10G10B10A2_UINT;
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		return FORMAT_R11G11B10_FLOAT;
		break;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return FORMAT_R8G8B8A8_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return FORMAT_R8G8B8A8_UNORM;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return FORMAT_R8G8B8A8_UNORM_SRGB;
		break;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return FORMAT_R8G8B8A8_UINT;
		break;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return FORMAT_R8G8B8A8_SNORM;
		break;
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return FORMAT_R8G8B8A8_SINT;
		break;
	case DXGI_FORMAT_R16G16_TYPELESS:
		return FORMAT_R16G16_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16_FLOAT:
		return FORMAT_R16G16_FLOAT;
		break;
	case DXGI_FORMAT_R16G16_UNORM:
		return FORMAT_R16G16_UNORM;
		break;
	case DXGI_FORMAT_R16G16_UINT:
		return FORMAT_R16G16_UINT;
		break;
	case DXGI_FORMAT_R16G16_SNORM:
		return FORMAT_R16G16_SNORM;
		break;
	case DXGI_FORMAT_R16G16_SINT:
		return FORMAT_R16G16_SINT;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
		return FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT:
		return FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R32_FLOAT:
		return FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_R32_UINT:
		return FORMAT_R32_UINT;
		break;
	case DXGI_FORMAT_R32_SINT:
		return FORMAT_R32_SINT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		return FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return FORMAT_D24_UNORM_S8_UINT;
		break;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		return FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return FORMAT_X24_TYPELESS_G8_UINT;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
		return FORMAT_R8G8_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8_UNORM:
		return FORMAT_R8G8_UNORM;
		break;
	case DXGI_FORMAT_R8G8_UINT:
		return FORMAT_R8G8_UINT;
		break;
	case DXGI_FORMAT_R8G8_SNORM:
		return FORMAT_R8G8_SNORM;
		break;
	case DXGI_FORMAT_R8G8_SINT:
		return FORMAT_R8G8_SINT;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
		return FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT_R16_FLOAT:
		return FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT_D16_UNORM:
		return FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R16_UNORM:
		return FORMAT_R16_UNORM;
		break;
	case DXGI_FORMAT_R16_UINT:
		return FORMAT_R16_UINT;
		break;
	case DXGI_FORMAT_R16_SNORM:
		return FORMAT_R16_SNORM;
		break;
	case DXGI_FORMAT_R16_SINT:
		return FORMAT_R16_SINT;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
		return FORMAT_R8_TYPELESS;
		break;
	case DXGI_FORMAT_R8_UNORM:
		return FORMAT_R8_UNORM;
		break;
	case DXGI_FORMAT_R8_UINT:
		return FORMAT_R8_UINT;
		break;
	case DXGI_FORMAT_R8_SNORM:
		return FORMAT_R8_SNORM;
		break;
	case DXGI_FORMAT_R8_SINT:
		return FORMAT_R8_SINT;
		break;
	case DXGI_FORMAT_A8_UNORM:
		return FORMAT_A8_UNORM;
		break;
	case DXGI_FORMAT_R1_UNORM:
		return FORMAT_R1_UNORM;
		break;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return FORMAT_R9G9B9E5_SHAREDEXP;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		return FORMAT_R8G8_B8G8_UNORM;
		break;
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return FORMAT_G8R8_G8B8_UNORM;
		break;
	case DXGI_FORMAT_BC1_TYPELESS:
		return FORMAT_BC1_TYPELESS;
		break;
	case DXGI_FORMAT_BC1_UNORM:
		return FORMAT_BC1_UNORM;
		break;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return FORMAT_BC1_UNORM_SRGB;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:
		return FORMAT_BC2_TYPELESS;
		break;
	case DXGI_FORMAT_BC2_UNORM:
		return FORMAT_BC2_UNORM;
		break;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return FORMAT_BC2_UNORM_SRGB;
		break;
	case DXGI_FORMAT_BC3_TYPELESS:
		return FORMAT_BC3_TYPELESS;
		break;
	case DXGI_FORMAT_BC3_UNORM:
		return FORMAT_BC3_UNORM;
		break;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return FORMAT_BC3_UNORM_SRGB;
		break;
	case DXGI_FORMAT_BC4_TYPELESS:
		return FORMAT_BC4_TYPELESS;
		break;
	case DXGI_FORMAT_BC4_UNORM:
		return FORMAT_BC4_UNORM;
		break;
	case DXGI_FORMAT_BC4_SNORM:
		return FORMAT_BC4_SNORM;
		break;
	case DXGI_FORMAT_BC5_TYPELESS:
		return FORMAT_BC5_TYPELESS;
		break;
	case DXGI_FORMAT_BC5_UNORM:
		return FORMAT_BC5_UNORM;
		break;
	case DXGI_FORMAT_BC5_SNORM:
		return FORMAT_BC5_SNORM;
		break;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return FORMAT_B5G6R5_UNORM;
		break;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return FORMAT_B5G5R5A1_UNORM;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return FORMAT_B8G8R8A8_UNORM;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return FORMAT_B8G8R8X8_UNORM;
		break;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		break;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		return FORMAT_B8G8R8A8_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return FORMAT_B8G8R8A8_UNORM_SRGB;
		break;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		return FORMAT_B8G8R8X8_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return FORMAT_B8G8R8X8_UNORM_SRGB;
		break;
	case DXGI_FORMAT_BC6H_TYPELESS:
		return FORMAT_BC6H_TYPELESS;
		break;
	case DXGI_FORMAT_BC6H_UF16:
		return FORMAT_BC6H_UF16;
		break;
	case DXGI_FORMAT_BC6H_SF16:
		return FORMAT_BC6H_SF16;
		break;
	case DXGI_FORMAT_BC7_TYPELESS:
		return FORMAT_BC7_TYPELESS;
		break;
	case DXGI_FORMAT_BC7_UNORM:
		return FORMAT_BC7_UNORM;
		break;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return FORMAT_BC7_UNORM_SRGB;
		break;
	case DXGI_FORMAT_AYUV:
		return FORMAT_AYUV;
		break;
	case DXGI_FORMAT_Y410:
		return FORMAT_Y410;
		break;
	case DXGI_FORMAT_Y416:
		return FORMAT_Y416;
		break;
	case DXGI_FORMAT_NV12:
		return FORMAT_NV12;
		break;
	case DXGI_FORMAT_P010:
		return FORMAT_P010;
		break;
	case DXGI_FORMAT_P016:
		return FORMAT_P016;
		break;
	case DXGI_FORMAT_420_OPAQUE:
		return FORMAT_420_OPAQUE;
		break;
	case DXGI_FORMAT_YUY2:
		return FORMAT_YUY2;
		break;
	case DXGI_FORMAT_Y210:
		return FORMAT_Y210;
		break;
	case DXGI_FORMAT_Y216:
		return FORMAT_Y216;
		break;
	case DXGI_FORMAT_NV11:
		return FORMAT_NV11;
		break;
	case DXGI_FORMAT_AI44:
		return FORMAT_AI44;
		break;
	case DXGI_FORMAT_IA44:
		return FORMAT_IA44;
		break;
	case DXGI_FORMAT_P8:
		return FORMAT_P8;
		break;
	case DXGI_FORMAT_A8P8:
		return FORMAT_A8P8;
		break;
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return FORMAT_B4G4R4A4_UNORM;
		break;
	case DXGI_FORMAT_FORCE_UINT:
		return FORMAT_FORCE_UINT;
		break;
	default:
		break;
	}
	return FORMAT_UNKNOWN;
}
inline USAGE _ConvertUsage_Inv(D3D11_USAGE value)
{
	switch (value)
	{
	case D3D11_USAGE_DEFAULT:
		return USAGE_DEFAULT;
		break;
	case D3D11_USAGE_IMMUTABLE:
		return USAGE_IMMUTABLE;
		break;
	case D3D11_USAGE_DYNAMIC:
		return USAGE_DYNAMIC;
		break;
	case D3D11_USAGE_STAGING:
		return USAGE_STAGING;
		break;
	default:
		break;
	}
	return USAGE_DEFAULT;
}

inline Texture1DDesc _ConvertTexture1DDesc_Inv(const D3D11_TEXTURE1D_DESC* pDesc)
{
	Texture1DDesc desc;
	desc.Width = pDesc->Width;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = pDesc->ArraySize;
	desc.Format = _ConvertFormat_Inv(pDesc->Format);
	desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

	return desc;
}
inline Texture2DDesc _ConvertTexture2DDesc_Inv(const D3D11_TEXTURE2D_DESC* pDesc)
{
	Texture2DDesc desc;
	desc.Width = pDesc->Width;
	desc.Height = pDesc->Height;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = pDesc->ArraySize;
	desc.Format = _ConvertFormat_Inv(pDesc->Format);
	desc.SampleDesc.Count = pDesc->SampleDesc.Count;
	desc.SampleDesc.Quality = pDesc->SampleDesc.Quality;
	desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

	return desc;
}
inline Texture3DDesc _ConvertTexture3DDesc_Inv(const D3D11_TEXTURE3D_DESC* pDesc)
{
	Texture3DDesc desc;
	desc.Width = pDesc->Width;
	desc.Height = pDesc->Height;
	desc.Depth = pDesc->Depth;
	desc.MipLevels = pDesc->MipLevels;
	desc.Format = _ConvertFormat_Inv(pDesc->Format);
	desc.Usage = _ConvertUsage_Inv(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags_Inv(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags_Inv(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags_Inv(pDesc->MiscFlags);

	return desc;
}


// Local Helpers:
const void* const __nullBlob[1024] = { 0 }; // this is initialized to nullptrs!



// Engine functions

GraphicsDevice_DX11::GraphicsDevice_DX11(wiWindowRegistration::window_type window, bool fullscreen) : GraphicsDevice()
{
	FULLSCREEN = fullscreen;

#ifndef WINSTORE_SUPPORT
	RECT rect = RECT();
	GetClientRect(window, &rect);
	SCREENWIDTH = rect.right - rect.left;
	SCREENHEIGHT = rect.bottom - rect.top;
#else WINSTORE_SUPPORT
	SCREENWIDTH = (int)window->Bounds.Width;
	SCREENHEIGHT = (int)window->Bounds.Height;
#endif

	HRESULT hr = E_FAIL;

	for (int i = 0; i<GRAPHICSTHREAD_COUNT; i++) 
	{
		SAFE_INIT(commandLists[i]);
		SAFE_INIT(deviceContexts[i]);
	}

	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	ID3D11Device* _device = nullptr;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &_device
			, &featureLevel, &deviceContexts[GRAPHICSTHREAD_IMMEDIATE]);

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		stringstream ss("");
		ss << "Failed to create the graphics device! ERROR: " << std::hex << hr;
		wiHelper::messageBox(ss.str(), "Error!");
		exit(1);
	}

	hr = _device->QueryInterface(__uuidof(ID3D11Device3), (void**)&device);
	if (FAILED(hr))
	{
		stringstream ss("");
		ss << "Failed to create the graphics device! ERROR: " << std::hex << hr;
		wiHelper::messageBox(ss.str(), "Error!");
		exit(1);
	}
	SAFE_RELEASE(_device);

	D3D_FEATURE_LEVEL aquiredFeatureLevel = device->GetFeatureLevel();
	DX11 = ((aquiredFeatureLevel >= D3D_FEATURE_LEVEL_11_0) ? true : false);
	CONSERVATIVE_RASTERIZATION = aquiredFeatureLevel >= D3D_FEATURE_LEVEL_12_0;

	IDXGIDevice2 * pDXGIDevice;
	hr = device->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory2 * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&pIDXGIFactory);


	DXGI_SWAP_CHAIN_DESC1 sd = { 0 };
	sd.Width = SCREENWIDTH;
	sd.Height = SCREENHEIGHT;
	sd.Format = _ConvertFormat(GetBackBufferFormat());
	sd.Stereo = false;
	sd.SampleDesc.Count = 1; // Don't use multi-sampling.
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2; // Use double-buffering to minimize latency.
	sd.Flags = 0;
	sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

#ifndef WINSTORE_SUPPORT
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.Scaling = DXGI_SCALING_STRETCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	fullscreenDesc.RefreshRate.Numerator = 60;
	fullscreenDesc.RefreshRate.Denominator = 1;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // needs to be unspecified for correct fullscreen scaling!
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	fullscreenDesc.Windowed = !fullscreen;
	hr = pIDXGIFactory->CreateSwapChainForHwnd(device, window, &sd, &fullscreenDesc, nullptr, &swapChain);
#else
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
	sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

	hr = pIDXGIFactory->CreateSwapChainForCoreWindow(device, reinterpret_cast<IUnknown*>(window), &sd, nullptr, &swapChain);
#endif

	if (FAILED(hr)) 
	{
		wiHelper::messageBox("Failed to create a swapchain for the graphics device!", "Error!");
		exit(1);
	}

	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
	// ensures that the application will only render after each VSync, minimizing power consumption.
	hr = pDXGIDevice->SetMaximumFrameLatency(1);

	hr = deviceContexts[GRAPHICSTHREAD_IMMEDIATE]->QueryInterface(__uuidof(userDefinedAnnotations[GRAPHICSTHREAD_IMMEDIATE]),
		reinterpret_cast<void**>(&userDefinedAnnotations[GRAPHICSTHREAD_IMMEDIATE]));

	DEFERREDCONTEXT_SUPPORT = false;
	D3D11_FEATURE_DATA_THREADING threadingFeature;
	device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingFeature, sizeof(threadingFeature));
	if (threadingFeature.DriverConcurrentCreates && threadingFeature.DriverCommandLists) {
		DEFERREDCONTEXT_SUPPORT = true;
		for (int i = 0; i<GRAPHICSTHREAD_COUNT; i++) {
			if (i == (int)GRAPHICSTHREAD_IMMEDIATE)
				continue;
			hr = device->CreateDeferredContext(0, &deviceContexts[i]);
			hr = deviceContexts[i]->QueryInterface(__uuidof(userDefinedAnnotations[i]),
				reinterpret_cast<void**>(&userDefinedAnnotations[i]));
		}
	}
	else {
		DEFERREDCONTEXT_SUPPORT = false;
	}


	// Create a render target view
	backBuffer = NULL;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr)) {
		wiHelper::messageBox("BackBuffer creation Failed!", "Error!");
		exit(1);
	}

	hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	if (FAILED(hr)) {
		wiHelper::messageBox("Main Rendertarget creation Failed!", "Error!");
		exit(1);
	}

	// Setup the main viewport
	viewPort.Width = (FLOAT)SCREENWIDTH;
	viewPort.Height = (FLOAT)SCREENHEIGHT;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

}
GraphicsDevice_DX11::~GraphicsDevice_DX11()
{
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(swapChain);

	for (int i = 0; i<GRAPHICSTHREAD_COUNT; i++) {
		SAFE_RELEASE(commandLists[i]);
		SAFE_RELEASE(deviceContexts[i]);
	}

	SAFE_RELEASE(device);
}

void GraphicsDevice_DX11::SetResolution(int width, int height)
{
	if (width != SCREENWIDTH || height != SCREENHEIGHT)
	{
		SCREENWIDTH = width;
		SCREENHEIGHT = height;
		swapChain->ResizeBuffers(2, width, height, _ConvertFormat(GetBackBufferFormat()), 0);
		RESOLUTIONCHANGED = true;
	}
}

Texture2D GraphicsDevice_DX11::GetBackBuffer()
{
	Texture2D result;
	result.texture2D_DX11 = backBuffer;
	backBuffer->AddRef();
	return result;
}

bool GraphicsDevice_DX11::CheckCapability(GRAPHICSDEVICE_CAPABILITY capability)
{
	switch (capability)
	{
	case wiGraphicsTypes::GraphicsDevice::GRAPHICSDEVICE_CAPABILITY_TESSELLATION:
		return DX11;
		break;
	case wiGraphicsTypes::GraphicsDevice::GRAPHICSDEVICE_CAPABILITY_MULTITHREADED_RENDERING:
		return DEFERREDCONTEXT_SUPPORT;
		break;
	case wiGraphicsTypes::GraphicsDevice::GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION:
		return CONSERVATIVE_RASTERIZATION;
		break;
	default:
		break;
	}
	return false;
}

HRESULT GraphicsDevice_DX11::CreateBuffer(const GPUBufferDesc *pDesc, const SubresourceData* pInitialData, GPUBuffer *ppBuffer)
{
	D3D11_BUFFER_DESC desc; 
	desc.ByteWidth = pDesc->ByteWidth;
	desc.Usage = _ConvertUsage(pDesc->Usage);
	desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
	desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
	desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);
	desc.StructureByteStride = pDesc->StructureByteStride;

	D3D11_SUBRESOURCE_DATA* data = _ConvertSubresourceData(pInitialData);

	ppBuffer->desc = *pDesc;
	HRESULT hr = device->CreateBuffer(&desc, data, &ppBuffer->resource_DX11);
	assert(SUCCEEDED(hr) && "GPUBuffer creation failed!");

	if (SUCCEEDED(hr))
	{
		// Create resource views if needed
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			ZeroMemory(&srv_desc, sizeof(srv_desc));
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv_desc.BufferEx.FirstElement = 0;

			if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// This is a Raw Buffer

				srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
				srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				srv_desc.BufferEx.NumElements = desc.ByteWidth / 4;
			}
			else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				// This is a Structured Buffer

				srv_desc.Format = DXGI_FORMAT_UNKNOWN;
				srv_desc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}

			hr = device->CreateShaderResourceView(ppBuffer->resource_DX11, &srv_desc, &ppBuffer->shaderResourceView_DX11);

			assert(SUCCEEDED(hr) && "ShaderResourceView of the GPUBuffer could not be created!");
		}
		if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
			ZeroMemory(&uav_desc, sizeof(uav_desc));
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.FirstElement = 0;

			if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// This is a Raw Buffer

				uav_desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
				uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
				uav_desc.Buffer.NumElements = desc.ByteWidth / 4;
			}
			else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				// This is a Structured Buffer

				uav_desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
				uav_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}

			hr = device->CreateUnorderedAccessView(ppBuffer->resource_DX11, &uav_desc, &ppBuffer->unorderedAccessView_DX11);

			assert(SUCCEEDED(hr) && "UnorderedAccessView of the GPUBuffer could not be created!");
		}
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateTexture1D(const Texture1DDesc* pDesc, const SubresourceData *pInitialData, Texture1D **ppTexture1D)
{
	if ((*ppTexture1D) == nullptr)
	{
		(*ppTexture1D) = new Texture1D;
	}
	(*ppTexture1D)->desc = *pDesc;

	D3D11_TEXTURE1D_DESC desc = _ConvertTexture1DDesc(&(*ppTexture1D)->desc);
	D3D11_SUBRESOURCE_DATA* data = _ConvertSubresourceData(pInitialData);

	HRESULT hr = S_OK;

	hr = device->CreateTexture1D(&desc, data, &((*ppTexture1D)->texture1D_DX11));
	assert(SUCCEEDED(hr) && "Texture1D creation failed!");
	if (FAILED(hr))
		return hr;

	CreateShaderResourceView(*ppTexture1D);

	if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		assert((*ppTexture1D)->independentRTVArraySlices == false && "TextureArray UAV not implemented!");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		ZeroMemory(&uav_desc, sizeof(uav_desc));
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
		uav_desc.Texture2D.MipSlice = 0;

		hr = device->CreateUnorderedAccessView((*ppTexture1D)->texture1D_DX11, &uav_desc, &(*ppTexture1D)->unorderedAccessView_DX11);

		assert(SUCCEEDED(hr) && "UnorderedAccessView of the Texture1D could not be created!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateTexture2D(const Texture2DDesc* pDesc, const SubresourceData *pInitialData, Texture2D **ppTexture2D)
{
	if ((*ppTexture2D) == nullptr)
	{
		(*ppTexture2D) = new Texture2D;
	}
	(*ppTexture2D)->desc = *pDesc;

	if ((*ppTexture2D)->desc.SampleDesc.Count > 1)
	{
		UINT quality;
		device->CheckMultisampleQualityLevels(_ConvertFormat((*ppTexture2D)->desc.Format), (*ppTexture2D)->desc.SampleDesc.Count, &quality);
		(*ppTexture2D)->desc.SampleDesc.Quality = quality - 1;
		if (quality == 0)
		{
			assert(0 && "MSAA Samplecount not supported!");
			(*ppTexture2D)->desc.SampleDesc.Count = 1;
		}
	}

	D3D11_TEXTURE2D_DESC desc = _ConvertTexture2DDesc(&(*ppTexture2D)->desc);
	D3D11_SUBRESOURCE_DATA* data = _ConvertSubresourceData(pInitialData);

	HRESULT hr = S_OK;
	
	hr = device->CreateTexture2D(&desc, data, &((*ppTexture2D)->texture2D_DX11));
	assert(SUCCEEDED(hr) && "Texture2D creation failed!");
	if (FAILED(hr))
		return hr;

	CreateRenderTargetView(*ppTexture2D);
	CreateShaderResourceView(*ppTexture2D);
	CreateDepthStencilView(*ppTexture2D);


	if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		assert((*ppTexture2D)->independentRTVArraySlices == false && "TextureArray UAV not implemented!");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		ZeroMemory(&uav_desc, sizeof(uav_desc));
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uav_desc.Texture2D.MipSlice = 0;

		hr = device->CreateUnorderedAccessView((*ppTexture2D)->texture2D_DX11, &uav_desc, &(*ppTexture2D)->unorderedAccessView_DX11);

		assert(SUCCEEDED(hr) && "UnorderedAccessView of the Texture2D could not be created!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateTexture3D(const Texture3DDesc* pDesc, const SubresourceData *pInitialData, Texture3D **ppTexture3D)
{
	if ((*ppTexture3D) == nullptr)
	{
		(*ppTexture3D) = new Texture3D;
	}
	(*ppTexture3D)->desc = *pDesc;

	D3D11_TEXTURE3D_DESC desc = _ConvertTexture3DDesc(&(*ppTexture3D)->desc);
	D3D11_SUBRESOURCE_DATA* data = _ConvertSubresourceData(pInitialData);

	HRESULT hr = S_OK;

	hr = device->CreateTexture3D(&desc, data, &((*ppTexture3D)->texture3D_DX11));
	assert(SUCCEEDED(hr) && "Texture3D creation failed!");
	if (FAILED(hr))
		return hr;

	CreateShaderResourceView(*ppTexture3D);
	CreateRenderTargetView(*ppTexture3D);


	if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		ZeroMemory(&uav_desc, sizeof(uav_desc));
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uav_desc.Texture3D.MipSlice = 0;
		uav_desc.Texture3D.FirstWSlice = 0;
		uav_desc.Texture3D.WSize = desc.Depth;

		hr = device->CreateUnorderedAccessView((*ppTexture3D)->texture3D_DX11, &uav_desc, &(*ppTexture3D)->unorderedAccessView_DX11);

		assert(SUCCEEDED(hr) && "UnorderedAccessView of the Texture2D could not be created!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateShaderResourceView(Texture1D* pTexture)
{
	if (pTexture->shaderResourceView_DX11 != nullptr)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		UINT arraySize = pTexture->desc.ArraySize;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = _ConvertFormat(pTexture->desc.Format);

		if (arraySize > 1)
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			shaderResourceViewDesc.Texture1DArray.FirstArraySlice = 0;
			shaderResourceViewDesc.Texture1DArray.ArraySize = arraySize;
			shaderResourceViewDesc.Texture1DArray.MostDetailedMip = 0; //from most detailed...
			shaderResourceViewDesc.Texture1DArray.MipLevels = -1; //...to least detailed
		}
		else
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			shaderResourceViewDesc.Texture1D.MostDetailedMip = 0; //from most detailed...
			shaderResourceViewDesc.Texture1D.MipLevels = -1; //...to least detailed
		}

		hr = device->CreateShaderResourceView(pTexture->texture1D_DX11, &shaderResourceViewDesc, &pTexture->shaderResourceView_DX11);

		assert(SUCCEEDED(hr) && "ShaderResourceView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateShaderResourceView(Texture2D* pTexture)
{
	if (pTexture->shaderResourceView_DX11 != nullptr)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		UINT arraySize = pTexture->desc.ArraySize;
		UINT sampleCount = pTexture->desc.SampleDesc.Count;
		bool multisampled = sampleCount > 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		// Try to resolve shader resource format:
		switch (pTexture->desc.Format)
		{
		case FORMAT_R16_TYPELESS:
			shaderResourceViewDesc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case FORMAT_R32_TYPELESS:
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case FORMAT_R24G8_TYPELESS:
			shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		default:
			shaderResourceViewDesc.Format = _ConvertFormat(pTexture->desc.Format);
			break;
		}

		if (arraySize > 1)
		{
			if (pTexture->desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				if (arraySize > 6)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					shaderResourceViewDesc.TextureCubeArray.First2DArrayFace = 0;
					shaderResourceViewDesc.TextureCubeArray.NumCubes = arraySize / 6;
					shaderResourceViewDesc.TextureCubeArray.MostDetailedMip = 0; //from most detailed...
					shaderResourceViewDesc.TextureCubeArray.MipLevels = -1; //...to least detailed
				}
				else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					shaderResourceViewDesc.TextureCube.MostDetailedMip = 0; //from most detailed...
					shaderResourceViewDesc.TextureCube.MipLevels = -1; //...to least detailed
				}
			}
			else
			{
				if (multisampled)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					shaderResourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
					shaderResourceViewDesc.Texture2DMSArray.ArraySize = arraySize;
				}
				else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
					shaderResourceViewDesc.Texture2DArray.ArraySize = arraySize;
					shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0; //from most detailed...
					shaderResourceViewDesc.Texture2DArray.MipLevels = -1; //...to least detailed
				}
			}
		}
		else
		{
			if (multisampled)
			{
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0; //from most detailed...
				shaderResourceViewDesc.Texture2D.MipLevels = -1; //...to least detailed
			}
		}

		hr = device->CreateShaderResourceView(pTexture->texture2D_DX11, &shaderResourceViewDesc, &pTexture->shaderResourceView_DX11);

		assert(SUCCEEDED(hr) && "ShaderResourceView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateShaderResourceView(Texture3D* pTexture)
{
	if (pTexture->shaderResourceView_DX11 != nullptr)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = _ConvertFormat(pTexture->desc.Format);
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		shaderResourceViewDesc.Texture3D.MostDetailedMip = 0; //from most detailed...
		shaderResourceViewDesc.Texture3D.MipLevels = -1; //...to least detailed

		hr = device->CreateShaderResourceView(pTexture->texture3D_DX11, &shaderResourceViewDesc, &pTexture->shaderResourceView_DX11);

		assert(SUCCEEDED(hr) && "ShaderResourceView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateRenderTargetView(Texture2D* pTexture)
{
	if (!pTexture->renderTargetViews_DX11.empty())
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		UINT arraySize = pTexture->desc.ArraySize;
		UINT sampleCount = pTexture->desc.SampleDesc.Count;
		bool multisampled = sampleCount > 1;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = _ConvertFormat(pTexture->desc.Format);
		renderTargetViewDesc.Texture2DArray.MipSlice = 0;

		if (pTexture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
		{
			// TextureCube, TextureCubeArray...
			UINT slices = arraySize / 6;

			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			renderTargetViewDesc.Texture2DArray.MipSlice = 0;

			if (pTexture->independentRTVCubemapFaces)
			{
				// independent faces
				for (UINT i = 0; i < arraySize; ++i)
				{
					renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
					renderTargetViewDesc.Texture2DArray.ArraySize = 1;

					pTexture->renderTargetViews_DX11.push_back(nullptr);
					hr = device->CreateRenderTargetView(pTexture->texture2D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[i]);
				}
			}
			else
			{

				if (pTexture->independentRTVArraySlices)
				{
					// independent slices
					for (UINT i = 0; i < slices; ++i)
					{
						renderTargetViewDesc.Texture2DArray.FirstArraySlice = i * 6;
						renderTargetViewDesc.Texture2DArray.ArraySize = 6;

						pTexture->renderTargetViews_DX11.push_back(nullptr);
						hr = device->CreateRenderTargetView(pTexture->texture2D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[i]);
					}
				}
				else
				{
					// everything in one
					renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
					renderTargetViewDesc.Texture2DArray.ArraySize = arraySize;

					pTexture->renderTargetViews_DX11.push_back(nullptr);
					hr = device->CreateRenderTargetView(pTexture->texture2D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[0]);
				}
			}
		}
		else
		{
			// Texture2D, Texture2DArray...
			if (arraySize > 1 && pTexture->independentRTVArraySlices)
			{
				for (UINT i = 0; i < arraySize; ++i)
				{
					if (multisampled)
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
						renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = i;
						renderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
					}
					else
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
						renderTargetViewDesc.Texture2DArray.ArraySize = 1;
						renderTargetViewDesc.Texture2DArray.MipSlice = 0;
					}

					pTexture->renderTargetViews_DX11.push_back(nullptr);
					hr = device->CreateRenderTargetView(pTexture->texture2D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[i]);
				}
			}
			else
			{
				if (arraySize > 1)
				{
					if (multisampled)
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
						renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = 0;
						renderTargetViewDesc.Texture2DMSArray.ArraySize = arraySize;
					}
					else
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
						renderTargetViewDesc.Texture2DArray.ArraySize = arraySize;
						renderTargetViewDesc.Texture2DArray.MipSlice = 0;
					}
				}
				else
				{
					if (multisampled)
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
					}
					else
					{
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						renderTargetViewDesc.Texture2D.MipSlice = 0;
					}
				}

				pTexture->renderTargetViews_DX11.push_back(nullptr);
				hr = device->CreateRenderTargetView(pTexture->texture2D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[0]);
			}
		}
		
		assert(SUCCEEDED(hr) && "RenderTargetView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateRenderTargetView(Texture3D* pTexture)
{
	if (!pTexture->renderTargetViews_DX11.empty())
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = _ConvertFormat(pTexture->desc.Format);

		if (pTexture->independentRTVArraySlices)
		{
			for (UINT i = 0; i < pTexture->GetDesc().Depth; ++i)
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
				renderTargetViewDesc.Texture3D.MipSlice = 0;
				renderTargetViewDesc.Texture3D.FirstWSlice = i;
				renderTargetViewDesc.Texture3D.WSize = 1;

				pTexture->renderTargetViews_DX11.push_back(nullptr);
				hr = device->CreateRenderTargetView(pTexture->texture3D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[i]);
			}
		}
		else
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			renderTargetViewDesc.Texture3D.MipSlice = 0;
			renderTargetViewDesc.Texture3D.FirstWSlice = 0;
			renderTargetViewDesc.Texture3D.WSize = -1;

			pTexture->renderTargetViews_DX11.push_back(nullptr);
			hr = device->CreateRenderTargetView(pTexture->texture3D_DX11, &renderTargetViewDesc, &pTexture->renderTargetViews_DX11[0]);
		}

		assert(SUCCEEDED(hr) && "RenderTargetView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateDepthStencilView(Texture2D* pTexture)
{
	if (!pTexture->depthStencilViews_DX11.empty())
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	if (pTexture->desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		UINT arraySize = pTexture->desc.ArraySize;
		UINT sampleCount = pTexture->desc.SampleDesc.Count;
		bool multisampled = sampleCount > 1;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Texture2DArray.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;

		// Try to resolve depth stencil format:
		switch (pTexture->desc.Format)
		{
		case FORMAT_R16_TYPELESS:
			depthStencilViewDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
		case FORMAT_R32_TYPELESS:
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
		case FORMAT_R24G8_TYPELESS:
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			depthStencilViewDesc.Format = _ConvertFormat(pTexture->desc.Format);
			break;
		}

		if (pTexture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
		{
			// TextureCube, TextureCubeArray...
			UINT slices = arraySize / 6;

			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilViewDesc.Texture2DArray.MipSlice = 0;

			if (pTexture->independentRTVCubemapFaces)
			{
				// independent faces
				for (UINT i = 0; i < arraySize; ++i)
				{
					depthStencilViewDesc.Texture2DArray.FirstArraySlice = i;
					depthStencilViewDesc.Texture2DArray.ArraySize = 1;

					pTexture->depthStencilViews_DX11.push_back(nullptr);
					hr = device->CreateDepthStencilView(pTexture->texture2D_DX11, &depthStencilViewDesc, &pTexture->depthStencilViews_DX11[i]);
				}
			}
			else
			{

				if (pTexture->independentRTVArraySlices)
				{
					// independent slices
					for (UINT i = 0; i < slices; ++i)
					{
						depthStencilViewDesc.Texture2DArray.FirstArraySlice = i * 6;
						depthStencilViewDesc.Texture2DArray.ArraySize = 6;

						pTexture->depthStencilViews_DX11.push_back(nullptr);
						hr = device->CreateDepthStencilView(pTexture->texture2D_DX11, &depthStencilViewDesc, &pTexture->depthStencilViews_DX11[i]);
					}
				}
				else
				{
					// everything in one
					depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
					depthStencilViewDesc.Texture2DArray.ArraySize = arraySize;

					pTexture->depthStencilViews_DX11.push_back(nullptr);
					hr = device->CreateDepthStencilView(pTexture->texture2D_DX11, &depthStencilViewDesc, &pTexture->depthStencilViews_DX11[0]);
				}
			}
		}
		else
		{
			// Texture2D, Texture2DArray...
			if (arraySize > 1 && pTexture->independentRTVArraySlices)
			{
				for (UINT i = 0; i < arraySize; ++i)
				{
					if (multisampled)
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
						depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = i;
						depthStencilViewDesc.Texture2DMSArray.ArraySize = 1;
					}
					else
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
						depthStencilViewDesc.Texture2DArray.MipSlice = 0;
						depthStencilViewDesc.Texture2DArray.FirstArraySlice = i;
						depthStencilViewDesc.Texture2DArray.ArraySize = 1;
					}

					pTexture->depthStencilViews_DX11.push_back(nullptr);
					hr = device->CreateDepthStencilView(pTexture->texture2D_DX11, &depthStencilViewDesc, &pTexture->depthStencilViews_DX11[i]);
				}
			}
			else
			{
				if (arraySize > 1)
				{
					if (multisampled)
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
						depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = 0;
						depthStencilViewDesc.Texture2DMSArray.ArraySize = arraySize;
					}
					else
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
						depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
						depthStencilViewDesc.Texture2DArray.ArraySize = arraySize;
						depthStencilViewDesc.Texture2DArray.MipSlice = 0;
					}
				}
				else
				{
					if (multisampled)
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
					}
					else
					{
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
						depthStencilViewDesc.Texture2D.MipSlice = 0;
					}
				}

				pTexture->depthStencilViews_DX11.push_back(nullptr);
				hr = device->CreateDepthStencilView(pTexture->texture2D_DX11, &depthStencilViewDesc, &pTexture->depthStencilViews_DX11[0]);
			}
		}

		assert(SUCCEEDED(hr) && "DepthStencilView Creation failed!");
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateInputLayout(const VertexLayoutDesc *pInputElementDescs, UINT NumElements,
	const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, VertexLayout *pInputLayout)
{
	D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[NumElements];
	for (UINT i = 0; i < NumElements; ++i)
	{
		desc[i].SemanticName = pInputElementDescs[i].SemanticName;
		desc[i].SemanticIndex = pInputElementDescs[i].SemanticIndex;
		desc[i].Format = _ConvertFormat(pInputElementDescs[i].Format);
		desc[i].InputSlot = pInputElementDescs[i].InputSlot;
		desc[i].AlignedByteOffset = pInputElementDescs[i].AlignedByteOffset;
		if (desc[i].AlignedByteOffset == APPEND_ALIGNED_ELEMENT)
			desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[i].InputSlotClass = _ConvertInputClassification(pInputElementDescs[i].InputSlotClass);
		desc[i].InstanceDataStepRate = pInputElementDescs[i].InstanceDataStepRate;
	}

	HRESULT hr = device->CreateInputLayout(desc, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, &pInputLayout->resource_DX11);

	SAFE_DELETE_ARRAY(desc);

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateVertexShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, VertexShader *pVertexShader)
{
	return device->CreateVertexShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pVertexShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreatePixelShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, PixelShader *pPixelShader)
{
	return device->CreatePixelShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pPixelShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateGeometryShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, GeometryShader *pGeometryShader)
{
	return device->CreateGeometryShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pGeometryShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateGeometryShaderWithStreamOutput(const void *pShaderBytecode, SIZE_T BytecodeLength, const StreamOutDeclaration *pSODeclaration,
	UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ClassLinkage* pClassLinkage, GeometryShader *pGeometryShader)
{
	D3D11_SO_DECLARATION_ENTRY* decl = new D3D11_SO_DECLARATION_ENTRY[NumEntries];
	for (UINT i = 0; i < NumEntries; ++i)
	{
		decl[i].Stream = pSODeclaration[i].Stream;
		decl[i].SemanticName = pSODeclaration[i].SemanticName;
		decl[i].SemanticIndex = pSODeclaration[i].SemanticIndex;
		decl[i].StartComponent = pSODeclaration[i].StartComponent;
		decl[i].ComponentCount = pSODeclaration[i].ComponentCount;
		decl[i].OutputSlot = pSODeclaration[i].OutputSlot;
	}

	if (RasterizedStream == SO_NO_RASTERIZED_STREAM)
		RasterizedStream = D3D11_SO_NO_RASTERIZED_STREAM;

	HRESULT hr = device->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, decl, NumEntries, pBufferStrides,
		NumStrides, RasterizedStream, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pGeometryShader->resource_DX11);

	SAFE_DELETE_ARRAY(decl);

	return hr;
}
HRESULT GraphicsDevice_DX11::CreateHullShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, HullShader *pHullShader)
{
	return device->CreateHullShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pHullShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateDomainShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, DomainShader *pDomainShader)
{
	return device->CreateDomainShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pDomainShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateComputeShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ClassLinkage* pClassLinkage, ComputeShader *pComputeShader)
{
	return device->CreateComputeShader(pShaderBytecode, BytecodeLength, (pClassLinkage == nullptr ? nullptr : pClassLinkage->resource_DX11), &pComputeShader->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateBlendState(const BlendStateDesc *pBlendStateDesc, BlendState *pBlendState)
{
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = pBlendStateDesc->AlphaToCoverageEnable;
	desc.IndependentBlendEnable = pBlendStateDesc->IndependentBlendEnable;
	for (int i = 0; i < 8; ++i)
	{
		desc.RenderTarget[i].BlendEnable = pBlendStateDesc->RenderTarget[i].BlendEnable;
		desc.RenderTarget[i].SrcBlend = _ConvertBlend(pBlendStateDesc->RenderTarget[i].SrcBlend);
		desc.RenderTarget[i].DestBlend = _ConvertBlend(pBlendStateDesc->RenderTarget[i].DestBlend);
		desc.RenderTarget[i].BlendOp = _ConvertBlendOp(pBlendStateDesc->RenderTarget[i].BlendOp);
		desc.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(pBlendStateDesc->RenderTarget[i].SrcBlendAlpha);
		desc.RenderTarget[i].DestBlendAlpha = _ConvertBlend(pBlendStateDesc->RenderTarget[i].DestBlendAlpha);
		desc.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(pBlendStateDesc->RenderTarget[i].BlendOpAlpha);
		desc.RenderTarget[i].RenderTargetWriteMask = _ParseColorWriteMask(pBlendStateDesc->RenderTarget[i].RenderTargetWriteMask);
	}

	pBlendState->desc = *pBlendStateDesc;
	return device->CreateBlendState(&desc, &pBlendState->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateDepthStencilState(const DepthStencilStateDesc *pDepthStencilStateDesc, DepthStencilState *pDepthStencilState)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = pDepthStencilStateDesc->DepthEnable;
	desc.DepthWriteMask = _ConvertDepthWriteMask(pDepthStencilStateDesc->DepthWriteMask);
	desc.DepthFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->DepthFunc);
	desc.StencilEnable = pDepthStencilStateDesc->StencilEnable;
	desc.StencilReadMask = pDepthStencilStateDesc->StencilReadMask;
	desc.StencilWriteMask = pDepthStencilStateDesc->StencilWriteMask;
	desc.FrontFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilDepthFailOp);
	desc.FrontFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilFailOp);
	desc.FrontFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->FrontFace.StencilFunc);
	desc.FrontFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilPassOp);
	desc.BackFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilDepthFailOp);
	desc.BackFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilFailOp);
	desc.BackFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->BackFace.StencilFunc);
	desc.BackFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilPassOp);

	pDepthStencilState->desc = *pDepthStencilStateDesc;
	return device->CreateDepthStencilState(&desc, &pDepthStencilState->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateRasterizerState(const RasterizerStateDesc *pRasterizerStateDesc, RasterizerState *pRasterizerState)
{
	D3D11_RASTERIZER_DESC2 desc;
	desc.FillMode = _ConvertFillMode(pRasterizerStateDesc->FillMode);
	desc.CullMode = _ConvertCullMode(pRasterizerStateDesc->CullMode);
	desc.FrontCounterClockwise = pRasterizerStateDesc->FrontCounterClockwise;
	desc.DepthBias = pRasterizerStateDesc->DepthBias;
	desc.DepthBiasClamp = pRasterizerStateDesc->DepthBiasClamp;
	desc.SlopeScaledDepthBias = pRasterizerStateDesc->SlopeScaledDepthBias;
	desc.DepthClipEnable = pRasterizerStateDesc->DepthClipEnable;
	desc.ScissorEnable = pRasterizerStateDesc->ScissorEnable;
	desc.MultisampleEnable = pRasterizerStateDesc->MultisampleEnable;
	desc.AntialiasedLineEnable = pRasterizerStateDesc->AntialiasedLineEnable;
	desc.ConservativeRaster = (pRasterizerStateDesc->ConservativeRasterizationEnable == TRUE ? D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	desc.ForcedSampleCount = 0;

	pRasterizerState->desc = *pRasterizerStateDesc;
	return device->CreateRasterizerState2(&desc, &pRasterizerState->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateSamplerState(const SamplerDesc *pSamplerDesc, Sampler *pSamplerState)
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = _ConvertFilter(pSamplerDesc->Filter);
	desc.AddressU = _ConvertTextureAddressMode(pSamplerDesc->AddressU);
	desc.AddressV = _ConvertTextureAddressMode(pSamplerDesc->AddressV);
	desc.AddressW = _ConvertTextureAddressMode(pSamplerDesc->AddressW);
	desc.MipLODBias = pSamplerDesc->MipLODBias;
	desc.MaxAnisotropy = pSamplerDesc->MaxAnisotropy;
	desc.ComparisonFunc = _ConvertComparisonFunc(pSamplerDesc->ComparisonFunc);
	desc.BorderColor[0] = pSamplerDesc->BorderColor[0];
	desc.BorderColor[1] = pSamplerDesc->BorderColor[1];
	desc.BorderColor[2] = pSamplerDesc->BorderColor[2];
	desc.BorderColor[3] = pSamplerDesc->BorderColor[3];
	desc.MinLOD = pSamplerDesc->MinLOD;
	desc.MaxLOD = pSamplerDesc->MaxLOD;

	pSamplerState->desc = *pSamplerDesc;
	return device->CreateSamplerState(&desc, &pSamplerState->resource_DX11);
}
HRESULT GraphicsDevice_DX11::CreateQuery(const GPUQueryDesc *pDesc, GPUQuery *pQuery)
{
	pQuery->desc = *pDesc;
	pQuery->async_frameshift = pQuery->desc.async_latency;

	D3D11_QUERY_DESC desc;
	desc.MiscFlags = 0;
	desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
	if (pDesc->Type == GPU_QUERY_TYPE_OCCLUSION)
	{
		desc.Query = D3D11_QUERY_OCCLUSION;
	}
	else if (pDesc->Type == GPU_QUERY_TYPE_TIMESTAMP)
	{
		desc.Query = D3D11_QUERY_TIMESTAMP;
	}
	else if (pDesc->Type == GPU_QUERY_TYPE_TIMESTAMP_DISJOINT)
	{
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	}

	HRESULT hr = E_FAIL;
	if (pQuery->desc.async_latency > 0)
	{
		pQuery->resource_DX11.resize(pQuery->desc.async_latency + 1);
		pQuery->active.resize(pQuery->desc.async_latency + 1);
		for (size_t i = 0; i < pQuery->resource_DX11.size(); ++i)
		{
			hr = device->CreateQuery(&desc, &pQuery->resource_DX11[i]);
			assert(SUCCEEDED(hr) && "GPUQuery creation failed!");
		}
	}
	else
	{
		pQuery->resource_DX11.resize(1);
		pQuery->active.resize(1);
		hr = device->CreateQuery(&desc, &pQuery->resource_DX11[0]);
		assert(SUCCEEDED(hr) && "GPUQuery creation failed!");
	}

	return hr;
}


void GraphicsDevice_DX11::PresentBegin()
{
	LOCK();

	BindViewports(1, &viewPort);
	deviceContexts[GRAPHICSTHREAD_IMMEDIATE]->OMSetRenderTargets(1, &renderTargetView, 0);
	float ClearColor[4] = { 0, 0, 0, 1.0f }; // red,green,blue,alpha
	deviceContexts[GRAPHICSTHREAD_IMMEDIATE]->ClearRenderTargetView(renderTargetView, ClearColor);

}
void GraphicsDevice_DX11::PresentEnd()
{
	swapChain->Present(VSYNC, 0);


	deviceContexts[GRAPHICSTHREAD_IMMEDIATE]->OMSetRenderTargets(0, nullptr, nullptr);

	UnBindResources(0, TEXSLOT_COUNT, GRAPHICSTHREAD_IMMEDIATE);

	FRAMECOUNT++;

	RESOLUTIONCHANGED = false;

	UNLOCK();
}

void GraphicsDevice_DX11::ExecuteDeferredContexts()
{
	for (int i = 0; i < GRAPHICSTHREAD_COUNT; i++)
	{
		if (i != GRAPHICSTHREAD_IMMEDIATE && commandLists[i] != nullptr)
		{
			deviceContexts[GRAPHICSTHREAD_IMMEDIATE]->ExecuteCommandList(commandLists[i], true);
			commandLists[i]->Release();
			commandLists[i] = nullptr;

			UnBindResources(0, TEXSLOT_COUNT, (GRAPHICSTHREAD)i);
		}
	}
}
void GraphicsDevice_DX11::FinishCommandList(GRAPHICSTHREAD thread)
{
	if (thread == GRAPHICSTHREAD_IMMEDIATE)
		return;
	deviceContexts[thread]->FinishCommandList(true, &commandLists[thread]);
}

void GraphicsDevice_DX11::BindViewports(UINT NumViewports, const ViewPort *pViewports, GRAPHICSTHREAD threadID) 
{
	D3D11_VIEWPORT* pd3dViewPorts = new D3D11_VIEWPORT[NumViewports];
	for (UINT i = 0; i < NumViewports; ++i)
	{
		pd3dViewPorts[i].TopLeftX = pViewports[i].TopLeftX;
		pd3dViewPorts[i].TopLeftY = pViewports[i].TopLeftY;
		pd3dViewPorts[i].Width = pViewports[i].Width;
		pd3dViewPorts[i].Height = pViewports[i].Height;
		pd3dViewPorts[i].MinDepth = pViewports[i].MinDepth;
		pd3dViewPorts[i].MaxDepth = pViewports[i].MaxDepth;
	}
	deviceContexts[threadID]->RSSetViewports(NumViewports, pd3dViewPorts);
	SAFE_DELETE_ARRAY(pd3dViewPorts);
}
void GraphicsDevice_DX11::BindRenderTargets(UINT NumViews, Texture* const *ppRenderTargets, Texture2D* depthStencilTexture, GRAPHICSTHREAD threadID, UINT arrayIndex)
{
	ID3D11RenderTargetView* renderTargetViews[8];
	ZeroMemory(renderTargetViews, sizeof(renderTargetViews));
	for (UINT i = 0; i < min(NumViews, 8); ++i)
	{
		assert(ppRenderTargets[i]->renderTargetViews_DX11.size() > arrayIndex && "Invalid rendertarget arrayIndex!");
		renderTargetViews[i] = ppRenderTargets[i]->renderTargetViews_DX11[arrayIndex];
	}
	assert(depthStencilTexture == nullptr || depthStencilTexture->depthStencilViews_DX11.size() > arrayIndex && "Invalid depthstencil arrayIndex!");
	ID3D11DepthStencilView* depthStencilView = (depthStencilTexture == nullptr ? nullptr : depthStencilTexture->depthStencilViews_DX11[arrayIndex]);
	deviceContexts[threadID]->OMSetRenderTargets(NumViews, renderTargetViews, depthStencilView);
}
void GraphicsDevice_DX11::ClearRenderTarget(Texture* pTexture, const FLOAT ColorRGBA[4], GRAPHICSTHREAD threadID, UINT arrayIndex)
{
	assert(pTexture->renderTargetViews_DX11.size() > arrayIndex && "Invalid rendertarget arrayIndex!");
	deviceContexts[threadID]->ClearRenderTargetView(pTexture->renderTargetViews_DX11[arrayIndex], ColorRGBA);
}
void GraphicsDevice_DX11::ClearDepthStencil(Texture2D* pTexture, UINT ClearFlags, FLOAT Depth, UINT8 Stencil, GRAPHICSTHREAD threadID, UINT arrayIndex)
{
	assert(pTexture->depthStencilViews_DX11.size() > arrayIndex && "Invalid depthstencil arrayIndex!");
	UINT _flags = 0;
	if (ClearFlags & CLEAR_DEPTH)
		_flags |= D3D11_CLEAR_DEPTH;
	if (ClearFlags & CLEAR_STENCIL)
		_flags |= D3D11_CLEAR_STENCIL;
	deviceContexts[threadID]->ClearDepthStencilView(pTexture->depthStencilViews_DX11[arrayIndex], _flags, Depth, Stencil);
}
void GraphicsDevice_DX11::BindResourcePS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID) 
{
	if (resource != nullptr)
		deviceContexts[threadID]->PSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindResourceVS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID)
{
	if (resource != nullptr)
		deviceContexts[threadID]->VSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindResourceGS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID)
{
	if (resource != nullptr)
		deviceContexts[threadID]->GSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindResourceDS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID) 
{
	if (resource != nullptr)
		deviceContexts[threadID]->DSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindResourceHS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID)
{
	if (resource != nullptr)
		deviceContexts[threadID]->HSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindResourceCS(const GPUResource* resource, int slot, GRAPHICSTHREAD threadID) 
{
	if (resource != nullptr)
		deviceContexts[threadID]->CSSetShaderResources(slot, 1, &resource->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::BindUnorderedAccessResourceCS(const GPUUnorderedResource* resource, int slot, GRAPHICSTHREAD threadID)
{
	if (resource != nullptr)
		deviceContexts[threadID]->CSSetUnorderedAccessViews(slot, 1, &resource->unorderedAccessView_DX11, nullptr);
}
void GraphicsDevice_DX11::UnBindResources(int slot, int num, GRAPHICSTHREAD threadID)
{
	assert(num <= ARRAYSIZE(__nullBlob) && "Extend nullBlob to support more resource unbinding!");
	deviceContexts[threadID]->PSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	deviceContexts[threadID]->VSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	deviceContexts[threadID]->GSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	deviceContexts[threadID]->HSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	deviceContexts[threadID]->DSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	deviceContexts[threadID]->CSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
}
void GraphicsDevice_DX11::UnBindUnorderedAccessResources(int slot, int num, GRAPHICSTHREAD threadID)
{
	assert(num <= ARRAYSIZE(__nullBlob) && "Extend nullBlob to support more resource unbinding!");
	deviceContexts[threadID]->CSSetUnorderedAccessViews(slot, num, (ID3D11UnorderedAccessView**)__nullBlob, 0);
}
void GraphicsDevice_DX11::BindSamplerPS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->PSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindSamplerVS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->VSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindSamplerGS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->GSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindSamplerHS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->HSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindSamplerDS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->DSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindSamplerCS(const Sampler* sampler, int slot, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->CSSetSamplers(slot, 1, &sampler->resource_DX11);
}
void GraphicsDevice_DX11::BindConstantBufferPS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID)
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->PSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindConstantBufferVS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID)
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->VSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindConstantBufferGS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID)
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->GSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindConstantBufferDS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID)
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->DSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindConstantBufferHS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID) 
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->HSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindConstantBufferCS(const GPUBuffer* buffer, int slot, GRAPHICSTHREAD threadID) 
{
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->CSSetConstantBuffers(slot, 1, &res);
}
void GraphicsDevice_DX11::BindVertexBuffer(const GPUBuffer* vertexBuffer, int slot, UINT stride, GRAPHICSTHREAD threadID) 
{
	UINT offset = 0;
	ID3D11Buffer* res = vertexBuffer ? vertexBuffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->IASetVertexBuffers(slot, 1, &res, &stride, &offset);
}
void GraphicsDevice_DX11::BindIndexBuffer(const GPUBuffer* indexBuffer, GRAPHICSTHREAD threadID)
{
	ID3D11Buffer* res = indexBuffer ? indexBuffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->IASetIndexBuffer(res, DXGI_FORMAT_R32_UINT, 0);
}
void GraphicsDevice_DX11::BindPrimitiveTopology(PRIMITIVETOPOLOGY type, GRAPHICSTHREAD threadID)
{
	D3D11_PRIMITIVE_TOPOLOGY d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (type)
	{
	case TRIANGLELIST:
		d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case TRIANGLESTRIP:
		d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case POINTLIST:
		d3dType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case LINELIST:
		d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PATCHLIST:
		d3dType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		break;
	default:
		break;
	};
	deviceContexts[threadID]->IASetPrimitiveTopology(d3dType);
}
void GraphicsDevice_DX11::BindVertexLayout(const VertexLayout* layout, GRAPHICSTHREAD threadID)
{
	ID3D11InputLayout* res = layout ? layout->resource_DX11 : nullptr;
	deviceContexts[threadID]->IASetInputLayout(res);
}
void GraphicsDevice_DX11::BindBlendState(const BlendState* state, GRAPHICSTHREAD threadID)
{
	static float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static UINT sampleMask = 0xffffffff;
	deviceContexts[threadID]->OMSetBlendState(state->resource_DX11, blendFactor, sampleMask);
}
void GraphicsDevice_DX11::BindBlendStateEx(const BlendState* state, const XMFLOAT4& blendFactor, UINT sampleMask, GRAPHICSTHREAD threadID) 
{
	float fblendFactor[4] = { blendFactor.x, blendFactor.y, blendFactor.z, blendFactor.w };
	deviceContexts[threadID]->OMSetBlendState(state->resource_DX11, fblendFactor, sampleMask);
}
void GraphicsDevice_DX11::BindDepthStencilState(const DepthStencilState* state, UINT stencilRef, GRAPHICSTHREAD threadID) 
{
	deviceContexts[threadID]->OMSetDepthStencilState(state->resource_DX11, stencilRef);
}
void GraphicsDevice_DX11::BindRasterizerState(const RasterizerState* state, GRAPHICSTHREAD threadID) 
{
	deviceContexts[threadID]->RSSetState(state->resource_DX11);
}
void GraphicsDevice_DX11::BindStreamOutTarget(const GPUBuffer* buffer, GRAPHICSTHREAD threadID) 
{
	UINT offsetSO[1] = { 0 };
	ID3D11Buffer* res = buffer ? buffer->resource_DX11 : nullptr;
	deviceContexts[threadID]->SOSetTargets(1, &res, offsetSO);
}
void GraphicsDevice_DX11::BindPS(const PixelShader* shader, GRAPHICSTHREAD threadID) 
{
	ID3D11PixelShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->PSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::BindVS(const VertexShader* shader, GRAPHICSTHREAD threadID) 
{
	ID3D11VertexShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->VSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::BindGS(const GeometryShader* shader, GRAPHICSTHREAD threadID)
{
	ID3D11GeometryShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->GSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::BindHS(const HullShader* shader, GRAPHICSTHREAD threadID)
{
	ID3D11HullShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->HSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::BindDS(const DomainShader* shader, GRAPHICSTHREAD threadID)
{
	ID3D11DomainShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->DSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::BindCS(const ComputeShader* shader, GRAPHICSTHREAD threadID)
{
	ID3D11ComputeShader* res = shader ? shader->resource_DX11 : nullptr;
	deviceContexts[threadID]->CSSetShader(res, nullptr, 0);
}
void GraphicsDevice_DX11::Draw(int vertexCount, GRAPHICSTHREAD threadID) 
{
	deviceContexts[threadID]->Draw(vertexCount, 0);
}
void GraphicsDevice_DX11::DrawIndexed(int indexCount, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->DrawIndexed(indexCount, 0, 0);
}
void GraphicsDevice_DX11::DrawInstanced(int vertexCount, int instanceCount, GRAPHICSTHREAD threadID) 
{
	deviceContexts[threadID]->DrawInstanced(vertexCount, instanceCount, 0, 0);
}
void GraphicsDevice_DX11::DrawIndexedInstanced(int indexCount, int instanceCount, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}
void GraphicsDevice_DX11::Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void GraphicsDevice_DX11::GenerateMips(Texture* texture, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->GenerateMips(texture->shaderResourceView_DX11);
}
void GraphicsDevice_DX11::CopyTexture2D(Texture2D* pDst, const Texture2D* pSrc, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->CopyResource(pDst->texture2D_DX11, pSrc->texture2D_DX11);
}
void GraphicsDevice_DX11::CopyTexture2D_Region(Texture2D* pDst, UINT dstMip, UINT dstX, UINT dstY, const Texture2D* pSrc, UINT srcMip, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->CopySubresourceRegion(pDst->texture2D_DX11, D3D11CalcSubresource(dstMip, 0, pDst->GetDesc().MipLevels), dstX, dstY, 0, 
		pSrc->texture2D_DX11, D3D11CalcSubresource(srcMip, 0, pSrc->GetDesc().MipLevels), nullptr);
}
void GraphicsDevice_DX11::MSAAResolve(Texture2D* pDst, const Texture2D* pSrc, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->ResolveSubresource(pDst->texture2D_DX11, 0, pSrc->texture2D_DX11, 0, _ConvertFormat(pDst->desc.Format));
}
void GraphicsDevice_DX11::UpdateBuffer(GPUBuffer* buffer, const void* data, GRAPHICSTHREAD threadID, int dataSize)
{
	assert(buffer->desc.Usage != USAGE_IMMUTABLE && "Cannot update IMMUTABLE GPUBuffer!");
	HRESULT hr;
	if (dataSize > (int)buffer->desc.ByteWidth) 
	{
		// grow the buffer if new datasize exceeds buffer size
		buffer->resource_DX11->Release();
		buffer->desc.ByteWidth = dataSize * 2;
		hr = CreateBuffer(&buffer->desc, nullptr, buffer);
		assert(SUCCEEDED(hr) && "GPUBuffer growing failed!");
	}

	if (buffer->desc.Usage == USAGE_DYNAMIC) 
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = deviceContexts[threadID]->Map(buffer->resource_DX11, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(hr) && "GPUBuffer mapping failed!");
		memcpy(mappedResource.pData, data, (dataSize >= 0 ? dataSize : buffer->desc.ByteWidth));
		deviceContexts[threadID]->Unmap(buffer->resource_DX11, 0);
	}
	else 
	{
		deviceContexts[threadID]->UpdateSubresource(buffer->resource_DX11, 0, nullptr, data, 0, 0);
	}
}
GPUBuffer* GraphicsDevice_DX11::DownloadBuffer(GPUBuffer* buffer, GRAPHICSTHREAD threadID) 
{
	GPUBuffer* debugbuf = new GPUBuffer;

	GPUBufferDesc desc = buffer->GetDesc();
	desc.CPUAccessFlags = CPU_ACCESS_READ;
	desc.Usage = USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(CreateBuffer(&desc, nullptr, debugbuf)))
	{
		deviceContexts[threadID]->CopyResource(debugbuf->resource_DX11, buffer->resource_DX11);
	}

	return debugbuf;
}
void GraphicsDevice_DX11::Map(GPUBuffer* resource, UINT subResource, MAP mapType, UINT mapFlags, MappedSubresource* mappedResource, GRAPHICSTHREAD threadID) {
	assert(mapFlags == 0 && "MapFlags not implemented!");
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	D3D11_MAP d3dMapType = D3D11_MAP_WRITE_DISCARD;
	switch (mapType)
	{
	case wiGraphicsTypes::MAP_READ:
		d3dMapType = D3D11_MAP_READ;
		break;
	case wiGraphicsTypes::MAP_WRITE:
		d3dMapType = D3D11_MAP_WRITE;
		break;
	case wiGraphicsTypes::MAP_READ_WRITE:
		d3dMapType = D3D11_MAP_READ_WRITE;
		break;
	case wiGraphicsTypes::MAP_WRITE_DISCARD:
		d3dMapType = D3D11_MAP_WRITE_DISCARD;
		break;
	case wiGraphicsTypes::MAP_WRITE_NO_OVERWRITE:
		d3dMapType = D3D11_MAP_WRITE_NO_OVERWRITE;
		break;
	default:
		break;
	}
	HRESULT hr = deviceContexts[threadID]->Map(resource->resource_DX11, subResource, d3dMapType, mapFlags, &d3dMappedResource);
	mappedResource->pData = d3dMappedResource.pData;
	mappedResource->DepthPitch = d3dMappedResource.DepthPitch;
	mappedResource->RowPitch = d3dMappedResource.RowPitch;
}
void GraphicsDevice_DX11::Unmap(GPUBuffer* resource, UINT subResource, GRAPHICSTHREAD threadID) 
{
	deviceContexts[threadID]->Unmap(resource->resource_DX11, subResource);
}
void GraphicsDevice_DX11::SetScissorRects(UINT numRects, const Rect* rects, GRAPHICSTHREAD threadID)
{
	if (rects != nullptr)
	{
		D3D11_RECT* pRects = new D3D11_RECT[numRects];
		for (UINT i = 0; i < numRects; ++i)
		{
			pRects[i].bottom = rects[i].bottom;
			pRects[i].left = rects[i].left;
			pRects[i].right = rects[i].right;
			pRects[i].top = rects[i].top;
		}
		deviceContexts[threadID]->RSSetScissorRects(numRects, pRects);
		SAFE_DELETE(pRects);
	}
	else
	{
		deviceContexts[threadID]->RSSetScissorRects(numRects, nullptr);
	}
}

void GraphicsDevice_DX11::QueryBegin(GPUQuery *query, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->Begin(query->resource_DX11[query->async_frameshift]);
	query->active[query->async_frameshift] = true;
}
void GraphicsDevice_DX11::QueryEnd(GPUQuery *query, GRAPHICSTHREAD threadID)
{
	deviceContexts[threadID]->End(query->resource_DX11[query->async_frameshift]);
	query->active[query->async_frameshift] = true;
}
bool GraphicsDevice_DX11::QueryRead(GPUQuery *query, GRAPHICSTHREAD threadID)
{
	query->async_frameshift = (query->async_frameshift + 1) % (query->desc.async_latency + 1);
	const int _readQueryID = query->async_frameshift;
	const UINT _flags = (query->desc.async_latency > 0 ? D3D11_ASYNC_GETDATA_DONOTFLUSH : 0);

	if (!query->active[_readQueryID])
	{
		return true;
	}

	assert(threadID == GRAPHICSTHREAD_IMMEDIATE && "A query can only be read on the immediate graphics thread!");

	HRESULT hr = S_OK;
	switch (query->desc.Type)
	{
	case GPU_QUERY_TYPE_TIMESTAMP:
		hr = deviceContexts[threadID]->GetData(query->resource_DX11[_readQueryID], &query->result_timestamp, sizeof(query->result_timestamp), _flags);
		break;
	case GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
		{
			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT _temp;
			hr = deviceContexts[threadID]->GetData(query->resource_DX11[_readQueryID], &_temp, sizeof(_temp), _flags);
			query->result_disjoint = _temp.Disjoint;
			query->result_timestamp_frequency = _temp.Frequency;
		}
		break;
	case GPU_QUERY_TYPE_OCCLUSION:
		hr = deviceContexts[threadID]->GetData(query->resource_DX11[_readQueryID], &query->result_passed_sample_count, sizeof(query->result_passed_sample_count), _flags);
		query->result_passed = query->result_passed_sample_count != 0;
		break;
	case GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
	default:
		hr = deviceContexts[threadID]->GetData(query->resource_DX11[_readQueryID], &query->result_passed, sizeof(query->result_passed), _flags);
		break;
	}

	query->active[_readQueryID] = false;

	return hr != S_FALSE;
}


HRESULT GraphicsDevice_DX11::CreateTextureFromFile(const string& fileName, Texture2D **ppTexture, bool mipMaps, GRAPHICSTHREAD threadID)
{
	HRESULT hr = E_FAIL;
	(*ppTexture) = new Texture2D();

	if (!fileName.substr(fileName.length() - 4).compare(string(".dds")))
	{
		// Load dds
		hr = CreateDDSTextureFromFile(device, wstring(fileName.begin(),fileName.end()).c_str(), (ID3D11Resource**)&(*ppTexture)->texture2D_DX11, &(*ppTexture)->shaderResourceView_DX11);
	}
	else
	{
		// Load WIC
		if (mipMaps && threadID == GRAPHICSTHREAD_IMMEDIATE)
			LOCK();
		hr = CreateWICTextureFromFile(mipMaps, device, deviceContexts[threadID], wstring(fileName.begin(), fileName.end()).c_str(), (ID3D11Resource**)&(*ppTexture)->texture2D_DX11, &(*ppTexture)->shaderResourceView_DX11);
		if (mipMaps && threadID == GRAPHICSTHREAD_IMMEDIATE)
			UNLOCK();
	}

	if (FAILED(hr)) {
		SAFE_DELETE(*ppTexture);
	}
	else {
		D3D11_TEXTURE2D_DESC desc;
		(*ppTexture)->texture2D_DX11->GetDesc(&desc);
		(*ppTexture)->desc = _ConvertTexture2DDesc_Inv(&desc);
	}

	return hr;
}
HRESULT GraphicsDevice_DX11::SaveTexturePNG(const string& fileName, Texture2D *pTexture, GRAPHICSTHREAD threadID)
{
	Texture2D* tex2D = dynamic_cast<Texture2D*>(pTexture);
	if (tex2D != nullptr)
	{
		return SaveWICTextureToFile(deviceContexts[threadID], pTexture->texture2D_DX11, GUID_ContainerFormatPng, wstring(fileName.begin(), fileName.end()).c_str());
	}
	return E_FAIL;
}
HRESULT GraphicsDevice_DX11::SaveTextureDDS(const string& fileName, Texture *pTexture, GRAPHICSTHREAD threadID)
{
	Texture2D* tex2D = dynamic_cast<Texture2D*>(pTexture);
	if (tex2D != nullptr)
	{
		return SaveDDSTextureToFile(deviceContexts[threadID], tex2D->texture2D_DX11, wstring(fileName.begin(), fileName.end()).c_str());
	}
	return E_FAIL;
}

void GraphicsDevice_DX11::EventBegin(const string& name, GRAPHICSTHREAD threadID)
{
	userDefinedAnnotations[threadID]->BeginEvent(wstring(name.begin(), name.end()).c_str());
}
void GraphicsDevice_DX11::EventEnd(GRAPHICSTHREAD threadID)
{
	userDefinedAnnotations[threadID]->EndEvent();
}
void GraphicsDevice_DX11::SetMarker(const string& name, GRAPHICSTHREAD threadID)
{
	userDefinedAnnotations[threadID]->SetMarker(wstring(name.begin(),name.end()).c_str());
}

}
