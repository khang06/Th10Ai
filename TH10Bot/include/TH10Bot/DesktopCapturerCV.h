#pragma once

#include <opencv2/core/directx.hpp>
#include <GraphCap/DesktopCapturer.h>

namespace th
{
	class DesktopCapturerCV :
		public DesktopCapturerGPU
	{
	public:
		DesktopCapturerCV(Direct3D& d3d);

	private:
		cv::ocl::Context m_oclContext;
	};
}
