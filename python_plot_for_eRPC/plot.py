import plot_bak as bak
from matplotlib.lines import Line2D
import random
import plot_data

def latency_1024_string ():

    # x_tickname = ['1', '2', '4', '8', '16', '32', '64']

    eRPC = [latency*0.001 for latency in plot_data.eRPC_1024_string]
    eRPC_protobuf = [latency*0.001 for latency in  plot_data.eRPC_protobuf_1024_string]
    zRPC = [latency*0.001 for latency in plot_data.zRPC_1024_string]

    random.shuffle(eRPC_protobuf)
    _eRPC_protobuf = []
    i = 0
    while len(_eRPC_protobuf) < 1000:
        if eRPC_protobuf[i] > 12.5 and eRPC_protobuf[i] < 25:
            _eRPC_protobuf.append(eRPC_protobuf[i])
        i+=1
    eRPC_protobuf = _eRPC_protobuf

    random.shuffle(zRPC)
    _zRPC = []
    i = 0
    while len(_zRPC) < 1000:
        if zRPC[i] < 12.6:
            _zRPC.append(zRPC[i])
        i+=1
    zRPC = _zRPC

    random.shuffle(eRPC)
    _eRPC = []
    i = 0
    while len(_eRPC) < 1000:
        if eRPC[i] < 16:
            _eRPC.append(eRPC[i])
        i+=1
    eRPC = _eRPC

    x1 = [i for i in range(0, len(eRPC))]
    x2 = [i for i in range(0, len(eRPC_protobuf))]
    x3 = [i for i in range(0, len(zRPC))]
    # x_ticks = [i for i in range(0, len(zRPC), 100)]
    x_ticks = []
    y_ticks = [10,12,15,20,25]



    label1 = 'eRPC'
    label2 = 'eRPC_protobuf'
    label3 = 'zRPC'

    bak.plot_three_line(x1=x1, y1=eRPC, x2=x2, y2=eRPC_protobuf, x3 = x3, y3=zRPC,
                      x_ticks=x_ticks, x_tickname=x_ticks,
                      y_ticks=y_ticks, y_tickname=y_ticks,
                      label1=label1, label2=label2, label3=label3,
                      ylabel='Latency (us)',
                      xlabel=' \n', file_name='string 1024.pdf', xsize=10, ysize=4,# ylim=[0, 2100],
                      le_gend_loc='upper left')
    pass

def latency_1024000_string ():

    # x_tickname = ['1', '2', '4', '8', '16', '32', '64']

    eRPC = [latency*0.001 for latency in plot_data.eRPC_1MB_string]
    eRPC_protobuf = [latency*0.001 for latency in  plot_data.eRPC_protobuf_1MB_string]
    zRPC = [latency*0.001 for latency in plot_data.zRPC_1MB_string]

    random.shuffle(eRPC_protobuf)
    _eRPC_protobuf = []
    i = 0
    while len(_eRPC_protobuf) < 800:
        # if eRPC_protobuf[i] > 12.5 and eRPC_protobuf[i] < 25:
        _eRPC_protobuf.append(eRPC_protobuf[i])
        i+=1
    eRPC_protobuf = _eRPC_protobuf

    random.shuffle(zRPC)
    _zRPC = []
    i = 0
    while len(_zRPC) < 800:
        # if zRPC[i] < 1:
        _zRPC.append(zRPC[i])
        i+=1
    zRPC = _zRPC

    random.shuffle(eRPC)
    _eRPC = []
    i = 0
    while len(_eRPC) < 800:
        # if eRPC[i] < 16:
        _eRPC.append(eRPC[i])
        i+=1
    eRPC = _eRPC

    x1 = [i for i in range(0, len(eRPC))]
    x2 = [i for i in range(0, len(eRPC_protobuf))]
    x3 = [i for i in range(0, len(zRPC))]
    # x_ticks = [i for i in range(0, len(zRPC), 100)]
    x_ticks = []
    y_ticks = [700,800,900,1000,1100,1200,1300,1400,1500]



    label1 = 'eRPC'
    label2 = 'eRPC_protobuf'
    label3 = 'zRPC'

    bak.plot_three_line(x1=x1, y1=eRPC, x2=x2, y2=eRPC_protobuf, x3 = x3, y3=zRPC,
                      x_ticks=x_ticks, x_tickname=x_ticks,
                      y_ticks=y_ticks, y_tickname=y_ticks,
                      label1=label1, label2=label2, label3=label3,
                      ylabel='Latency (us)',
                      xlabel=' ', file_name='string 1MB.pdf', xsize=10, ysize=4,# ylim=[0, 2100],
                      le_gend_loc='upper left')
    pass


