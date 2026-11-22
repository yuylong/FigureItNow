# FigureItNow7 - Windows 安装包制作（Qt Installer Framework）

## 概述

安装包由 [packaging/build_installer.ps1](../packaging/build_installer.ps1) 一键生成，脚本自动完成「Release 构建 →
暂存安装树 → 收集 Qt 运行库 → 渲染 IFW 配置 → 生成安装程序」全流程，不需要手工调用 `windeployqt` 或
`binarycreator`。

```powershell
cd FigureItNow7\packaging
.\build_installer.ps1 -Clean
# 产物：FigureItNow7\dist\FigureItNow7-<version>-win64-setup.exe
```

## 相关文件

```
FigureItNow7/
├── packaging/
│   ├── build_installer.ps1          # 一键脚本
│   ├── config/config.xml            # 安装器全局配置模板
│   ├── config/style.qss             # 安装器向导样式表（<StyleSheet> 引用）
│   ├── config/controlscript.qs      # 升级控制脚本（<ControlScript> 引用，覆盖安装前自动清理旧版）
│   ├── packages/io.github.yuylong.figureitnow7/          # 主程序组件（必选，脚本锁定为灰色勾选）
│   │   ├── data/                    # 暂存目录（生成物）
│   │   └── meta/
│   │       ├── package.xml          # 组件元数据模板
│   │       ├── license.html         # GPL-3 声明（富文本，许可页自动按 HTML 渲染）
│   │       └── installscript.qs     # 开始菜单 / 桌面快捷方式 / .fsc 文件关联
│   ├── packages/io.github.yuylong.figureitnow7examples/  # 示例脚本组件（平级、可选、默认勾选）
│   │   ├── data/                    # examples/ 暂存目录（生成物）
│   │   └── meta/
│   │       ├── package.xml          # 组件元数据模板
│   │       └── installscript.qs     # 开始菜单「Example Scripts」文件夹快捷方式
│   ├── build-ifw/                   # 渲染后的暂存树（生成物，忽略）
│   └── repository/                  # hybrid 模式的更新仓库（生成物，忽略）
├── cmake/
│   ├── BuildVersion.cmake           # 版本号唯一实现
│   └── FindBuildEnv.cmake           # Qt 与配套 MinGW 探测唯一实现
├── build-release/                   # Release 构建目录（生成物）
└── dist/                            # 安装包输出（生成物）
```

`build-release/` 与日常开发的 `build/` 相互独立，出包不会影响开发构建。

## 前置要求

| 依赖 | 要求 | 检查方式 |
|---|---|---|
| Qt 6 + 配套 MinGW | 已固定 Qt 6.12.0 + MinGW 15.1.0；旧工具链不再作为默认方案 | 无需配置，脚本自动探测 |
| CMake | 3.21+（`CMakeLists.txt` 的最低要求），需在 `PATH` 中 | `cmake --version` |
| Qt Installer Framework | 已验证 **4.11**，如 `C:\Qt\Tools\QtInstallerFramework\4.11\bin` | 无需配置，脚本自动递归查找 |

IFW 若未安装：Qt Online Installer → **Qt → Build tools → Qt Installer Framework**。

Qt 与配套 MinGW 的探测规则由 [cmake/FindBuildEnv.cmake](../cmake/FindBuildEnv.cmake) 统一实现（同一份逻辑
也被 CMake 构建和 VSCode 配置生成器复用），规则是「同一安装根下最新版本胜出」。需要固定到
Qt 6.12.0 + MinGW 15.1.0 时显式传 `-QtDir` 和 `-MingwBin`。

## 从源码生成

### 1. 获取源码

```powershell
git clone https://github.com/yuylong/FigureItNow.git
cd FigureItNow\FigureItNow7\packaging
```

> **不要用 `--depth=1` 浅克隆**，否则检出里没有 tag，版本号会退化成 `0.0.0`。已经浅克隆的话执行
> `git fetch --tags` 补齐。

版本号来自单一实现 [cmake/BuildVersion.cmake](../cmake/BuildVersion.cmake)：`v0.5-alpha-191-gd4ee9d3` →
安装包版本 `0.5.191`，与应用内 `$APP_VERSION`、About 对话框同源。工作区有未提交改动时会带 `-dirty` 后缀
（只影响显示名，不影响数字版本）；正式出包建议使用干净检出。

### 2. 一键生成

```powershell
.\build_installer.ps1 -Clean
```

被执行策略拦截时：

```powershell
powershell -ExecutionPolicy Bypass -File .\build_installer.ps1 -Clean
```

`-Clean` 会先删除 `build-release/` 和 `packaging/build-ifw/`，保证从零开始；去掉该开关即为增量构建。

### 3. 产物

```
FigureItNow7\dist\FigureItNow7-0.5.191-win64-setup.exe
```

## 脚本内部流程

