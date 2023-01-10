### AutoUpdate 简介
AutoUpdate是基于QT的通用桌面程序自动升级更新解决方案。

### QT版本
5.15.X

### 开发工具
 Qt Creator
 
### 后端接口配置格式
后端接口配置为json格式文件，这里保存的文件名为auto_upgrade.json，可自定义。

```json
{
    "version": 100,
    "versionStr": "1.0.0",
    "type": 1,
    "zipurl": "http://192.168.1.1/autoupdate/main.zip",
    "mainAppName":"main.exe",
    "filelist": [
        {
            "path": "/",
            "sublist": [
                "http://192.168.1.1/autoupdate/dist/libmysql.dll",
                "http://192.168.1.1/autoupdate/dist/main.exe"
            ]
        },
        {
        	"path": "dir",
            "sublist": [
                "http://192.168.1.1/autoupdate/dist/dir/xxx1.dll",
                "http://192.168.1.1/autoupdate/dist/dir/xxx2.dll"
            ]
        }
    ]
}

```

其中：

* version为纯数字版本号（这是升级判断的依据）；
* versionStr为显示的版本号；
* type为更新模式，1表示更新指定文件（即是filelist字段内容，此时zipurl可为空），2表示压缩包方式更新,压缩包为zip压缩（此时filelist可为[]）；
* mainAppName是主程序名称，更新完毕后会自动启动主程序；
* zipurl压缩包地址，type为2时候有效，必须为zip格式压缩；
* filelist 是指定文件更新方式，其中path为空或/ 表示主目录，有具体目录名称表示子目录(子目录前请后勿加/)

压缩包内容示例(dist是待压缩的目录)：
```
dist
├── dir
│   ├── xxx1.dll
│   └── xxx2.dll
├── main.exe
└── libmysql.dll
```

### 本程序相关配置
* 版本号文件为version.dat二进制文件，只记录数字版本号，写入版本号示例代码（写入版本号100）：

```c++
    QString configFilePath = QDir::currentPath()+"/version.dat";
    configFilePath = QDir::toNativeSeparators(configFilePath);
    QFile writeFile(configFilePath);
    writeFile.open(QIODevice::WriteOnly);
    QDataStream out(&writeFile);
    out << 100;
    writeFile.close();
```

* 后端配置地址，位于当前目录的setting.ini文件内的upgrade节点，如：

```ini

[upgrade]
url=http://192.168.1.1/autoupdate/auto_update.json

```
 
### 发布
##### Windows环境打包

1. 用Release模式运行一遍;
2. 在Build directory目录下的dist目录中会生成autoUpgrader.exe可执行文件；
3. 打开QT命令行(开始菜单里面/QT下)，进入dist目录；
4. 执行：windeployqt autoUpgrader.exe
5. 将系统盘windows/system32/下的msvcp140_1.dll和vcruntime140_1.dll复制到dist目录。
