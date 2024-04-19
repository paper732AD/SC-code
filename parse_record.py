import os
import sys
import xlwt

latency_50 = []
latency_90 = []
latency_95 = []
latency_99 = []
latency_avg = []
bandwidth = []
sum_bw = 0
qps = []
sum_qps = 0
bandwidthgbps=[]
book : xlwt.Workbook
sheet : any

def parse_str (lines = [str]):
    global latency_50,latency_90,latency_95,latency_99,latency_avg,bandwidth,qps,sum_qps,sum_bw
    for line in lines:
        # print(line[1:5])
        if(line[1:5] == 'avge'):
            latency_avg.append(float(line[15+1:20+1]))
            print('average latency: "'+line[15+1:20+1]+'"')
        if(line[1:5] == '50th'):
            latency_50.append(float(line[15+1:20+1]))
            # print('50th latency: '+line[15:20])
        if(line[1:5] == '90th'):
            latency_90.append(float(line[15+1:20+1]))
            # print('90th latency: '+line[15:20])
        if(line[1:5] == '95th'):
            latency_95.append(float(line[15+1:20+1]))
            # print('95th latency: '+line[15:20])
        if(line[1:5] == '99th'):
            latency_99.append(float(line[15+1:20+1]))
            # print('99th latency: '+line[15:20])
        if(line[:4] == 'QPS '):
            qps.append(float(line[7:]))
            sum_qps += qps[-1]
            # print('QPS latency: '+line[7:-1])
        if(line[:4] == 'Band'):
            bandwidth.append(float(line[19:]))
            sum_bw += bandwidth[-1]
        if(line[len(line)-7:-1] == 'Gbps/s'):
            bandwidthgbps.append(float(line[6:-7]))
            print('bandwidthgbps '+line[6:-7])
            # print('tail of bandwidthgbps '+line[len(line)-7:-1])

def get_sum (datas = [float]):
    sum = 0.0
    for data in datas:
        sum += data
    return sum

def get_mean (datas = [float]):
    return get_sum(datas)/len(datas)

def parse_record (record_len : int):
    file_list = ['record0.log','record1.log','record2.log','record3.log','record4.log','record5.log','record6.log','record7.log',
    'record8.log','record9.log','record10.log','record11.log','record12.log','record13.log','record14.log','record15.log','record16.log']
    

    for file in file_list[0:record_len]:
        with open(file, 'r') as fff:
            lines = fff.readlines()
            parse_str(lines)

    print(latency_50)
    print(latency_90)
    print(latency_95)
    print(latency_99)
    print(latency_avg)
    print(bandwidth)
    print(qps)

    with open('record.total.log', 'w+') as f:
        f.write('average latency_avg:\t%f\n'%get_mean(latency_avg))
        f.write('average latency_50:\t\t%f\n'%get_mean(latency_50))
        f.write('average latency_90:\t\t%f\n'%get_mean(latency_90))
        f.write('average latency_95:\t\t%f\n'%get_mean(latency_95))
        f.write('average latency_99:\t\t%f\n\n'%get_mean(latency_99))
        
        f.write('average qps:\t\t\t%f\n'%get_mean(qps))
        f.write('total qps:\t\t\t\t%f\n\n'%sum_qps)
        
        f.write('average bandwidth:\t\t%f\n'%get_mean(bandwidth))
        f.write('total bandwidth:\t\t%f\n'%sum_bw)

def clean_array ():
    global latency_50,latency_90,latency_95,latency_99,latency_avg,bandwidth,qps,sum_qps,sum_bw
    latency_50.clear()
    latency_90.clear()
    latency_95.clear()
    latency_99.clear()
    latency_avg.clear()
    bandwidth.clear()
    bandwidthgbps.clear()
    qps.clear()

