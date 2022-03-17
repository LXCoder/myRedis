# myRedis
写了个跳表，并封装成一个ZSet类，通过命令表的模式模仿redis的有序集合命令功能


## 使用方法：
#### 编译服务端生成可执行文件
make                
#### 编译客户端生成可执行文件
make myRedis-cli    

#### 先启动服务端你
./myRedis-server
#### 启动客户端
./myRedis-cli 127.0.0.1 8888
