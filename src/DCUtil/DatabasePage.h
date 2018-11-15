#pragma once

#include <QObject>
#include "QThread"
#include "QList"

#include "DCUtil/mputilDLL.h"
#include "OpenThreads/Atomic"
#include "OpenThreads/Thread"
#include "OpenThreads/ScopedLock"
#include "OpenThreads/Block"
#include <list>

#include "DCGp/GpEntity.h"

namespace DCUtil
{

	
	struct DatabaseRequest
	{
		DatabaseRequest()
			: m_loadedNode(nullptr)
			, m_valid(false)
		{

		}

		DatabaseRequest(QString fileName, DcGp::DcGpEntity* parentNode)
			: m_loadedNode(nullptr)
			, m_valid(false)
		{
			m_fileName = fileName;
			m_parentNode = parentNode;
		}

		bool Valid()
		{
			return m_valid;
		}

		DcGp::DcGpEntity* m_parentNode;
		DcGp::DcGpEntity* m_loadedNode;

		QString m_fileName;

		bool m_valid;
	};

	/** 在分页节点的遍历中调用该函数*/
	class MPUTIL_EXPORT DatabaseRequestHandler
	{
	public:

		DatabaseRequestHandler() {}

		virtual ~DatabaseRequestHandler() {}
		virtual void RequestNodeFile(const QString& fileName, DcGp::DcGpEntity* parNode, boost::shared_ptr<DatabaseRequest>& dbRequest) = 0;

	protected:
		
	};


	class MPUTIL_EXPORT DatabasePage : public DatabaseRequestHandler
	{

	public:
		DatabasePage();
		~DatabasePage();

		virtual void RequestNodeFile(const QString& fileName, DcGp::DcGpEntity* parNode, boost::shared_ptr<DatabaseRequest>& dbRequest);

		void UpdateScene();

		class MPUTIL_EXPORT DatabaseThread : public OpenThreads::Thread
		{
		public:

			enum Mode
			{
				HANDLE_ALL_REQUESTS,
				HANDLE_NON_HTTP,
				HANDLE_ONLY_HTTP
			};

			DatabaseThread(DatabasePage* pager, Mode mode, const std::string& name);
			virtual ~DatabaseThread();

			void setName(const std::string& name) { _name = name; }
			const std::string& getName() const { return _name; }

			void setDone(bool done) { _done.exchange(done ? 1 : 0); }
			bool getDone() const { return _done != 0; }

			void setActive(bool active) { _active = active; }
			bool getActive() const { return _active; }

			virtual int cancel();

			virtual void run();

		protected:

			

			OpenThreads::Atomic _done;
			volatile bool       _active;
			DatabasePage*      _pager;
			Mode                _mode;
			std::string         _name;

		};

		struct MPUTIL_EXPORT RequestQueue
		{
		public:

			RequestQueue(DatabasePage* pager)
			{
				_pager = pager;
			}
			virtual ~RequestQueue()
			{

			}

			void add(DatabaseRequest* databaseRequest)
			{
				_requestList.push_back(databaseRequest);

			}

			void swap(QList<DatabaseRequest* >& requestList)
			{
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
				_requestList.swap(requestList);
			}

			DatabaseRequest* takeFirst()
			{
				if (!_requestList.isEmpty())
				{
					DatabaseRequest* dr = _requestList.takeFirst();
					updateBlock();
					return dr;
				}
				else
				{
					return nullptr;
				}
				
			}

			void block()
			{
				_block.block();
			}

			virtual void updateBlock() 
			{
				_block.set((!_requestList.empty() || !_childrenToDeleteList.empty()));
			}

			OpenThreads::Block              _block;
			DatabasePage*                   _pager;
			QList<DatabaseRequest* >                 _requestList;
			OpenThreads::Mutex          _requestMutex;

			OpenThreads::Mutex          _childrenToDeleteListMutex;
			QList<DatabaseRequest* >                 _childrenToDeleteList;

			bool doRelease;

		protected:
			
		};
	private:
		//! 需要管理几个队列和读写线程
		DatabaseThread*              m_databaseThreads;

		//! 需读取文件队列，锁。主线程中为该队列赋值，读写线程中不间断的从此队列取值。
		RequestQueue*			m_fileRequestQueue;   //！两个线程之间文件共享
		RequestQueue*			m_dataToMergeQueue;   //！两个线程之前数据加载结果共享

		bool m_startThreadCalled;

		//! 全局运行锁，确保读写线程顺利启动
		//! OpenThreads::Mutex              m_run_mutex;

		//! 数据请求锁
		OpenThreads::Mutex              m_dr_mutex;

		bool                            m_done;
	};
}


