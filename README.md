## 编译环境
- ubuntu 18.04
- ros melodic
- Qt creator 4.5.2
- Qt 5.9.5

## 运行流程
### Qt Creator中运行
1. 打开Qt Creator后，点击坐上角　文件->打开文件或项目　在文件选择窗口中分别选择local.pro和remote.pro打开两个项目
2. 在左上方项目区域中右键单击local文件夹后点击运行，同样的方式右键单击remote文件夹点击运行即可运行两个项目进行通信（注意先运行local项目再运行remote项目，否则会remote项目会提示无连接）
### 在ros中运行
1. 将ros_qt文件夹放到ros工作空间的src目录下
2. 回到工作空间一级目录使用catkin_make指令编译工作空间
3. 启动roscore,再输入 rosrun ros_qt qt_ros_node 即可执行文件
#### 在ros中使用注意事项
1. 编译使用的Qt5,确保电脑已安装Qt5,或者修改CMakeLists.txt文件里Qt库的配置为Qt4
2. 若找不到 package ,可能是环境变量设置问题,在终端输入:
~$ `echo "source /home/你的用户名/工作空间所在地址/devel/setup.bash" >> ~/.bashrc`  (如: `echo "source /home/pengpengfei/catkin_ws/devel/setup.bash" >> ~/.bashrc`) 
~$ `source ~/.bashrc`
然后再尝试能不能找到包
3. 包和可执行文件为临时命名,后期可根据需求修改
4. 程序中还未添加ros节点,根据需要添加节点传递数据
## 关于文件移动到测试区
- 测试的时候可以在Qt Creator直接运行loacl和remote两个项目进行测试，也可以在ros中启动ros_qt qt_ros_node，然后在Qt Creator中运行remote项目进行测试，效果是一样的。由于remote只是模拟的裁判方程序，所以没有放进ros里
- 运行前将Raw-Camera-Timestamp.csv文件和Raw-withOffset.csv文件放至/tmp路径下，以便进行读取文件内容并在在指定路径下生成新的文件。
- 在remote项目中修改了发送的令牌内容。发送两个令牌，时间戳是随便写的，令牌的值设置的为0（由于现在的测试文件是未加密的，设置为0在解密的时候就不会影响文件内容），令牌的类型为C和P，分别对应Raw-Camera-Timestamp.csv文件和Raw-GPS-withOffset.csv文件。
- 文件的转移在loacl的main.cpp文件的DecodeToken_string函数中进行，相关路径变量名分别设为path和path1
- 运行效果是如果测试区没有这两个csv文件会生成两个csv文件，如果有的话会改写文件内容为/tmp路径下对应文件的内容
