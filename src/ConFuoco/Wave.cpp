#include "Wave.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		Wave::Wave() :
			buffer(0), handle(0), buffer_size(0), buffered(-1), format(0), frequency(0)
		{ }

		void Wave::release()
		{
			delete[] static_cast<char *>(this->buffer);
			this->buffer = 0;
			this->buffer_size = 0;
			this->buffered = -1;
			this->format = 0;
			this->frequency = 0;
			this->handle = 0;
		}
	}
}
