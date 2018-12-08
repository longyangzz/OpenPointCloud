# freecad编译  
1、下载0.17 release源码包  
2、下载libpack包，FreeCADLibs_11.5.3_x64_VC12  
3、cmake gui生成vs2013 工程，需要设置libpack路径   
4、打开vs工程编译  
![freecad窗口](../screenshot/freecad-main.png)   

## python
python模块搜索路径  
```
import sys  
for p in sys.path:  
    print p  
```
