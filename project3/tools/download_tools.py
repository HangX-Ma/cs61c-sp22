from pathlib import Path
import urllib.request
import shutil
import textwrap
import sys

bufsize = 64 * 1024

tools_dir = Path(__file__).resolve().parent
logisim_path = tools_dir / "logisim-evolution.jar"
venus_path = tools_dir / "venus.jar"


def print_same_line(line):
    try:
        cols, rows = shutil.get_terminal_size(fallback=(80, 24))
        line = textwrap.shorten(line, cols)
    except:
        pass
    sys.stdout.write("\r" + line)


def print_progress(prefix, filename, bytes_total, bytes_read):
    mb_read = bytes_read / 1024 / 1024
    mb_total = bytes_total / 1024 / 1024
    percent = bytes_read * 100 / bytes_total
    print_same_line(
        f"{prefix}Downloading {filename} [{percent:5.2f}%, {mb_read:.2f}/{mb_total:.2f} MB]"
    )


def download(url, dest_path, prefix="", progress=True):
    tmp_dest_path = Path(dest_path).with_name(f".{dest_path.name}.tmp")
    print(f"{prefix}Checking...")
    with urllib.request.urlopen(url) as resp:
        filename = Path(resp.url).name
        sys.stdout.write(f"{prefix}Downloading {filename}...")
        bytes_total = "?"
        try:
            bytes_total = int(resp.getheader("content-length"))
        except:
            pass
        with tmp_dest_path.open("wb") as dest:
            bytes_read = 0
            i = 0
            while True:
                buf = resp.read(bufsize)
                if not buf:
                    print_progress(prefix, filename, bytes_total, bytes_total)
                    break
                dest.write(buf)
                bytes_read += len(buf)
                if progress:
                    i += 1
                    if i & 3 == 0:
                        print_progress(prefix, filename, bytes_total, bytes_read)
        sys.stdout.write("\n")
    tmp_dest_path.replace(dest_path)


if __name__ == "__main__":
    download(
        "https://inst.eecs.berkeley.edu/~cs61c/tools/logisim-evolution-latest.jar",
        logisim_path,
        prefix="[logisim-evolution] ",
    )
    download(
        "https://inst.eecs.berkeley.edu/~cs61c/tools/venus-latest.jar",
        venus_path,
        prefix="[venus] ",
    )
