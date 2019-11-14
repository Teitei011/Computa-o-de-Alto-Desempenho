import os

for file in os.listdir(os.getcwd()):
    if file.endswith(".edgelist"):
        string = "./nao_otimizado " + str(file) + " >> all_time_otimizado.dat"
        print(string)
        os.system(string)

os.system("rm *.TO_REMOVE")
