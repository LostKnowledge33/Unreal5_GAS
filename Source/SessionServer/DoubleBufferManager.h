#pragma once

#include "DoubleBuffer.h"

class DoubleBufferManager
{
public:
	DoubleBufferManager() = default;
	~DoubleBufferManager() = default;

	DoubleBufferManager(const DoubleBufferManager&) = delete;
	DoubleBufferManager& operator=(const DoubleBufferManager&) = delete;

	DoubleBuffer& GetDB_Queue()
	{
		return DoubleBuffer_DB;
	}

	DoubleBuffer& GetServer_Queue()
	{
		return DoubleBuffer_Server;
	}

	bool DBQueueIsEmpty()
	{
		return DoubleBuffer_DB.IsEmpty();
	}

	static DoubleBufferManager& GetInstance()
	{
		static DoubleBufferManager instance;
		return instance;
	}

private:

	DoubleBuffer DoubleBuffer_DB;
	DoubleBuffer DoubleBuffer_Server;

};