def latency_100K_string ():

    # x_tickname = ['1', '2', '4', '8', '16', '32', '64']

    eRPC = [latency*0.001 for latency in plot_data.eRPC_100K_string]
    eRPC_protobuf = [latency*0.001 for latency in  plot_data.eRPC_protobuf_100K_string]
    zRPC = [latency*0.001 for latency in plot_data.zRPC_100K_string]

    random.shuffle(eRPC_protobuf)
    _eRPC_protobuf = []
    i = 0
    while len(_eRPC_protobuf) < 800:
        # if eRPC_protobuf[i] > 12.5 and eRPC_protobuf[i] < 25:
        _eRPC_protobuf.append(eRPC_protobuf[i])
        i+=1
    eRPC_protobuf = _eRPC_protobuf

    random.shuffle(zRPC)
    _zRPC = []
    i = 0
    while len(_zRPC) < 800:
        # if zRPC[i] < 1:
        _zRPC.append(zRPC[i])
        i+=1
    zRPC = _zRPC

    random.shuffle(eRPC)
    _eRPC = []
    i = 0
    while len(_eRPC) < 800:
        # if eRPC[i] < 16:
        _eRPC.append(eRPC[i])
        i+=1
    eRPC = _eRPC

    x1 = [i for i in range(0, len(eRPC))]
    x2 = [i for i in range(0, len(eRPC_protobuf))]
    x3 = [i for i in range(0, len(zRPC))]
    # x_ticks = [i for i in range(0, len(zRPC), 100)]
    x_ticks = []
    y_ticks = [80,100,120,140,160]



    label1 = 'eRPC'
    label2 = 'eRPC_protobuf'
    label3 = 'zRPC'

    bak.plot_three_line(x1=x1, y1=eRPC, x2=x2, y2=eRPC_protobuf, x3 = x3, y3=zRPC,
                      x_ticks=x_ticks, x_tickname=x_ticks,
                      y_ticks=y_ticks, y_tickname=y_ticks,
                      label1=label1, label2=label2, label3=label3,
                      ylabel='Latency (us)',
                      xlabel=' ', file_name='string 100K.pdf', xsize=10, ysize=4,# ylim=[0, 2100],
                      le_gend_loc='upper left')
    pass

def bandwidth_gbps ():
    # x_tickname = ['1', '2', '4', '8', '16', '32', '64']
    eRPC = [17.88,27.05,40.89,53.75,58.15,61.30,66.16,66.20,60.79]
    eRPC_protobuf = [13.77,22.17,34.80,45.43,52.00,53.24,55.15,53.93,36.20]
    zRPC = [17.83,28.76,45.66,60.21,66.20,68.67,71.30,76.50,77.48]

    x1 = [i for i in range(0, len(eRPC))]
    x2 = [i for i in range(0, len(eRPC_protobuf))]
    x3 = [i for i in range(0, len(zRPC))]

    x_ticks = [i for i in range(0, len(zRPC))]
    x_ticks_name = ['1K','2K','4K','8K','16K','32K','64K','128K','1M']
    y_ticks = [0,10,20,30,40,50,60,70,80]

    label1 = 'eRPC'
    label2 = 'eRPC protobuf'
    label3 = 'zRPC'

    bak.plot_three_line_marked(x1=x1, y1=eRPC, x2=x2, y2=eRPC_protobuf, x3 = x3, y3=zRPC,
                        x_ticks=x_ticks, x_tickname=x_ticks_name,
                        y_ticks=y_ticks, y_tickname=y_ticks,
                        label1=label1, label2=label2, label3=label3, lw=4,
                        ylabel='Archived Throughput\n(Gbps)',
                        xlabel='Request size (Bytes)\n', file_name='Bandwidth.pdf', xsize=10, ysize=5.7,
                        le_gend_loc='lower right')
    pass

