/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Platform.h"
#include "ConFuoco/Wave.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		Wave::Wave() :
			buffer(nullptr), handle(nullptr), buffer_size(0), buffered(-1), format(0), frequency(0)
		{ }

		void Wave::release()
		{
			delete[] static_cast<char *>(this->buffer);
			this->buffer = nullptr;
			this->buffer_size = 0;
			this->buffered = -1;
			this->format = 0;
			this->frequency = 0;
			this->handle = nullptr;
		}
	}
}
