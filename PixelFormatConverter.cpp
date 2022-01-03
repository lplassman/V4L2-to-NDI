#include <cstring>
#include "PixelFormatConverter.h"


#define MIN_FRAME_WIDTH 4
#define MIN_FRAME_HEIGHT 4


zs::PixelFormatConverter::PixelFormatConverter() {


}


zs::PixelFormatConverter::~PixelFormatConverter() {


}


bool zs::PixelFormatConverter::Convert(Frame& src, Frame& dst) {

	if (src.height % 2 != 0)
		return false;

	switch (src.fourcc) {

	case (uint32_t)ValidFourccCodes::RGB24:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return RGB24_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return RGB24_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return RGB24_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return RGB24_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return RGB24_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return RGB24_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::BGR24:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return BGR24_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return BGR24_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return BGR24_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return BGR24_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return BGR24_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return BGR24_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::UYVY:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return UYVY_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return UYVY_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return UYVY_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return UYVY_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return UYVY_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return UYVY_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::YUY2:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return YUY2_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return YUY2_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return YUY2_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return YUY2_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return YUY2_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return YUY2_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::Y800:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return Y800_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return Y800_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return Y800_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return Y800_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return Y800_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return Y800_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::NV12:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return NV12_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return NV12_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return NV12_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return NV12_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return NV12_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return Copy(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return NV12_to_YUV1(src, dst);
		default:
			return false;
		}
		break;

	case (uint32_t)ValidFourccCodes::YUV1:
		switch (dst.fourcc) {
		case (uint32_t)ValidFourccCodes::RGB24:
			return YUV1_to_RGB24(src, dst);
		case (uint32_t)ValidFourccCodes::BGR24:
			return YUV1_to_BGR24(src, dst);
		case (uint32_t)ValidFourccCodes::UYVY:
			return YUV1_to_UYVY(src, dst);
		case (uint32_t)ValidFourccCodes::YUY2:
			return YUV1_to_YUY2(src, dst);
		case (uint32_t)ValidFourccCodes::Y800:
			return YUV1_to_Y800(src, dst);
		case (uint32_t)ValidFourccCodes::NV12:
			return YUV1_to_NV12(src, dst);
		case (uint32_t)ValidFourccCodes::YUV1:
			return Copy(src, dst);
		default:
			return false;
		}
		break;

	default:
		return false;
	}

	return false;

}


void zs::PixelFormatConverter::GetVersion(uint32_t& major, uint32_t& minor) {

	major = majorVersion;
	minor = minorVersion;

}


