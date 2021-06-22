// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#pragma once

#include<oxu/framework/utils/vector2.hpp>

namespace oxu
{
	class PlayField
	{
	private:
		framework::Vector2<float> startPoint;
		float m_oxu_px;
		
	public:
		PlayField();

		const float &get_oxu_px() const;

		inline const framework::Vector2<float> &getStartPoint() const
		{
			return startPoint;
		}
		
	};
}
