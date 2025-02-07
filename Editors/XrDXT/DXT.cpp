// DXT.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dds/ddsTypes.h"
#include "dds.h"
#include "nvtt.h"

#pragma comment(lib, "nvtt.lib")

using namespace nvtt;

const u32 fcc_DXT1 = MAKEFOURCC('D', 'X', 'T', '1');
const u32 fcc_DXT2 = MAKEFOURCC('D', 'X', 'T', '2');
const u32 fcc_DXT3 = MAKEFOURCC('D', 'X', 'T', '3');
const u32 fcc_DXT4 = MAKEFOURCC('D', 'X', 'T', '4');
const u32 fcc_DXT5 = MAKEFOURCC('D', 'X', 'T', '5');

struct dds_writer : public OutputHandler
{
	dds_writer(HFILE &fileout);

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel);
	virtual bool writeData(const void *data, int size);
	virtual void endImage();
	HFILE &w;
};

inline dds_writer::dds_writer(HFILE &_fileout) : w(_fileout) {}

void dds_writer::beginImage(int size, int width, int height, int depth, int face, int miplevel) {}
void dds_writer::endImage() {}

bool dds_writer::writeData(const void *data, int size)
{
	if (size == sizeof(DDS_HEADER))
	{
		// correct DDS header
		DDS_HEADER *hdr = (DDS_HEADER *)data;

		if (hdr->dwSize == size)
		{
			switch (hdr->ddspf.dwFourCC)
			{
			case fcc_DXT1:
			case fcc_DXT2:
			case fcc_DXT3:
			case fcc_DXT4:
			case fcc_DXT5:
				hdr->ddspf.dwRGBBitCount = 0;
				break;
			}
		}
	}

	_write(w, data, size);
	return true;
}

struct dds_error : public ErrorHandler
{
	virtual void error(Error e);
};

