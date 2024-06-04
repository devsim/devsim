#!/usr/bin/env python

# ARGUMENTS
# testexe with full path
# goldenresults directory with full path
# name of the test output file to create and to compare with the goldenresults

import argparse
import os
import subprocess

parser = argparse.ArgumentParser()

parser.add_argument("--testexe", help="binary to test", required=False)
parser.add_argument("--goldendir", help="golden results directory", required=True)
parser.add_argument("--working", help="working directory", required=False, default="")
parser.add_argument(
    "--output", help="output filename to compare with goldenresult", required=True
)
parser.add_argument("--args", help="list of input arguments", nargs="+", required=False)
args = parser.parse_args()

if args.working:
    output_file = os.path.abspath(os.path.join(args.working, args.output))
    compare_file = os.path.abspath(os.path.join(args.goldendir, args.output))
else:
    head, tail = os.path.split(args.output)
    if head:
        output_file = os.path.abspath(args.output)
        compare_file = os.path.abspath(os.path.join(args.goldendir, tail))


if output_file == compare_file:
    raise RuntimeError("output and golden file cannot be the same file " + output_file)

if args.working:
    os.chdir(args.working)
if args.testexe:
    arguments = [
        args.testexe,
    ]
    if args.args:
        arguments.extend(args.args)
    with open(output_file, "w") as ofile:
        process = subprocess.run(arguments, stdout=ofile, stderr=subprocess.STDOUT)
        if process.returncode != 0:
            raise RuntimeError(
                "%s returned error code %d" % (args.testexe, process.returncode)
            )

f1stat = os.stat(output_file)
f2stat = os.stat(compare_file)

with open(output_file) as f1:
    with open(compare_file) as f2:
        while True:
            a1 = next(f1, "SENTINELlkj")
            a2 = next(f2, "SENTINELlkj")
            if a1 != a2:
                raise RuntimeError("%s differs from %s" % (output_file, compare_file))
            elif a1 == "SENTINELlkj":
                break
