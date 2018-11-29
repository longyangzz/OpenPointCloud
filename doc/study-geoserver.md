# postgis   
## shapfile导入到postgresql中
1、使用自带的工具导入   
2、导入会提示failed，执行如下语句      
```
执行以下的sql语句：  
-- Enable PostGIS (includes raster)
CREATE EXTENSION postgis;
-- Enable Topology
CREATE EXTENSION postgis_topology;
-- fuzzy matching needed for Tiger
CREATE EXTENSION fuzzystrmatch;
-- Enable US Tiger Geocoder
CREATE EXTENSION postgis_tiger_geocoder;
```  
## postgresql设置远程连接   
![示例图](../screenshot/postgressql-remote.png)   
