# 软工导论开题报告

姓名|学号|版本管理员
--|:--:|--:
何青阳|19335055|√
贺雅迪|19335058|
曾嘉乐|19335009|

## 选题背景与意义
### 1.1 选题背景

因特网（Internet）是一个全球性的计算机互联网络，因特网上具有大量的技术资料数据库，其信息媒体包括数据、图像、文字、声音、视频等多种形式，信息属性有数据、交换软件、图书、档案等等。信息内容涉及通信、天文、医疗、政府机关、娱乐等各个方面。

万维网（WWW，也可简称为web）是因特网的一部分，它是一种建立在因特网上的全球性的
交互的、动态的分布式的信息系统，是一种建立在因特网上的网络服务。它的最基本概念就是超文本传输。

目前市场上有30多种web应用服务器，每种服务器在部署速度和易用程度上都有一定的差别，因此可以看出，web服务器已经是一项较为成熟的技术。

### 1.2 选题意义

网络技术是当前计算机领域的的热门方向之一，在日常生活中，网络为我们的学习、工作、娱乐提供了极大的便利条件，在信息时代，网络编程对程序员来说是一项非常重要的技能。

通过这次从零开始搭建一个web服务器的过程，我们能对网络底层编程产生一个更深刻的理解，对HTTP等协议原理的掌握更加熟练。并且web服务器是一种比较有用的软件，搭建过程中获得的技能也是很让人收益的。

另外，我们也希望在这个过程中能通过自己的方式进行一些不一样的尝试和创新，通过这个项目培养出一定的项目搭建能力、团队协作能力、文档表述能力与问题解决能力。
## 研究内容与目标
### 2.1 研究内容
本项目主要研究的是如何在Linux下主要使用C++搭建一个轻量级的网络服务器，并在这个过程中实践和熟悉网络编程。

首先是unix环境下高级编程的学习和网络编程的研究。另外为了维护用户数据，对数据库操作与sql编程也在我们的研究范围之内。同时也会对一些应用层协议尤其是HTTP、HTTPS协议进行研究和实现。我们也有兴趣想要在这个过程中尝试写一个我们自己的应用层协议和解析器。

除此之外，为了完成一些并发操作以及提高效率，本项目还会涉及到一些多线程开发、分布式的内容。

### 2.2 研究目标

通过阶段性地完成本次项目，我们希望在最终能够搭建一个功能较为完整、可用性较强、安全性较高的web服务器，并且在这个过程中充分掌握网络编程、unix环境操作、项目开发基本流程、团队协作等重要的能力。

## 研究方法与手段

本项目采用增量开发模型。

首先经过需求分析和概要设计，需要确定系统的基于增量构件的需求框架，并以需求框架中构件的组成及关系为依据，完成对软件系统的体系结构设计。

具体来说，优先实现服务器的核心功能组建，如建立并维护网络连接、解析并响应http请求、管理本地数据库等。同时尝试周边构件的实现，如客户认证、日志记录与加密传输等并将其集成至现有系统。每次集成入新构件，都要对系统进行测试此种开发方法要求各组建有较好的独立性与简洁的接口。

## 预期研究成果
本课题为构造一个linux环境下web服务器，预期成果如下：

1. 采用线程池＋非阻塞socket+epoll+事件处理的并发模型
2. 使用状态机解析HTTP请求报文，支持解析GET和POST请求
3. 完成访问数据库实现的注册和登录功能及校验
4. 实现记录服务器运行状态的同步/异步日志系统
5. 实现上万并发连接数的数据交换（用Webbench进行压力测试)

本服务器添加事务处理和数据库连接池等额外功能，可以为用户提供网络信息获取服务，向客户端提供文件、网页等内容。作为Internet上最大的计算机群，可以支持较大容量的并发连接。

## 参考文献

[1]  W.Richard Stevens, Stephen A.Rago unix环境高级编程[M]

[2] 史蒂文斯, 芬纳, 鲁道夫 unix网络编程[M]

[3] 骆耀祖 Linux网络服务器管理教程[M]