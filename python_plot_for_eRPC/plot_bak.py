import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

font_label_y = {
    'family': 'Times New Roman',
    'weight': 'normal',
    'size': 26
}
font_label_x = {
    'family': 'Times New Roman',
    'weight': 'normal',
    'size': 26
}

font_legend = {
    'family': 'Times New Roman',
    'weight': 'normal',
    'size': 26
}

ticks_size = 26

# color = ['#FFA50077', '#e5737377', '#20B2AA77', '#cbccd677', '#FFA50077', '#DDA0DD77', '#0288d177' , '#20B2AA77']#,  '#cae798']
color = ['#FFA500', '#e57373', '#20B2AA', '#cbccd6', '#FFA500', '#DDA0DD', '#0288d1' , '#20B2AA']#,  '#cae798']
# color = ['#e57373', '#20B2AA','#cbccd6', '#FFA500', '#DDA0DD', '#0288d1' ]#,  '#cae798']

def set_xticks(subfig):
    x_ticks_size = ticks_size

    labels = subfig.get_xticklabels()
    [label.set_fontsize(x_ticks_size) for label in labels]
    [label.set_fontname('Times New Roman') for label in labels]

    labels = subfig.get_yticklabels()
    [label.set_fontsize(x_ticks_size) for label in labels]
    [label.set_fontname('Times New Roman') for label in labels]

def auto_text(subfig, rects, annotate):
    i = 0
    for rect in rects:
        subfig.text(rect.get_x() - 0.1 * rect.get_width(), 1 * rect.get_height(), annotate[i], ha='left',
                    va='bottom', font='Times New Roman', fontsize=22)
        i += 1


def plot_three_line(x1=[], y1=[], x2=[], y2=[], x3=[], y3=[],
                    label1='', label2='', label3='',
                    x_ticks=[], x_tickname=[],
                    y_ticks=[], y_tickname=[],lw=2,
                    title='',
                    xlabel='', ylabel='',
                    file_name='',
                    legend_element={},
                    xsize=10, ysize=2,
                    wspace=0.2,
                    ylim=[], xlim=[], le_gend_loc=''):
    fig = plt.figure(figsize=(xsize, ysize), constrained_layout=True)
    subfig = plt.subplot()
    print('plotting')
    if len(x1) != 0 :
        # subfig.scatter(np.array(x1), np.array(y1), color=color[0], label=label1, lw=lw+1)
        subfig.plot(np.array(x1), np.array(y1), color=color[0], label=label1, lw=lw)
    if len(x2) != 0 :
        # subfig.scatter(np.array(x2), np.array(y2), color=color[1], label=label2, lw=lw+1)
        subfig.plot(np.array(x2), np.array(y2), color=color[1], label=label2, lw=lw)
    if len(x3) != 0 :
        # subfig.scatter(np.array(x3), np.array(y3), color=color[2], label=label3, lw=1)
        subfig.plot(np.array(x3), np.array(y3), color=color[2], label=label3, lw=lw)
    print('plotted')

    # plt.subplots_adjust(wspace=wspace)  # 调整子图间距
    subfig.set_xticks(x_ticks)
    subfig.set_xticklabels(x_tickname)
    subfig.set_yticks(y_ticks)
    subfig.set_yticklabels(y_tickname)
    set_xticks(subfig)
    # subfig[i].set_title(title[i])

    subfig.grid(axis='y', color='gray', linestyle=':')

    subfig.set_ylabel(ylabel, font_label_y)
    subfig.set_xlabel(xlabel, font_label_x)
    subfig.legend(loc=le_gend_loc, prop=font_legend)
    subfig.spines['top'].set_visible(False)
    subfig.spines['right'].set_visible(False)

    if not len(ylim) == 0:
        subfig.set_ylim(ylim[0], ylim[1])
    if not len(xlim) == 0:
        subfig.set_xlim(xlim[0], xlim[1])
    # subfig[i].spines['bottom'].set_visible(False)
    # subfig[i].spines['left'].set_visible(False)

    # subfig[0].set_ylim(0, 100)
    # i = len(x) - 1
    # subfig[i].legend(handles=legend_element, loc=2, bbox_to_anchor=(1.05, 1.0), prop=font_legend, borderaxespad=0.)

    # fig.tight_layout()  # 调整整体空白
    plt.subplot_tool()
    plt.savefig(file_name)
    print('showing')
    plt.show()

    pass