| 步骤 | 内容 |
|---|---|
| 1 | 用 `cmake -P cmake/BuildVersion.cmake` 取版本号，不做第二套 git 解析 |
| 2 | 用 `cmake -P cmake/FindBuildEnv.cmake` 取 Qt / MinGW，并探测 IFW |
| 3 | 可选 `-Clean` 清理旧输出 |
| 4 | `cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release` 并构建 |
| 5 | `cmake --install build-release --prefix <data>` 生成暂存树 |
| 6 | `windeployqt --release --compiler-runtime` 收集 Qt 插件与 MinGW 运行库 |
| 7 | 校验必需文件（exe、`platforms/qwindows.dll`、三个 MinGW DLL），缺失时从 MinGW 目录补拷并告警 |
| 8 | 复制 `examples/` 到示例组件的数据目录；`README.md`、`LICENSE` 进主组件 |
| 9 | 渲染 `config.xml` / 两个组件的 `package.xml` 模板，复制 `meta/` 下其余文件 |
| 10 | `binarycreator` 生成安装包；hybrid 模式再调用 `repogen` 生成更新仓库 |

默认模式是 **offline**（`binarycreator --offline-only`），安装后不联网更新。

## 验证安装包

交互安装：直接双击 `dist\...\setup.exe`，会弹 UAC，安装到 `C:\Program Files\FigureItNow7`。

无人值守安装（**必须在管理员会话里执行**，非提权会报
`Cannot elevate access rights while running from command line`）：

```powershell
Start-Process .\dist\FigureItNow7-0.5.191-win64-setup.exe -Verb RunAs `
  -ArgumentList '--accept-licenses','--confirm-command','install' -Wait
```

卸载（使用安装后生成的维护工具）：

```powershell
& 'C:\Program Files\FigureItNow7\FigureItNow7Maintenance.exe' --confirm-command --accept-licenses uninstall
```

升级 / 覆盖安装：对已安装的机器直接再次运行新版 `setup.exe`（同一目标目录）即可。IFW 本身拒绝向已含安装的
目录安装（`TargetDirectoryInUse` 硬错误，`--default-answer` 也无法跳过），升级逻辑由
[config/controlscript.qs](../packaging/config/controlscript.qs) 实现：检测到目标目录里有旧维护工具时，先静默
调用它执行 `purge`（卸载全部组件并删除程序目录，连同快捷方式、`.fsc` 关联与「设置」中的卸载条目一并清理），
再按全新安装继续。因此「设置」里始终只有最后一次安装、不会出现两条；旧版本里已被新版移除的文件也不会残留。
程序正在运行时安装器会提示先关闭再重试。

安装包含两个平级组件：**FigureItNow7**（主程序，必选）与 **Example Scripts**（示例脚本，默认勾选、
可在向导中取消，安装到 `examples\` 文件夹；标识符用单段名 `figureitnow7examples`——点分段会把它嵌成
主程序的下级，连字符又是 IFW 保留给版本号的字符）。组件树按 `<SortingPriority>` 降序排列（相同优先级
按显示名排序，因此主程序设 100、示例设 10，保证主程序在上）。主程序不使用 `ForcedInstallation`——IFW
对必装组件不渲染任何复选框——而是在组件脚本里 `component.enabled = false` 锁定：显示为灰色、带勾、
点击与「恢复默认 / 全部取消」都无法改变其选中状态。安装后会创建开始菜单（全体用户，由脚本设置
`AllUsers=true`，与 Program Files 全局安装一致）和桌面快捷方式，并注册 `.fsc` 文件关联（默认打开方式 +
图标 + 「打开方式」列表），均由主组件的
[installscript.qs](../packaging/packages/io.github.yuylong.figureitnow7/meta/installscript.qs) 负责；
示例组件的
[installscript.qs](../packaging/packages/io.github.yuylong.figureitnow7examples/meta/installscript.qs)
另建一个指向 `examples\` 文件夹的开始菜单快捷方式「Example Scripts」；卸载时一并清理（文件关联按
MSDN 惯例只移除自家 ProgID，不覆盖期间被其他程序接管时的 `.fsc` 默认值）。

`.fsc` 关联写入 `HKLM\Software\Classes`（安装需要管理员权限，属全局关联），ProgID 使用 IFW 默认的
`fsc_auto_file`（不传自定义 `ProgId=`：该参数在执行时会被从记录的参数里剥掉，卸载时 undo 找不到它就会
清错键、留下注册表残留），升级安装不会破坏已有关联。文件图标引用 exe 的第一个内嵌图标——仓库目前还没有
`.rc` / `.ico`（见下文「尚未实现」），在此之前显示的是 Windows 通用可执行文件图标。

## 常用参数

```powershell
# 只重打包，复用已有 Release 构建（改配置或脚本时最快）
.\build_installer.ps1 -SkipBuild

# 只做到暂存目录，不生成安装包（没装 IFW 时也能验证其余环节）
.\build_installer.ps1 -SkipPackage

# 显式指定工具链 / IFW（自动探测失效时）
.\build_installer.ps1 -QtDir C:\Qt\6.12.0\mingw_64 -MingwBin C:\Qt\Tools\mingw1510_64\bin `
                      -IfwRoot C:\Qt\Tools\QtInstallerFramework\4.11\bin

# 带上 OpenGL 软件回退等全量 Qt 后端（默认排除，约省 50 MB）
.\build_installer.ps1 -Full

# 附带更多 Qt 内置翻译（默认只 zh_CN；传空字符串表示不带任何翻译）
.\build_installer.ps1 -Translations zh_CN,zh_TW

# 有在线更新仓库后：离线安装 + 在线更新
.\build_installer.ps1 -Mode hybrid -RepoUrl https://yuylong.github.io/FigureItNow/updates
```

