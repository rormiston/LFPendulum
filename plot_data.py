import matplotlib
matplotlib.use("agg")
import matplotlib.pyplot as plt
import numpy as np
import argparse
import sys
import os
import scipy.signal as sig


def set_plot_style():
    plt.style.use('bmh')
    matplotlib.rcParams.update({
        'axes.grid': True,
        'axes.titlesize': 'medium',
        'font.family': 'serif',
        'font.size': 12,
        'grid.color': 'w',
        'grid.linestyle': '-',
        'grid.alpha': 0.5,
        'grid.linewidth': 1,
        'legend.borderpad': 0.2,
        'legend.fancybox': True,
        'legend.fontsize': 13,
        'legend.framealpha': 0.7,
        'legend.handletextpad': 0.1,
        'legend.labelspacing': 0.2,
        'legend.loc': 'best',
        'lines.linewidth': 1.5,
        'savefig.bbox': 'tight',
        'savefig.pad_inches': 0.02,
        'text.usetex': False,
        'text.latex.preamble': r'\usepackage{txfonts}'
    })

    matplotlib.rc("savefig", dpi=100)
    matplotlib.rc("figure", figsize=(7, 4))


def parse_command_line():
    parser = argparse.ArgumentParser()
    parser.add_argument('--datafile', '-d',
			help    = 'CSV file to read',
			default = 'Data.csv',
			dest    = "datafile")

    return parser.parse_args()


def plot_psd(target, fs = 200,
             nfft       = 1600,
             plotDir    = '.',
             plotName   = None,
             close      = True,
             labels     = None,
             fmin       = 0,
             fmax       = 100):

    if plotName is None:
        plotName = 'Pendulum PSD'

    if close:
        ff1, pp = sig.welch(target, fs=fs, nperseg=nfft,axis=-1)
        strain = np.sqrt(pp).T
        plt.loglog(ff1.T, strain)
        plt.title(plotName + ' PSD')
    else:
        plt.title('Pendulum PSDs')
        plotName = 'Pendulum'
        for i in range(target.shape[1] - 1):
            ff1, pp = sig.welch(target[:, i + 1], fs=fs, nperseg=nfft,axis=-1)
            strain = np.sqrt(pp).T
            plt.loglog(ff1.T, strain, alpha=0.8, label=labels[i])

    plt.grid(True, which='minor')
    plt.ylabel(r'Strain (m/$\sqrt{\rm Hz}$)')
    plt.xlabel('Frequency (Hz)')
    plt.legend()
    figName = '{0}/{1}'.format(plotDir, plotName.replace(' ', '_'))
    plt.savefig('{}_psd.png'.format(figName))
    plt.close()


def plot_timeseries(time, data, label):
    plt.plot(time, data, label=label)
    plt.title('{} Timeseries'.format(label))
    plt.ylabel('Displacement (Volts)')
    plt.xlabel('Time (s)')
    plt.legend(loc=1)
    plt.savefig('{}_timeseries.png'.format(label))
    plt.close()


# Unpack cmd flags
params = parse_command_line()
if not os.path.isfile(params.datafile):
    sys.exit('ERROR: File not found')

# create array
data   = np.genfromtxt(params.datafile, delimiter=',')
labels = ['bottom', 'top', 'z_mag', 'y_mag',
          'x_mag', 'z_seis', 'y_seis', 'x_seis']

# Make individual PSDs
set_plot_style()
for i in range(len(labels)):
    plot_psd(data[:, i+1], plotName=labels[i])
    plot_timeseries(data[:, 0], data[:, i + 1], labels[i])

# Put all PSDs on one plot
plot_psd(data, close=False, labels=labels)