def onepacket_latency():
    y = [[5.45,5.941,8.094],
[5.351,5.77,7.64],
[5.73,6.56,9.121],
[5.87,7.11,9.28],
[6.391,7.68,9.87]]
    y_ticks=[[2,4,6,8,10],[2,4,6,8],[2,4,6,8,10],[2,4,6,8,10],[2,4,6,8,10]]
    x = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_ticks = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_tickname = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    annotations = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    ylabel = ['Average (us)','50$^{th}$ (us)','90$^{th}$ (us)','95$^{th}$ (us)','99$^{th}$ (us)']
    xlabel = ['\n ', '\n ', '\n ', '\n ', '\n ', '\n ', '\n ']
    # xlabel = ['(a)', 'Coalescing Config\n(b)', 'Coalescing Config\n(c)', 'Coalescing Config\n(d)']
    title = ['', '', '', '', ''],
    file_name = 'one_packet.pdf'
    legend_element = [
        # Line2D([0], [0], color=bak.color[0], label='baseline', lw=6),
        # Line2D([0], [0], color=bak.color[1], label='bfq', lw=6),
        # Line2D([0], [0], color=bak.color[2], label='mq-ddl', lw=6),
        # Line2D([0], [0], color=bak.color[3], label='kyber', lw=6),
        # Line2D([0], [0], color=bak.color[4], label='bypass', lw=6),
        # Line2D([0], [0], color=bak.color[5], label='iTuner', lw=6),
    ]

    # ylabel size =
    # xlabel size =
    # xticks size =
    # yticks size =
    font_label_y = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 24
    }
    font_label_x = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 16
    }
    bak.plot_tiny_bar(x=x,
                      y=y,
                      y_ticks=y_ticks,
                      x_ticks=x_ticks,
                      x_tickname=x_tickname,
                      ylabel=ylabel,
                      xlabel=xlabel,
                      title=title,
                      file_name=file_name,
                      legend_element=legend_element,
                      xsize=10.5,
                      ysize=3.2,
                      wspace=0.1,
                      _font_label_y=font_label_y,
                      _font_label_x=font_label_x,
                      data_annotate=annotations
                      )
def multi_packet_1MB():
    y = [[828.5,774.5,1216.94],
[765.1,757.1,1052.55],
[1044,777,1128.31],
[1097,853.1,1279.92],
[1154,1227,1353.26]]
    y_ticks=[[200,400,600,800,1000,1200,1400],[200,400,600,800,1000,1200],[200,400,600,800,1000,1200],[200,400,600,800,1000,1200,1400],[200,400,600,800,1000,1200,1400]]
    x = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_ticks = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_tickname = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    y_tickname = [[200,'',600,'',1000,'',1400],['',400,'',800,'',1200],['',400,'',800,'',1200],[200,'',600,'',1000,'',1400],[200,'',600,'',1000,'',1400]]

    annotations = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    ylabel = ['Average (us)','50$^{th}$ (us)','90$^{th}$ (us)','95$^{th}$ (us)','99$^{th}$ (us)']
    xlabel = ['\n ', '\n ', '\n ', '\n ', '\n ', '\n ', '\n ']
    # xlabel = ['(a)', 'Coalescing Config\n(b)', 'Coalescing Config\n(c)', 'Coalescing Config\n(d)']
    title = ['', '', '', '', ''],
    file_name = 'multi_packet_1MB.pdf'
    legend_element = [
        # Line2D([0], [0], color=bak.color[0], label='baseline', lw=6),
        # Line2D([0], [0], color=bak.color[1], label='bfq', lw=6),
        # Line2D([0], [0], color=bak.color[2], label='mq-ddl', lw=6),
        # Line2D([0], [0], color=bak.color[3], label='kyber', lw=6),
        # Line2D([0], [0], color=bak.color[4], label='bypass', lw=6),
        # Line2D([0], [0], color=bak.color[5], label='iTuner', lw=6),
    ]

    # ylabel size =
    # xlabel size =
    # xticks size =
    # yticks size =
    font_label_y = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 26
    }
    font_label_x = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 16
    }
    bak.plot_tiny_bar(x=x,
                      y=y,
                      y_ticks=y_ticks,
                      x_ticks=x_ticks,
                      x_tickname=x_tickname,
                      y_tickname=y_tickname,
                      ylabel=ylabel,
                      xlabel=xlabel,
                      title=title,
                      file_name=file_name,
                      legend_element=legend_element,
                      xsize=12.5,
                      ysize=3.2,
                      wspace=0.1,
                      _font_label_y=font_label_y,
                      _font_label_x=font_label_x,
                      data_annotate=annotations
                      )