def plot_three_line_marked(x1=[], y1=[], x2=[], y2=[], x3=[], y3=[],
                    label1='', label2='', label3='',
                    x_ticks=[], x_tickname=[],
                    y_ticks=[], y_tickname=[],lw=2,
                    title='',
                    xlabel='', ylabel='',
                    file_name='',
                    legend_element={},
                    xsize=10, ysize=2,
                    wspace=0.2,
                    ylim=[], xlim=[], le_gend_loc=''):
    fig = plt.figure(figsize=(xsize, ysize), constrained_layout=True)
    subfig = plt.subplot()
    print('plotting')
    if len(x1) != 0 :
        # subfig.scatter(np.array(x1), np.array(y1), color=color[0], label=label1, lw=lw+1)
        subfig.plot(np.array(x1), np.array(y1), color=color[0], label=label1, lw=lw,marker='v'
         ,markeredgecolor=color[0],markersize=lw,markeredgewidth=10)
    if len(x2) != 0 :
        # subfig.scatter(np.array(x2), np.array(y2), color=color[1], label=label2, lw=lw+1)
        subfig.plot(np.array(x2), np.array(y2), color=color[1], label=label2, lw=lw,marker='.'
         ,markeredgecolor=color[1],markersize=lw,markeredgewidth=10)
    if len(x3) != 0 :
        # subfig.scatter(np.array(x3), np.array(y3), color=color[2], label=label3, lw=1)
        subfig.plot(np.array(x3), np.array(y3), color=color[2], label=label3, lw=lw,marker='X'
         ,markeredgecolor=color[2],markersize=lw,markeredgewidth=10)
    print('plotted')

    # plt.subplots_adjust(wspace=wspace)  # 调整子图间距
    subfig.set_xticks(x_ticks)
    subfig.set_xticklabels(x_tickname)
    subfig.set_yticks(y_ticks)
    subfig.set_yticklabels(y_tickname)
    set_xticks(subfig)
    # subfig[i].set_title(title[i])

    subfig.grid(axis='y', color='gray', linestyle=':')

    subfig.set_ylabel(ylabel, font_label_y)
    subfig.set_xlabel(xlabel, font_label_x)
    subfig.legend(loc=le_gend_loc, prop=font_legend)
    subfig.spines['top'].set_visible(False)
    subfig.spines['right'].set_visible(False)

    if not len(ylim) == 0:
        subfig.set_ylim(ylim[0], ylim[1])
    if not len(xlim) == 0:
        subfig.set_xlim(xlim[0], xlim[1])
    # subfig[i].spines['bottom'].set_visible(False)
    # subfig[i].spines['left'].set_visible(False)

    # subfig[0].set_ylim(0, 100)
    # i = len(x) - 1
    # subfig[i].legend(handles=legend_element, loc=2, bbox_to_anchor=(1.05, 1.0), prop=font_legend, borderaxespad=0.)

    # fig.tight_layout()  # 调整整体空白
    plt.subplot_tool()
    plt.savefig(file_name)
    print('showing')
    plt.show()

    pass

def bench_mark(x1=[], y1=[], x2=[], y2=[], y_o=[],y_ticks=[],
               label1='',
               label2='',
               x_ticks=[], x_tickname=[],
               title='',
               width=0.36,
               ylabel='',
               xlabel='',
               file_name='',
               legend_element={},
               xsize=7, ysize=2,
               wspace=0.2,
               ylim=[], xlim=[], le_gend_loc=''):
    x = np.array(x1)
    y1 = np.array(y1) / 2
    y2 = np.array(y2) / 2
    y_o = np.array(y_o) / 2

    x_tickname = ['Latency', 'Total Time', 'Interrupt', 'IOPS']
    fig = plt.figure(figsize=[xsize, ysize])
    subfig = plt.subplot(1, 2, 1)

    # fig.tight_layout()  # 调整整体空白
    plt.subplots_adjust(wspace=0.1, hspace=0)  # 调整子图间距

    subfig.bar(x - 0.5 * width, y_o, width, label='Baseline', color=color[0])
    subfig.bar(x + 0.5 * width, y1, width, label='iTuner', color=color[1])
    subfig.set_xticks(x)
    subfig.set_xticklabels(x_tickname, font_label_x)
    # subfig.legend(loc='upper center', prop=font_legend)
    for a, b in zip(x + 0.5 * width, y1):  ##控制标签位置
        subfig.text(a - 0.00 * width, b + 0.01, '%.2f' % (b * 2), ha='center', va='bottom', font='Times New Roman', fontsize=24)
    set_xticks(subfig)
    subfig.spines['top'].set_visible(False)
    subfig.spines['right'].set_visible(False)
    subfig.set_yticks([0.25,0.5,0.75])
    subfig.set_yticklabels(['0.5','1','1.5'])
    subfig.grid(axis='y', color='gray', linestyle=':')
    subfig.set_title('db_bench', font_label_y)
    # lgd = subfig.legend(loc='lower left', prop=font_legend)
    # lgd.NumColumns = 2

    subfig = plt.subplot(1, 2, 2)
    subfig.bar(x - 0.5 * width, y_o, width, label='Baseline', color=color[0])
    subfig.bar(x + 0.5 * width, y2, width, label='iTuner', color=color[1])
    subfig.set_xticks(x)
    subfig.set_xticklabels(x_tickname, font_label_x)
    # lgd = subfig.legend(loc='lower left', prop=font_legend)
    # lgd.NumColumns = 3
    set_xticks(subfig)
    for a, b in zip(x + 0.5 * width, y2):  ##控制标签位置
        subfig.text(a - 0.00 * width, b + 0.01, '%.2f' % (b * 2), ha='center', va='bottom', font='Times New Roman', fontsize=21)
    subfig.spines['top'].set_visible(False)
    subfig.spines['right'].set_visible(False)
    subfig.set_yticks([0.25,0.5,0.75])
    subfig.set_yticklabels(['0.5','1','1.5'])
    subfig.grid(axis='y', color='gray', linestyle=':')
    subfig.set_title('YCSB', font_label_y)

    # subfig = plt.subplot(1,2,2)
    # subfig.legend(handles=legend_element, loc=2, bbox_to_anchor=(1.0, 1.0), prop=font_legend, borderaxespad=0.)
    # fig.tight_layout()  # 调整整体空白

    plt.savefig('bench_mark.pdf')
    plt.show()

    pass


