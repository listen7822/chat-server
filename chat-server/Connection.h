//#pragma once
//
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/asio/detail/win_iocp_overlapped_ptr.hpp>
//#include "Queue.h"
//#include "RingBuffer.h"
//
//class Connection : public Monitor
//{
//public:
//	struct InitConfig {
//		int index;
//		boost::asio::ip::tcp::socket lintenSocket;
//		int recvBufferCount;
//		int sendBufferCount;
//		int sendBufferSize;
//		int processPacketCount;
//		int serverPort;
//		int workerThreadCount;
//		int processThreadCount;
//
//		InitConfig () {
//			std::memset (this, 0, sizeof (InitConfig));
//		}
//	};
//
//	typedef struct _OVERLAPPED_EX{
//		WSAOVERLAPPED overlapped;
//
//	}
//
//	Connection ();
//	~Connection ();
//
//	void InitializeConnection ();
//	bool CreateConnection (InitConfig &initConfig);
//	bool CloseConnection (bool force = false);
//	bool BindIOService (HANDLE &hService);
//	bool RecvPost (char* pNext, DWORD remain);
//	bool SendPost ();
//	void SetSocket (boost::asio::ip::tcp::socket &socket) { m_Socket = socket; }
//	boost::asio::ip::tcp::socket GetSocket () { return m_Socket; }
//	bool BinldAcceptSocket ();
//	char* PrepareSendpacket (int len);
//	bool ReleaseRecvPacket ();
//	bool ReleaseSendPacket (LPOVERLAPPED_EX lpSendOverlappedEx = nullptr);
//
//	inline void SetConnectionIp (char* ip) { ::memccpy (m_Ip), ip, MAX_IP_LENGTH); }
//	inline PBYTE GetConnectionIp () { return m_Ip; }
//
//	inline int GetIndex () { return m_Index; }
//
//	inline int GetRecvBufferSize () { return m_RecvBufferSize; }
//	inline int GetSendBufferSize () { return m_SendBufferSize; }
//
//	inline int GetRecvIoRefCount () { return m_RecvIoRefCount; }
//	inline int GetSendIoRefCount () { return m_SendIdRefCount; }
//	inline int GetAcceptIoRefCount () { return m_AcceptIoRefCount; }
//
//	inline int IncrementRecvIoRefCount () { return ++m_RecvIoRefCount; }
//	inline int IncrementSendIoRefCount () { return ++m_SendIoRefCoutn; }
//	inline int IncrementAcceptIoRefCount () { return ++m_AcceptIoRefCount; }
//
//	inline int DecrementRecvIoRefCount () { return (m_RecvIoRefCount ? --m_RecvIoRefCount : 0); }
//	inline int DecrementSendIoRefCount () { return (m_SendIoRefCount ? --m_SendIoRefCount : 0); }
//	inline int DecrementAcceptIoRefCount () { return (m_AcceptIoRefCount ? --m_AcceptIoRefCount : 0); }
//
//	LPOVERLAPPED_EX m_l[RecvOverlappedEx;
//	LPOVERLAPPED_EX m_lpSendOverlappedEx;
//
//	RingBuffer m_RingRecvBuffer;
//	RingBuffer m_RingSendBuffer;
//
//	char m_AddressBuffer[1024];
//	bool m_IsClosed;
//	bool m_IsConect;
//	bool m_IsSend;
//
//private:
//	boost::asio::ip::tcp::socket m_Socket;
//	int m_RecvBufferSize;
//	int m_SendBufferSize;
//	
//	boost::winapi::BYTE_ m_Ip[MAX_IP_LENGTH];
//	int m_Index;
//
//	Monitor m_Connection;
//	boost::asio::ip::tcp::socket m_SocketListener;
//	boost::winapi::HANDLE_ m_hIOCP;
//	boost::winapi::DWORD_ m_SendIoRefCount;
//	boost::winapi::DWORD_ m_RecvIoRefCount;
//	boost::winapi::DWORD_ m_AcceptIoRefCount;
//
//	Connection (const Connection &rhs);
//	Connection &operator=(const Connection &rhs);
//};
//
