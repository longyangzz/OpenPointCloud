# GPPageLod设计工作流程

1、view初始化时候传入文件名，返回实体对象，作为sceneroot绑定到view；

2、进入render函数中后，根据视图距离，遍历判断当前pageentiy关联的子对象文件名；

3、判断当前是否在可见范围内，则发送请求加载信息到队列中，触发数据加载线程独立运行

4、在数据加载线程中，读取数据，生成requestlist并将实体对象绑定到这个数组，

_pager->m_dataToMergeQueue->_requestList.push_back(request);目的是为了在每一帧的update中

将新生成的对列数据，交换到渲染队列中。



