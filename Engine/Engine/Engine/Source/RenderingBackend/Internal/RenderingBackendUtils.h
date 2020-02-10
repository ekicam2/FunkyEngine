#pragma once

#define FK_DEFINE_RENDERINGBACKEND_TYPE(__name__, __handle__)						 \
struct __name__ {																	 \
	__name__() = default;															 \
	__name__(__handle__ NewHandle) : Handle(NewHandle) {} 							 \
	operator __handle__() const { return Handle; }									 \
	bool IsValid() const { return Handle != RenderingBackend::INVALID_INDEX; }		 \
private:																			 \
	__handle__ Handle = RenderingBackend::INVALID_INDEX;							 \
};