void dds_error::error(Error e)
{
	switch (e)
	{
	case Error_Unknown:
		MessageBox(0, "Unknown error", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	case Error_InvalidInput:
		MessageBox(0, "Invalid input", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	case Error_UnsupportedFeature:
		MessageBox(0, "Unsupported feature", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	case Error_CudaError:
		MessageBox(0, "CUDA error", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	case Error_FileOpen:
		MessageBox(0, "File open error", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	case Error_FileWrite:
		MessageBox(0, "File write error", "DXT compress error", MB_ICONERROR | MB_OK);
		break;
	}
}

ENGINE_API u32 *Build32MipLevel(u32 &_w, u32 &_h, u32 &_p, u32 *pdwPixelSrc, STextureParams *fmt, float blend)
{
	R_ASSERT(pdwPixelSrc);
	R_ASSERT((_w % 2) == 0);
	R_ASSERT((_h % 2) == 0);
	R_ASSERT((_p % 4) == 0);

	u32 dwDestPitch = (_w / 2) * 4;
	u32 *pNewData = xr_alloc<u32>((_h / 2) * dwDestPitch);
	u8 *pDest = (u8 *)pNewData;
	u8 *pSrc = (u8 *)pdwPixelSrc;

	float mixed_a = (float)u8(fmt->fade_color >> 24);
	float mixed_r = (float)u8(fmt->fade_color >> 16);
	float mixed_g = (float)u8(fmt->fade_color >> 8);
	float mixed_b = (float)u8(fmt->fade_color >> 0);

	float inv_blend = 1.f - blend;

	for (u32 y = 0; y < _h; y += 2)
	{
		u8 *pScanline = pSrc + y * _p;

		for (u32 x = 0; x < _w; x += 2)
		{
			u8 *p1 = pScanline + x * 4;
			u8 *p2 = p1 + 4;

			if (1 == _w)
				p2 = p1;

			u8 *p3 = p1 + _p;

			if (1 == _h)
				p3 = p1;

			u8 *p4 = p2 + _p;

			if (1 == _h)
				p4 = p2;

			float c_r = float(u32(p1[0]) + u32(p2[0]) + u32(p3[0]) + u32(p4[0])) / 4.f;
			float c_g = float(u32(p1[1]) + u32(p2[1]) + u32(p3[1]) + u32(p4[1])) / 4.f;
			float c_b = float(u32(p1[2]) + u32(p2[2]) + u32(p3[2]) + u32(p4[2])) / 4.f;
			float c_a = float(u32(p1[3]) + u32(p2[3]) + u32(p3[3]) + u32(p4[3])) / 4.f;

			if (fmt->flags.is(STextureParams::flFadeToColor))
			{
				c_r = c_r * inv_blend + mixed_r * blend;
				c_g = c_g * inv_blend + mixed_g * blend;
				c_b = c_b * inv_blend + mixed_b * blend;
			}

			if (fmt->flags.is(STextureParams::flFadeToAlpha))
				c_a = c_a * inv_blend + mixed_a * blend;

			float A = (c_a + c_a / 8.f);
			int _r = int(c_r);
			clamp(_r, 0, 255);
			*pDest++ = u8(_r);
			int _g = int(c_g);
			clamp(_g, 0, 255);
			*pDest++ = u8(_g);
			int _b = int(c_b);
			clamp(_b, 0, 255);
			*pDest++ = u8(_b);
			int _a = int(A);
			clamp(_a, 0, 255);
			*pDest++ = u8(_a);
		}
	}

	_w /= 2;
	_h /= 2;
	_p = _w * 4;
	return pNewData;
}

IC u32 GetPowerOf2Plus1(u32 v)
{
	u32 cnt = 0;
	while (v)
	{
		v >>= 1;
		cnt++;
	};
	return cnt;
}

void FillRect(u8 *data, u8 *new_data, u32 offs, u32 pitch, u32 h, u32 full_pitch)
{
	for (u32 i = 0; i < h; i++)
		CopyMemory(data + (full_pitch * i + offs), new_data + i * pitch, pitch);
}

int DXTCompressImage(LPCSTR out_name, u8 *raw_data, u32 w, u32 h, u32 pitch, STextureParams *fmt, u32 depth)
{
	R_ASSERT((0 != w) && (0 != h));

	HFILE gFileOut = _open(out_name, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC, _S_IWRITE);

	if (gFileOut == -1)
	{
		fprintf(stderr, "Can't open output file %s\n", out_name);
		return false;
	}

	bool result = false;
	InputOptions in_opts;
	in_opts.setTextureLayout((fmt->type == STextureParams::ttCubeMap) ? TextureType_Cube : TextureType_2D, w, h);

	if (fmt->flags.is(STextureParams::flGenerateMipMaps))
		in_opts.setMipmapGeneration(true);
	else
		in_opts.setMipmapGeneration(false);

	switch (fmt->mip_filter)
	{
		// KD: in all the projects used default mipmap filter - triangle
	case STextureParams::kMIPFilterBox:
		in_opts.setMipmapFilter(MipmapFilter_Box);
		break;
	case STextureParams::kMIPFilterTriangle:
		in_opts.setMipmapFilter(MipmapFilter_Triangle);
		break;
	case STextureParams::kMIPFilterKaiser:
		in_opts.setMipmapFilter(MipmapFilter_Kaiser);
		break;
	}

	in_opts.setWrapMode(WrapMode_Clamp);
	in_opts.setNormalMap(false);
	in_opts.setConvertToNormalMap(false);
	in_opts.setGamma(2.2f, 2.2f);
	in_opts.setNormalizeMipmaps(false);

	CompressionOptions comp_opts;

	switch (fmt->fmt)
	{
	case STextureParams::tfDXT1:
		comp_opts.setFormat(Format_DXT1);
		break;
	case STextureParams::tfADXT1:
		comp_opts.setFormat(Format_DXT1a);
		break;
	case STextureParams::tfDXT3:
		comp_opts.setFormat(Format_DXT3);
		break;
	case STextureParams::tfDXT5:
		comp_opts.setFormat(Format_DXT5);
		break;
	case STextureParams::tfRGB:
		comp_opts.setFormat(Format_RGB);
		break;
	case STextureParams::tfRGBA:
		comp_opts.setFormat(Format_RGBA);
		break;
	}

	comp_opts.setQuality(Quality_Highest);
	comp_opts.setQuantization(!!(fmt->flags.is(STextureParams::flDitherColor)), false, !!(fmt->flags.is(STextureParams::flBinaryAlpha)));

	HFILE fileout = _open(out_name, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC, _S_IWRITE);

	if (fileout == -1)
	{
		fprintf(stderr, "Can't open output file %s\n", out_name);
		return false;
	}

	OutputOptions out_opts;

	dds_writer dds(gFileOut);
	out_opts.setOutputHandler(&dds);
	dds_error dds_e;
	out_opts.setErrorHandler(&dds_e);

	if ((fmt->flags.is(STextureParams::flGenerateMipMaps)) && (STextureParams::kMIPFilterAdvanced == fmt->mip_filter))
	{
		in_opts.setMipmapGeneration(false);
		u8 *pImagePixels = 0;
		int numMipmaps = GetPowerOf2Plus1(__min(w, h));
		u32 line_pitch = w * 2 * 4;
		pImagePixels = xr_alloc<u8>(line_pitch * h);
		u32 w_offs = 0;
		u32 dwW = w;
		u32 dwH = h;
		u32 dwP = pitch;
		u32 *pLastMip = xr_alloc<u32>(w * h * 4);
		CopyMemory(pLastMip, raw_data, w * h * 4);
		FillRect(pImagePixels, (u8 *)pLastMip, w_offs, pitch, dwH, line_pitch);
		w_offs += dwP;

		float inv_fade = clampr(1.f - float(fmt->fade_amount) / 100.f, 0.f, 1.f);
		float blend = fmt->flags.is_any(STextureParams::flFadeToColor | STextureParams::flFadeToAlpha) ? inv_fade : 1.f;

		for (int i = 1; i < numMipmaps; i++)
		{
			u32 *pNewMip = Build32MipLevel(dwW, dwH, dwP, pLastMip, fmt, i < fmt->fade_delay ? 0.f : 1.f - blend);
			FillRect(pImagePixels, (u8 *)pNewMip, w_offs, dwP, dwH, line_pitch);
			xr_free(pLastMip);
			pLastMip = pNewMip;
			pNewMip = 0;
			w_offs += dwP;
		}

		xr_free(pLastMip);

		RGBAImage pImage(w * 2, h);
		rgba_t *pixels = pImage.pixels();
		u8 *pixel = pImagePixels;

		for (u32 k = 0; k < w * 2 * h; k++, pixel += 4)
			pixels[k].set(pixel[0], pixel[1], pixel[2], pixel[3]);

		in_opts.setMipmapData(pixels, w, h);
		result = Compressor().process(in_opts, comp_opts, out_opts);
		xr_free(pImagePixels);
	}
	else
	{
		RGBAImage pImage(w, h);
		rgba_t *pixels = pImage.pixels();
		u8 *pixel = raw_data;

		for (u32 k = 0; k < w * h; k++, pixel += 4)
			pixels[k].set(pixel[0], pixel[1], pixel[2], pixel[3]);

		in_opts.setMipmapData(pixels, w, h);
		result = Compressor().process(in_opts, comp_opts, out_opts);
	}

	_close(gFileOut);

	if (result == false)
	{
		unlink(out_name);
		return 0;
	}
	else
		return 1;
}

/*
int DXTCompressImage	(LPCSTR out_name, u8* raw_data, u32 w, u32 h, u32 pitch, STextureParams* fmt, u32 depth)
{
	// TSMP: todo
	MessageBoxA(0, "TSMP: DXTCompressImage not implemented!", "Error", 0);
	return false;
	/*CTimer T; T.Start();

	Msg("DXT: Compressing Image: %s %uX%u", out_name, w, h);

	R_ASSERT(0 != w && 0 != h);
	BearImage Image;
	BearTexturePixelFormat Format = BearTexturePixelFormat::R8G8B8A8;
	switch (fmt->fmt)
	{
		case STextureParams::tfDXT1: 	Format = BearTexturePixelFormat::BC1; 	  break;
		case STextureParams::tfADXT1:	Format = BearTexturePixelFormat::BC1a; 	  break;
		case STextureParams::tfDXT3: 	Format = BearTexturePixelFormat::BC2; 	  break;
		case STextureParams::tfDXT5: 	Format = BearTexturePixelFormat::BC3;	  break;
		case STextureParams::tfBC4: 	Format = BearTexturePixelFormat::BC4;	  break;
		case STextureParams::tfBC5: 	Format = BearTexturePixelFormat::BC5;	  break;
		case STextureParams::tfBC6: 	Format = BearTexturePixelFormat::BC6;	  break;
		case STextureParams::tfBC7: 	Format = BearTexturePixelFormat::BC7;	  break;
		case STextureParams::tfRGB: 	Format = BearTexturePixelFormat::R8G8B8;  break;
		case STextureParams::tfRGBA: 	Format = BearTexturePixelFormat::R8G8B8A8;break;
	}

	Image.Create(w, h, 1, 1, BearTexturePixelFormat::R8G8B8A8);
	bear_copy(*Image, raw_data, w * h * 4);
	Image.SwapRB();
	BearResizeFilter ResizeFilter = BearResizeFilter::Default;
	switch (fmt->mip_filter)
	{
	case STextureParams::kMIPFilterBox:       ResizeFilter = BearResizeFilter::Box;     break;
	case STextureParams::kMIPFilterTriangle:    ResizeFilter = BearResizeFilter::Triangle; break;
	case STextureParams::kMIPFilterKaiser:     ResizeFilter = BearResizeFilter::Catmullrom;   break;
	}
	Image.GenerateMipmap(ResizeFilter);
	Image.Convert(Format);
	Msg("DXT: Compressing Image: 2 [Closing File]. Time from start %f ms", T.GetElapsed_sec() * 1000.f);
	return Image.SaveToDds(out_name);;
}
*/

extern int DXTCompressBump(LPCSTR out_name, u8 *raw_data, u8 *normal_map, u32 w, u32 h, u32 pitch, STextureParams *fmt, u32 depth);

extern "C" __declspec(dllexport) int DXTCompress(LPCSTR out_name, u8 *raw_data, u8 *normal_map, u32 w, u32 h, u32 pitch,
												 STextureParams *fmt, u32 depth)
{
	switch (fmt->type)
	{
	case STextureParams::ttImage:
	case STextureParams::ttCubeMap:
	case STextureParams::ttNormalMap:
	case STextureParams::ttTerrain:
		return DXTCompressImage(out_name, raw_data, w, h, pitch, fmt, depth);
		break;
	case STextureParams::ttBumpMap:
		return DXTCompressBump(out_name, raw_data, normal_map, w, h, pitch, fmt, depth);
		break;
	default:
		NODEFAULT;
	}
	return -1;
}