bool zs::PixelFormatConverter::Copy(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size < src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.size != src.size || dst.data == nullptr) {

		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[src.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.fourcc = src.fourcc;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	memcpy(dst.data, src.data, src.size);

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_BGR24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {
		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	for (size_t i = 0; i < (size_t)src.size; i = i + 3) {

		dst.data[i] = src.data[i + 2];
		dst.data[i + 1] = src.data[i + 1];
		dst.data[i + 2] = src.data[i];

	}

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R0, G0, B0, R1, G1, B1, Y0, Y1, U, V;
	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		R0 = (float)src.data[i];
		G0 = (float)src.data[i + 1];
		B0 = (float)src.data[i + 2];
		R1 = (float)src.data[i + 3];
		G1 = (float)src.data[i + 4];
		B1 = (float)src.data[i + 5];

		Y0 = 0.299f * R0 + 0.587f * G0 + 0.114f * B0;
		Y1 = 0.299f * R1 + 0.587f * G1 + 0.114f * B1;
		U = 0.492f * (B0 - Y0) + 128.0f;
		V = 0.877f * (R0 - Y0) + 128.0f;

		dst.data[j + 1] = (uint8_t)Y0;
		dst.data[j + 3] = (uint8_t)Y1;
		dst.data[j] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[j + 2] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		j += 4;
	}

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R0, G0, B0, R1, G1, B1, Y0, Y1, U, V;
	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		R0 = (float)src.data[i];
		G0 = (float)src.data[i + 1];
		B0 = (float)src.data[i + 2];
		R1 = (float)src.data[i + 3];
		G1 = (float)src.data[i + 4];
		B1 = (float)src.data[i + 5];

		Y0 = 0.299f * R0 + 0.587f * G0 + 0.114f * B0;
		Y1 = 0.299f * R1 + 0.587f * G1 + 0.114f * B1;
		U = 0.492f * (B0 - Y0) + 128.0f;
		V = 0.877f * (R0 - Y0) + 128.0f;

		dst.data[j] = (uint8_t)Y0;
		dst.data[j + 2] = (uint8_t)Y1;
		dst.data[j + 1] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[j + 3] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		j += 4;
	}

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_Y800(Frame& src, Frame& dst) {
	
	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 3) {

		dst.data[j] = (uint8_t)(0.299f * (float)src.data[i] +
			0.587f * (float)src.data[i + 1] +
			0.114f * (float)src.data[i + 2]);
		++j;

	}

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R00, G00, B00, R01, G01, B01, R10, G10, B10, R11, G11, B11, Y00, Y01, Y10, Y11, U, V;
	size_t pos;
	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			pos = i * (size_t)src.width * 3 + j * 3;
			R00 = (float)src.data[pos];
			G00 = (float)src.data[pos + 1];
			B00 = (float)src.data[pos + 2];

			pos = pos + 3;
			R01 = (float)src.data[pos];
			G01 = (float)src.data[pos + 1];
			B01 = (float)src.data[pos + 2];

			pos = (i + 1) * (size_t)src.width * 3 + j * 3;
			R10 = (float)src.data[pos];
			G10 = (float)src.data[pos + 1];
			B10 = (float)src.data[pos + 2];

			pos = pos + 3;
			R11 = (float)src.data[pos];
			G11 = (float)src.data[pos + 1];
			B11 = (float)src.data[pos + 2];

			Y00 = 0.299f * R00 + 0.587f * G00 + 0.114f * B00;
			Y01 = 0.299f * R01 + 0.587f * G01 + 0.114f * B01;
			Y10 = 0.299f * R10 + 0.587f * G10 + 0.114f * B10;
			Y11 = 0.299f * R11 + 0.587f * G11 + 0.114f * B11;

			dst.data[i * dst.width + j] = (uint8_t)Y00;
			dst.data[i * dst.width + j + 1] = (uint8_t)Y01;
			dst.data[(i + 1) * dst.width + j] = (uint8_t)Y10;
			dst.data[(i + 1) * dst.width + j + 1] = (uint8_t)Y11;

			U = 0.492f * (B00 - Y00) + 128.0f;
			V = 0.877f * (R00 - Y00) + 128.0f;

			dst.data[(dst.height + p) * dst.width + j] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
			dst.data[(dst.height + p) * dst.width + j + 1] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		}

		++p;

	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {

		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	for (size_t i = 0; i < (size_t)src.size; i = i + 3) {

		dst.data[i] = src.data[i + 2];
		dst.data[i + 1] = src.data[i + 1];
		dst.data[i + 2] = src.data[i];

	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R0, G0, B0, R1, G1, B1, Y0, Y1, U, V;
	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		B0 = (float)src.data[i];
		G0 = (float)src.data[i + 1];
		R0 = (float)src.data[i + 2];
		B1 = (float)src.data[i + 3];
		G1 = (float)src.data[i + 4];
		R1 = (float)src.data[i + 5];

		Y0 = 0.299f * R0 + 0.587f * G0 + 0.114f * B0;
		Y1 = 0.299f * R1 + 0.587f * G1 + 0.114f * B1;
		U = 0.492f * (B0 - Y0) + 128.0f;
		V = 0.877f * (R0 - Y0) + 128.0f;

		dst.data[j + 1] = (uint8_t)Y0;
		dst.data[j + 3] = (uint8_t)Y1;
		dst.data[j] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[j + 2] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		j += 4;
	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R0, G0, B0, R1, G1, B1, Y0, Y1, U, V;
	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		B0 = (float)src.data[i];
		G0 = (float)src.data[i + 1];
		R0 = (float)src.data[i + 2];
		B1 = (float)src.data[i + 3];
		G1 = (float)src.data[i + 4];
		R1 = (float)src.data[i + 5];

		Y0 = 0.299f * R0 + 0.587f * G0 + 0.114f * B0;
		Y1 = 0.299f * R1 + 0.587f * G1 + 0.114f * B1;
		U = 0.492f * (B0 - Y0) + 128.0f;
		V = 0.877f * (R0 - Y0) + 128.0f;

		dst.data[j] = (uint8_t)Y0;
		dst.data[j + 2] = (uint8_t)Y1;
		dst.data[j + 1] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[j + 3] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		j += 4;
	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_Y800(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 3) {

		dst.data[j] = (uint8_t)(0.299f * (float)src.data[i + 2] +
			0.587f * (float)src.data[i + 1] +
			0.114f * (float)src.data[i]);
		++j;

	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R00, G00, B00, R01, G01, B01, R10, G10, B10, R11, G11, B11, Y00, Y01, Y10, Y11, U, V;
	size_t pos;
	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			pos = i * (size_t)src.width * 3 + j * 3;
			B00 = (float)src.data[pos];
			G00 = (float)src.data[pos + 1];
			R00 = (float)src.data[pos + 2];

			pos = pos + 3;
			B01 = (float)src.data[pos];
			G01 = (float)src.data[pos + 1];
			R01 = (float)src.data[pos + 2];

			pos = (i + 1) * (size_t)src.width * 3 + j * 3;
			B10 = (float)src.data[pos];
			G10 = (float)src.data[pos + 1];
			R10 = (float)src.data[pos + 2];

			pos = pos + 3;
			B11 = (float)src.data[pos];
			G11 = (float)src.data[pos + 1];
			R11 = (float)src.data[pos + 2];

			Y00 = 0.299f * R00 + 0.587f * G00 + 0.114f * B00;
			Y01 = 0.299f * R01 + 0.587f * G01 + 0.114f * B01;
			Y10 = 0.299f * R10 + 0.587f * G10 + 0.114f * B10;
			Y11 = 0.299f * R11 + 0.587f * G11 + 0.114f * B11;

			dst.data[i * dst.width + j] = (uint8_t)Y00;
			dst.data[i * dst.width + j + 1] = (uint8_t)Y01;
			dst.data[(i + 1) * dst.width + j] = (uint8_t)Y10;
			dst.data[(i + 1) * dst.width + j + 1] = (uint8_t)Y11;

			U = 0.492f * (B00 - Y00) + 128.0f;
			V = 0.877f * (R00 - Y00) + 128.0f;

			dst.data[(dst.height + p) * dst.width + j] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
			dst.data[(dst.height + p) * dst.width + j + 1] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

		}

		++p;

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	float Y0, Y1, U, V, R0, G0, B0, R1, G1, B1;
	for (size_t i = 0; i < (size_t)src.size; i = i + 4) {

		U = (float)src.data[i];
		Y0 = (float)src.data[i + 1];
		V = (float)src.data[i + 2];
		Y1 = (float)src.data[i + 3];


		R0 = Y0 + 1.140f * (V - 128.0f);
		G0 = Y0 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B0 = Y0 + 2.032f * (U - 128.0f);

		R1 = Y1 + 1.140f * (V - 128.0f);
		G1 = Y1 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B1 = Y1 + 2.032f * (U - 128.0f);

		dst.data[j]     = R0 > 255.0f ? 255 : R0 < 0.0f ? 0 : (uint8_t)R0;
		dst.data[j + 1] = G0 > 255.0f ? 255 : G0 < 0.0f ? 0 : (uint8_t)G0;
		dst.data[j + 2] = B0 > 255.0f ? 255 : B0 < 0.0f ? 0 : (uint8_t)B0;

		dst.data[j + 3] = R1 > 255.0f ? 255 : R1 < 0.0f ? 0 : (uint8_t)R1;
		dst.data[j + 4] = G1 > 255.0f ? 255 : G1 < 0.0f ? 0 : (uint8_t)G1;
		dst.data[j + 5] = B1 > 255.0f ? 255 : B1 < 0.0f ? 0 : (uint8_t)B1;

		j += 6;

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_BGR24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	float Y0, Y1, U, V, R0, G0, B0, R1, G1, B1;
	for (size_t i = 0; i < (size_t)src.size; i = i + 4) {

		U = (float)src.data[i];
		Y0 = (float)src.data[i + 1];
		V = (float)src.data[i + 2];
		Y1 = (float)src.data[i + 3];


		R0 = Y0 + 1.140f * (V - 128.0f);
		G0 = Y0 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B0 = Y0 + 2.032f * (U - 128.0f);

		R1 = Y1 + 1.140f * (V - 128.0f);
		G1 = Y1 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B1 = Y1 + 2.032f * (U - 128.0f);

		dst.data[j + 2] = R0 > 255.0f ? 255 : R0 < 0.0f ? 0 : (uint8_t)R0;
		dst.data[j + 1] = G0 > 255.0f ? 255 : G0 < 0.0f ? 0 : (uint8_t)G0;
		dst.data[j]     = B0 > 255.0f ? 255 : B0 < 0.0f ? 0 : (uint8_t)B0;

		dst.data[j + 5] = R1 > 255.0f ? 255 : R1 < 0.0f ? 0 : (uint8_t)R1;
		dst.data[j + 4] = G1 > 255.0f ? 255 : G1 < 0.0f ? 0 : (uint8_t)G1;
		dst.data[j + 3] = B1 > 255.0f ? 255 : B1 < 0.0f ? 0 : (uint8_t)B1;

		j += 6;

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {

		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	for (size_t i = 0; i < (size_t)src.size; i = i + 4) {

		dst.data[i] = src.data[i + 1];
		dst.data[i + 1] = src.data[i];
		dst.data[i + 2] = src.data[i + 3];
		dst.data[i + 3] = src.data[i + 2];

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_Y800(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 1; i < (size_t)src.size; i = i + 2) {
		
		dst.data[j] = src.data[i];
		++j;

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			dst.data[i * src.width + j] = src.data[i * src.width * 2 + j * 2 + 1];
			dst.data[i * src.width + j + 1] = src.data[i * src.width * 2 + j * 2 + 3];
			dst.data[(i + 1) * src.width + j] = src.data[(i + 1) * src.width * 2 + j * 2 + 1];
			dst.data[(i + 1) * src.width + j + 1] = src.data[(i + 1) * src.width * 2 + j * 2 + 3];
			dst.data[(src.height + p) * src.width + j] = src.data[i * src.width * 2 + j * 2];
			dst.data[(src.height + p) * src.width + j + 1] = src.data[i * src.width * 2 + j * 2 + 2];

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::YUY2_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	float Y0, Y1, U, V, R0, G0, B0, R1, G1, B1;
	for (size_t i = 0; i < (size_t)src.size; i = i + 4) {

		U = (float)src.data[i + 1];
		Y0 = (float)src.data[i];
		V = (float)src.data[i + 3];
		Y1 = (float)src.data[i + 2];


		R0 = Y0 + 1.140f * (V - 128.0f);
		G0 = Y0 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B0 = Y0 + 2.032f * (U - 128.0f);

		R1 = Y1 + 1.140f * (V - 128.0f);
		G1 = Y1 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B1 = Y1 + 2.032f * (U - 128.0f);

		dst.data[j] = R0 > 255.0f ? 255 : R0 < 0.0f ? 0 : (uint8_t)R0;
		dst.data[j + 1] = G0 > 255.0f ? 255 : G0 < 0.0f ? 0 : (uint8_t)G0;
		dst.data[j + 2] = B0 > 255.0f ? 255 : B0 < 0.0f ? 0 : (uint8_t)B0;

		dst.data[j + 3] = R1 > 255.0f ? 255 : R1 < 0.0f ? 0 : (uint8_t)R1;
		dst.data[j + 4] = G1 > 255.0f ? 255 : G1 < 0.0f ? 0 : (uint8_t)G1;
		dst.data[j + 5] = B1 > 255.0f ? 255 : B1 < 0.0f ? 0 : (uint8_t)B1;

		j += 6;

	}

	return true;

}


bool zs::PixelFormatConverter::YUY2_to_BGR24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	float Y0, Y1, U, V, R0, G0, B0, R1, G1, B1;
	for (size_t i = 0; i < (size_t)src.size; i = i + 4) {

		U = (float)src.data[i + 1];
		Y0 = (float)src.data[i];
		V = (float)src.data[i + 3];
		Y1 = (float)src.data[i + 2];


		R0 = Y0 + 1.140f * (V - 128.0f);
		G0 = Y0 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B0 = Y0 + 2.032f * (U - 128.0f);

		R1 = Y1 + 1.140f * (V - 128.0f);
		G1 = Y1 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B1 = Y1 + 2.032f * (U - 128.0f);

		dst.data[j + 2] = R0 > 255.0f ? 255 : R0 < 0.0f ? 0 : (uint8_t)R0;
		dst.data[j + 1] = G0 > 255.0f ? 255 : G0 < 0.0f ? 0 : (uint8_t)G0;
		dst.data[j]     = B0 > 255.0f ? 255 : B0 < 0.0f ? 0 : (uint8_t)B0;

		dst.data[j + 5] = R1 > 255.0f ? 255 : R1 < 0.0f ? 0 : (uint8_t)R1;
		dst.data[j + 4] = G1 > 255.0f ? 255 : G1 < 0.0f ? 0 : (uint8_t)G1;
		dst.data[j + 3] = B1 > 255.0f ? 255 : B1 < 0.0f ? 0 : (uint8_t)B1;

		j += 6;

	}

	return true;

}


bool zs::PixelFormatConverter::YUY2_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {

		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	for (size_t i = 0; i < (size_t)src.size / 4; i++) {

		const uint32_t yuy2 = ((uint32_t*)src.data)[i];
		uint32_t uyvy = (yuy2 >> 8) & 0x00ff00ff;
		uyvy |= ( yuy2 & 0x00ff00ff ) << 8;
		((uint32_t*)dst.data)[i] = uyvy;
	}

	return true;

}


bool zs::PixelFormatConverter::YUY2_to_Y800(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 2) {

		dst.data[j] = src.data[i];
		++j;

	}

	return true;

}//bool zs::PixelFormatConverter::YUY2_to_Y800...


bool zs::PixelFormatConverter::YUY2_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			dst.data[i * src.width + j] = src.data[i * src.width * 2 + j * 2];
			dst.data[i * src.width + j + 1] = src.data[i * src.width * 2 + j * 2 + 2];
			dst.data[(i + 1) * src.width + j] = src.data[(i + 1) * src.width * 2 + j * 2];
			dst.data[(i + 1) * src.width + j + 1] = src.data[(i + 1) * src.width * 2 + j * 2 + 2];
			dst.data[(src.height + p) * src.width + j] = src.data[i * src.width * 2 + j * 2 + 1];
			dst.data[(src.height + p) * src.width + j + 1] = src.data[i * src.width * 2 + j * 2 + 3];

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::Y800_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; ++i) {

		dst.data[j] = src.data[i];
		dst.data[j + 1] = src.data[i];
		dst.data[j + 2] = src.data[i];
		j = j + 3;

	}

	return true;

}


bool zs::PixelFormatConverter::Y800_to_BGR24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; ++i) {

		dst.data[j] = src.data[i];
		dst.data[j + 1] = src.data[i];
		dst.data[j + 2] = src.data[i];
		j = j + 3;

	}

	return true;

}


