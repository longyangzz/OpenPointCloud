#pragma once

#include <QObject>
#include "QMutex"

#include "DCGp/GpEntity.h"

#include <list>

class EntityQueueManager : public QObject
{
	Q_OBJECT

public:
	EntityQueueManager(QObject *parent = nullptr);
	~EntityQueueManager();


	void Add(DcGp::DcGpEntity* entity);

	void GetEntityQueue(std::list<DcGp::DcGpEntity* >& enList);

private:
	QMutex m_mutex;

	//! 实体队列节点指针
	std::list<DcGp::DcGpEntity* > m_queueList;
};
