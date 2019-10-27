//#pragma once
//#include "Thread.h"
//#include "Singleton.h"
//#include "Queue.h"
//
//class Log : public Thread, public Singleton<Log>
//{
//public:
//	enum LOG_TYPE {
//		NONE = 0,
//		DEBUG,
//		INFO,
//		ERROR
//	};
//
//	enum LOG_STORAGE_TYPE {
//		NONE = 0,
//		FILE = 1,
//		DB,
//		UDP,
//		TCP
//	};
//	
//	enum LOG_FILE_TYPE
//	{
//		NONE = 0,
//		TXT,
//		XML
//	};
//
//	struct LogConfig
//	{
//		int m_LogInfoTypes[MAX_STORAGE_TYPE];
//		char m_LogFileName[MAX_FILE_NAME_LENGTH];
//		LOG_FILE_TYPE m_LogFileType;
//
//		char m_IP[MAX_IP_LENGTH];
//		int m_UDPPort;
//		int m_TCPPort;
//
//		char m_DBName[MAX_DB_NAME_LENGTH];
//		char m_DBId[MAX_DB_ID_LENGTH];
//		char m_DBPassword[MAX_DB_PW_LENGTH];
//
//		HWND m_hWnd;
//		DWORD m_ProcessTick;
//		DWORD m_FileMaxSize;
//
//		LogConfig () {
//			ZeroMemory (this, sizeof (LogConfig));
//			m_ProcessTick = DEFAULT_TICK;
//			m_UDPPort = DEFAULT_UDP_PORT;
//			m_TCPPort = DEFALUT_TCP_PORT;
//			m_FileMaxSize = FILE_MAX_SIZE;
//		}
//	};
//
//	struct LogMsg {
//		LOG_TYPE logType;
//		char m_szOutputString[MAX_OUTPUT_LENGTH];
//	};
//
//	Log ();
//	~Log ();
//
//	bool Initialize (LogConfig &logConfig);
//	void LogOutput (LOG_TYPE logType, char* szOutString);
//	void LogOutputLastError (char* szOutputString);
//	void CloseAllLog ();
//
//	void OnProcess () override;
//	void SetHWND (HWND hWnd = nullptr) { m_hWnd = hWnd; }
//	size_t GetQueueSize () { return m_LogMsg.GetQueueSize (); }
//	inline void InsertMsgToQueue (LogMsg* pLogMsg) {
//		m_LogMsg.PushQueue (pLogMsg);
//	}
//
//	static char m_szOutString[MAX_OUTPUT_LENGTH];
//	static LogMsg m_LogMsg[MAX_QUEUE_COUNT];
//	static Monitor m_csLog;
//
//	bool InitializeLog (LogConfig &logConfig);
//	void Log (LOG_TYPE logType, char* szOutputString);
//	void LastError (char* szOutputString);
//	void CloseLog ();
//
//private:
//	int m_LogInfoTypes[MAX_STORAGE_TYPE];
//	char m_LogFileName[MAX_FILE_NAME_LENGTH];
//	LOG_FILE_TYPE m_LogFileType;
//
//	char m_IP[MAX_IP_LENGTH];
//	int m_UDPPort;
//	int m_TCPPort;
//
//	char m_DBName[MAX_DB_NAME_LENGTH];
//	char m_DBId[MAX_DB_ID_LENGTH];
//	char m_DBPassword[MAX_DB_PW_LENGTH];
//
//	char m_szOutStr[MAX_OUTPUT_LENGTH];
//
//	HWND m_hWnd;
//	HANDLE m_hLogFile;
//
//	SOCKET m_UDPSocket;
//	SOCKET m_TCPSocekt;
//
//	Queue<LogMsg* > m_LogMsg;
//	int m_MsgBufferIdx;
//	DWORD m_FileMaxSize;
//
//	void OutputFile (char* szOutputString);
//	void OutputDB (char* szOutputString);
//	void OutputWindow (LOG_TYPE logType, char* szOutputString);
//	void OutputWnd (char* szOutputString);
//	void OutputUDP (LOG_TYPE logType, char* szOutputString);
//	void OutputTCP (LOG_TYPE logType, char* szOutputString);
//
//	bool InitializeDB ();
//	bool InitializeFile ();
//	bool InitializeUDP ();
//	bool InitializeTCP ();
//};
//