bool zs::PixelFormatConverter::Y800_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 2) {

		dst.data[j] = 0;
		dst.data[j + 1] = src.data[i];
		dst.data[j + 2] = 0;
		dst.data[j + 3] = src.data[i + 1];
		j = j + 4;

	}

	return true;

}


bool zs::PixelFormatConverter::Y800_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t j = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 2) {

		dst.data[j + 1] = 0;
		dst.data[j] = src.data[i];
		dst.data[j + 3] = 0;
		dst.data[j + 2] = src.data[i + 1];
		j = j + 4;

	}

	return true;

}


bool zs::PixelFormatConverter::Y800_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	memcpy(dst.data, src.data, (size_t)src.width * (size_t)src.height);
	memset(&dst.data[src.width * src.height], 0, ((size_t)src.width * (size_t)src.height) / 2);

	return true;

}


bool zs::PixelFormatConverter::NV12_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {
		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float Y00, Y01, Y10, Y11, U, V, R, G, B;
	size_t p = src.height;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			Y00 = (float)src.data[i * src.width + j];
			Y01 = (float)src.data[i * src.width + j + 1];
			Y10 = (float)src.data[(i + 1) * src.width + j];
			Y11 = (float)src.data[(i + 1) * src.width + j + 1];
			U = (float)src.data[p * src.width + j];
			V = (float)src.data[p * src.width + j + 1];

			R = Y00 + 1.140f * (V - 128.0f);
			G = Y00 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y00 + 2.032f * (U - 128.0f);
			dst.data[i * dst.width * 3 + j * 3]     = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[i * dst.width * 3 + j * 3 + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[i * dst.width * 3 + j * 3 + 2] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y01 + 1.140f * (V - 128.0f);
			G = Y01 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y01 + 2.032f * (U - 128.0f);
			dst.data[i * dst.width * 3 + j * 3 + 3] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[i * dst.width * 3 + j * 3 + 4] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[i * dst.width * 3 + j * 3 + 5] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y10 + 1.140f * (V - 128.0f);
			G = Y10 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y10 + 2.032f * (U - 128.0f);
			dst.data[(i + 1) * dst.width * 3 + j * 3]     = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 2] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y11 + 1.140f * (V - 128.0f);
			G = Y11 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y11 + 2.032f * (U - 128.0f);
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 3] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 4] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 5] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::NV12_to_BGR24(Frame& src, Frame& dst) {
	
	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {
		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float Y00, Y01, Y10, Y11, U, V, R, G, B;
	size_t p = src.height;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			Y00 = (float)src.data[i * src.width + j];
			Y01 = (float)src.data[i * src.width + j + 1];
			Y10 = (float)src.data[(i + 1) * src.width + j];
			Y11 = (float)src.data[(i + 1) * src.width + j + 1];
			U = (float)src.data[p * src.width + j];
			V = (float)src.data[p * src.width + j + 1];

			R = Y00 + 1.140f * (V - 128.0f);
			G = Y00 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y00 + 2.032f * (U - 128.0f);
			dst.data[i * dst.width * 3 + j * 3 + 2] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[i * dst.width * 3 + j * 3 + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[i * dst.width * 3 + j * 3]     = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y01 + 1.140f * (V - 128.0f);
			G = Y01 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y01 + 2.032f * (U - 128.0f);
			dst.data[i * dst.width * 3 + j * 3 + 5] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[i * dst.width * 3 + j * 3 + 4] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[i * dst.width * 3 + j * 3 + 3] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y10 + 1.140f * (V - 128.0f);
			G = Y10 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y10 + 2.032f * (U - 128.0f);
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 2] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[(i + 1) * dst.width * 3 + j * 3]     = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

			R = Y11 + 1.140f * (V - 128.0f);
			G = Y11 - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
			B = Y11 + 2.032f * (U - 128.0f);
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 5] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 4] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 3] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::NV12_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t k = src.height;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			dst.data[i * src.width * 2 + j * 2 + 1] = src.data[i * src.width + j];
			dst.data[i * src.width * 2 + j * 2 + 3] = src.data[i * src.width + j + 1];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 1] = src.data[(i + 1) * src.width + j];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 3] = src.data[(i + 1) * src.width + j + 1];
			dst.data[i * src.width * 2 + j * 2] = src.data[k * src.width + j];
			dst.data[i * src.width * 2 + j * 2 + 2] = src.data[k * src.width + j + 1];
			dst.data[(i + 1) * src.width * 2 + j * 2] = src.data[k * src.width + j];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 2] = src.data[k * src.width + j + 1];

		}
		++k;
	}

	return true;

}


