//
//  ViewController.m
//  NESLoggerDemo
//
//  Created by Nestor on 15/3/24.
//  Copyright (c) 2015年 NesTalk. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()<UITableViewDelegate,UITableViewDataSource>

@property (nonatomic,retain) UITableView *table;
@property (nonatomic,retain) NSArray *datasource;

@end

@implementation ViewController

-(NSArray *)datasource
{
    if (!_datasource) {
        _datasource = @[@"LogPath",@"Debug",@"Info",@"Warn",@"WarnAlert",@"Error",@"Fatal",@"Assert",@"AssertFlag",@"AssertReturn",@"AssertErrorReturn",@"AssertReturnVoid",@"AssertNotNull",@"AssertNotNullReturn",@"AssertNotNullReturnValue",@"UploadLog",@"ClearLogsBeforeToday",@"ClearAllLogs"];
    }
    return _datasource;
}

-(UITableView *)table
{
    if (!_table) {
        _table = [[UITableView alloc] initWithFrame:self.view.bounds style:UITableViewStylePlain];
        _table.delegate = self;
        _table.dataSource = self;
        _table.tableFooterView = [UIView new];
    }
    return _table;
}

#pragma mark -
#pragma mark UITableView datasource method

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.datasource.count;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *reuseId = @"reuseId";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:reuseId];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseId];
    }
    cell.textLabel.text = [self.datasource objectAtIndex:indexPath.row];
    return cell;
}

#pragma mark -
#pragma mark UITableViewDelegate method

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    [self performSelector:NSSelectorFromString([NSString stringWithFormat:@"test%@",[self.datasource objectAtIndex:indexPath.row]])];
#pragma clang diagnostic pop
}

-(void)testLogPath
{
    NSLog(@"LOG_PATH:%@",LOG_PATH);
}

-(void)testDebug
{
    NESDebug(@"Debug Message in %@",self);
}

-(void)testInfo
{
    NESInfo(@"Info Message in %@",self);
}

-(void)testWarn
{
    NESWarn(@"Warn message in %@",self);
}

-(void)testWarnAlert
{
    UIAlertController *ac = [UIAlertController alertControllerWithTitle:@"自定义Alert" message:@"这是一个自定义的AlertController" preferredStyle:UIAlertControllerStyleAlert];
    [ac addAction:[UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        NSLog(@"%s-[%s]:alert 确定",__func__,__TIME__);
        [ac dismissViewControllerAnimated:YES completion:nil];
    }]];
    [ac addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        NSLog(@"%s-[%s]:alert 取消",__func__,__TIME__);
        [ac dismissViewControllerAnimated:YES completion:nil];
    }]];
    NESWarnAlert(ac, @"这是输出到文件中的%@",@"记录");
}

-(void)testError
{
    NESError(@"Error message in %@",self);
}

-(void)testFatal
{
    NESFatal(@"Fatal message in %@",self);
}

-(void)testAssert
{
    NESAssert(1==1, @"nothing happens");
    NESAssert(1!=1, @"condition is false, print assert level log in %@",self);
}

-(void)testAssertFlag
{
    if (NESAssertFlag(1==1, @"nothing happens"))
        NSLog(@"this message will shows up cause the flag is YES");
    if (NESAssertFlag(1!=1, @"condition is false, print assert level log in %@",self))
        NSLog(@"nothing happens cause the flag is NO");
}

-(int)testAssertReturn
{
    NESAssertReturn(1==1, @"nothing happens in %@", INT_MAX,NSStringFromSelector(_cmd));
    NSLog(@"this message will shows up cause '1==1' is YES, function will not return");
    NESAssertReturn(1!=1, @"print assert level log in %@, and return the value behind", INT_MAX,NSStringFromSelector(_cmd));
    NSLog(@"this message will not shows up casue '1!=1' is NO");
    return 0;
}

-(void)testAssertErrorReturn
{
    NSError *err = nil;
    int ret = [self testAssertErrorReturnSupport:&err];
    NSLog(@"err is %@,return value is:%d",err,ret);
}

-(int)testAssertErrorReturnSupport:(NSError **)err
{
    NESAssertErrorReturn(1==1, err, @"error will be nil and no log will be printed.", INT_MAX);
    NSLog(@"error is:%@",*err);
    NESAssertErrorReturn(1!=1, err, @"error will be set by a NSError object, and return the value behind immdiately", INT_MAX);
    NSLog(@"error is:%@ -- this message will not be printed cause the function is already returned",*err);
    return 0;
}

-(void)testAssertReturnVoid
{
    NESAssertReturnVoid(1==1, @"nothing happens in %@",NSStringFromSelector(_cmd));
    NSLog(@"this message will be printed, cause '1==1' is YES.");
    NESAssertReturnVoid(1!=1, @"print assert level log and return immdiately.");
    NSLog(@"this message will never be printed");
}

-(void)testAssertNotNull
{
    BOOL flag = NESAssertNotNull([NSObject new], @"nothing happends in %@",NSStringFromSelector(_cmd));
    NSLog(@"object is %@.",flag ? @"not nil" : @"nil");
    NESAssertNotNull(nil, @"print assert level log cause object is nil.");
    NSLog(@"object is %@.",flag ? @"not nil" : @"nil");
    
    if (NESAssertNotNull(nil, @"pass nil to NESAssertNotNull this message will shows up and if-branch will get a NO")) {
        NSLog(@"Test Object is not nil");
        //do anything you want when the object is not nil;
    }
}

-(void)testAssertNotNullReturn
{
    NESAssertNotNullReturn([NSObject new], @"nothing happends in %@",NSStringFromSelector(_cmd));
    NSLog(@"this message will be printed, cause object is not nil.");
    NESAssertNotNullReturn(nil, @"print assert level log and return immdiately, cause object is nil.");
    NSLog(@"this message will never be printed");
}

-(NSObject *)testAssertNotNullReturnValue
{
    NESAssertNotNullReturnValue([NSObject new], @"nothing happens in %@", [NSObject new],NSStringFromSelector(_cmd));
    NSLog(@"this message will be printed, cause object is not nil.");
    NESAssertNotNullReturnValue(nil, @"print assert level log and return the object behind, cause object is %@.", [NSObject new],nil);
    NSLog(@"this message will never be printed");
    return nil;
}

-(void)testUploadLog
{
    //is a simple way to check log contents by this method
    NESUploadLog(^(NSString *logString){
        NSLog(@"log file contents:\n%@",logString);
        //upload this log yourself;
    });
}

-(void)testClearLogsBeforeToday
{
    NESClearLogBefore([NSDate date]);
}

-(void)testClearAllLogs
{
    NESClearLogBefore(nil);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"NESLogger Demo";

    [self.view addSubview:self.table];
    
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
