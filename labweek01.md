# 开题报告
## 组员
19335055何青阳

19335058贺雅迪

19335009曾嘉乐

## 选题
web服务器

## 选题目的与意义
Web服务器一般指网站服务器，是指驻留于因特网上某种类型计算机的程序，可以处理浏览器等Web客户端的请求并返回相应响应，也可以放置网站文件，让全世界浏览；可以放置数据文件，让全世界下载。

其现实价值主要体现在可以处理网页客户端请求，传输超文本文件，可以为特定用户提供网上浏览信息服务。

## 选题思路

Web服务器的主要功能是：提供网上信息浏览服务，响应终端的服务请求并进行处理。

常用性能评估指标：并发连接处理能力、每分钟负载、负载平衡、丢包率、最大响应时间等

大致的开发过程：需求分析->数据建模->功能设计->优化->压力测试

最终的目的是要完成一个web服务器的设计，实现web服务器基本功能

## 技术路线

技术原理：
web服务器为用户提供网络信息获取服务。为此，服务器需要管理与客户端的TCP链接，识别访问者身份，解析http文件，处理多用户访问，管理数据库系统，保障用户信息安全等。

为实现上述功能可能涉及涉及的原理与技术有：

传输层协议：在传输层，使用操作系统提供的接口实现基于TCP/IP协议的可靠数据传输。

应用层协议： HTTP（超文本传输协议）是一个基于请求与响应模式的、无效的、应用层的协议。绝大数的Web开发都是建立在HTTP 协议之上的Web应用。Web服务器应当能够解析客户端浏览器发送的HTTP请求含义并作出适当的回应。

客户端认证：使用用户名-密码认证、IP地址跟踪、cookie技术等实现用户认证与身份识别。

并发性：使用多个进程为多个用户同时提供服务。使用操作系统提供的API进行进程管理。

安全性：使用加密协议、加密算法、证书等方法保障传输数据的保密性、可用性、完整性。

分布式：构造分布式系统以增强服务器的可靠性可用性和扩展性。

开发工具：linux系统下主要使用C++编程

技术难题：本项目所采用的技术的成熟度都比较高，目前还未遇到难以解决的问题。
## 相关支持条件
平台:目标为制作一个web服务器平台实现基本功能

工具:

画图工具：draw.io

编辑工具：markdown

编程工具：visual studio

其他：网络编程相关(如webbench等)

技术成熟度：本项目用到的主要技术有http协议，可靠数据传输，并发与多线程，分布式系统等，这些技术均已有较多的参考资料和实现方式，较为成熟。




