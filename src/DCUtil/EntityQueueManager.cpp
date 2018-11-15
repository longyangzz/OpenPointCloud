#include "EntityQueueManager.h"

#include "DCGp/Custom/GpPointCloud.h"

EntityQueueManager::EntityQueueManager(QObject *parent)
	: QObject(parent)
{

}

EntityQueueManager::~EntityQueueManager()
{
}

void EntityQueueManager::Add(DcGp::DcGpEntity* entity)
{
	m_mutex.lock();
	
	m_queueList.push_back(entity);

	m_mutex.unlock();
}

void EntityQueueManager::GetEntityQueue(std::list<DcGp::DcGpEntity* >& enList)
{
	
	m_mutex.lock();

	m_queueList.swap(enList);
	//list = m_queueList;
	//m_queueList.clear();
	m_mutex.unlock();
}