   
# 撮合服务器

## 代码路径

    Dposchain_V2\boker\pair

## 配置信息

### 配置文件路径
    events.json（保存配置信息）
    keystore.json（保存账号信息）
    
### 配置格式

```json
{
    "RPC":"http://192.168.3.35:8545",   RPC地址
    "WsAddr":"ws://192.168.3.35:8546",  WebSocket地址
    "ContractAddr":"0x1822513dD5cab174760E2E9209656a00330C0B30",    Chainware合约地址
    "KeystoreFile":"keystore.json",     账号json
    "Passwrod":"123456",                账号密码
    "Listen":"0.0.0.0:18085",           Web监听的端口，手动发送http请求，用于测试
    "Events":[
        {
            "Index":1,
            "Use":1,
            "Signature":"SetNode(address,uint256,bytes32,bytes32)",
            "Comment":"同步单个节点事件"
        },
        {
            "Index":2,
            "Use":1,
            "Signature":"NodeEnrollEvent(address,uint256,bytes32)",
            "Comment":"节点上线事件"
        },
        {
            "Index":3,
            "Use":1,
            "Signature":"NodeDisconnectEvent(address)",
            "Comment":"节点下线事件"
        },
        {
            "Index":101,
            "Use":1,
            "Signature":"SetTask(bytes32,uint256,address,taskState)",
            "Comment":"同步事件"
        },
        {
            "Index":102,
            "Use":1,
            "Signature":"TasksRequestEvent(bytes32,uint256)",
            "Comment":"任务请求撮合事件"
        },
        {
            "Index":103,
            "Use":1,
            "Signature":"TasksCancelEvent(bytes32)",
            "Comment":"任务取消事件"
        },
        {
            "Index":104,
            "Use":1,
            "Signature":"TasksCompleteEvent(address,bytes32)",
            "Comment":"任务完成事件"
        }
    ]
}
```

### 测试HTTP请求,请求均调用合约中后缀为_Test接口，参数和正式接口不一致。

#### 节点加入
    http://***/NodeEnroll?nodeAddr=8978bfdd91fd9c52d60737b5359ee544836fe66a&hashRate=100000000
	
#### 任务请求
	http://***/TaskRequest?clientAddr=8978bfdd91fd9c52d60737b5359ee544836fe66c&index=1&taskGas=500000000&codeHash=10922333&taskIp=***
	
#### 任务取消
	http://***/TaskCancle?orderID=f765172a78d7a7ebf954a8e13a0650b14ca38de81e86337e9f8f0e6acc5613a6&clientAddr=8978bfdd91fd9c52d60737b5359ee544836fe66c
	
#### 任务完成
	http://****/TaskComplete?node=8978bfdd91fd9c52d60737b5359ee544836fe66a&orderID=45a690b39818f49677310e10c4a6e73d1560284c4ec35212c4fbdab3277bab4a&realGas=40000&resultHash=12345
	
#### 节点退出
	http://****/NodeDisconnect?node=8978bfdd91fd9c52d60737b5359ee544836fe66a


### 撮合服务器特殊业务处理流程概述

#### 有任务节点下线
	1：得到下线节点assignedClient
	2：判断节点assignedClient是否为空，如果为空直接下线。
	3：如果不为空，根据assignedClient从正在处理任务map中得到任务信息。
	4：将此任务放入得到分配任务队列中，等待下一次分配。
	
#### 任务没有合适的撮合节点
	1：判断任务需要花费的时间（completeTime := (task.TaskGas.Uint64() / v.HashRate.Uint64()) + header.PREPARE_TIME）
	2：判断花费时间是否大于最大时间（MAX_WAITING_TIME = 300 //最大等待时间（秒））
	3：如果大于则判断下一个空闲节点。
	4：空闲节点最大探测数量（MAX_SEARCH_ASSIGNED_NODE = 10）
