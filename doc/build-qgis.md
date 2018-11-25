# qgis Debug版本编译  
--------------
记录QGIS的编译资料，主要是编译debug版本，有助于研究学习qgis的框架结构，本次测试编译成功的版本为
64位debug和release版本   

# 依赖库版本信息  
vs2015  
## 从osgeo4W下载安装  
1、 qt 5.11  
2、 python 3.7  
## 从cygwin(cygwin安装包)下载安装  
1、bison.exe  
2、flex.exe  
## 源码下载编译  
1、 qwt使用svn检出6.1 stable release版本  
svn://svn.code.sf.net/p/qwt/code/branches/qwt-6.1  
2、 qtkeychain 下载源码使用camke 3.7 + vs2015 x64编译  
https://github.com/frankosterfeld/qtkeychain   
3、 qscintilla  下载QScintilla_gpl-2.11.dev1811171407源码编译,doc目录下的index.html中有编译说明   
需要配置好qmake的路径，vs的cl.exe的路径，通过导入vs安装目录下amd64中的环境变量文件引入    
https://www.riverbankcomputing.com/news  
https://www.riverbankcomputing.com/static/Downloads/QScintilla/QScintilla_gpl-2.11.dev1811171407.zip  
4、 qca 源码编译  
依赖于openssl， openssl从osgeo4w下载，qca使用qca-2.1.3源码编译  
http://mirrors.shu.edu.cn/kde/ftp/stable/qca/2.1.3/src/qca-2.1.3.tar.xz  
# 源码编译  
通过cmake配置好相关的依赖后，打开vs工程进行编译，由于qgis有大约400个项目，编译时间较慢，  
本人将所有项目移出后只添加了 qgis_app\qgis_core\qgis_analysis\qgis_gui\ui\这几个项目    
qgis可以通过读取qgis.env文件，设置不同类型文件的搜索目录，本人为了简单，没有使用这样的   
配置，而是直接将依赖的dll全部拷贝到exe目录下  
## 编译遇到的异常问题
1、 文件中有未识别的变量： 编码问题，选中文件另存为带签名的utf-8  
2、 debug 模式下无法加载qgis_app， 找不到对应的模块（loadlibrary）  
这种原因是由于找不到依赖的dll导致的， 可以建立个测试程序exe，会自动提示用户缺少某个库   
```
QgsApplication a(argc, argv, true);
	QtGuiApplication1 w;
	w.show();
	return a.exec();
```
3、 debug启动后，错误信息提示QWidget: Must construct a QApplication before a QWidget， 一定是因为同时依赖了qt debug库和release库  
使用depends tool查看下哪个dll依赖了不同的库，选择正确的即可， 并不是debug中所有的库都必须使用debug的版本，一般我们只关系qt为debug版本  