bool zs::PixelFormatConverter::NV12_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t k = src.height;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			dst.data[i * src.width * 2 + j * 2] = src.data[i * src.width + j];
			dst.data[i * src.width * 2 + j * 2 + 2] = src.data[i * src.width + j + 1];
			dst.data[(i + 1) * src.width * 2 + j * 2] = src.data[(i + 1) * src.width + j];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 2] = src.data[(i + 1) * src.width + j + 1];
			dst.data[i * src.width * 2 + j * 2 + 1] = src.data[k * src.width + j];
			dst.data[i * src.width * 2 + j * 2 + 3] = src.data[k * src.width + j + 1];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 1] = src.data[k * src.width + j];
			dst.data[(i + 1) * src.width * 2 + j * 2 + 3] = src.data[k * src.width + j + 1];

		}
		++k;
	}

	return true;

}


bool zs::PixelFormatConverter::NV12_to_Y800(Frame& src, Frame& dst) {
	
	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	memcpy(dst.data, src.data, dst.size);

	return true;

}


bool zs::PixelFormatConverter::RGB24_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {
		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R, G, B, Y, U, V;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 3) {
		
		R = (float)src.data[i];
		G = (float)src.data[i + 1];
		B = (float)src.data[i + 2];

		Y = 0.299f * R + 0.587f * G + 0.114f * B;
		U = 0.492f * (B - Y) + 128.0f;
		V = 0.877f * (R - Y) + 128.0f;

		dst.data[i]     = Y > 255.0f ? 255 : Y < 0.0f ? 0 : (uint8_t)Y;
		dst.data[i + 1] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[i + 2] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_RGB24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {
		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R, G, B, Y, U, V;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 3) {

		Y = (float)src.data[i];
		U = (float)src.data[i + 1];
		V = (float)src.data[i + 2];

		R = Y + 1.140f * (V - 128.0f);
		G = Y - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B = Y + 2.032f * (U - 128.0f);

		dst.data[i]     = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
		dst.data[i + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
		dst.data[i + 2] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

	}

	return true;

}


bool zs::PixelFormatConverter::BGR24_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {

		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R, G, B, Y, U, V;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 3) {

		B = (float)src.data[i];
		G = (float)src.data[i + 1];
		R = (float)src.data[i + 2];

		Y = 0.299f * R + 0.587f * G + 0.114f * B;
		U = 0.492f * (B - Y) + 128.0f;
		V = 0.877f * (R - Y) + 128.0f;

		dst.data[i] = Y > 255.0f ? 255 : Y < 0.0f ? 0 : (uint8_t)Y;
		dst.data[i + 1] = U > 255.0f ? 255 : U < 0.0f ? 0 : (uint8_t)U;
		dst.data[i + 2] = V > 255.0f ? 255 : V < 0.0f ? 0 : (uint8_t)V;

	}

	return true;

}