def write_defination_to_sheet (volume = 0):
    global sheet

    defination_str = ['序列化后','初始化后','第一个包发送的时间',
                '最后一个包发送的时间','接受到第一个回复的时间','收到最后一个回复的时间',
                #'反序列化后的时间',
                '最终的延迟']
    common_info = ['average latency','50th','90 th','95 th','99 th']
    last_info = [' ','带宽', 'QPS', '带宽 Gbps']

    r = 1
    for st in defination_str:
        sheet.write(r,0,st)
        r += 1
        for info in common_info:
            sheet.write(r,0,info)
            r += 1
    for last in last_info:
        sheet.write(r,0,last)
        r += 1
    sheet.col(0).width = 5000


    
    

def write_data_to_xlwt(col, file_name, col_name, latency_avg, latency_50, latency_90, latency_95, latency_99, bandwidth, qps, bandwidthgbps):
    global sheet

    sheet.col(col).width = 5000

    index = 0
    r = 0
    sheet.write(r, col, file_name)
    r += 1
    for index in range(0, len(col_name)):
        # sheet.write(r, col, col_name[index])
        sheet.write(r, col, '  ')
        r += 1
        sheet.write(r, col, latency_avg[index])
        r += 1
        sheet.write(r, col, latency_50[index])
        r += 1
        sheet.write(r, col, latency_90[index])
        r += 1
        sheet.write(r, col, latency_95[index])
        r += 1
        sheet.write(r, col, latency_99[index])
        r += 1
    sheet.write(r, col, '  ')
    r += 1    
    if len(bandwidth) > 0:
        sheet.write(r, col, bandwidth[0])
        r += 1
    if len(qps) > 0:
        sheet.write(r, col, qps[0])
        r += 1
    if len(bandwidthgbps) > 0:
        sheet.write(r, col, bandwidthgbps[0])
        r += 1

    

def parse_latency(prefix=''):
    global book,sheet
    # file_list = ['latency_test_512.txt','latency_test_1024.txt','latency_test_8192.txt','latency_test_10240.txt',
    #              'latency_test_20480.txt','latency_test_32768.txt','latency_test_102400.txt','latency_test_204800.txt','latency_test_1024000.txt', ]
    file_list = ['record_detail_512.txt','record_detail_1024.txt','record_detail_2048.txt','record_detail_4096.txt',
                 'record_detail_8192.txt','record_detail_16384.txt',
                 'record_detail_32768.txt','record_detail_65536.txt','record_detail_102400.txt','record_detail_131072.txt',
                 'record_detail_1024000.txt']
    col = 1 # 数据写到第几列
    book = xlwt.Workbook(encoding='utf-8')
    sheet = book.add_sheet('latency_test')
    write_defination_to_sheet()

    col_name = []

    for file in file_list:
        i = 0
        file=prefix+file
        with open(file,'r',encoding='utf-8') as f:
            records = f.readlines();
            #out_serialize
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #out_init_time
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #first_send
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #out_before_send
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #first_receive
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #out_at_receive
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:i+6])
            i = i+6
            #out_after_deserialize
            # col_name.append(records[i].split(' ')[0])
            # parse_str(records[i+1:i+6])
            # i = i+6
            #total
            col_name.append(records[i].split(' ')[0])
            parse_str(records[i+1:])

            print(col_name)
            print(latency_avg)
            print(latency_50)
            print(latency_90)
            print(latency_95)
            print(latency_99)
            print(bandwidth)
            print(qps)
            print(bandwidthgbps)

            write_data_to_xlwt(col, file.split('\\')[-1], col_name, latency_avg, latency_50, latency_90, latency_95, latency_99, bandwidth, qps,bandwidthgbps)
            col += 1
            clean_array()
            col_name.clear()


    book.save('zrpc.xls')

            


if __name__ == '__main__':
    # print(sys.argv[1])
    parse_name = str(sys.argv[1])

    if parse_name == 'record':
        record_len = int(sys.argv[2])
        parse_record(record_len)
    elif parse_name == 'latency':
        prefix = sys.argv[2]
        parse_latency(prefix)


    
        