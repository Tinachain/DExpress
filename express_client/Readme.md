
# HLS流协议传输说明

###  配置信息

配置文件config.json
``` 
  {
	"测试注释": "是否启动测试。",
	"test": 1,
	"Web注释": "Web监听的端口。",
	"web": {
		"port": 8084
	},
	"日志注释": "日志保存的相对目录。",
	"log": {
		"path": "log"
	},
	"RUDP注释": "RUDP服务端的IP、端口、是否启用延时等配置。",
	"rudp": {
		"ip": "192.168.3.89",
		"port": 41002,
		"start注释": "是否启用RUDP",
		"start": 1,
		"delay注释": "是否启用延时，如果启用则接收自动延时相应毫秒数后输出数据流，从而达到稳流作用",
		"delay": 0,
		"delay_interval注释": "延时输出毫秒数量，只用在delay为真时才起作用。",
		"delay_interval": 2000,
		"reconnect注释": "RUDP端口连接后，等待多少秒钟进行重连。",
		"reconnect": 30
	},
	"协议类型注释": "0表示使用HLS协议，则HLS配置起作用。1表示使用UDP组播协议，则multicast配置起作用。",
	"protocol": 0,
	"HLS协议注释": "",
	"hls": {
		"concurrency注释": "存放文件的队列长度",
		"concurrency": 10,
		"resend_interval注释": "文件重发时长设置（单位秒）",
		"resend_interval": 20,
		"files": {
			"max注释": "自动删除文件的触发条件，文件达到此数量时，则启动过期文件删除。",
			"max": 100,
			"delete注释": "达到自动删除文件时，一次性删除文件的数量。",
			"delete": 50
		},
		"file": {
			"base注释": "使用echo配置以后，客户端保存文件的主目录。",
			"base": "/home/fxh7622/channellist"
		}
	},
	"UDP组播协议注释": "",
	"multicast": {
		"group_ip注释": "组播地址",
		"group_ip": "239.30.10.121",
		"group_port注释": "组播端口",
		"group_port": 8121
	}
}

```

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