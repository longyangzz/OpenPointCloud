#include "DatabasePage.h"
#include "OpenThreads/Thread"
#include "OpenThreads/ScopedLock"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/GUIEventHandle.h"
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"

using namespace DCUtil;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DatabaseThread
//
DatabasePage::DatabaseThread::DatabaseThread(DatabasePage* pager, Mode mode, const std::string& name) :
	_done(false),
	_active(false),
	_pager(pager),
	_mode(mode),
	_name(name)
{
}



DatabasePage::DatabaseThread::~DatabaseThread()
{
	cancel();
}

int DatabasePage::DatabaseThread::cancel()
{
	int result = 0;

	if (isRunning())
	{
		setDone(true);

		switch (_mode)
		{
		case(HANDLE_ALL_REQUESTS):
			//_pager->_fileRequestQueue->release();
			break;
		case(HANDLE_NON_HTTP):
			//_pager->_fileRequestQueue->release();
			break;
		case(HANDLE_ONLY_HTTP):
			//_pager->_httpRequestQueue->release();
			break;
		}

		// then wait for the the thread to stop running.
		while (isRunning())
		{

			OpenThreads::Thread::YieldCurrentThread();
		}

	}

	return result;

}

void DatabasePage::DatabaseThread::run()
{
	RequestQueue* readQueue;

	readQueue = _pager->m_fileRequestQueue;

	do 
	{
		readQueue->block();

		DatabaseRequest* request = new DatabaseRequest;
		Point_3D shift;
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(readQueue->_requestMutex);
			if (!readQueue->_requestList.isEmpty())
			{
				request = readQueue->takeFirst();
				shift = request->m_parentNode->GetShift();
				
			}
		}
	
		
		QString fileName = request->m_fileName;
		if (!fileName.isEmpty())
		{
			QString extension = QFileInfo(fileName).suffix();
			//根据扩展名，遍历IO文件夹中的插件，选择合适的io解析器解析文件
			QDir dir(DCCore::GetIOPluginPath());

			DCCore::IoPlugin* currentIo = 0;

			bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
			if (hasPlugin && currentIo)
			{
				double coordinatesShift[3] = { shift.x,shift.y,shift.z };
				bool shiftable = true;
				DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, false, &shiftable, coordinatesShift, nullptr);

				OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(_pager->m_dr_mutex);
				request->m_loadedNode = entity;

				OpenThreads::ScopedLock<OpenThreads::Mutex> listLock(
					_pager->m_dataToMergeQueue->_requestMutex);
				_pager->m_dataToMergeQueue->_requestList.push_back(request);
				request = 0;
			}

			
		}
		bool firstTime = true;
		if (firstTime)
		{
			// do a yield to get round a peculiar thread hang when testCancel() is called 
			// in certain circumstances - of which there is no particular pattern.
			YieldCurrentThread();
			firstTime = false;
		}

	} while (!_done);
}


DatabasePage::DatabasePage()
	: DCUtil::DatabaseRequestHandler()
{
	m_startThreadCalled = false;

	m_databaseThreads = new DatabaseThread(this, DatabaseThread::HANDLE_NON_HTTP, "HANDLE_NON_HTTP");

	m_dataToMergeQueue = new RequestQueue(this);
	m_fileRequestQueue = new RequestQueue(this);

	if (m_startThreadCalled)
	{

		m_databaseThreads->startThread();
	}
}

DatabasePage::~DatabasePage()
{
	if (m_databaseThreads)
	{
		delete m_databaseThreads;
		m_databaseThreads = nullptr;
	}
}

void DatabasePage::UpdateScene()
{
	QList<DatabaseRequest* > localFileLoadedList;

	// 线程保护中
	m_dataToMergeQueue->swap(localFileLoadedList);

	for (QList<DatabaseRequest* >::iterator itr = localFileLoadedList.begin();
		itr != localFileLoadedList.end();
		++itr)
	{
		DatabaseRequest* databaseRequest = *itr;

		DcGp::DcGpEntity* parent = databaseRequest->m_parentNode;
		if (parent)
		{

			if (databaseRequest->m_loadedNode)
			{
				parent->AddChild(databaseRequest->m_loadedNode);
			}
			

		}
	}
}


//! 主线程 分页节点遍历时候调用
void DatabasePage::RequestNodeFile(const QString& fileName, DcGp::DcGpEntity* parNode, boost::shared_ptr<DatabaseRequest>& dbRequest)
{
	//!传入父节点
	DcGp::DcGpEntity* parent = parNode;


	//! 是否数据已读，如果找到已有加载请求，则foundEntry为true
	bool foundEntry = false;

	
	//!通过判断传过来的dbRequest参数，dbRequest存在，则说明已经添加到队列中了
	//! 既然已经添加到队列中了，就要判断一下是否在队列中已经没数据线程加载完成了。
	if (dbRequest)
	{
		bool reQueue = false;   //!重新放入队列中

		//! 数据请求锁
		OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(m_dr_mutex);

		//!判断数据请求的valid状态,true没有在读取线程中读取中
		if (!(dbRequest)->Valid())
		{
			//dbRequest = 0;
		}
		else
		{
			foundEntry = true;

			//!判断是否在对列中已被加载
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_fileRequestQueue->_requestMutex);


			if (reQueue)
			{
				m_fileRequestQueue->_requestList.push_back(dbRequest.get());
				m_fileRequestQueue->updateBlock();
			}
		}

	}

	//! 第一次节点加载的时候，创建了newRequest，并给传入的dbRequest赋值了，
	//!赋值了，传入读写线程了，但是不代表已经读取并完成了。所以需要
	//! 在第二次对同一节点请求时候使用上边的reQueue参数判断是否需要重新加载。
	if (!foundEntry)
	{
		//! w文件队列加锁，操作队列,重新创建一个DatabaseRequest
		OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(m_dr_mutex);

		//! 重新一份request
		boost::shared_ptr<DCUtil::DatabaseRequest > newRequest(new DCUtil::DatabaseRequest);
		dbRequest = newRequest;
		newRequest->m_fileName = fileName;
		newRequest->m_parentNode = parent;
		newRequest->m_valid = true;

		//! w文件队列加锁，操作队列,重新创建一个DatabaseRequest
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_fileRequestQueue->_requestMutex);
		m_fileRequestQueue->_requestList.push_back(newRequest.get());
		m_fileRequestQueue->updateBlock();
	}


	if (!m_startThreadCalled)
	{
		//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_run_mutex);

		if (!m_startThreadCalled)
		{
			m_startThreadCalled = true;
			m_done = false;

			if (!m_databaseThreads)
			{
				m_databaseThreads = new DatabaseThread(this, DatabaseThread::HANDLE_NON_HTTP, "HANDLE_NON_HTTP");
			}

			
			m_databaseThreads->startThread();
			
		}
	}
}


/****************************************************************************************************/