def plot_tiny_bar(x=[], y=[], x_ticks=[], x_tickname=[],  y_tickname=[], title=[], ylabel=[], y_ticks=[],
                  xlabel=[], width=0.8,
                  file_name='', legend_element={}, xsize=10, ysize=2,
                  wspace=0.2, _font_label_y={}, _font_label_x={}, data_annotate=[]):
    global font_label_y, font_label_x
    if _font_label_y == {}:
        _font_label_y = font_label_y
    if _font_label_x == {}:
        _font_label_x = font_label_x

    if y_tickname == []:
        y_tickname = y_ticks
    if len(data_annotate) == 0:
        data_annotate = y
    fig = plt.figure(figsize=(xsize, ysize))
    plt.subplots_adjust(wspace=wspace)  # 调整子图间距
    subfig = []
    for i in range(0, len(x)):
        if len(x) >= 8:
            _subfig = plt.subplot(2, int(len(x)/2), i+1)
        else:
            _subfig = plt.subplot(1, len(x), i + 1)
        subfig.append(_subfig)
    # _subfig_for_legend = plt.subplot(1, len(x)+1 ,len(x)+1)
    # subfig.append(_subfig_for_legend)
    _color = []
    _color.append(color[1])
    _color.append(color[2])
    _color.append(color[1])
    for i in range(0, len(x)):
        # print(len(x[i]))
        bar_info = subfig[i].bar(np.array(x[i]), np.array(y[i]), width=width, align='center',
                                 color=_color[0:len(x[i])])
        # color=[color[0],color[2]]) # for mix workload
        subfig[i].set_xticks(x_ticks[i])
        if len(y_ticks)>i and len(y_ticks[i]) > 0:
            subfig[i].set_yticks(y_ticks[i])
        subfig[i].set_xticklabels(x_tickname[i])
        subfig[i].set_yticklabels(y_tickname[i])
        set_xticks(subfig[i])
        # subfig[i].set_title(title[i])

        subfig[i].grid(axis='y', color='gray', linestyle=':')

        # subfig[i].set_ylabel(ylabel[i], _font_label_y)
        subfig[i].set_xlabel(xlabel[i], _font_label_x)
        subfig[i].set_title(ylabel[i], _font_label_y)
        subfig[i].spines['top'].set_visible(False)
        subfig[i].spines['right'].set_visible(False)
        # subfig[i].spines['bottom'].set_visible(False)
        # subfig[i].spines['left'].set_visible(False)

        auto_text(subfig[i], bar_info, data_annotate[i])
    # subfig[0].set_ylim(0,100)
    # i = len(x) - 1
    # subfig[i].legend(handles=legend_element, loc=2, bbox_to_anchor=(1, 1.2), prop=font_legend, borderaxespad=0.)

    fig.tight_layout()  # 调整整体空白
    plt.subplot_tool()
    plt.savefig(file_name)
    plt.show()