//
//  NESLogger.h
//  NESLoggerDemo
//
//  Created by Nestor on 23/3/20.
//  Copyright (c) 2015年 NesTalk. All rights reserved.
//]

#ifndef __NES_LOGGER_H
#define __NES_LOGGER_H

//========consts============

/** LOG级别定义 
 通过0000 0000 0000 0000标识输出级别和目标,高8位表示输出目标,低8位代表输出级别
 例:
 1111 0000 0000 0001 表示向控制台和文件进行输出DEBUG级别信息,16进制表示为0xF001
 1100 0000 0000 0001 表示仅向控制台输出DEBUG信息,16进制标示为0xC001
 0011 0000 0000 0001 表示仅向文件输出DEBUG信息,16进制表示为0x3001
 0011 1111 0000 0001 表示像控制台和UIAlert输出DEBUG信息,16进制表示为0x3F01
 */
#define LOG_LEVEL_DEBUG     0xC001           ///调试,仅在DEBUG状态下输出,默认只输出到控制台
#define LOG_LEVEL_INFO   0x3002               ///信息,通常用作提示性文本,默认只输出到本地log文件
#define LOG_LEVEL_WARN    0x3F04           ///警告,某些情况下可能会导致程序错误或影响程序正常执行,默认输出到文件和UIAlertView
#define LOG_LEVEL_ERROR   0xF008           ///错误,没有获得正常预期的结果,但是不会影响程序继续执行,默认输出到控制台和
#define LOG_LEVEL_FATAL      0xF010       ///严重错误,会造成程序中断
#define LOG_LEVEL_ASSERT    0xF0FF        //伪断言,辅助进行各种判断,在不满足条件时输出,但是不会中断程序,默认输出到控制台和文件,但不会弹出Alert

#define TARGET_MASK_FILE 0x3000
#define TARGET_MASK_CONSOLE 0xC000
#define TARGET_MASK_ALERT 0xF00

/** LOG输出模式定义 */

///调试模式,包含所有输出内容
#define LOG_MODE_DEBUG (LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL | LOG_LEVEL_WARN)
///信息模式,包含除DEBUG之外的全部输出
#define LOG_MODE_INFO (LOG_LEVEL_INFO | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL | LOG_LEVEL_WARN)
///警告模式,包含除DEBUG和INFO之外的全部输出内容
#define LOG_MODE_WARN (LOG_LEVEL_ERROR | LOG_LEVEL_WARN | LOG_LEVEL_FATAL)
///调试模式,仅显示ERROR和FATAL
#define LOG_MODE_ERROR (LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)
///不做任何输出
#define LOG_MODE_NONE 0x0

/**LOG输出目标定义
 输出目标分为全局定义和级别定义,全局定义通过下面两个宏进行控制,级别定义通过不同级别的二进制编码控制
 仅当全局定义和级别定义同时为真时才会输出到指定目标
 */

//控制台输出
#define LOG_TARGET_CONSOLE    0x1
//输出至文件
#define LOG_TARGET_FILE          0x2

#define LOG_PATH [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"NESLogger"]

//默认字符串常量
//参数顺序:输出级别,日期,时间,文件,行号,函数名,消息
#define NESLOGGER_DEFAULT_C_LOG_FORMAT "%s [%s %s] [%s:%d] %s:\n-->\t%s\n"
//参数含义:要判断的表达式
#define NESLOGGER_DEFAULT_OC_LOG_FORMAT @"Condition '%s' failed : "
#define NESLOGGER_DEFAULT_ALERT_TITLE @"出问题啦..."
#define NESLOGGER_DEFAULT_ALERT_OK_BUTTON_TITLE @"确定"

//========end of consts============

/** 输出模式
 DEBUG模式下默认为DEBUG模式,输出至控制台和文件,所有输出内容都将被显示
 非DEBUG模式下为WARN模式,输出至文件,Debug和Info内容将被隐藏,
 默认log文件保存在~/Library/Caches/(projectName_Log)目录下
 ASSERT始终可用
 可以通过位运算自定义所需的模式,或直接引用已定义的宏
 */
#ifdef DEBUG
#define LOG_MODE LOG_MODE_DEBUG
#define LOG_TARGET LOG_TARGET_CONSOLE | LOG_TARGET_FILE
#else
#define LOG_MODE LOG_MODE_WARN
#define LOG_TARGET LOG_TARGET_FILE
#endif

