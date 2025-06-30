#pragma once

#include <queue>

#include "SpinLock.h"
#include "Packet.h"


class DoubleBuffer
{

public:
	DoubleBuffer() = default;
	~DoubleBuffer() = default;
	
	void EnqueueWritePacket(PacketInfo packet_);
	void SwapQueue();
	
	bool IsEmpty();

	PacketInfo* DequeueReadPacket();
	
private:

	std::queue<PacketInfo> ReadQueue;
	std::queue<PacketInfo> WriteQueue;

	SpinLock spinLock;

	int swapCount = 0;
};

