
import sys
import subprocess

args = sys.argv[1:]

legacy_options = {
    "--flash-mode",
    "--flash-freq",
    "--flash-size",
}

if args and any("esptool" in arg.lower() for arg in args):

    converted = []
    i = 0

    while i < len(args):
        arg = args[i]

        if arg in legacy_options:
            if i + 1 >= len(args):
                sys.exit("Missing value for " + arg)

            value = args[i + 1]
            converted.extend([arg, value])
            i += 2

        else:
            converted.append(arg)
            i += 1

    args = converted

sys.exit(subprocess.call([sys.executable] + args))
