# -
一个用来计算卫星轨道的bug层出不穷的计算器
使用IDE：Qt Creator
编译器：Qt_5_11_2_MinGW_32bit

关于编译源文件方法可以自行百度“qt生成可执行文件”

//todo

--法向切向变轨节点

--万年历

--把行星数据设为config文件

更新日志：

18.11.25

实际上对于辛四阶龙格库塔法，如果哈密顿量显含t的话，在求解时会使用到比上一步对应时间t还要早的函数值，还有比这一步结束时间t还要晚的函数值，所以预先解的行星运动应该留出一定的空余，而我一开始没有，这导致了一大堆bug中的一些bug。具体做法是求解行星时从初始条件逆向求解总求解时长的百分之一，防止访问越界的发生。这个修复不多，先不更新release。

18.11.29

-修正了关闭窗口无法删去节点的bug

-修正了调节倍数时的bug

-在Probe类重新计算前清除数据，防止出现不可预测的bug

18.12.7

-关闭mainwindow时关闭子窗口

-解微分方程使用独立线程

-加入行星碰撞半径

-行星名字改为string储存

-修复了退出时窗口未响应的bug

-计算时按钮变灰防止误按

-加入换参考系的功能

19.1.7

-改变部分程序结构

-添加读取保存轨道

-添加能用的粒子群算法，细节还不完善

-求解器步长函数改变防止经过奇点时误差过大

-修复一些bug
