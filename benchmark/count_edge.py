import os
os.chdir("/home/shou/repos")
for folder in os.listdir("/home/shou/repos"):
    final_res1 = {}
    final_res2 = {}
    for i in os.listdir(folder + "/cov"):
        path = folder + "/cov/" + i
        with open(path) as fp:
            content = fp.read()
            res = eval(content)
            for k in res:
                final_res1[k] = 1

    for i in os.listdir(folder + "/cov_orig"):
        try:
            path = folder + "/cov_orig/" + i
            with open(path) as fp:
                content = fp.read()
                res = eval(content)
                for k in res:
                    final_res2[k] = 1
        except:
            pass
    print(folder, len(final_res1) - len(final_res2))
    # if len(final_res) < 1: # remove websites with no index.php
    #     os.system(f"rm -rf {folder}")
    # os.system(f"mv {folder}/cov {folder}/cov_orig")
