
# HLS流协议服务端说明

###  配置信息

配置文件config.json
``` 
  {
	"日志注释": "日志保存的相对目录。",
	"log": {
		"path": "log"
	},

	"RUDP注释": "RUDP服务端的IP、端口、是否启用延时等配置。",
	"rudp": {
		"port": 41002,
		"delay": 1,
		"delay_interval": 2000
	},

	"协议类型注释": "0表示使用HLS协议，则HLS配置起作用。1表示使用UDP组播协议，则multicast配置起作用。",
	"protocol": 0,

	"HLS协议注释": "",
	"hls": {
		"echo注释": "是否将传输过来的数据弹回给客户端 0：不弹回给客户端，1：弹回给客户端",
		"echo": 0,
		"files": {
			"max注释": "自动删除文件的触发条件，文件达到此数量时，则启动过期文件删除。",
			"max": 100,
			"delete注释": "达到自动删除文件时，一次性删除文件的数量。",
			"delete": 50
		},
		"file": {
			"base注释": "通过HLS协议保存文件的路径",
			"base": "/home/fxh7622"
		}
	},

	"UDP组播协议注释": "",
	"multicast": {
		"group_ip注释": "组播地址",
		"group_ip": "127.0.0.1",
		"group_port注释": "组播端口",
		"group_port": 9999
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