## NESLoggerDemo
基于宏定义编写的日志工具,参照了java的log4j进行了日志级别设置,可以根据debug和release编译不同的日志级别.
包含了3个输出目标:控制台,文件和UIAlertController
加入了快捷的日志上传和清理方法,减少日志文件占用的磁盘空间

##How to use

 *日志函数共有6个,分别是:

    `NESDebug(@"Debug Message in %@",self);`
    `NESInfo(@"Info Message in %@",self);`
    `NESWarn(@"Warn message in %@",self);`
    `NESWarnAlert(ac, @"这是输出到文件中的%@",@"记录");`
    `NESError(@"Error message in %@",self);`
    `NESFatal(@"Fatal message in %@",self);`

 *断言共8个,分别是:
    
    `NESAssert`
    `NESAssertFlag`
    `NESAssertReturn`
    `NESAssertErrorReturn`
    `NESAssertReturnVoid`
    `NESAssertNotNull`
    `NESAssertNotNullReturn`
    `NESAssertNotNullReturnValue`

 *日志整理函数2个:
    
    `NESUploadLog`
    `NESClearLogBefore`

 *引入头文件直接使用即可,详细内容参见Demo源码
