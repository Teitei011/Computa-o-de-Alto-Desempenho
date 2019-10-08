import sys
import os
import string

import filecmp
path = os.getcwd()


for filename in os.listdir():
    executable = "./vector.exe "
    if filename.endswith(".edgelist"):
        print("Running {}".format(filename))
        executable += filename
        # print(executable)
        os.system(executable) # Running the program

        # Now it will compare the output calculated with the real result
        if(filecmp.cmp(filename.replace(".edgelist", ".trg"), filename.replace(".edgelist", ".TO_REMOVE"))):
            print("### SUCCESS: Files Are Identical! ###\n\n")
        else:
            print("### WARNING: Files Are Different! ###\n\n")

# os.system("rm *.TO_REMOVE")
