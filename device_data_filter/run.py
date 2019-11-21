#!/usr/bin/env python3

import argparse

import pandas as pd


def make_args_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Filter monitoring data for a specific device"
    )
    parser.add_argument(
        "device_index",
        type=int,
        help="index of device to filter data by",
        nargs='?',
        default=0,
    )
    parser.add_argument(
        "input_data",
        type=argparse.FileType(
            mode='rt',
            encoding='utf-8',
        ),
        help="path to monitor log file",
        nargs='?',
        default='-',
    )
    parser.add_argument(
        "output_data",
        type=argparse.FileType(
            mode='wt',
            encoding='utf-8',
            bufsize=1,  # line buffering
        ),
        help="path to extracted data file",
        nargs='?',
        default='-',
    )
    return parser


def main() -> None:
    args_parser = make_args_parser()
    args = args_parser.parse_args()

    df = pd.read_csv(args.input_data)
    df = df[df.device_index == args.device_index]
    df.to_csv(
        path_or_buf=args.output_data,
        sep=',',
        header=True,
        index=False,
    )


if __name__ == '__main__':
    main()
