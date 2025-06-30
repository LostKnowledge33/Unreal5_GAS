#include "DoubleBuffer.h"

void DoubleBuffer::EnqueueWritePacket(PacketInfo packet_)
{
	while (spinLock.IsLocked()) {}

	WriteQueue.push(std::move(packet_));
}

void DoubleBuffer::SwapQueue()
{
	if (ReadQueue.empty() && WriteQueue.empty()) return;
	if (!ReadQueue.empty()) return;

	spinLock.Lock();
	swap(ReadQueue, WriteQueue);

	while (!WriteQueue.empty())
		WriteQueue.pop();

	printf("[DoubleBuffer] SwapQueue %d  \n", swapCount++);
	spinLock.UnLock();
}

bool DoubleBuffer::IsEmpty()
{
	return ReadQueue.empty();
}

PacketInfo* DoubleBuffer::DequeueReadPacket()
{
	while (spinLock.IsLocked()) {}

	if (ReadQueue.empty())
	{
		return nullptr;
	}
	else
	{
		PacketInfo* packet = &ReadQueue.front();
		ReadQueue.pop();

		return packet;
	}
	
	return nullptr;
}
