#!/usr/bin/env python3

import argparse
import io


def make_args_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Extract data from monitor log"
    )
    parser.add_argument(
        "monitor_log",
        type=argparse.FileType(
            mode='rt',
        ),
        help="path to monitor log file",
        nargs='?',
        default='-',
    )
    parser.add_argument(
        "extracted_data",
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


def rewind_log_headers(stream: io.TextIOWrapper) -> None:
    for line in stream:
        line = line.strip()
        if line.startswith("Monitoring GPUs"):
            break


def output_data(istream: io.TextIOWrapper, ostream: io.TextIOWrapper) -> None:
    for line in istream:
        line = line.strip()
        if not line:
            continue
        ostream.write(f"{line}\n")


def main() -> None:
    args_parser = make_args_parser()
    args = args_parser.parse_args()

    rewind_log_headers(args.monitor_log)
    output_data(args.monitor_log, args.extracted_data)


if __name__ == '__main__':
    main()
