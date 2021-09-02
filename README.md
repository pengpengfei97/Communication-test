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
1. 使用的Qt5,确保电脑已安装Qt5,或者修改CMakeLists.txt文件里Qt库的配置为Qt4
2. 若找不到 package ,可能是环境变量设置问题,在终端输入:
`~$ echo "source /home/你的用户名/工作空间所在地址/devel/setup.bash" >> ~/.bashrc`  (如: `echo "source /home/pengpengfei/catkin_ws/devel/setup.bash" >> ~/.bashrc`) 
`~$ source ~/.bashrc`
然后再尝试能不能找到包
3. 包和可执行文件为临时命名,后期可根据需求修改
4. 程序中还未添加ros节点,根据需要添加节点传递数据
