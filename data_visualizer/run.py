#!/usr/bin/env python3

import argparse
import math

from enum import Enum

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


FIGURE_WIDTH_INCH = 22
FIGURE_HEIGHT_INCH = 9
FIGURE_SIZE = (FIGURE_WIDTH_INCH, FIGURE_HEIGHT_INCH)

FIGURE_DPI = 300
FIGURE_STYLE = 'seaborn'
FIGURE_LINE_ALPHA = 0.7

FIGURE_TICKS_SIZE = 4
FIGURE_TICKS_WIDTH = 1.5
FIGURE_X_TICKS_COUNT = 25
FIGURE_Y_SCALING = 1.1

MILLIS_IN_UNIT = 1000

DATA_WINDOW_SECONDS = 180     # 3 minutes
DATA_POLLING_PERIOD_MS = 250  # same as for monitor
DATA_POLLING_FREQUENCY = MILLIS_IN_UNIT // DATA_POLLING_PERIOD_MS


class Color(Enum):
    fan_speed = "#2a7ab9"
    temperature = "#51315e"
    power_usage = "#7bb274"


def make_args_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Visualize device data"
    )
    parser.add_argument(
        "output_file_path_format",
        type=str,
        help="format of output file paths",
        nargs='?',
        default="monitor.{suffix}.png",
    )
    parser.add_argument(
        "input_data",
        type=argparse.FileType(
            mode='rt',
            encoding='utf-8',
        ),
        help="path to device data file",
        nargs='?',
        default='-',
    )
    return parser


def format_millis(value: int) -> str:
    minutes, seconds = divmod(int(value / MILLIS_IN_UNIT), 60)
    return "{:02d}:{:02d}".format(minutes, seconds)


def make_patch_spines_invisible(ax) -> None:
    ax.set_frame_on(True)
    ax.patch.set_visible(False)
    for sp in ax.spines.values():
        sp.set_visible(False)


def set_x_labels(ax) -> None:
    start, end = ax.get_xlim()
    step = int(math.floor((end - start) / FIGURE_X_TICKS_COUNT))
    ax.xaxis.set_ticks(np.arange(start, end, step))
    labels = map(format_millis, ax.get_xticks())
    ax.set_xticklabels(labels)


def draw_figure(
    df: pd.DataFrame,
    file_path: str,
    fan_speed_limit: int,
    temperature_limit: int,
    power_usage_limit: int,
) -> None:

    with plt.style.context(FIGURE_STYLE):
        plt.clf()

        fig, ax1 = plt.subplots()
        fig.subplots_adjust(right=0.8)

        ax2 = ax1.twinx()

        ax3 = ax1.twinx()
        ax3.spines["right"].set_position(("axes", 1.07))
        make_patch_spines_invisible(ax3)
        ax3.spines["right"].set_visible(True)

        df['fan_speed'].plot(
            ax=ax1,
            figsize=FIGURE_SIZE,
            color=Color.fan_speed.value,
            label='fan',
            alpha=FIGURE_LINE_ALPHA,
        )
        df['temperature'].plot(
            ax=ax2,
            figsize=FIGURE_SIZE,
            color=Color.temperature.value,
            label='tmp',
            alpha=FIGURE_LINE_ALPHA,
        )
        df['power_usage'].plot(
            ax=ax3,
            figsize=FIGURE_SIZE,
            color=Color.power_usage.value,
            label='pwr',
            alpha=FIGURE_LINE_ALPHA,
        )

        ax1.set_ylim(0, fan_speed_limit)
        ax2.set_ylim(0, temperature_limit)
        ax3.set_ylim(0, power_usage_limit)

        ax1.set_xlabel("Time, mm:ss")
        ax1.set_ylabel("Fan RPM, %",      color=Color.fan_speed.value)
        ax2.set_ylabel("Temperature, °C", color=Color.temperature.value)
        ax3.set_ylabel("Power, W",        color=Color.power_usage.value)

        ax1.tick_params(
            axis='x',
            size=FIGURE_TICKS_SIZE,
            width=FIGURE_TICKS_WIDTH,
        )
        ax1.tick_params(
            axis='y',
            colors=Color.fan_speed.value,
            size=FIGURE_TICKS_SIZE,
            width=FIGURE_TICKS_WIDTH,
        )
        ax2.tick_params(
            axis='y',
            colors=Color.temperature.value,
            size=FIGURE_TICKS_SIZE,
            width=FIGURE_TICKS_WIDTH,
        )
        ax3.tick_params(
            axis='y',
            colors=Color.power_usage.value,
            size=FIGURE_TICKS_SIZE,
            width=FIGURE_TICKS_WIDTH,
        )

        set_x_labels(ax1)

        plt.savefig(
            file_path,
            dpi=FIGURE_DPI,
            legend=False,
            bbox_inches='tight',
        )


def draw_full_window(
    df: pd.DataFrame,
    file_path_format: str,
    fan_speed_limit: int,
    temperature_limit: int,
    power_usage_limit: int,
) -> None:

    file_path = file_path_format.format(suffix="full")
    print(f"Drawing full window to '{file_path}'")
    draw_figure(
        df=df,
        file_path=file_path,
        fan_speed_limit=fan_speed_limit,
        temperature_limit=temperature_limit,
        power_usage_limit=power_usage_limit,
    )


def draw_subwindows(
    df: pd.DataFrame,
    file_path_format: str,
    fan_speed_limit: int,
    temperature_limit: int,
    power_usage_limit: int,
) -> None:

    samples_per_window = DATA_POLLING_FREQUENCY * DATA_WINDOW_SECONDS
    samples_n = df.shape[0]
    windows_n = int(math.ceil(samples_n / samples_per_window))

    for i in range(windows_n):
        file_path = file_path_format.format(suffix=str(i + 1))
        print(f"Drawing subwindow {i + 1} of {windows_n} to '{file_path}'")

        start = i * samples_per_window
        end = start + samples_per_window

        draw_figure(
            df=df.iloc[start:end],
            file_path=file_path,
            fan_speed_limit=fan_speed_limit,
            temperature_limit=temperature_limit,
            power_usage_limit=power_usage_limit,
        )


def main() -> None:
    args_parser = make_args_parser()
    args = args_parser.parse_args()

    df = pd.read_csv(args.input_data)

    df.index = df.timestamp_ms - df.timestamp_ms.min()
    df.power_usage = df.power_usage / MILLIS_IN_UNIT

    fan_speed_limit = 100

    temperature_limit = max(df.temperature)
    temperature_limit *= FIGURE_Y_SCALING

    power_usage_limit = max(df.power_usage)
    power_usage_limit *= FIGURE_Y_SCALING

    draw_full_window(
        df=df,
        file_path_format=args.output_file_path_format,
        fan_speed_limit=fan_speed_limit,
        temperature_limit=temperature_limit,
        power_usage_limit=power_usage_limit,
    )

    draw_subwindows(
        df=df,
        file_path_format=args.output_file_path_format,
        fan_speed_limit=fan_speed_limit,
        temperature_limit=temperature_limit,
        power_usage_limit=power_usage_limit,
    )


if __name__ == '__main__':
    main()
