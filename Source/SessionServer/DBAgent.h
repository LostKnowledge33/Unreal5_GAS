#pragma once

#include "PacketManager.h"
#include "DoubleBufferManager.h"
#include <mysql.h>
#include <iostream>
#include <queue>
#include <process.h>
#include <unordered_map>


class DBAgent
{
public:
	DBAgent();
	~DBAgent();

	void ProcessQueue();

private:
	
	uintptr_t thread_handle;

	DoubleBufferManager* doubleBufferManager = nullptr;

	MYSQL* mysql = nullptr;

	const char* host_name		= "localhost";  // 접속할 MySQL 서버가 설치된 호스트의 이름이나 IP
	const char* user_name		= "root";       // MySQL 사용자 계정
	const char* user_password	= "atomic";     // MySQL 사용자 비밀번호
	const char* database		= "game_db";    // 접속 후, 사용할 DB 명
	
	unsigned int port_no = 3306;				// MySQL 서버 접속 포트
	unsigned int flags = 0;

	
	typedef void(DBAgent::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16, char*);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> DBFuntionDictionary;

	/*
	*	DB Process Functions
	*/

	bool ProcessLoginQuery(const char* query_, OUT bool& result_);
	void ProcessLogin(UINT32 clientIndex_, char* pPacket_);

	bool ProcessCharacterInfoQuery(const char* query_, OUT bool& result_);
	void ProcessCharacterInfo(UINT32 clientIndex_, char* pPacket_);

};

