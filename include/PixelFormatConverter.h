#pragma once
// VERSION: 1.0
// AUTHOR: Zaplatnikov Sergey (s.zaplatnikov@zaplatnikov.com)
#include <cstdint>
#include <vector>
#include <VideoDataStructures.h>


namespace zs {

	/**
	\brief Class for converting pixel format
	*/
	class PixelFormatConverter {

	public:

		/// Class constructor
		PixelFormatConverter();

		/// Class destructor
		~PixelFormatConverter();

		/**
		\brief Method for converting pizel format
		\param[in] src Source image
		\param[out] dst Output image
		\return TRUE - success, FALSE - error 
		*/
		bool Convert(Frame& src, Frame& dst);

		/**
		\brief Method to get version
		\param[out] major Major index of version
		\param[out] minor Minor index of version
		*/
		void GetVersion(uint32_t& major, uint32_t& minor);

		/**
		\brief Method for check valid format
		\param[in] fourcc FOURCC code to check
		\return TRUE - code is valid, FALSE - code not valid
		*/
		bool isFourccCodeValid(uint32_t fourcc);

		/**
		\brief Get supported FOURCC codes list
		\return Method return list of supported FOURCC codes
		*/
		std::vector<uint32_t> GetSupportedFourccCodes();

	private:

		/// Major index of version
		const uint32_t majorVersion = 1;
		/// Minor index of version
		const uint32_t minorVersion = 0;

		/// Method to copy frame
		bool Copy(Frame& src, Frame& dst);
		/// Method to convert RGB24 to BGR24
		bool RGB24_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert RGB24 to UYVY
		bool RGB24_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert RGB24 to YUY2
		bool RGB24_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert RGB24 to Y800
		bool RGB24_to_Y800(Frame& src, Frame& dst);
		/// Method to convert RGB24 to NV12
		bool RGB24_to_NV12(Frame& src, Frame& dst);
		/// Method to convert RGB24 to YUV1
		bool RGB24_to_YUV1(Frame& src, Frame& dst);
		/// Method to convert BGR24 to RGB24
		bool BGR24_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert BGR24 to UYVY
		bool BGR24_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert BGR24 to YUY2
		bool BGR24_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert BGR24 to Y800
		bool BGR24_to_Y800(Frame& src, Frame& dst);
		/// Method to convert BGR24 to NV12
		bool BGR24_to_NV12(Frame& src, Frame& dst);
		/// Method to convert RGB24 to YUV1
		bool BGR24_to_YUV1(Frame& src, Frame& dst);
		/// Method to convert UYVY to RGB24
		bool UYVY_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert UYVY to BGR24
		bool UYVY_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert UYVY to YUY2
		bool UYVY_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert UYVY to YUY2
		bool UYVY_to_Y800(Frame& src, Frame& dst);
		/// Method to convert UYVY to NV12
		bool UYVY_to_NV12(Frame& src, Frame& dst);
		/// Method to convert UYVY to YUV1
		bool UYVY_to_YUV1(Frame& src, Frame& dst);
		/// Method to convert YUY2 to RGB24
		bool YUY2_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert YUY2 to BGR24
		bool YUY2_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert YUY2 to UYVY
		bool YUY2_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert YUY2 to Y800
		bool YUY2_to_Y800(Frame& src, Frame& dst);
		/// Method to convert YUY2 to NV12
		bool YUY2_to_NV12(Frame& src, Frame& dst);
		/// Method to convert Y800 to RGB24
		bool Y800_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert Y800 to BGRB24
		bool Y800_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert Y800 to UYVY
		bool Y800_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert Y800 to YUY2
		bool Y800_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert Y800 to NV12
		bool Y800_to_NV12(Frame& src, Frame& dst);
		/// Method to convert NV12 to RGB24
		bool NV12_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert NV12 to BGR24
		bool NV12_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert NV12 to UYVY
		bool NV12_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert NV12 to BGR24
		bool NV12_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert NV12 to Y800
		bool NV12_to_Y800(Frame& src, Frame& dst);
		/// Method to convert YUV1 to RGB24
		bool YUV1_to_RGB24(Frame& src, Frame& dst);
		/// Method to convert YUV1 to BGR24
		bool YUV1_to_BGR24(Frame& src, Frame& dst);
		/// Method to convert YUV1 to UYVY
		bool YUV1_to_UYVY(Frame& src, Frame& dst);
		/// Method to convert YUV1 to YUY2
		bool YUV1_to_YUY2(Frame& src, Frame& dst);
		/// Method to convert YUV1 to Y800
		bool YUV1_to_Y800(Frame& src, Frame& dst);
		/// Method to convert YUV1 to NV12
		bool YUV1_to_NV12(Frame& src, Frame& dst);
		/// Method to convert YUV2 to YUV1
		bool YUY2_to_YUV1(Frame& src, Frame& dst);
		/// Method to convert Y800 to YUV1
		bool Y800_to_YUV1(Frame& src, Frame& dst);
		/// Method to convert NV12 to YUV1
		bool NV12_to_YUV1(Frame& src, Frame& dst);

	};//class...

}//namespace...



