#include "Wave.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		void Wave::release()
		{
			this->buffer_size = 0;
			this->buffered = -1;
			this->format = 0;
			this->frequency = 0;
			delete[] this->buffer;
			this->buffer = 0;
		}
	}
}
