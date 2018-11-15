#pragma once

#include <QObject>
#include <QRunnable>

#include "QMutex"

#include "DCGp/GpEntity.h"
#include "DCGp/Custom/PageLodEntity.h"

#include "DCUtil/EntityQueueManager.h"

class PageLodTaskThread : public QObject, public QRunnable
{
	

public:
	PageLodTaskThread(EntityQueueManager* queue, DcGp::PageLodEntity* pageLodEntity, float viewDistance);
	~PageLodTaskThread();

	void run();

private:
	EntityQueueManager* m_queueManager;
	DcGp::PageLodEntity* m_pageLodEntity;
	float m_viewDistance;

	//! Æ«ÒÆÁ¿
	Point_3D m_shift;

	//!Ëø
	QMutex m_mutex;
};