| 参数 | 默认值 | 说明 |
|---|---|---|
| `-Mode` | `offline` | `offline` 纯离线安装；`hybrid` 离线装 + 在线更新 |
| `-RepoUrl` | 空 | hybrid 必填，IFW 会自行拼 `/Updates.xml` |
| `-IfwRoot` | 自动探测 | IFW 的 `bin` 目录；也读环境变量 `QTIFWDIR` |
| `-QtDir` | 自动探测 | Qt 安装目录，如 `C:\Qt\6.12.0\mingw_64` |
| `-MingwBin` | 自动探测 | MinGW `bin` 目录；也读环境变量 `MINGW_PATH` |
| `-Translations` | `zh_CN` | 逗号分隔的 Qt 内置翻译语言 |
| `-Full` | 关 | 部署全部 Qt 图形后端 |
| `-Clean` | 关 | 清空 `build-release` 与 `build-ifw` |
| `-SkipBuild` | 关 | 复用已有 `build-release` |
| `-SkipPackage` | 关 | 停在暂存目录，不调用 `binarycreator` |

## 预期输出

```
FigureItNow7 installer build
    Mode        : offline
    Version     : 0.5.191

==> Detecting build environment
    [OK] Qt: C:/Qt/6.12.0/mingw_64 (version 6.12.0)
    [OK] MinGW: C:/Qt/Tools/mingw1510_64/bin
    [OK] IFW: C:\Qt\Tools\QtInstallerFramework\4.11\bin

==> Cleaning previous output
==> Configuring and building Release

==> Installing into staging directory
==> Deploying Qt runtime (windeployqt)
==> Verifying staging completeness
    [OK] All required files present
    [OK] Staging: 29 files, 50.8 MB

==> Copying bundled resources
    [OK] examples component (19 files)

==> Rendering IFW configuration
==> Building installer
    [OK] Installer: ...\dist\FigureItNow7-0.5.191-win64-setup.exe (52 MB)
Done.
```

全新构建约 2～4 分钟（构建约 2 分钟，打包约 1 分钟）。

## 注意事项与常见问题

- **构建会改写 `.vscode\settings.json` / `launch.json`**。这是 CMake 配置阶段的既有行为（由
  [cmake/VscodeConfig.cmake](../cmake/VscodeConfig.cmake) 按探测到的 Qt 写入），出包后编辑器可能切到
  Qt 6.12.0 / MinGW 15.1.0。直连 CMake 时可用 `-DFIN_GENERATE_VSCODE_CONFIG=OFF` 关闭；安装脚本目前不透传该开关。
- **IFW 4.11 没有 `--silent` 选项**，传了只会得到 `Unknown option: silent`。headless 模式由
  `--confirm-command install|uninstall` 触发。
- **`setup.exe --help-all` 会直接进入安装流程**，不要把它当纯帮助命令。
- **找不到 `binarycreator.exe`**：传 `-IfwRoot` 或设置 `QTIFWDIR`。脚本默认递归搜索 `C:\Qt\Tools`、
  `C:\Program Files\Qt`、`C:\Program Files (x86)\Qt`。
- **找不到 Qt / MinGW**：传 `-QtDir` / `-MingwBin`，或设置 `QTDIR` / `MINGW_PATH`。
- **版本号显示 0.0.0**：源码目录不是 git 工作树，或浅克隆缺少 tag。
- **`meta/` 里新增的文件没被打包**：脚本会复制 `meta/` 下除 `package.xml`（需模板渲染）之外的所有文件，
  但组件脚本还必须在 `package.xml` 里用 `<Script>` 声明，否则 `binarycreator` 会静默忽略。
- **安装包需要管理员权限**：`package.xml` 中 `RequiresAdminRights=true`、`TargetDir` 为
  `@ApplicationsDir@/FigureItNow7`，属预期设计。
- **向导窗口外观**：初始大小由 `config.xml` 的 `WizardDefaultWidth/Height`（800×600）控制；
  页面四周留白来自 `WizardStyle=Classic`（IFW 默认的 ModernStyle 页边距为零，文字会贴住窗口边沿）；
  侧边页面列表的背景与分隔线由 `config/style.qss` 提供。许可页内容按 HTML 富文本渲染
  （`license.html` 必须以标签开头，否则退化为纯文本显示）。

## 尚未实现

- EXE 元信息与安装包图标（仓库暂无 `.rc` 与 `.ico`，`icons/` 下只有 png；`.fsc` 文件图标暂用 exe 图标，落地后自动生效）
- 数字签名（未签名会触发 SmartScreen 提示）
- 32 位安装包（当前仅 MinGW 64 位）
- CI 自动出包（GitHub Actions）
- 在线更新仓库（`-Mode hybrid` 的代码路径已就绪，等待 GitHub Pages 托管）