bool zs::PixelFormatConverter::UYVY_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 6) {
		dst.data[i] = src.data[p + 1];
		dst.data[i + 1] = src.data[p];
		dst.data[i + 2] = src.data[p + 2];
		dst.data[i + 3] = src.data[p + 3];
		dst.data[i + 4] = src.data[p];
		dst.data[i + 5] = src.data[p + 2];
		p = p + 4;
	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_BGR24(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.size) {
		delete[] dst.data;
		dst.size = src.size;
		dst.data = new uint8_t[dst.size];
	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	float R, G, B, Y, U, V;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 3) {

		Y = (float)src.data[i];
		U = (float)src.data[i + 1];
		V = (float)src.data[i + 2];

		R = Y + 1.140f * (V - 128.0f);
		G = Y - 0.395f * (U - 128.0f) - 0.581f * (V - 128.0f);
		B = Y + 2.032f * (U - 128.0f);

		dst.data[i + 2] = R > 255.0f ? 255 : R < 0.0f ? 0 : (uint8_t)R;
		dst.data[i + 1] = G > 255.0f ? 255 : G < 0.0f ? 0 : (uint8_t)G;
		dst.data[i] = B > 255.0f ? 255 : B < 0.0f ? 0 : (uint8_t)B;

	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_UYVY(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		dst.data[p + 1] = src.data[i];
		dst.data[p] = src.data[i + 1];
		dst.data[p + 2] = src.data[i + 2];
		dst.data[p + 3] = src.data[i + 3];
		p = p + 4;

	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_YUY2(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 2) {

		delete[] dst.data;
		dst.size = src.width * src.height * 2;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 6) {

		dst.data[p] = src.data[i];
		dst.data[p + 1] = src.data[i + 1];
		dst.data[p + 3] = src.data[i + 2];
		dst.data[p + 2] = src.data[i + 3];
		p = p + 4;

	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_Y800(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height) {

		delete[] dst.data;
		dst.size = src.width * src.height;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)src.size; i = i + 3) {

		dst.data[p] = src.data[i];
		++p;

	}

	return true;

}


bool zs::PixelFormatConverter::YUV1_to_NV12(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 3 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * (src.height + src.height / 2)) {

		delete[] dst.data;
		dst.size = src.width * (src.height + src.height / 2);
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = src.height;
	for (size_t i = 0; i < (size_t)src.height; i = i + 2) {
		for (size_t j = 0; j < (size_t)src.width; j = j + 2) {

			dst.data[i * (size_t)src.width + j] = src.data[i * (size_t)src.width * 3 + j * 3];
			dst.data[i * (size_t)src.width + j + 1] = src.data[i * (size_t)src.width * 3 + j * 3 + 3];
			dst.data[(i + 1) * (size_t)src.width + j] = src.data[(i + 1) * (size_t)src.width * 3 + j * 3];
			dst.data[(i + 1) * (size_t)src.width + j + 1] = src.data[(i + 1) * (size_t)src.width * 3 + j * 3 + 3];
			dst.data[p * src.width + j] = src.data[i * (size_t)src.width * 3 + j * 3 + 1];
			dst.data[p * src.width + j + 1] = src.data[i * (size_t)src.width * 3 + j * 3 + 2];

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::YUY2_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height * 2 ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 6) {
		dst.data[i] = src.data[p];
		dst.data[i + 1] = src.data[p + 1];
		dst.data[i + 2] = src.data[p + 3];
		dst.data[i + 3] = src.data[p + 2];
		dst.data[i + 4] = src.data[p + 1];
		dst.data[i + 5] = src.data[p + 3];
		p = p + 4;
	}//for...

	return true;

}


bool zs::PixelFormatConverter::Y800_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * src.height ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = 0;
	for (size_t i = 0; i < (size_t)dst.size; i = i + 3) {

		dst.data[i]     = src.data[p];
		dst.data[i + 1] = 0;
		dst.data[i + 2] = 0;
		++p;

	}

	return true;

}


bool zs::PixelFormatConverter::NV12_to_YUV1(Frame& src, Frame& dst) {

	if (src.data == nullptr ||
		src.size != src.width * (src.height + src.height / 2) ||
		src.width < MIN_FRAME_WIDTH ||
		src.height < MIN_FRAME_HEIGHT)
		return false;

	if (dst.data == nullptr || dst.size != src.width * src.height * 3) {

		delete[] dst.data;
		dst.size = src.width * src.height * 3;
		dst.data = new uint8_t[dst.size];

	}

	dst.width = src.width;
	dst.height = src.height;
	dst.sourceID = src.sourceID;
	dst.frameID = src.frameID;

	size_t p = dst.height;
	for (size_t i = 0; i < src.height; i = i + 2) {
		for (size_t j = 0; j < src.width; j = j + 2) {
			
			dst.data[i * dst.width * 3 + j * 3] = src.data[i * src.width + j];
			dst.data[i * dst.width * 3 + j * 3 + 3] = src.data[i * src.width + j + 1];
			dst.data[(i + 1) * dst.width * 3 + j * 3] = src.data[(i + 1) * src.width + j];
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 3] = src.data[(i + 1) * src.width + j + 1];
			
			dst.data[i * dst.width * 3 + j * 3 + 1] = src.data[p * src.width + j];
			dst.data[i * dst.width * 3 + j * 3 + 2] = src.data[p * src.width + j + 1];

			dst.data[i * dst.width * 3 + j * 3 + 4] = src.data[p * src.width + j];
			dst.data[i * dst.width * 3 + j * 3 + 5] = src.data[p * src.width + j + 1];

			dst.data[(i + 1) * dst.width * 3 + j * 3 + 1] = src.data[p * src.width + j];
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 2] = src.data[p * src.width + j + 1];

			dst.data[(i + 1) * dst.width * 3 + j * 3 + 4] = src.data[p * src.width + j];
			dst.data[(i + 1) * dst.width * 3 + j * 3 + 5] = src.data[p * src.width + j + 1];

		}
		++p;
	}

	return true;

}


bool zs::PixelFormatConverter::isFourccCodeValid(uint32_t fourcc) {

	switch (fourcc) {
	case (uint32_t)zs::ValidFourccCodes::RGB24:
		return true;
	case (uint32_t)zs::ValidFourccCodes::BGR24:
		return true;
	case (uint32_t)zs::ValidFourccCodes::Y800:
		return true;
	case (uint32_t)zs::ValidFourccCodes::UYVY:
		return true;
	case (uint32_t)zs::ValidFourccCodes::YUY2:
		return true;
	case (uint32_t)zs::ValidFourccCodes::YUV1:
		return true;
	case (uint32_t)zs::ValidFourccCodes::NV12:
		return true;
	default:
		return false;
	}

	return false;

}


std::vector<uint32_t> zs::PixelFormatConverter::GetSupportedFourccCodes() {

	std::vector<uint32_t> codesList;
	codesList.push_back((uint32_t)zs::ValidFourccCodes::RGB24);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::BGR24);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::Y800);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::UYVY);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::YUY2);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::YUV1);
	codesList.push_back((uint32_t)zs::ValidFourccCodes::NV12);
	
	return codesList;

}