///基本输出宏,手动指定输出级别,不建议直接使用
#define NESLog(fmt,level,...) \
do{\
    if((level) & (LOG_MODE)){\
        NSString *logLevel;\
        switch ((level)) {\
            case LOG_LEVEL_DEBUG:logLevel = @"<DEBUG>";break;\
            case LOG_LEVEL_ASSERT:logLevel = @"<ASSERT>"; break;\
            case LOG_LEVEL_ERROR:logLevel = @"<ERROR>"; break;\
            case LOG_LEVEL_INFO:logLevel = @"<INFO>"; break;\
            case LOG_LEVEL_WARN:logLevel = @"<WARN>"; break;\
            case LOG_LEVEL_FATAL:logLevel = @"<FATAL>";break;\
        }\
        if((LOG_TARGET_CONSOLE & LOG_TARGET) && (level & TARGET_MASK_CONSOLE)){\
            NSMutableString *str = [[NSMutableString alloc] initWithFormat:fmt,##__VA_ARGS__];\
            printf(NESLOGGER_DEFAULT_C_LOG_FORMAT ,logLevel.UTF8String,__DATE__,__TIME__,[[NSString stringWithUTF8String:__FILE__] lastPathComponent].UTF8String,__LINE__,[NSString stringWithUTF8String:__PRETTY_FUNCTION__].UTF8String,str.UTF8String);\
        }\
        if((LOG_TARGET_FILE & LOG_TARGET) && (level & TARGET_MASK_FILE)){\
            dispatch_async(dispatch_get_global_queue(0, 0), ^{\
                NSString *path = LOG_PATH;\
                [[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];\
                NSDate *date = [NSDate date];\
                NSDateFormatter *dateFmt = [[NSDateFormatter alloc] init];\
                [dateFmt setDateFormat:@"yyyy-MM-dd"];\
                NSString *logName = [[dateFmt stringFromDate:date] stringByAppendingPathExtension:@"log"];\
                path = [path stringByAppendingPathComponent:logName];\
                FILE* log = fopen(path.UTF8String, "a");\
                if (log) {\
                    NSMutableString *str = [[NSMutableString alloc] initWithFormat:fmt,##__VA_ARGS__];\
                    fprintf(log, NESLOGGER_DEFAULT_C_LOG_FORMAT ,logLevel.UTF8String,__DATE__,__TIME__,[[NSString stringWithUTF8String:__FILE__] lastPathComponent].UTF8String,__LINE__,[NSString stringWithUTF8String:__PRETTY_FUNCTION__].UTF8String,str.UTF8String);\
                    fclose(log);\
                }\
            });\
        }\
    }\
}while(0)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 18:15:04
 *
 *  将所有本地日志上传至服务器,该宏会将所有本地日志拼接成一个字符串传递给用于上传的block,具体的上传逻辑自行编写
 *  实例:
 *
 *  NESUploadLog(^(NSString *log){
 *      //upload log by your own code...
 *  });
 *
 *  @param uploadBlock 格式为void (^)(NSString *)类型的block
 */
#define NESUploadLog(uploadBlock) \
do{\
    dispatch_async(dispatch_get_global_queue(0, 0), ^{\
        NSString *path = LOG_PATH;\
        NSArray *arr = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:nil];\
        NSMutableString *log = [[NSMutableString alloc] init];\
        [arr enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {\
            NSString *name = obj;\
            if ([name rangeOfString:@".log"].location == NSNotFound) return;\
            [log appendString:name];\
            [log appendString:@"\n=================\n"];\
            NSString *content = [NSString stringWithContentsOfFile:[path stringByAppendingPathComponent:name] encoding:NSUTF8StringEncoding error:nil];\
            [log appendString:content];\
            [log appendString:@"///////////////////////\n"];\
        }];\
        void (^upload)(NSString *) = uploadBlock;\
        upload(log);\
    });\
}while(0)\

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 18:21:40
 *
 *  清理本地日志宏,删除指定日期之前的所有日志,当天的日志不会被删除
 *  实例:
 *  NESClearLogBefore([NSDate date]); //删除今天之前的所有日志
 *  NESClearLogBefore(nil); //删除所有日志
 *
 *  @param date 目标日期
 */
#define NESClearLogBefore(date) \
do{\
    NSDate *d = date;\
    dispatch_async(dispatch_get_global_queue(0, 0), ^{\
        NSString *path = LOG_PATH;\
        if (d == nil) [[NSFileManager defaultManager] removeItemAtPath:path error:nil];\
        else\
        {\
            NSDateFormatter *fmt = [[NSDateFormatter alloc] init];\
            [fmt setDateFormat:@"yyyy-MM-dd"];\
            NSTimeInterval time = d.timeIntervalSince1970;\
            NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:nil];\
            [files enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {\
                NSString *name = obj;\
                if ([name rangeOfString:@".log"].location == NSNotFound) return;\
                name = [[name componentsSeparatedByString:@"."] firstObject];\
                if (time - [fmt dateFromString:name].timeIntervalSince1970 > 60*60*24 ) {\
                    [[NSFileManager defaultManager] removeItemAtPath:[path stringByAppendingPathComponent:obj] error:nil];\
                }\
            }];\
        }\
    });\
}while(0)\

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 13:12:30
 *
 *  伪断言,仅当条件失败时输出信息
 *
 *  @param condition 要判断的条件
 *  @param fmt       消息格式描述字符串
 *  @param ...       消息参数列表
 */
#define NESAssert(condition,fmt,...) \
do{\
    if(!(condition)){\
        NESLog(NESLOGGER_DEFAULT_OC_LOG_FORMAT fmt,LOG_LEVEL_ASSERT, #condition, ##__VA_ARGS__);\
    }\
} while(0)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 13:08:34
 *
 *  判断结果伪断言,将判断的结果放入指定的变量中,并在条件失败时输出信息
 *
 *  @param condition 要判断的条件
 *  @param fmt       错误消息格式描述字符串
 *  @param flag      保存表达式结果的变量
 *  @param ...       消息参数
 */
#define NESAssertFlag(condition,fmt,...)\
^(){NESAssert(condition,fmt,##__VA_ARGS__);return condition;}()

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 13:04:32
 *
 *  错误返回伪断言,当给定条件失败时返回给定值并输出失败信息
 *
 *  @param condition   要判断的条件
 *  @param fmt         错误消息格式描述字符串
 *  @param returnValue 指定返回值
 *  @param ...         消息参数
 *
 *  @return 指定的返回值
 */
#define NESAssertReturn(condition,fmt,returnValue,...)\
do{\
    NESAssert(condition,fmt,##__VA_ARGS__);\
    if(!(condition)) return returnValue;\
} while(0)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 13:04:25
 *
 *  错误返回伪断言,当给定条件失败时初始化一个NSError给指定的变量,并将由fmt和可变参拼接的消息作为Error描述,打印错消息后返回指定值
 *
 *  @param condition   要进行判断的条件
 *  @param error       NSError的二级指针,NSError **err
 *  @param fmt         错误消息格式描述字符串
 *  @param returnValue 出错时返回值
 *  @param ...         错误消息参数
 *
 *  @return 指定的返回值
 */
#define NESAssertErrorReturn(condition,error,fmt,returnValue,...)\
do{\
    NESAssert(condition,fmt,##__VA_ARGS__);\
    if(!(condition)){\
        NSString *msg = [NSString stringWithFormat:fmt,##__VA_ARGS__];\
        if(error){*error = [NSError errorWithDomain:msg code:59969 userInfo:nil];}\
        return returnValue;\
    }\
} while(0)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 13:43:25
 *
 *  条件失败时直接返回
 */
#define NESAssertReturnVoid(condition,fmt,...) NESAssertReturn(condition,fmt,, ##__VA_ARGS__)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 14:49:02
 *
 *  判断指定对象或者表达式是否为nil,返回结果,并当指定表达式为nil时输出信息
 */
#define NESAssertNotNull(condition,fmt,...) \
^(){NESAssert((condition)!=nil,fmt,##__VA_ARGS__);return (condition)!=nil;}()

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 14:51:29
 *
 *  当表达式为nil时输出信息并返回
 */
#define NESAssertNotNullReturn(condition,fmt,...) NESAssertReturnVoid((condition)!=nil,fmt,##__VA_ARGS__)

/**
 *  @author writen by Nestor. Personal site - http://www.nestor.me , 15-03-23 15:01:07
 *
 *  当表达式为nil时输出信息并返回指定值
 */
#define NESAssertNotNullReturnValue(condition,fmt,returnValue,...) NESAssertReturn((condition)!=nil,fmt,returnValue,##__VA_ARGS__)

/** 按照error级别输出 */
#define NESError(fmt,...) NESLog(fmt,LOG_LEVEL_ERROR, ##__VA_ARGS__)
/** 按照warn级别输出 */
#define NESWarn(fmt,...) NESWarnAlert(nil,fmt, ##__VA_ARGS__)
/** 按照warn级别输出,同时显示一个由用户自定义的UIAlertController*/
#define NESWarnAlert(alertController,fmt,...) \
do{\
    NESLog(fmt,LOG_LEVEL_WARN,##__VA_ARGS__);\
    if(!(LOG_LEVEL_WARN & TARGET_MASK_ALERT)) break;\
    UIAlertController *nesLoggerAlertController = alertController;\
    if(!nesLoggerAlertController){\
        NSMutableString *msg = [[NSMutableString alloc] initWithFormat:fmt,##__VA_ARGS__];\
        nesLoggerAlertController = [UIAlertController alertControllerWithTitle:NESLOGGER_DEFAULT_ALERT_TITLE message:msg preferredStyle:UIAlertControllerStyleAlert];\
        [nesLoggerAlertController addAction:[UIAlertAction actionWithTitle:NESLOGGER_DEFAULT_ALERT_OK_BUTTON_TITLE style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {\
            [nesLoggerAlertController dismissViewControllerAnimated:YES completion:nil];\
        }]];\
    }\
    id target = self;\
    while (target && ![target isKindOfClass:[UIViewController class]])\
        target = ((UIResponder *)target).nextResponder;\
    if(target) [target presentViewController:nesLoggerAlertController animated:YES completion:nil];\
}while(0)\
/** 按照info级别输出 */
#define NESInfo(fmt,...) NESLog(fmt,LOG_LEVEL_INFO, ##__VA_ARGS__)
/** 按照Debug级别输出 */
#define NESDebug(fmt,...) NESLog(fmt,LOG_LEVEL_DEBUG, ##__VA_ARGS__)
/** 按照Fatal级别输出 */
#define NESFatal(fmt,...) NESLog(fmt,LOG_LEVEL_FATAL, ##__VA_ARGS__)
#endif
