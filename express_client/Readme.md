
# HLSЭ�鴫������

###  ������ָ���ļ�

**����URL��** 
- ` http://127.0.0.1:8081 `
  
**����ʽ��**
- POST 

**������** 

|������|��ѡ|����|˵��|
|:----    |:---|:----- |-----   |
|protocol |��  |int |Э�����ͣ�1��HLSЭ�飬0��UDP�鲥Э�飩   |
|hls |��  |string | HLSЭ���ʶ    |
|path |��  |string | �ļ�·��������·����    |
|extra     |��  |string | �ļ���չ��    |
|rename     |��  |string | ��������    |

 **����ʾ��**

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

 **���ز���˵��** 

|������|����|˵��|
|:-----  |:-----|-----                           |
|groupid |int   |�û���id��1����������Ա��2����ͨ�û�  |