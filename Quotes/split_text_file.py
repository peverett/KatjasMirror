# Open the text file defined on the command line.
# Split text paragraphs separated by a blank line, reformat them nicely
# and save as individual files in the format 'FILE<#>.txt - where # is
# an incrementing 4-digit number. 9999 should be enough.
#
# Usage: split_text_file.py <input_file.txt>

import sys
import os

if len(sys.argv) != 2:
    sys.exit("Usage: split_text_file.py <input_file.txt>\n")

INPUT_FILE = sys.argv[1]

if not os.path.exists(INPUT_FILE):
    sys.exit("File '{}' does not exist!\n".format(INPUT_FILE))

if not os.path.isfile(INPUT_FILE):
    sys.exit("File '{}' is not a file!\n".format(INPUT_FILE))

print("'{}' is Okay!\n".format(INPUT_FILE))

new_paragraph = True
para_lines = []
para_count = 0
count = 0

MAX_CHARS = 25
MAX_LINES = 10
SPACE = 1

def reflow_paragraph(para):
    """
    Reflows the paragraph so that line breaks are inserted and overall
    formatting is nice. Limits are set by MAX_CHARS and MAX_LINES.
    A '-' causes a newline before it.
    A '^' Forces a newline (and deletes the $).
    """
    words = " ".join(para).split(" ");
    new_para = []
    line = ""
    for word in words:
        if (word[0] == '^'):
            new_para.append(line)
            line = ""
        elif ((len(line) + len(word) + SPACE) > MAX_CHARS) \
        or (word[0] == '-'):
            new_para.append(line)
            line = word
        else:
            if line:
                line = " ".join([ line, word ])
            else:
                line = word
    new_para.append(line)

    if len(new_para) > MAX_LINES:
        print("Max number of lines in a quote exceeded")
    return "\n".join(new_para);


with open(INPUT_FILE) as fp:
    for line in fp:
        count += 1
        line = line.strip()
        if len(line):
            new_paragraph = False
            para_lines.append(line)
        else:
            para = reflow_paragraph(para_lines)
            para_count += 1
            out_file = "FILE{:04d}.txt".format(para_count)
            with open(out_file, "w") as ofp:
                ofp.write("{}\n".format(para))
            print("Generated {}".format(out_file))
            new_para = True
            para_lines = []

print("\nAll done.\n")
            


