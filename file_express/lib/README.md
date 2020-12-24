# DExpress(Data Express)

------

DExpress是一款低延时安全数据传输产品。这款产品采用HARQ（混合自动重传请求）技术，实现了底层数据的可靠传输，FEC冗余包控制在10%之内。同时采用了AES加密算法，为每次传输都进行了数据加密，保证了数据在传输过程中的安全、私密性。同时拥塞控制采用了类似WebRTC的拥塞算法（丢包拥塞和卡尔曼滤波器），从而保证了在不同网络情况下，发送数据占用带宽能够非常平滑的进行扩充或者收敛。

## HLS低延时安全传输架构图
![image](https://github.com/Tinachain/DExpress/blob/master/image/framework_hls.jpg)

## UDP组播公网传输架构图
![image](https://github.com/Tinachain/DExpress/blob/master/image/framework_udp.jpg)

## 性能导图
![image](https://github.com/Tinachain/DExpress/blob/master/image/performance.jpg)