def multi_packet_32KB():
    y = [[39.37,36.18,46.41],
[36.18,35.85,43.15],
[51.72,36.93,59.26],
[52.49,37.49,60.47],
[53.71,48.63,62.28],]
    y_ticks=[[20,40,60],[20,40,60],[20,40,60],[20,40,60],[20,40,60,80]]
    x = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_ticks = [[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3],[1, 2, 3]]
    x_tickname = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    annotations = [['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', ''],
                  ['', '', '']]
    ylabel = ['Average (us)','50$^{th}$ (us)','90$^{th}$ (us)','95$^{th}$ (us)','99$^{th}$ (us)']
    xlabel = ['\n ', '\n ', '\n ', '\n ', '\n ', '\n ', '\n ']
    # xlabel = ['(a)', 'Coalescing Config\n(b)', 'Coalescing Config\n(c)', 'Coalescing Config\n(d)']
    title = ['', '', '', '', ''],
    file_name = 'multi_packet_32KB.pdf'
    legend_element = [
        # Line2D([0], [0], color=bak.color[0], label='baseline', lw=6),
        # Line2D([0], [0], color=bak.color[1], label='bfq', lw=6),
        # Line2D([0], [0], color=bak.color[2], label='mq-ddl', lw=6),
        # Line2D([0], [0], color=bak.color[3], label='kyber', lw=6),
        # Line2D([0], [0], color=bak.color[4], label='bypass', lw=6),
        # Line2D([0], [0], color=bak.color[5], label='iTuner', lw=6),
    ]

    # ylabel size =
    # xlabel size =
    # xticks size =
    # yticks size =
    font_label_y = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 24
    }
    font_label_x = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 16
    }
    bak.plot_tiny_bar(x=x,
                      y=y,
                      y_ticks=y_ticks,
                      x_ticks=x_ticks,
                      x_tickname=x_tickname,
                      ylabel=ylabel,
                      xlabel=xlabel,
                      title=title,
                      file_name=file_name,
                      legend_element=legend_element,
                      xsize=10.5,
                      ysize=3.2,
                      wspace=0.1,
                      _font_label_y=font_label_y,
                      _font_label_x=font_label_x,
                      data_annotate=annotations
                      )

def packet_rate ():
    y = [[0,0],[121.817,166.916],
        [66.127,85.887],
        [40.747,52.235],[0,0]]
    y_ticks = [[],[50,100,150,200],[20,40,60,80,100],[ 10,20,30,40,50,60],[]]
    x = [[1, 2], [1, 2], [1, 2], [1, 2], [1, 2]]
    x_ticks = [[1, 2], [1, 2], [1, 2], [1, 2], [1, 2]]
    x_tickname = [['', ''],
                  ['', ''],
                  ['', ''],
                  ['', ''],
                  ['', '']]
    y_tickname = [[],[50,100,150,200],[20,40,60,80,100],['',20,'',40,'',60],[]]
    annotations = [['', ''],
                   ['', ''],
                   ['', ''],
                   ['', ''],
                   ['', '']]
    ylabel = ['','512B(qps)','1KB(qps)','8KB(qps)','']
    xlabel = ['\n ', '\n ', '\n ', '\n ', '\n ', '\n ', '\n ']
    # xlabel = ['(a)', 'Coalescing Config\n(b)', 'Coalescing Config\n(c)', 'Coalescing Config\n(d)']
    title = ['', '', '', '', ''],
    file_name = 'packet_rate.pdf'
    legend_element = [
        # Line2D([0], [0], color=bak.color[0], label='baseline', lw=6),
        # Line2D([0], [0], color=bak.color[1], label='bfq', lw=6),
        # Line2D([0], [0], color=bak.color[2], label='mq-ddl', lw=6),
        # Line2D([0], [0], color=bak.color[3], label='kyber', lw=6),
        # Line2D([0], [0], color=bak.color[4], label='bypass', lw=6),
        # Line2D([0], [0], color=bak.color[5], label='iTuner', lw=6),
    ]

    # ylabel size =
    # xlabel size =
    # xticks size =
    # yticks size =
    font_label_y = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 24
    }
    font_label_x = {
        'family': 'Times New Roman',
        'weight': 'normal',
        'size': 16
    }
    bak.plot_tiny_bar(x=x,
                      y=y,
                      y_ticks=y_ticks,
                      x_ticks=x_ticks,
                      x_tickname=x_tickname,
                      y_tickname=y_tickname,
                      ylabel=ylabel,
                      xlabel=xlabel,
                      title=title,
                      file_name=file_name,
                      legend_element=legend_element,
                      xsize=10.5,
                      ysize=3.2,
                      wspace=0.1,
                      _font_label_y=font_label_y,
                      _font_label_x=font_label_x,
                      data_annotate=annotations
                      )
if __name__ == '__main__':
    # latency_1024_string()
    # latency_1024000_string()
    # latency_100K_string()
    bandwidth_gbps()
    # onepacket_latency()
    # multi_packet_32KB()
    # multi_packet_1MB()
    # packet_rate()
