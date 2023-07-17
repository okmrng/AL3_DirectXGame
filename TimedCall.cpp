#include "TimedCall.h"

TimedCall::TimedCall(std::function<void(void)> f, uint32_t time) { 
	f_ = f;

	time_ = time;
}

void TimedCall::Update() { 
	if (finish_) {
		return;
	}

	time_--;
	
	if (time_ <= 0) {
		finish_ = true;

		// コールバック呼び出し
		f_();
	}
}
