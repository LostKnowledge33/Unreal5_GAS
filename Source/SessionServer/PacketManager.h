#pragma once

#include "Packet.h"
#include "DoubleBufferManager.h"
#include <unordered_map>
#include <deque>
#include <functional>
#include <thread>
#include <mutex>

class UserManager;
class RoomManager;
class RedisManager;

class PacketManager {
public:
	PacketManager() = default;
	~PacketManager() = default;

	void Init(const UINT32 maxClient_);

	bool Run();

	void End();

	void ReceivePacketData(const UINT32 clientIndex_, const UINT32 size_, char* pData_);

	void PushSystemPacket(PacketInfo packet_);
		
	std::function<void(UINT32, UINT32, char*)> SendPacketFunc;

private:
	void CreateCompent(const UINT32 maxClient_);
	
	void ClearConnectionInfo(INT32 clientIndex_);

	void EnqueuePacketData(const UINT32 clientIndex_);
	PacketInfo DequePacketData();

	PacketInfo DequeSystemPacketData();

	void ProcessQueue();
	void ProcessPacket();

	void ProcessRecvPacket(const UINT32 clientIndex_, const UINT16 packetId_, const UINT16 packetSize_, char* pPacket_);

	void ProcessUserConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);
	void ProcessUserDisConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);
	
	void ProcessLogin(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);
	void ProcessLoginResponse(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	void ProcessLobbyCharacter(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);
	void ProcessLobbyCharacterResponse(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_);

	DoubleBufferManager* doubleBufferManager = nullptr;

	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16, char*);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> mRecvFuntionDictionary;

	UserManager* mUserManager;
		
	std::function<void(int, char*)> mSendMQDataFunc;

	bool mIsRunProcessThread = false;
	
	std::thread mProcessThread;
	std::mutex mLock;
	
	std::deque<UINT32> mInComingPacketUserIndex;
	std::deque<PacketInfo> mSystemPacketQueue;

	std::thread DBAgentProcessThread;
};

