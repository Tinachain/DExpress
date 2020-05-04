
# HLS协议传输描述

###  请求传输指定文件

**请求URL：** 
- ` http://127.0.0.1:8081 `
  
**请求方式：**
- POST 

**参数：** 

|参数名|必选|类型|说明|
|:----    |:---|:----- |-----   |
|protocol |是  |int |协议类型（1：HLS协议，0：UDP组播协议）   |
|hls |是  |string | HLS协议标识    |
|path |是  |string | 文件路径（绝对路径）    |
|extra     |是  |string | 文件扩展名    |
|rename     |是  |string | 传输后改名    |

 **返回示例**

``` 
  {
    "protocol":1,
    "hls":{
        "path":"/data/channellist/channel01/2300.m3u8",
        "extra":".m3u8",
        "rename":"index.m3u8"
    }
}
```

 **返回参数说明** 

|参数名|类型|说明|
|:-----  |:-----|-----                           |
|groupid |int   |用户组id，1：超级管理员；2：普通用户  |