import os

for file in os.listdir(os.getcwd()):
    if file.endswith(".edgelist"):
        string = "./otimizado " + str(file) + " >> all_time_otimizado.dat"
        print(string)
        os.system(string)

os.system("rm *.trg && rm *.TO_REMOVE")
