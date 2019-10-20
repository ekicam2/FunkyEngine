#include "IThread.h"

Funky::Core::Thread::IThread::IThread(str const& Name, Thread::Type ThreadType)
	: Name(Name), Type(ThreadType)
{

}